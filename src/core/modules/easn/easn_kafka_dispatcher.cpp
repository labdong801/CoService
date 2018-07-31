//
// Created by lml on 2018/7/9.
//

#include <iomanip>
#include "easn_kafka_dispatcher.h"
#include "../../wf_log.h"

using namespace easn;

kafka_dispatcher::kafka_dispatcher(const std::string &topic,const std::string & broker) : wf_kafka(RD_KAFKA_PRODUCER,broker), rkt(nullptr){
    rkt = rd_kafka_topic_new(rk, topic.c_str(), NULL);
    if (!rkt) {
        wf_error << "创建topic失败:" <<
                rd_kafka_err2str(rd_kafka_last_error()) << std::endl;
        exit(-1);
    }

    wf_info << "创建KAFKA Topic:" << topic << std::endl;
}

kafka_dispatcher::~kafka_dispatcher() {

//    wf_info << "easn_kafka_dispatchercher()" << std::endl;

    rd_kafka_flush(rk, 10*1000 /* wait for max 10 seconds */);
    /* Destroy topic object */
    rd_kafka_topic_destroy(rkt);
}

//将一个消息放进kafka
void kafka_dispatcher::produce(std::string sid,std::string type,unsigned char *buf, int size) {
    //header
    rd_kafka_headers_t * hdrs = rd_kafka_headers_new(2);
    rd_kafka_resp_err_t err = rd_kafka_header_add(hdrs, "sid", -1, sid.c_str(), -1);
    if (err) {
        wf_error << "kafka消息头部创建失败,取消发送:" << rd_kafka_err2str(err) <<  std::endl;
        return;
    }
    err = rd_kafka_header_add(hdrs, "type", -1, type.c_str(), -1);
    if (err) {
        wf_error << "kafka消息头部创建失败,取消发送:" << rd_kafka_err2str(err) <<  std::endl;
        return;
    }

    wf_info << "KAFKA <"+type+">[" + sid + "] >> " << std::setw(size) << buf << std::endl;

    do_produce(rk,rkt,hdrs,RD_KAFKA_PARTITION_UA,
               RD_KAFKA_MSG_F_COPY,
               buf,size,
               NULL,0);
}



