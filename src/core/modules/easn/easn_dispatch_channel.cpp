//
// Created by lml on 2018/7/2.
//

#include <bits/signum.h>
#include <unistd.h>
#include <cstring>
#include <iomanip>
#include "easn_dispatch_channel.h"
#include "../../wf_log.h"
#include "../../config.h"
#include "easn_dispatcher_manager.h"

using namespace wf;
using namespace easn;

dispatch_channel::dispatch_channel(std::string topic,std::string broker) :topic(topic), wf_kafka(RD_KAFKA_CONSUMER,broker) {
    rd_kafka_resp_err_t err;

    //重定向 rd_kafka_poll()队列到consumer_poll()队列
    rd_kafka_poll_set_consumer(rk);

    //创建一个Topic+Partition的存储空间(list/vector)
    topics_t = rd_kafka_topic_partition_list_new(1);
    //把Topic+Partition加入list
    rd_kafka_topic_partition_list_add(topics_t, topic.c_str(), RD_KAFKA_PARTITION_UA );
    //开启consumer订阅，匹配的topic将被添加到订阅列表中
    if((err = rd_kafka_subscribe(rk, topics_t))){
        fprintf(stderr, "%% Failed to start consuming topics: %s\n", rd_kafka_err2str(err));
        exit(-1);
    }

    queue_t = rd_kafka_queue_get_consumer(rk);

    if (pipe(fds) == -1){
        wf_error << "pipe() failed: " << strerror(errno) << std::endl;
        exit(-1);
    }

    rd_kafka_queue_io_event_enable(queue_t, fds[1], ".", 1);


}

dispatch_channel::~dispatch_channel() {

    rd_kafka_resp_err_t err = rd_kafka_unsubscribe(rk);
    if(err){
        fprintf(stderr, "%% Failed to unsubscribe: %s\n", rd_kafka_err2str(err));
    }else{
        wf_info << "取消订阅Topic:" << topic << std::endl;
    }

    err = rd_kafka_consumer_close(rk);
    if(err){
        fprintf(stderr, "%% Failed to close consumer: %s\n", rd_kafka_err2str(err));
    }else{
        wf_info << "关闭KAFKA监听" << std::endl;
    }


    //释放topics list使用的所有资源和它自己
    rd_kafka_topic_partition_list_destroy(topics_t);


    if(fds[0])
        close(fds[0]);

    if(fds[1])
        close(fds[1]);
}

void* dispatch_channel::get_handle() {

}

//开始服务
void dispatch_channel::start() {
    co_create(&routine, nullptr, kafka_routine, this);
    co_resume(routine);

}

//初始化
void dispatch_channel::init() {
    wf_info << "监听KAFKA Topice:" << topic <<std::endl;
}

//消费一个消息
void dispatch_channel::consume() {

    struct pollfd pf = { 0 };
    pf.fd = fds[0];
    pf.events = (POLLIN|POLLERR|POLLHUP);
    co_poll( co_get_epoll_ct(),&pf,1,-1/* -1:永不超时 */);  //此时协程已经让出去了

    //消耗掉事件(没有意义)
    char b;
    int r = read(pf.fd, &b, 1);

    //读数据
    rd_kafka_message_t *rkmessage ;
    for(;;){
        rkmessage = rd_kafka_consume_queue(queue_t,0 /*不阻塞*/);
        if(rkmessage){
            handle_message(rkmessage, NULL);
            /*释放rkmessage的资源，并把所有权还给rdkafka*/
            rd_kafka_message_destroy(rkmessage);
        }else{
            //没有数据了
            break;
        }
    }

}

void dispatch_channel::handle_message(rd_kafka_message_t *rkmessage,void *opaque) {
    if (rkmessage->err) {
        if (rkmessage->err == RD_KAFKA_RESP_ERR__PARTITION_EOF) {
            return;
        }

        if (rkmessage->rkt)
            wf_error << "Kafka Consume error:"
                        << " topic:" << rd_kafka_topic_name(rkmessage->rkt)
                                    << " offset:" << rkmessage->offset
                                                 << " partition:" << rkmessage->partition
                                                                  << " msg:" << rd_kafka_message_errstr(rkmessage) << std::endl;
        else
            wf_error << "Kafka Consume error:"
                        << rd_kafka_err2str(rkmessage->err) << ":" << rd_kafka_message_errstr(rkmessage) << std::endl;

        if (rkmessage->err == RD_KAFKA_RESP_ERR__UNKNOWN_PARTITION ||
            rkmessage->err == RD_KAFKA_RESP_ERR__UNKNOWN_TOPIC)
//            run = 0;
        return;
    }

//    if (rkmessage->key_len) {
//        printf("Key: %.*s\n",
//               (int)rkmessage->key_len, (char *)rkmessage->key);
//    }

    rd_kafka_headers_t *hdrs;
    if (!rd_kafka_message_headers(rkmessage, &hdrs)) {
        const void *val;
        size_t size;
        rd_kafka_header_get(hdrs,0,"sid",&val,&size);
        wf_info << "KAFKA [" << std::setw(size) << (char *)val <<"] << " << std::setw((int)rkmessage->len) << (char *)rkmessage->payload << std::endl;
    }else{
        wf_info << "KAFKA << " << std::setw((int)rkmessage->len) << (char *)rkmessage->payload << std::endl;
    }


}

//kafka协程
void * dispatch_channel::kafka_routine(void *arg) {

//    co_enable_hook_sys();

    dispatch_channel * channel = (dispatch_channel *)arg;
//    RdKafka::Message *msg = nullptr;
    for(;;){
        channel->consume();
    }
}

void dispatch_channel::debug() {
//    wf_info << "KAFKA BROKERS:" << KAFKA_BROKERS << std::endl;
//    auto producer = wf::kafka_manager::instance()->get_producer();
//    std::string sid = "测试sid";
//    producer->produce(sid,(unsigned char *) "helloworld", sizeof("helloworld"));
}

