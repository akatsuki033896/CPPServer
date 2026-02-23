#pragma once
#include "common.hpp"
#include <functional>
#include <memory>
#include <vector>

class Server {
    std::unique_ptr<EventLoop> main_reactor_; //只负责接受连接，然后分发给一个subReactor
    int next_conn_id_ = 0; // 连接ID生成器

    std::unique_ptr<Acceptor> acceptor_; // 负责监听新连接的Acceptor对象
    std::vector<std::unique_ptr<EventLoop>> sub_reactors_; //负责处理事件循环

    std::unordered_map<int, TCPConnection*> connections_map_; // 所有 TCP 连接
    std::unique_ptr<ThreadPool> thread_pool_;
    
    std::function<void(TCPConnection*)> on_connect_;
    std::function<void(TCPConnection*)> on_message_;
public:
    DISALLOW_COPY_AND_MOVE(Server);
    Server(const char* ip, const int port);
    ~Server();
    
    void start();
    void setConnectionCallBack(std::function<void(TCPConnection*)> const&);
    void setMessageCallBack(std::function<void(TCPConnection*)> const&);

    void handleClose(int);
    void handleNewConnection(int); // 添加新建TCP连接，创建一个新的Channel对象并将其添加到事件循环中
};

// Dependencies: Channel -> Epoll -> EventLoop -> Server

// 主从Reactor多线程模式。在这个模式中，服务器以事件驱动作为核心，服务器线程只负责mainReactor的新建连接任务，同时维护一个线程池，每一个线程也是一个事件循环，新连接建立后分发给一个subReactor开始事件监听，有事件发生则在当前线程处理。