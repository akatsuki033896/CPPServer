#include "Server.hpp"
#include "TCPConnection.hpp"
#include "EventLoop.hpp"
#include "Channel.hpp"
#include "Acceptor.hpp"
#include "ThreadPool.hpp"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <thread>

Server::Server(const char* ip, const int port) {
    std::cout << "Server Start!" << '\n';
    main_reactor_ = std::make_unique<EventLoop>();
    acceptor_ = std::make_unique<Acceptor>(main_reactor_.get(), ip, port); //Acceptor由且只由mainReactor负责
    // 创建一个函数对象，被调用时执行 this->newConnection(sock);
    acceptor_->setNewConnectionCallBack(
        [this] (int sockfd) {
            this->handleNewConnection(sockfd);
        }
    );
    
    size_t size = static_cast<int>(std::thread::hardware_concurrency()); // 获取线程数量即 subreactor 数量
    thread_pool_ = std::make_unique<ThreadPool>(size);

    for (size_t i = 0; i < size; i++) {
        std::unique_ptr<EventLoop> sub_reactor = std::make_unique<EventLoop>(); // 每个线程是一个 Eventloop
        sub_reactors_.push_back(std::move(sub_reactor));
    }
}

Server::~Server() {}

void Server::handleNewConnection(int sockfd) {
    assert (sockfd != -1);
    uint64_t random = sockfd % sub_reactors_.size(); // 全随机调度，每个线程均匀负载
    TCPConnection *conn = new TCPConnection(sub_reactors_[random].get(), sockfd, next_conn_id_); // 每个连接分配一个 subReactor 处理

    conn->set_close_callback(
        [this] (int sockfd) {
            this->handleClose(sockfd);
        }
    );
    conn->set_message_callback(on_message_);
    connections_map_[sockfd] = conn; // 放入TCP连接的映射
    // 分配 id
    next_conn_id_++;
    if (next_conn_id_ == 1000) {
        next_conn_id_ = 0;
    }
}

void Server::handleClose(int sockfd) {
    if (sockfd != -1) {
        auto it = connections_map_.find(sockfd);
        assert(it != connections_map_.end());
        TCPConnection *conn = connections_map_[sockfd]; // 取出
        connections_map_.erase(sockfd);
        ::close(sockfd);
        conn = nullptr;
    }
}

void Server::setConnectionCallBack(std::function<void(TCPConnection*)> const& fn) {
    on_connect_ = fn;
}

void Server::setMessageCallBack(std::function<void(TCPConnection*)> const& fn) {
    on_message_ = fn;
}

void Server::start() {
    for (size_t i = 0; i < sub_reactors_.size(); i++) {
        std::function<void()> sub_loop = [this, i] () {
            sub_reactors_[i]->loop();
        };
        thread_pool_->addTask(sub_loop); // 启动 subReactor 线程
    }
    main_reactor_->loop(); // 启动 mainReactor 线程
}