//
// Created by lml on 2018/7/4.
//

#ifndef PROJECT_THREADPOOL_H
#define PROJECT_THREADPOOL_H

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <iostream>

namespace wf{

    class threadpool {

    public:

        threadpool();
        ~threadpool();

        //提交线程
        template<class F, class... Args>
        inline void schedule(F&&  f, Args&& ... args){
            boost::asio::post(*tp,std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        };

        static std::shared_ptr<threadpool> & instance();


    private:
        //线程池大小
        static const int MAX_THREAD_COUNT = 4;

        //单例模式
        static std::shared_ptr<threadpool> singleton;

        //线程池
        boost::asio::thread_pool * tp;
    };
}

#endif //PROJECT_THREADPOOL_H
