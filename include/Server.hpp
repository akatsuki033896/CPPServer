#pragma once
#include <map>

class EventLoop;
class Socket;
class Acceptor;
class Connection;

class Server {
    EventLoop *loop;
    Acceptor *acceptor = nullptr;
    std::map<int, Connection*> connections; // 所有 TCP 连接
public:
    Server(EventLoop *_loop);
    ~Server();

    void newConnection(Socket *sock); // 添加新建TCP连接，创建一个新的Channel对象并将其添加到事件循环中
    void deleteConnection(Socket *sock); // 断开TCP连接
};

// Dependencies: Channel -> Epoll -> EventLoop -> Server