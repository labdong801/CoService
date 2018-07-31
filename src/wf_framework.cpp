//
// Created by lml on 2018/7/11.
//

#include "wf_framework.h"
#include "event/wf_co_event.h"
#include "core/modules/easn/easn.h"

using namespace wf;

void wf::create_easn_server() {
    easn::create_server(40625,"mypid","10.10.2.22:9092");
}

int wf::event_loop(){
    return wf::wf_co_event::instance()->wf_co_event_loop();
}

