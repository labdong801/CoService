//
// Created by lml on 2018/6/30.
//

#include <co_routine.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <iomanip>
#include <signal.h>
#include "wf_co_event.h"
#include "../core/wf_log.h"
#include "../core/modules/easn/easn_server_channel.h"
#include "../core/modules/easn/easn_dispatch_channel.h"

using namespace wf;

//单例模式
std::shared_ptr<wf_co_event> wf_co_event::singleton = nullptr;
std::shared_ptr<wf_co_event> & wf_co_event::instance() {
    if (singleton == nullptr)
        singleton = std::make_shared<wf_co_event>();
    return singleton;
}

//构造函数
wf_co_event::wf_co_event():customer_chennel_list(std::make_shared<std::list<std::shared_ptr<channel>>>()){

}

//析构函数
wf_co_event::~wf_co_event(){

}

//添加服务通道
void wf_co_event::add_server_channel(std::shared_ptr<channel> & channel) {
    channel->init();
    customer_chennel_list->push_back(channel);
}



void * wf_co_event::test(void * arg){
    co_enable_hook_sys();
    for(;;){
        //观察server_channel
        easn::server_channel::debug();
        easn::dispatch_channel::debug();
        poll(NULL, 0, 3000);
    }
}


void stop(int signo)
{
    wf_info << "退出!!" << std::endl;
    exit(0);
}


//开启事件循环
int wf_co_event::wf_co_event_loop(){

    signal(SIGINT, stop);

    //至少需要加入一个服务
    if(customer_chennel_list->size() == 0){
        wf_error << "Error:至少需要添加一个服务" << std::endl;
        return -1;
    }

    //建立服务器处理连接协程
    for(std::list<std::shared_ptr<channel>>::iterator it = customer_chennel_list->begin();it != customer_chennel_list->end(); it++) {
        //启动服务
        (*it)->start();
    }

    //测试协程（可以去掉）
    stCoRoutine_t * test_routine = nullptr;
    co_create(&test_routine, nullptr, test, 0);
    co_resume(test_routine);

    co_eventloop( co_get_epoll_ct(),0,0 );
    return 0;
}


