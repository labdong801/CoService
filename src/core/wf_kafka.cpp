//
// Created by lml on 2018/7/9.
//

#include <cstring>
#include "wf_kafka.h"
#include "config.h"
#include <signal.h>

using namespace wf;

wf_kafka::wf_kafka(rd_kafka_type_t type,std::string broker):brokers(broker) {

    rd_kafka_conf_t *conf;
    rd_kafka_topic_conf_t *topic_conf;
    rd_kafka_resp_err_t err;
    char tmp[16];
    char errstr[512];

    /* Kafka configuration */
    conf = rd_kafka_conf_new();

    rd_kafka_conf_set_events(conf, RD_KAFKA_EVENT_DR);

//    //quick termination
    snprintf(tmp, sizeof(tmp), "%i", SIGIO);
    rd_kafka_conf_set(conf, "internal.termination.signal", tmp, NULL, 0);

    //topic configuration
    topic_conf = rd_kafka_topic_conf_new();

    /* Consumer groups require a group id */
    if (rd_kafka_conf_set(conf, "group.id", this->server_group().c_str(),
                          errstr, sizeof(errstr)) !=
        RD_KAFKA_CONF_OK) {
        fprintf(stderr, "%% %s\n", errstr);
        exit(-1);
    }

    /* Consumer groups always use broker based offset storage */
    if (rd_kafka_topic_conf_set(topic_conf, "offset.store.method",
                                "broker",
                                errstr, sizeof(errstr)) !=
        RD_KAFKA_CONF_OK) {
        fprintf(stderr, "%% %s\n", errstr);
        exit(-1);
    }

    /* Set default topic config for pattern-matched topics. */
    rd_kafka_conf_set_default_topic_conf(conf, topic_conf);

    //实例化一个顶级对象rd_kafka_t作为基础容器，提供全局配置和共享状态
    rk = rd_kafka_new(type, conf, errstr, sizeof(errstr));
    if(!rk){
        fprintf(stderr, "%% Failed to create new consumer:%s\n", errstr);
        exit(-1);
    }

    //Librdkafka需要至少一个brokers的初始化list
    if (rd_kafka_brokers_add(rk, server_brokers().c_str()) == 0){
        fprintf(stderr, "%% No valid brokers specified\n");
        exit(-1);
    }


}

//服务器IP
std::string wf_kafka::server_brokers() {
    return brokers;//easn::config::instance()->default_kafka_brokers();
}

//分组
std::string wf_kafka::server_group() {
    return easn::config::instance()->default_kafka_group();
}

wf_kafka::~wf_kafka() {

    /* Destroy handle */
    rd_kafka_destroy(rk);
}