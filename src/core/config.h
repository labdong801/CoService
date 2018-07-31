//
// Created by lml on 2018/7/9.
// easn默认配置
//

#ifndef PROJECT_CONFIG_H
#define PROJECT_CONFIG_H

#include <iostream>
#include <memory>

namespace easn{
    class config{

    private:
        config() = default;

        //禁止拷贝
        config(const config &);
        const config& operator=( const config& );


    public:

        ~config() = default;

        //单例模式
        static std::shared_ptr<config> instance(){
            return std::shared_ptr<config>(new config());
        }

        //kafka服务器地址
        std::string default_kafka_brokers(){
            return "127.0.0.1:9092";
        }

        //kafka分组
        std::string default_kafka_group(){
            return "wf";
        }


    };
}

#endif //PROJECT_CONFIG_H
