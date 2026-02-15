#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include "include/EventLoop.hpp"
#include "include/Server.hpp"
// Cmake找得到 "EventLoop.cpp" Clangd只能找到 "include/EventLoop.cpp"
int main() {
    EventLoop *loop = new EventLoop();
    Server *server = new Server(loop);
    loop->loop(); // 启动事件循环
    return 0;
}