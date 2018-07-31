//
// Created by lml on 2018/6/27.
//

#include <iostream>
#include <librdkafka/rdkafkacpp.h>
#include <memory>
#include <unistd.h>
#include "kafka_util.h"

int main() {
    const char* test_data = "helloworld";

    std::string topics = "test";
    std::string brokers = "10.10.2.22:9092";
    std::string group = "1";

    auto p  = std::make_shared<kafka_util>(brokers, topics, group, RdKafka::Topic::OFFSET_END);
    if (!p->init_client()){
        std::cerr << "kafka server initialize error" << std::endl;
        sleep(10);
    }else{
        std::cout << "start kafka consumer" << std::endl;
        p->consume(1000);
    }

    std::cout << "kafka consume exit!" << std::endl;

    return 0;
}