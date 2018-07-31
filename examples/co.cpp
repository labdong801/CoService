//
// Created by lml on 2018/6/28.
//


#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <memory.h>
#include <fcntl.h>
#include <stack>
#include <unistd.h>
#include <arpa/inet.h>
#include <iomanip>
#include <sys/ioctl.h>

#include "co_routine.h"

#define PORT 40625

struct task_t
{
    stCoRoutine_t *co;
    int fd;
};

static std::stack<task_t*> g_readwrite;
static int sockfd = -1;

static int set_non_block(int iSock)
{
    int iFlags;

    iFlags = fcntl(iSock, F_GETFL, 0);
    iFlags |= O_NONBLOCK;
    iFlags |= O_NDELAY;
    int ret = fcntl(iSock, F_SETFL, iFlags);
    return ret;
}

static int set_block(int iSock)
{
    int b_on = 1;
    int ret = ioctl (iSock, FIONBIO, &b_on);
    return ret;
}

int create_client_socket() {
    int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd < 0) {
        //创建socket失败
        std::cerr << "socket创建失败:" << errno << " " << strerror(errno) << std::endl;
        return fd;
    }
    struct sockaddr_in ser_sockaddr;
    bzero(&ser_sockaddr, sizeof(ser_sockaddr));
    ser_sockaddr.sin_family = AF_INET;
    ser_sockaddr.sin_port = htons(8888);
    ser_sockaddr.sin_addr.s_addr = inet_addr("10.10.2.35");

    if(set_block(fd) < 0){
        std::cerr << "socket设置阻塞失败:" << errno << " " << strerror(errno) << std::endl;
    }

    connect(fd, (struct sockaddr *)&ser_sockaddr, sizeof(ser_sockaddr));

    return fd;

}

int create_server_socket() {
    int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd < 0) {
        //创建socket失败
        std::cerr << "socket创建失败:" << errno << " " << strerror(errno) << std::endl;
        return fd;
    }
    struct sockaddr_in ser_sockaddr;
    bzero(&ser_sockaddr, sizeof(ser_sockaddr));
    ser_sockaddr.sin_family = AF_INET;
    ser_sockaddr.sin_port = htons(PORT);
    ser_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int ret = set_non_block(fd);
    if(ret < 0) {
        std::cerr << "socket设置非阻塞失败:" << errno << " " << strerror(errno) << std::endl;
    }

    bind(fd, (struct sockaddr *) &ser_sockaddr, sizeof(ser_sockaddr));
    listen(fd, 10);

    return fd;

}

// 处理Accept协程
int co_accept(int fd, struct sockaddr *addr, socklen_t *len );
static void *accept_routine( void * )
{
    co_enable_hook_sys();
    printf("accept_routine\n");
    fflush(stdout);
    for(;;)
    {
        //printf("pid %ld g_readwrite.size %ld\n",getpid(),g_readwrite.size());
        if( g_readwrite.empty() )
        {
            printf("empty\n"); //sleep
            struct pollfd pf = { 0 };
            pf.fd = -1;
            poll( &pf,1,1000);

            continue;

        }
        struct sockaddr_in addr; //maybe sockaddr_un;
        memset( &addr,0,sizeof(addr) );
        socklen_t len = sizeof(addr);

        int fd = co_accept(sockfd, (struct sockaddr *)&addr, &len);
        if( fd < 0 )
        {
            struct pollfd pf = { 0 };
            pf.fd = sockfd;
            pf.events = (POLLIN|POLLERR|POLLHUP);
            co_poll( co_get_epoll_ct(),&pf,1,1000 );
            continue;
        }
        if( g_readwrite.empty() )
        {
            close( fd );
            continue;
        }
        std::cout << "添加一个连接" << inet_ntoa(addr.sin_addr) << ":" << addr.sin_port <<  " 共：" << 1 << "个连接" <<std::endl;
        set_non_block( fd );

        //消耗掉一个co
        task_t *co = g_readwrite.top();
        co->fd = fd;
        g_readwrite.pop();
        co_resume( co->co );
    }
}


// 处理Read Write 协程
static void * readwrite_routine( void *arg )
{

    co_enable_hook_sys();

    task_t *co = (task_t*)arg;
    char buf[ 1024 * 16 ];
    for(;;)
    {
        if( -1 == co->fd )
        {
            g_readwrite.push( co );
            co_yield_ct();
            continue;
        }

        int fd = co->fd;
        co->fd = -1;

        for(;;)
        {
            struct pollfd pf = { 0 };
            pf.fd = fd;
            pf.events = (POLLIN|POLLERR|POLLHUP);
            co_poll( co_get_epoll_ct(),&pf,1,1000);

            int ret = read( fd,buf,sizeof(buf) );
            if( ret > 0 )
            {

                //模拟一次RPC
                //创建socket转发
                int clientfd = create_client_socket();
                write(clientfd, buf, ret);
                std::cout << "正在转发数据:" << clientfd << std::endl;
                memset(buf,0, sizeof(buf));
                ret = read(clientfd, buf, sizeof(buf));
                std::cout << "转发数据完成:" << clientfd << std::endl;

                std::cout << std::setw(ret) << buf << std::endl;
                ret = write( fd,buf,ret );
            }
            if( ret <= 0 )
            {
                // accept_routine->SetNonBlock(fd) cause EAGAIN, we should continue
                if (errno == EAGAIN)
                    continue;
                close( fd );
                std::cout << "关掉一个socket" << std::endl;
                break;
            }
        }

    }
}

int main(int argc,char *argv[]){

    sockfd = create_server_socket();
    std::cout << "sockfd:" << sockfd << std::endl;
    if(sockfd > 0){

        //创建读写协程
        task_t * task = (task_t*)calloc( 1,sizeof(task_t) );
        task->fd = -1;
        co_create( &(task->co),NULL,readwrite_routine,task );
        co_resume( task->co );

        //创建Accept协程
        stCoRoutine_t *accept_co = NULL;
        co_create( &accept_co,NULL,accept_routine,0 );
        co_resume( accept_co );

        co_eventloop( co_get_epoll_ct(),0,0 );

        return 0;
    }
    return errno;
}