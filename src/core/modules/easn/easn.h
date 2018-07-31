//
// Created by lml on 2018/7/10.
//

#ifndef PROJECT_EASN_H
#define PROJECT_EASN_H

#include <iostream>

namespace easn {


    void create_server(int port,std::string pid,std::string brokers);
    void dispatch_message(uint64_t sid,uint8_t type,uint8_t* data,size_t size);

//    void create_send_mq(std::string name);
//    void create_receive_mq(std::string name);
};


#endif //PROJECT_EASN_H
