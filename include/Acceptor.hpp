#pragma once

#include <functional>
class EventLoop;
class Socket;
class Channel;

// 对于所有的服务，都要使用Acceptor来建立连接，Acceptor本身并不知道是哪种服务， Acceptor 只有一个用于接受连接
class Acceptor {
    EventLoop *loop;
    Socket *sock = nullptr;
    Channel *accept_channel = nullptr;
    std::function<void(Socket*)> newConnectionCallBack;
public:
    Acceptor(EventLoop *_loop);
    ~Acceptor();
    void acceptConnection();
    void setNewConnectionCallBack(std::function<void(Socket*)> cb);
};