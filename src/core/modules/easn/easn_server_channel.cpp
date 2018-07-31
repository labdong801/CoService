//
// Created by lml on 2018/6/26.
//

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <co_routine.h>
#include <arpa/inet.h>
#include <iomanip>
#include "easn.h"
#include "easn_server_channel.h"
#include "../../../event/wf_co_event.h"
#include "../../wf_log.h"
#include "easn_package.h"
#include "easn_client_manager.h"
#include "easn_connection.h"

using namespace wf;
using namespace easn;


std::shared_ptr<std::stack<task_t*>> server_channel::m_tasks = std::make_shared<std::stack<task_t*>>();

server_channel::server_channel(int p):port(p),sockfd(-1),is_init(false){
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in ser_sockaddr;
    bzero(&ser_sockaddr, sizeof(ser_sockaddr));
    ser_sockaddr.sin_family = AF_INET;
    ser_sockaddr.sin_port = htons(p);
    ser_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    int ret = bind(sockfd, (struct sockaddr *)&ser_sockaddr, sizeof(ser_sockaddr));
    if(ret < 0){
        wf_error << "开启服务失败:[" << strerror(errno) << "]" << port << "端口被占用" << std::endl;
        exit(-1);
    }
    listen(sockfd, 10);

    ret = set_non_blocking(sockfd);
    if(ret < 0){
        wf_error << "设置非阻塞失败" << std::endl;
        exit(-1);
    }

}

server_channel::~server_channel(){
    //释放socket
    if(sockfd != -1){
        wf_info << "释放服务通道:" << port << std::endl;
        close(sockfd);
    }
}

//获取句柄
void * server_channel::get_handle(){
    return &(this->sockfd);
}

int server_channel::set_non_blocking(int fd) {
    int iFlags;

    iFlags = fcntl(fd, F_GETFL, 0);
    iFlags |= O_NONBLOCK;
    iFlags |= O_NDELAY;
    int ret = fcntl(fd, F_SETFL, iFlags);
    return ret;
}


int co_accept(int fd, struct sockaddr *addr, socklen_t *len );
void * server_channel::server_accept(void * arg){

    co_enable_hook_sys();

    //外部传入
    int fd = *(int*)arg;

    sockaddr_in clientddress;
    for(;;)
    {
        socklen_t len = sizeof(clientddress);
        int connfd = co_accept(fd, (struct sockaddr *)&clientddress, &len);
        if( connfd < 0 )
        {
            struct pollfd pf = { 0 };
            pf.fd = fd;
            pf.events = (POLLIN|POLLERR|POLLHUP);
            co_poll( co_get_epoll_ct(),&pf,1, -1 /* -1:永不超时 */);
            continue;
        }

        //如果没有空闲的协程，表示已经连接满了，拒绝新连接
        if( m_tasks->empty() )
        {
            wf_error << "连接池已满" << std::endl;
            close( connfd );
            continue;
        }

        //设置非阻塞
        set_non_blocking(connfd);

        wf_info << inet_ntoa(clientddress.sin_addr) << ":" << clientddress.sin_port << std::endl;

        //消耗掉一个空闲协程
        task_t *task = m_tasks->top();
        task->fd = connfd;
        m_tasks->pop();
        co_resume( task->co ); //主动切换协程
    }
}

