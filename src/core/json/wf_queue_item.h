//
// Created by lml on 2018/7/10.
//

#ifndef PROJECT_WF_QUEUE_ITEM_H
#define PROJECT_WF_QUEUE_ITEM_H

#include "nlohmann/json.hpp"
#include <cstdint>

namespace wf{
    class queue_item {
    public:
        explicit queue_item() = default;
        ~queue_item() = default;
        bool operator==(const queue_item &rhs) const;
        bool operator!=(const queue_item &rhs) const;

        std::string to_json_string();

    private:
        uint32_t sid;
        uint8_t  *data;
    };
}



#endif //PROJECT_WF_QUEUE_ITEM_H
