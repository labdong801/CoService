//
// Created by lml on 2018/6/15.
//

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <errno.h>
#include <list>
#include "wf_event.h"

//单例模式
std::shared_ptr<wf_event> wf_event::singleton = nullptr;
std::shared_ptr<wf_event> wf_event::instance() {
    if (singleton == nullptr)
        singleton = std::make_shared<wf_event>();
    return singleton;
}

wf_event::wf_event():fds(std::make_shared<std::list<int>>()),
                                     unauth_fds(std::make_shared<std::list<int>>()),
                                     customer_chennel_list(std::make_shared<std::list<std::shared_ptr<server_channel>>>()),
                                     running(false) {

}

//新增一个server通道
void wf_event::add_server_channel(std::shared_ptr<server_channel> channel) {
    customer_chennel_list->push_back(channel);
}

void wf_event::stop(){
    //停止运行线程
    running = false;
}

void wf_event::start() {

    //模拟http返回
    const char * http_response = "HTTP/1.0 200 OK\r\nServer: NetServer\r\nContent-Length: 11\r\nContent-type: text/plain\r\n\r\nHello world!\n";

    struct epoll_event event;   // 告诉内核要监听什么事件
    struct epoll_event wait_event; //内核监听完的结果

    //创建epoll
    int epfd = epoll_create(10); // 创建一个 epoll 的句柄，参数要大于 0。 Linux 2.6.8以后这个参数没有太大意义。
    if( -1 == epfd ){
        std::cerr << "epoll create failed ! " << errno << std::endl;
        return;
    }

    //添加服务Socket 到监听事件
    for(std::list<std::shared_ptr<server_channel>>::iterator it = customer_chennel_list->begin();it != customer_chennel_list->end(); it++) {
        event.data.fd = (*it)->get_socket();     //监听套接字
        event.events = EPOLLIN;       // 表示对应的文件描述符可以读
        int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, (*it)->get_socket(), &event);
        if (-1 == ret) {
            std::cerr << "Add server fd to epoll failed ! " << errno <<std::endl;
            return;
        }
    }

    running = true;
    while(running)
    {
        // 监视并等待多个文件（标准输入，udp套接字）描述符的属性变化（是否可读）
        // 没有属性变化，这个函数会阻塞，直到有变化才往下执行，这里没有设置超时

        int maxfd = fds->size() + 1;
        int ret = epoll_wait(epfd, &wait_event, 20 , 1);
        if(ret < 0){
            //Error
            std::cerr << "epoll 错误:" << strerror(errno) << " code:" << errno << std::endl;
        }else if(ret == 0){
            //No event
            continue;
        }

        //先检查是否是server socket
        for(std::list<std::shared_ptr<server_channel>>::iterator it = customer_chennel_list->begin();it != customer_chennel_list->end(); it++){
            if(( (*it)->get_socket() == wait_event.data.fd )
               && ( EPOLLIN == wait_event.events & EPOLLIN ) )
            {
                struct sockaddr_in cli_addr;
                socklen_t clilen = sizeof(cli_addr);

                int connfd = accept((*it)->get_socket(), (struct sockaddr *)&cli_addr, &clilen);
                fds->push_back(connfd);
                event.data.fd = connfd; //监听套接字
                event.events = EPOLLIN; // 表示对应的文件描述符可以读

                //6.1.3.事件注册函数，将监听套接字描述符 connfd 加入监听事件
                ret = epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &event);
                if(-1 == ret){
                    perror("epoll_ctl");
                    return;
                }
                std::cout << "添加一个连接" << inet_ntoa(cli_addr.sin_addr) << ":" << cli_addr.sin_port <<  " 共：" << fds->size() << "个连接" <<std::endl;

                if(--ret <= 0)
                    continue;
            }
        }

        //6.2继续响应就绪的描述符
        std::list<int>::iterator it = fds->begin();
        while (it != fds->end())
        {
            std::list<int>::iterator i = it++;

            if(( *i == wait_event.data.fd )
               && ( EPOLLIN == wait_event.events & (EPOLLIN|EPOLLERR) ))
            {
                int len = 0;
                char buf[1500] = "";


                //6.2.1接受客户端数据
                if((len = recv(*i , buf, sizeof(buf), 0)) < 0)
                {
                    //ECONNRESET tcp连接超时、RST
                    close(*i);
                    fds->erase(i);
                    perror("关闭一个连接：read error:");
                }
                else if(len == 0)//客户端关闭连接
                {
                    close(*i);
                    fds->erase(i);
                    std::cout << "关闭第fd" << *i <<"，共：" << fds->size() << "个连接" << std::endl;
                }
                else{
                    //正常接收到服务器的数据
//                    std::cout << "接收到信息:" << std::endl << buf << std::endl;
                    send(*i, http_response, strlen(http_response), 0);
                }

                if(--ret <= 0)
                    break;
            }
        }
    }

    //关闭socket
    close(epfd);
}


