cmake_minimum_required(VERSION 3.23)
project(Transit)

set(CMAKE_CXX_STANDARD 20)

add_executable(Transit
        initialization.cpp
        truck.cpp traffic_controller.cpp SystemFile.cpp sem_and_queue.cpp)
