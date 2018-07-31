#include <iostream>
#include <wf_framework.h>


int main() {
    //省略读配置部分代码

    //创建一个sasn服务
    wf::create_easn_server();

    return wf::event_loop();
}