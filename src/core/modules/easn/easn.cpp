//
// Created by lml on 2018/7/10.
//

#include <iomanip>
#include <sstream>
#include "easn.h"
#include "../../../event/wf_co_event.h"
#include "easn_server_channel.h"
#include "easn_dispatch_channel.h"
#include "../../config.h"
#include "easn_kafka_dispatcher.h"
#include "easn_dispatcher_manager.h"
#include "../../wf_log.h"

using namespace easn;
using namespace wf;

void easn::create_server(int port , std::string pid ,std::string brokers) {

    //kafka topic 名称
    std::string kafka_send_topic_name_str = "topic-send-" + pid;         //发送topic
    std::string kafka_receive_topic_name_str = "topic-receive-" + pid;   //接收topic

    //创建发送队列
    auto producer = std::make_shared<kafka_dispatcher>(kafka_send_topic_name_str,brokers);
    easn::dispatcher_manager::instance()->add_dispatcher(producer);

    //监听队列
    std::shared_ptr<wf::channel> kafka_channel = std::make_shared<easn::dispatch_channel>(kafka_receive_topic_name_str,brokers);
    wf::wf_co_event::instance()->add_server_channel(kafka_channel);

    //服务队列
    std::shared_ptr<wf::channel> channel = std::make_shared<easn::server_channel>(port);
    wf::wf_co_event::instance()->add_server_channel(channel);

}

void easn::dispatch_message(uint64_t sid, uint8_t type, uint8_t *data, size_t size) {

    wf_info << "send_kafka_message:" << std::setw(size) << (char *)data << std::endl;
    std::stringstream sid_ss;
    sid_ss << sid;
    easn::dispatcher_manager::instance()->get_dispatcher()->produce(sid_ss.str(),type_to_string(type),data,size);
}



