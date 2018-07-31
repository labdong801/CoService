cmake_minimum_required(VERSION 3.10)
project(wf)

set(CMAKE_CXX_STANDARD 11)

#头文件
include_directories(./include)

include_directories(./include
        ../third/libco/
        ${Boost_INCLUDE_DIRS})
link_directories(../third/libco/
        ${Boost_LIBRARY_DIRS})

# boost
set(BOOST_INCLUDEDIR /usr/local/include/)
set(BOOST_LIBRARYDIR /usr/local/lib/)
find_package(Boost 1.67.0 COMPONENTS system thread REQUIRED)

message("Binary Tree path: ${PROJECT_BINARY_DIR}")

add_library(wf STATIC
        core/modules/easn/easn_server_channel.cpp
        core/modules/easn/easn_server_channel.h
        event/wf_co_event.cpp
        event/wf_co_event.h
        core/channel.h
        core/wf_log.cpp
        core/wf_log.h
        core/modules/easn/easn_dispatch_channel.cpp
        core/modules/easn/easn_dispatch_channel.h
        core/threadpool.cpp
        core/threadpool.h
        core/modules/easn/easn_package.h
        core/modules/easn/easn_package.cpp
        core/crc16.cpp
        core/crc16.h
        core/wf_kafka.cpp
        core/wf_kafka.h
        core/config.h
        core/modules/easn/easn_kafka_dispatcher.cpp
        core/modules/easn/easn_kafka_dispatcher.h
        core/modules/easn/easn_dispatcher_manager.cpp
        core/modules/easn/easn_dispatcher_manager.h
        core/json/wf_queue_item.cpp
        core/json/wf_queue_item.h
        core/modules/easn/easn_connection.cpp
        core/modules/easn/easn_connection.h
        core/modules/easn/easn_client_manager.cpp
        core/modules/easn/easn_client_manager.h
        core/wf_time.cpp
        core/wf_time.h
        core/modules/easn/easn.cpp
        core/modules/easn/easn.h wf_framework.cpp include/wf_framework.h)


target_link_libraries( wf
        ${Boost_LIBRARIES}
        colib_static        #腾讯协程库
        pthread             #多线程
        dl                  #libco需要的依赖
#        rdkafka++           #kafka库
        rdkafka.a
        ssl.a
        crypto.a
        stdc++.a
        z
        )