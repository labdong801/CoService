//
// Created by lml on 2018/7/10.
//

#ifndef PROJECT_WF_CLIENT_MANAGER_H
#define PROJECT_WF_CLIENT_MANAGER_H

#include <memory>
#include <map>
#include "easn_connection.h"

namespace easn{

    typedef std::map<uint64_t ,easn_conn_t *> conn_map;

    class client_manager {
    public:
        //单例模式
        static std::shared_ptr<client_manager> & instance();
        ~client_manager();

        //创建一个新连接
        easn_conn_t * new_connection(int socket_fd);

        //删除一个连接
        void remove_connection(uint64_t sid);

        //根据sid获取对应的connection
        easn_conn_t * get_connection(uint64_t sid);


    private:
        //单例模式
        static std::shared_ptr<client_manager> singleton;
        client_manager();

        conn_map * connections;
    };
}



#endif //PROJECT_WF_CLIENT_MANAGER_H
