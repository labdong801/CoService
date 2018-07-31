//
// Created by lml on 2018/7/4.
//

#include "threadpool.h"

using namespace wf;


//单例模式
std::shared_ptr<threadpool> threadpool::singleton = nullptr;
std::shared_ptr<threadpool> & threadpool::instance() {
    if (singleton == nullptr)
        singleton = std::make_shared<threadpool>();
    return singleton;
}

threadpool::threadpool():tp(nullptr){
    tp = new boost::asio::thread_pool(MAX_THREAD_COUNT);
}

threadpool::~threadpool() {
    delete tp;
}