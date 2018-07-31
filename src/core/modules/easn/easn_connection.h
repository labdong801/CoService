//
// Created by lml on 2018/7/10.
//

#ifndef PROJECT_WF_CONNECTION_H
#define PROJECT_WF_CONNECTION_H


#include <cstdint>

namespace easn{
    //APPID大小
    const int APPID_SIZE = 32;

    //保存连接的结构体
    typedef struct {
        uint64_t sid;
        int socket_fd;
        int auth;
        char appid[APPID_SIZE];
    } easn_conn_t;
}

#endif //PROJECT_WF_CONNECTION_H