void * server_channel::server_read_write(void * arg){
    co_enable_hook_sys();

    task_t * task = (task_t*)arg;

    for(;;){
        if( -1 == task->fd )
        {
            co_yield_ct();
            continue;
        }

        //待处理的fd
        int fd = task->fd;
        //建立连接时的时间戳
        const int32_t accept_timestamp = time(NULL);
        //创建一个会话
        auto session = easn::client_manager::instance()->new_connection(fd);

        for(;;){
            int32_t timeout_left = -1;
            //如果没有通过鉴权，需要计算超时时间
            if(!(session->auth)){
                //当前时间戳
                int32_t current_timestamp = time(NULL);
                //剩余超时时间
                timeout_left = (TIMEOUT - (current_timestamp - accept_timestamp)) * 1000L;

                wf_info << "剩余超时时间:" << timeout_left << "ms" << std::endl;

                if(timeout_left <= 0){
                    wf_error << "鉴权超时关闭一个socket,fd:" << fd << std::endl;
                    close( fd );
                    break;
                }
            }

            struct pollfd pf = { 0 };
            pf.fd = fd;
            pf.events = (POLLIN|POLLERR|POLLHUP);
            co_poll( co_get_epoll_ct(),&pf,1,timeout_left/* -1:永不超时 */);  //此时协程已经让出去了
            unsigned char buf[ 1024 * 16 ];
            memset(buf,0,sizeof(buf));
            int ret = read( fd,buf,sizeof(buf) );
            if( ret > 0 )
            {
                //先判断数据包长度
                if( ret < packet_min_size()){
                    wf_error << "数据包长度太短:" << ret << "<" << packet_min_size() << std::endl;
                }else{
                    //处理数据包
                    handle_packet(fd,buf,0,ret,[&session](packet_t & pkt){
                        //转发给对应的微服务
                        if(pkt.info.type != TYPE_INIT && session->auth == 0){
                            //不是鉴权数据包，也没有授权此连接
                            // do nothing ...
                        }else{
                            //分发数据包
                            easn::dispatch_message(session->sid,pkt.info.type,&pkt.info.data,sizeof_data(pkt));
                            //自动授权
                            if(TYPE_INIT == pkt.info.type){
                                session->auth = 1;
                            }
                        }
                    });
                }
            }
            if( ret == 0 ){
                wf_info << "客户端关掉一个socket,fd:" << fd << std::endl;
                close( fd );
                break;
            }
            if( ret <= 0 )
            {
                if (errno == EAGAIN){
                    //如果当前socket是已经鉴权的
                    if(session->auth)
                        continue;
                    //如果当前socket没有鉴权
                    wf_error << "鉴权超时关闭一个socket,fd:" << fd << std::endl;
                    close( fd );
                    break;
                }
                wf_info << "发生错误关闭一个socket,fd:" << fd << " errno:" << errno << " msg:" << strerror(errno) << std::endl;
                close( fd );
                break;
            }
        }

        //当前协程重置为空闲，重新加入列表
        task->fd = -1;
        m_tasks->push(task);
    }

}

void server_channel::handle_packet(int fd, unsigned char *buf, int pos, int len,std::function<void (packet_t &)> callback) {
    //接收到客户端发出的消息
    packet_t rece_ptk; //接收数据包
    ack_t ack_ptk;     //应答数据包

    //查找数据包头
    int start_position = find_header_position(buf,pos,len);
    if(start_position == -1){
        wf_error << "数据包格式错误" << std::endl;
        return;
    }
    //数据包有效长度
    int length = len - start_position;

    //拷贝数据
    memcpy(rece_ptk.buf,buf + start_position,length);

    if(sizeof_packet(rece_ptk) > length){
        //数据包出错了
        wf_error << "packet error:" << std::endl;
        debug_print_packet(rece_ptk);
    }

    //检查CRC是否校验成功
    if(check_crc(rece_ptk.buf,sizeof_packet(rece_ptk))){
        //CRC校验成功。正确解析了一个数据包
        fill_ack_packet(ack_ptk,rece_ptk,SUCCESS);
        write(fd,ack_ptk.buf,sizeof(ack_t));
        wf_info << "packet:" << std::endl;
        debug_print_packet(rece_ptk);
        //回调
        callback(rece_ptk);
    }else{
        fill_ack_packet(ack_ptk,rece_ptk,CRC_ERROR);
        write(fd,ack_ptk.buf,sizeof(ack_t));
        wf_error << "CRC校验错误:";
        debug_print_packet(rece_ptk);
        //再检查是否有下一个数据包
        handle_packet(fd,buf,start_position + 2,len,callback); //start_position + 2 的意思是跳过这次的0xaa55头部，再匹配下一个0xaa55
    };

    if(ntohs(rece_ptk.info.length) < length){
        //数据包不止一个
        //递归调用
        handle_packet(fd,buf,start_position + sizeof_packet(rece_ptk),len,callback);
    }
}

//初始化
void server_channel::init() {

    if(!is_init.test_and_set()){
        m_tasks = std::make_shared<std::stack<task_t*>>();
        //建立客户端读写协程
        for(int i=0;i<MAX_ACCEPT_COUNT;i++){
            task_t * task = (task_t*)calloc( 1,sizeof(task_t) );
            task->fd = -1;
            m_tasks->push(task);
            co_create( &(task->co),NULL,server_read_write,task);
            co_resume( task->co );
        }

        wf_info << "创建TCP服务:[0.0.0.0:" << port << "]" <<std::endl;
    }

}

void server_channel::start(){
    co_create(&routine, nullptr, server_accept, &sockfd);
    co_resume(routine);
}

//返回一些当前服务的信息
void server_channel::debug() {
    wf_info << "当前连接数:" << (MAX_ACCEPT_COUNT - m_tasks->size()) << "/" << MAX_ACCEPT_COUNT << std::endl;
}



