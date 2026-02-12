cmake_minimum_required(VERSION 3.10)
PROJECT(CPP20Server)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(include)

add_executable(server server.cpp)
add_executable(client client.cpp)