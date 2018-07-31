//
// Created by lml on 2018/7/2.
//

#ifndef PROJECT_KAFKA_CHANNEL_H
#define PROJECT_KAFKA_CHANNEL_H

#include <iostream>
#include <memory>
#include "../../channel.h"
#include "../../wf_kafka.h"
#include <librdkafka/rdkafka.h>
using namespace wf;

namespace easn{
    class dispatch_channel :virtual public channel,virtual public wf_kafka{
    public:

        //构造 && 析构
        dispatch_channel(std::string topic,std::string broker);
        virtual ~dispatch_channel();

        virtual void * get_handle();

        //初始化
        virtual void init();
        //启动服务
        virtual void start();

        //打印调试信息
        static void debug();

    private:
        //管道fd
        int fds[2];
        std::string topic;

        rd_kafka_queue_t *queue_t;
        rd_kafka_topic_partition_list_t *topics_t;

//
        //消费一个消息
        void consume();

        //处理一个消息
        void handle_message(rd_kafka_message_t *rkmessage,
                            void *opaque);

        //kafka协程
        static void * kafka_routine( void * arg );
    };
}

#endif //PROJECT_KAFKA_CHANNEL_H
