//
// Created by lml on 2018/7/10.
//

#include <cstring>
#include <algorithm>
#include "easn_client_manager.h"
#include "../../wf_log.h"
#include "../../wf_time.h"
#include "easn_connection.h"

using namespace easn;
using namespace wf;

//单例模式
std::shared_ptr<client_manager> client_manager::singleton = nullptr;
std::shared_ptr<client_manager> & client_manager::instance() {
    if (singleton == nullptr)
        singleton = std::shared_ptr<client_manager>(new client_manager());
    return singleton;
}


client_manager::client_manager():connections(new conn_map()) {

}

client_manager::~client_manager() {
    //释放内存
    for(auto iter = connections->begin();iter != connections->end();){
        free(iter->second);
        connections->erase(iter);
    }
    delete(connections);
}

easn_conn_t * client_manager::new_connection(int socket_fd) {
    uint64_t sid = wf::time::timestamp(); //获取当前时间戳
    easn_conn_t * conn = (easn_conn_t *)malloc(sizeof(easn_conn_t));
    memset(conn,0, sizeof(easn_conn_t));
    conn->sid = sid;
    conn->socket_fd = socket_fd;
    connections->insert(std::pair<uint64_t,easn_conn_t*>(sid,conn));
    return conn;
}

void client_manager::remove_connection(uint64_t sid) {
    auto p = get_connection(sid);
    if(p != nullptr){
        wf_info << "清除会话信息:" << sid << std::endl;
        free(p);
        connections->erase(sid);
    }
}

easn_conn_t * client_manager::get_connection(uint64_t sid) {
    conn_map::iterator it;
    it = connections->find(sid);
    if(it == connections->end() ){
        wf_error << "没有找到sid:" << sid << std::endl;
        return nullptr;
    }
    return it->second;
}