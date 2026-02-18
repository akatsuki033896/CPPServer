#pragma once
#include <map>
#include <vector>

class EventLoop;
class Socket;
class Acceptor;
class Connection;
class ThreadPool;

class Server {
    EventLoop *mainReactor; //只负责接受连接，然后分发给一个subReactor
    std::vector<EventLoop*> subReactors; //负责处理事件循环
    Acceptor *acceptor = nullptr;
    std::map<int, Connection*> connections; // 所有 TCP 连接
    ThreadPool *thpool;
public:
    Server(EventLoop*);
    ~Server();

    // 原则上只能有一个，禁止拷贝
    Server(const Server&) = delete;
    Server operator=(const Server&) = delete;
    
    void handleReadEvent(int);
    void newConnection(Socket *sock); // 添加新建TCP连接，创建一个新的Channel对象并将其添加到事件循环中
    void deleteConnection(Socket *sock); // 断开TCP连接
};

// Dependencies: Channel -> Epoll -> EventLoop -> Server

// 主从Reactor多线程模式。在这个模式中，服务器以事件驱动作为核心，服务器线程只负责mainReactor的新建连接任务，同时维护一个线程池，每一个线程也是一个事件循环，新连接建立后分发给一个subReactor开始事件监听，有事件发生则在当前线程处理。