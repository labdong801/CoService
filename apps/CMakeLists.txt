cmake_minimum_required(VERSION 3.10)
project(apps)

set(CMAKE_CXX_STANDARD 11)

include_directories(../src/include)

set(CMAKE_C_FLAGS -fPIC)

add_executable(easn main.cpp)

target_link_libraries( easn wf )