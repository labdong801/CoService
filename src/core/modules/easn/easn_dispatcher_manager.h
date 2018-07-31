//
// Created by lml on 2018/7/9.
//

#ifndef PROJECT_WF_KAFKA_MANAGER_H
#define PROJECT_WF_KAFKA_MANAGER_H

#include <memory>
#include "easn_kafka_dispatcher.h"

namespace easn{

    class dispatcher_manager {
    public:
        //单例模式
        static std::shared_ptr<dispatcher_manager> & instance();
        ~dispatcher_manager();


        std::shared_ptr<kafka_dispatcher> get_dispatcher();
        void add_dispatcher(std::shared_ptr<kafka_dispatcher>);

    private:
        //单例模式
        static std::shared_ptr<dispatcher_manager> singleton;
        dispatcher_manager();

//        easn_kafka_dispatcher * producer;
        std::shared_ptr<kafka_dispatcher> dispatcher;
    };
}



#endif //PROJECT_WF_KAFKA_MANAGER_H
