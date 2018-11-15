cmake_minimum_required(VERSION 2.8.4)
project(UDP_rdt)

set(CMAKE_CXX_STANDARD 11)

add_executable(receiever server_udp.cpp classes.cpp)
add_executable(sender client_udp.cpp classes.cpp)