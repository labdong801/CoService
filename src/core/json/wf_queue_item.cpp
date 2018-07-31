//
// Created by lml on 2018/7/10.
//

#include "wf_queue_item.h"

using namespace wf;

bool wf::queue_item::operator==(const wf::queue_item &rhs) const {
    return sid == rhs.sid &&
           data == rhs.data;
}

bool wf::queue_item::operator!=(const wf::queue_item &rhs) const {
    return !(rhs == *this);
}

std::string queue_item::to_json_string() {

}
