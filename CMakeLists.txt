cmake_minimum_required(VERSION 3.3)
project(cpp11_simple_concurrent_queue)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp concurrent_queue.h concurrent_cout.h)
add_executable(cpp11_simple_concurrent_queue ${SOURCE_FILES})