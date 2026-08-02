cmake_minimum_required(VERSION 4.3)
project(HIS_cpp)

set(CMAKE_CXX_STANDARD 20)

add_executable(HIS_cpp main.cpp
        his_config.h
        his.h
        his_link.cpp
        his_tool.cpp
        his_tool.h)
