cmake_minimum_required(VERSION 3.10)
project(examples)

set(CMAKE_CXX_STANDARD 11)

include_directories(../src/include)

# 测试ensn传输协议
add_executable(test_protocol test_protocol.cpp test_util.cpp test_util.h)
target_link_libraries( test_protocol wf)

# 测试crc校验
add_executable(test_crc test_crc.cpp test_util.cpp test_util.h)
target_link_libraries( test_crc wf)

# 测试数据包分割
add_executable(test_packet_split test_packet_split.cpp test_util.cpp test_util.h)
target_link_libraries( test_packet_split wf)

