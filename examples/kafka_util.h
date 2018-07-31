//
// Created by lml on 2018/6/27.
//

#ifndef PROJECT_KAFKA_UTIL_H
#define PROJECT_KAFKA_UTIL_H

#include <iostream>
#include <librdkafka/rdkafkacpp.h>
#include <atomic>

class kafka_util {

public:
    kafka_util(const std::string& brokers, const std::string& topics, std::string groupid, int64_t offset)
            :brokers_(brokers),
            topics_(topics),
            groupid_(groupid),
            offset_(offset),
             running(false){

    };
    ~kafka_util(){};

    bool init_client();
    bool consume(int timeout_ms);
    void finalize();

private:
    void consumer(RdKafka::Message *msg, void *opt);

    std::atomic<bool> running;

    //初始化时传入
    std::string brokers_;
    std::string topics_;
    std::string groupid_;

    int64_t last_offset_ = 0;
    RdKafka::Consumer *kafka_consumer_ = nullptr;
    RdKafka::Topic    *topic_          = nullptr;
    int64_t           offset_          = RdKafka::Topic::OFFSET_BEGINNING;
    int32_t           partition_       = 0;
};


#endif //PROJECT_KAFKA_UTIL_H
