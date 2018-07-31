cmake_minimum_required(VERSION 3.10)
project(examples)

set(CMAKE_CXX_STANDARD 11)

include_directories(../src/include)

# 测试uv库
add_executable(uvtest uvtest.cpp)
target_link_libraries( uvtest uv)

# 测试grpc
link_directories(/usr/local/lib/)
add_executable(grpc grpc.cpp
        helloworld.pb.cc
        helloworld.pb.h
        helloworld.grpc.pb.cc
        helloworld.grpc.pb.h)

target_link_libraries( grpc protobuf grpc++_unsecure)

#测试kafka
add_executable(kafka kafka.cpp kafka_util.cpp kafka_util.h)
target_link_libraries(kafka rdkafka++)

#测试redis
add_executable(redis redis.cpp)
target_link_libraries(redis redis3m)

#测试阻塞socket
add_executable(bsocket bsocket.cpp)

#测试co
include_directories(../third/libco)
link_directories(../third/libco)
add_executable(cotest co.cpp)
target_link_libraries(cotest colib_static dl pthread)