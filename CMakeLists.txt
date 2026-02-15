cmake_minimum_required(VERSION 3.10)
PROJECT(CPP20Server)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(core src/Socket.cpp src/util.cpp src/Epoll.cpp)
target_include_directories(core PUBLIC include)

add_executable(server server.cpp)
target_link_libraries(server core)

add_executable(client client.cpp)
target_link_libraries(client core)