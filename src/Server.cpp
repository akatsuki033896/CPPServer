#include "Server.hpp"
#include "Connection.hpp"
#include "EventLoop.hpp"
#include "Socket.hpp"
#include "Channel.hpp"
#include "Acceptor.hpp"
#include <functional>
#include <iostream>

Server::Server(EventLoop *_loop) : loop(_loop) {
    std::cout << "Server Start!" << '\n';
    acceptor = new Acceptor(loop);
    // std::function<void(Socket*)> cb = std::bind(&Server::newConnection, this, std::placeholders::_1);
    // std::placeholders::_1 占位，未来调用这个回调函数时，第一个参数会被传给 newConnection
    
    // 创建一个函数对象，被调用时执行 this->newConnection(sock);
    acceptor->setNewConnectionCallBack(
        [this] (Socket *sock) {
            this->newConnection(sock);
        }
    );
}

Server::~Server() {
    delete acceptor;
}

void Server::newConnection(Socket *sock) {
    Connection *conn = new Connection(loop, sock);
    // std::function<void(Socket*)> cb = std::bind(&Server::deleteConnection, this, std::placeholders::_1);
    // conn->setDeleteConnectionCallBack(cb);
    conn->setDeleteConnectionCallBack(
        [this] (Socket* sock) {
            this->deleteConnection(sock);
        }
    );
    connections[sock->get_fd()] = conn; // 放入TCP连接的映射
}

void Server::deleteConnection(Socket* sock) {
    // TODO: change to unique_ptr
    Connection *conn = connections[sock->get_fd()]; // 取出
    connections.erase(sock->get_fd());
    delete conn;
}