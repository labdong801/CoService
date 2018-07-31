//
// Created by lml on 2018/7/9.
//

#ifndef PROJECT_WF_KAFKA_PRODUCER_H
#define PROJECT_WF_KAFKA_PRODUCER_H

#include "../../wf_kafka.h"
#include "../../wf_log.h"

using namespace wf;
namespace easn{
    class kafka_dispatcher :virtual public wf_kafka{
    public:
        kafka_dispatcher(const std::string &topic,const std::string &broker);
        ~kafka_dispatcher();

        //将消息放进队列中
        void produce(std::string sid,std::string type,unsigned char * buf , int size);


    private:
        //topics
        rd_kafka_topic_t *rkt;

        static inline rd_kafka_resp_err_t do_produce(rd_kafka_t *rk,
                                              rd_kafka_topic_t *rkt,
                                              rd_kafka_headers_t *hdrs,
                                              int32_t partition,
                                              int msgflags,
                                              void *payload, size_t size,
                                              const void *key, size_t key_size){

            if (hdrs) {
//                rd_kafka_headers_t *hdrs_copy;
//                hdrs_copy = rd_kafka_headers_copy(hdrs);


                rd_kafka_resp_err_t err = rd_kafka_producev(
                        rk,
                        RD_KAFKA_V_RKT(rkt),
                        RD_KAFKA_V_PARTITION(RD_KAFKA_PARTITION_UA),
                        RD_KAFKA_V_MSGFLAGS(RD_KAFKA_MSG_F_COPY),
                        RD_KAFKA_V_VALUE(payload, size),
                        RD_KAFKA_V_HEADERS(hdrs),//RD_KAFKA_V_HEADERS(hdrs_copy),
                        RD_KAFKA_V_END);

                if (err){
                    rd_kafka_headers_destroy(hdrs);
                }
            }else{
                wf_error << "kafka header 不能为空" << std::endl;
            }


        }
    };
}



#endif //PROJECT_WF_KAFKA_PRODUCER_H
