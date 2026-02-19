#include "Server.hpp"
#include "Connection.hpp"
#include "EventLoop.hpp"
#include "Socket.hpp"
#include "Channel.hpp"
#include "Acceptor.hpp"
#include "ThreadPool.hpp"
#include <cassert>
#include <cstdint>
#include <functional>
#include <iostream>
#include <thread>

Server::Server(EventLoop *loop) : main_reactor_(loop) {
    std::cout << "Server Start!" << '\n';
    acceptor_ = new Acceptor( main_reactor_); //Acceptor由且只由mainReactor负责
    // 创建一个函数对象，被调用时执行 this->newConnection(sock);
    acceptor_->setNewConnectionCallBack(
        [this] (Socket *sock) {
            this->newConnection(sock);
        }
    );
    
    int size = static_cast<int>(std::thread::hardware_concurrency()); // 获取线程数量即 subreactor 数量
    thread_pool_ = new ThreadPool(size);
    for (int i = 0; i < size; i++) {
        sub_reactors_.push_back(new EventLoop()); // 每个线程是一个 Eventloop
    }
    for (int i = 0; i < size; i++) {
        // std::function<void()> sub_loop = std::bind(&EventLoop::loop, subReactors[i]);
        std::function<void()> sub_loop = [loop = sub_reactors_[i]] () {
            loop->loop();
        };
        thread_pool_->add(std::move(sub_loop)); // 开启所有线程的循环
    }
}

Server::~Server() {
    delete acceptor_;
    delete thread_pool_;
}

void Server::newConnection(Socket *sock) {
    assert (sock->get_fd() != -1);
    uint64_t random = sock->get_fd() % sub_reactors_.size(); // 全随机调度，每个线程均匀负载
    Connection *conn = new Connection(main_reactor_, sock);
    conn->setDeleteConnectionCallBack(
        [this] (Socket* sock) {
            this->deleteConnection(sock);
        }
    );
    conn->setOnConnectionCallBack(on_connect_callback_);
    connections_[sock->get_fd()] = conn; // 放入TCP连接的映射
}

void Server::deleteConnection(Socket* sock) {
    int sockfd = sock->get_fd();
    if (sockfd != -1) {
        auto it = connections_.find(sock->get_fd());
        if (it != connections_.end()) {
            Connection *conn = connections_[sock->get_fd()]; // 取出
            connections_.erase(sockfd);
            delete conn; // Segment fault
            conn = nullptr;
        }
    }
}

void Server::onConnect(std::function<void(Connection*)> fn) {
    on_connect_callback_ = std::move(fn);
}