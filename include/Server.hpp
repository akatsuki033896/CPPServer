#pragma once
#include <functional>
#include <map>
#include <vector>

class EventLoop;
class Socket;
class Acceptor;
class Connection;
class ThreadPool;

class Server {
    EventLoop* main_reactor_; //只负责接受连接，然后分发给一个subReactor
    std::vector<EventLoop*> sub_reactors_; //负责处理事件循环
    Acceptor* acceptor_ = nullptr;
    std::map<int, Connection*> connections_; // 所有 TCP 连接
    ThreadPool* thread_pool_ = nullptr;
    std::function<void(Connection*)> on_connect_callback_;
public:
    explicit Server(EventLoop*);
    ~Server();

    // 原则上只能有一个，禁止拷贝
    Server(const Server&) = delete;
    Server operator=(const Server&) = delete;
    
    void newConnection(Socket*); // 添加新建TCP连接，创建一个新的Channel对象并将其添加到事件循环中
    void deleteConnection(Socket*); // 断开TCP连接
    void onConnect(std::function<void(Connection*)>);
};

// Dependencies: Channel -> Epoll -> EventLoop -> Server

// 主从Reactor多线程模式。在这个模式中，服务器以事件驱动作为核心，服务器线程只负责mainReactor的新建连接任务，同时维护一个线程池，每一个线程也是一个事件循环，新连接建立后分发给一个subReactor开始事件监听，有事件发生则在当前线程处理。