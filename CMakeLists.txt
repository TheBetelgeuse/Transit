cmake_minimum_required(VERSION 3.23)
project(Transit)

set(CMAKE_CXX_STANDARD 20)

add_executable(Transit
        initialization.cpp initialization.hpp
        truck.cpp traffic_controller.cpp traffic_controller.hpp SystemFile.hpp SystemFile.cpp)
