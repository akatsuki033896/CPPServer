#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include "Socket.hpp"
#include "Connection.hpp"
#include "include/EventLoop.hpp"
#include "include/Server.hpp"
// Cmake找得到 "EventLoop.cpp" Clangd只能找到 "include/EventLoop.cpp"
int main() {
    EventLoop *loop = new EventLoop();
    Server *server = new Server(loop);
    server->onConnect([] (Connection *conn) {
        conn->Read();
        if (conn->getState() == Connection::State::Closed) {
            conn->Close();
             return;
        }
        std::cout << "Message from client: " << conn->getSocket()->get_fd() << ":" << conn->readBuffer() << '\n';
        conn->setSendBuffer(conn->readBuffer());
        conn->Write();
    });
    loop->loop(); // 启动事件循环
    delete server;
    delete loop;
    return 0;
}