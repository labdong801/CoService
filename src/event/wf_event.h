//
// Created by lml on 2018/6/15.
//

#ifndef PROJECT_TCP_ASYNC_SERVER_H
#define PROJECT_TCP_ASYNC_SERVER_H

#include <list>
#include <memory>
#include <atomic>
#include "../third/thread_pool/threadpool.hpp"
#include "../core/modules/easn/easn_server_channel.h"

using namespace wf;

class wf_event {

public:

    wf_event();

    //单例模式
    static std::shared_ptr<wf_event> instance();

    //开启服务
    void start();
    void stop();

    //增加一个服务通道
    void add_server_channel(std::shared_ptr<server_channel>);


private:
    //单例模式
    static std::shared_ptr<wf_event> singleton;

    //是否运行
    std::atomic<bool> running;

    //服务列表
    std::shared_ptr<std::list<std::shared_ptr<server_channel>>> customer_chennel_list;

    //socket fd 列表
    std::shared_ptr<std::list<int>> fds;

    //Unauthorized socket fd 列表
    std::shared_ptr<std::list<int>> unauth_fds;

    //处理消息回调
//    void handleMessage(char & ,size_t);

};


#endif //PROJECT_TCP_ASYNC_SERVER_H
