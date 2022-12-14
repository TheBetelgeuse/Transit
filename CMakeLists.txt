cmake_minimum_required(VERSION 3.20.2)
project(Transit)

set(CMAKE_CXX_STANDARD 20)

add_executable(Transit
        initialization.cpp
        traffic_controller.cpp SystemFile.cpp sem_and_queue.cpp truck.cpp for_society.cpp for_society.hpp)
