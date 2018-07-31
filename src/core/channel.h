//
// Created by lml on 2018/7/2.
//

#ifndef PROJECT_CHANNEL_H
#define PROJECT_CHANNEL_H

//#include <co_routine_inner.h>
//#include "co_routine_inner.h"
#include "../../third/libco/co_routine_inner.h"

namespace wf{
    class channel {
    public:
        //构造函数
        channel():routine(nullptr){
        }
        //获取句柄（根据实际内容而定）
        virtual void * get_handle() = 0;
        //初始化服务
        virtual void init() = 0;
        //启动服务
        virtual void start() = 0;
        //析构函数
        virtual ~channel() {
        }

    protected:
        stCoRoutine_t * routine;

    private:
        //不允许拷贝
        channel(channel &);
        channel &operator=(const channel &);
    };
}



#endif //PROJECT_CHANNEL_H
