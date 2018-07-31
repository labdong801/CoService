//
// Created by lml on 2018/7/9.
//

#ifndef PROJECT_WF_KAFKA_H
#define PROJECT_WF_KAFKA_H


#include <librdkafka/rdkafka.h>
#include <iostream>

namespace wf{
    class wf_kafka {

    public:
        wf_kafka(rd_kafka_type_t type, std::string broker);

        virtual ~wf_kafka();

    protected:
        rd_kafka_t * rk;

        std::string brokers;

        //kafka 服务器IP
        std::string server_brokers();
        //kafka 分组
        std::string server_group();
    };

}


#endif //PROJECT_WF_KAFKA_H
