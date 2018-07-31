//
// Created by lml on 2018/7/9.
//

#include "easn_dispatcher_manager.h"
#include "../../wf_log.h"
#include "../../config.h"

using namespace easn;

//单例模式
std::shared_ptr<dispatcher_manager> dispatcher_manager::singleton = nullptr;
std::shared_ptr<dispatcher_manager> & dispatcher_manager::instance() {
    if (singleton == nullptr)
//        singleton = std::make_shared<kafka_manager>(new kafka_manager());
        singleton = std::shared_ptr<dispatcher_manager>(new dispatcher_manager());
    return singleton;
}

dispatcher_manager::dispatcher_manager():dispatcher(nullptr){
//    producer = new easn_kafka_dispatcher(easn::config::instance()->kafka_send_topic_name());
}

dispatcher_manager::~dispatcher_manager() {
//    delete producer;
}

std::shared_ptr<kafka_dispatcher> dispatcher_manager::get_dispatcher() {
    if(dispatcher == nullptr){
        throw "获取不到kafka producer!";
    }
    return dispatcher;
}

void dispatcher_manager::add_dispatcher(std::shared_ptr<kafka_dispatcher> p) {
    this->dispatcher = p;
}