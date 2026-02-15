#pragma once
#define READ_BUFFER_SIZE 1024

class EventLoop;
class Socket;

class Server {
    EventLoop *loop;
public:
    Server(EventLoop *_loop);
    ~Server();

    void handleReadEvent(int sockfd); // 处理读事件的函数，接受新的连接并将其添加到事件循环中
    void newConnection(Socket *serv_sock); // 处理新连接的函数，创建一个新的Channel对象并将其添加到事件循环中
};

// Dependencies: Channel -> Epoll -> EventLoop