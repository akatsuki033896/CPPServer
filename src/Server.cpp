#include "Server.hpp"
#include "Connection.hpp"
#include "EventLoop.hpp"
#include "Socket.hpp"
#include "Channel.hpp"
#include "Acceptor.hpp"
#include "ThreadPool.hpp"
#include <functional>
#include <iostream>
#include <thread>

Server::Server(EventLoop *_loop) : mainReactor(_loop) {
    std::cout << "Server Start!" << '\n';
    acceptor = new Acceptor( mainReactor); //Acceptor由且只由mainReactor负责
    // 创建一个函数对象，被调用时执行 this->newConnection(sock);
    acceptor->setNewConnectionCallBack(
        [this] (Socket *sock) {
            this->newConnection(sock);
        }
    );
    
    int size = std::thread::hardware_concurrency(); // 获取线程数量即 subreactor 数量
    thpool = new ThreadPool(size);
    for (int i = 0; i < size; i++) {
        subReactors.push_back(new EventLoop()); // 每个线程是一个 Eventloop
    }
    for (int i = 0; i < size; i++) {
        // std::function<void()> sub_loop = std::bind(&EventLoop::loop, subReactors[i]);
        std::function<void()> sub_loop = [loop = subReactors[i]] () {
            loop->loop();
        };
        thpool->add(sub_loop); // 开启所有线程的循环
    }
}

Server::~Server() {
    delete acceptor;
    delete thpool;
}

void Server::newConnection(Socket *sock) {
    if (sock->get_fd() != -1) {
        int random = sock->get_fd() % subReactors.size(); // 全随机调度，每个线程均匀负载
        Connection *conn = new Connection(mainReactor, sock);
        conn->setDeleteConnectionCallBack(
            [this] (Socket* sock) {
                this->deleteConnection(sock);
            }
        );
        connections[sock->get_fd()] = conn; // 放入TCP连接的映射
    }
}

void Server::deleteConnection(Socket* sock) {
    if (sock->get_fd() != -1) {
        auto it = connections.find(sock->get_fd());
        if (it != connections.end()) {
            Connection *conn = connections[sock->get_fd()]; // 取出
            connections.erase(sock->get_fd());
            delete conn; // Segment fault
        }
    }
}