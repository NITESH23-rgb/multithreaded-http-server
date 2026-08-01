#include "Server.h"

cmake_minimum_required(VERSION 3.10)

project(HttpServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

target_include_directories(server PRIVATE include)

add_executable(server
    src/main.cpp
    src/Server.cpp
    src/HttpResponse.cpp
)

find_package(Threads REQUIRED)
target_link_libraries(server PRIVATE Threads::Threads)