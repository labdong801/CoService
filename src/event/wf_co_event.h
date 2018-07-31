//
// Created by lml on 2018/6/30.
//

#ifndef PROJECT_WF_CO_EVENT_H
#define PROJECT_WF_CO_EVENT_H

#include <iostream>
#include <memory>
#include <list>
#include <stack>
#include "../core/channel.h"

namespace wf{

    class wf_co_event {

    public:
        //构造函数
        wf_co_event();

        //析构函数
        ~wf_co_event();

        //单例模式
        static std::shared_ptr<wf_co_event> & instance();

        //开始事件循环
        int wf_co_event_loop();

        //添加服务通道
        void add_server_channel(std::shared_ptr<wf::channel> &);

    private:
        //单例模式
        static std::shared_ptr<wf_co_event> singleton;

        //服务列表
        std::shared_ptr<std::list<std::shared_ptr<channel>>> customer_chennel_list;


        //测试协程
        static void * test(void * arg);

    };
}



#endif //PROJECT_WF_CO_EVENT_H
