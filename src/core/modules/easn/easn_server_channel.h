//
// Created by lml on 2018/6/26.
//

#ifndef PROJECT_SERVER_CHANNEL_H
#define PROJECT_SERVER_CHANNEL_H

#include <iostream>
#include <memory>
#include <list>
#include <stack>
#include "../../channel.h"
#include "easn_connection.h"
#include "easn_package.h"
using namespace wf;

namespace easn{

    struct task_t
    {
        stCoRoutine_t *co;
        int fd;
    };

    class server_channel :virtual public channel{

    public:
        server_channel(int p);
        virtual ~server_channel();

        virtual void * get_handle();
        //初始化
        virtual void init();
        //启动服务
        virtual void start();

        //打印调试信息
        static void debug();

    private:
        //客户端最大连接数
        static const int MAX_ACCEPT_COUNT = 10;
        //连接后没有授权超时时间
        static const int TIMEOUT = 10;

        //监听端口
        int port;
        //socket fd
        int sockfd;

        //是否初始化
        std::atomic_flag is_init;

        //任务栈
        static std::shared_ptr<std::stack<task_t*>> m_tasks;

        //将socket设置为非阻塞
        static int set_non_blocking(int fd);

        //服务器连接协程回调
        static void * server_accept( void * arg );

        //服务器读写协程回调
        static void * server_read_write( void * arg );

        static void handle_packet(int fd, unsigned char * buf,int pos,int len,std::function<void (packet_t &)> callback);
    };
}

#endif //PROJECT_SERVER_CHANNEL_H
