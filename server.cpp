#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include "TCPConnection.hpp"
#include "Server.hpp"
#include "Buffer.hpp"

// Cmake找得到 "EventLoop.cpp" Clangd只能找到 "include/EventLoop.cpp"
int main() {
    Server *server = new Server("127.0.0.1", 8080);
    server->setMessageCallBack([] (TCPConnection *conn) {
        std::cout << "Message from client: " << conn->id() << ":" << conn->read_buff() << '\n';
        conn->Send(conn->read_buff()->c_str()); // 回显
    });
    server->start();
    delete server;
    return 0;
}