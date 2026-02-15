#pragma once

#include <functional>
class EventLoop;
class Socket;
class InetAddress;
class Channel;

// 对于所有的服务，都要使用Acceptor来建立连接，Acceptor本身并不知道是哪种服务
class Acceptor {
    EventLoop *loop;
    Socket *sock;
    InetAddress *addr;
    Channel *accept_channel;
public:
    Acceptor(EventLoop *_loop);
    ~Acceptor();
    std::function<void(Socket*)> newConnectionCallBack;
    void acceptConnection();
    void setNewConnectionCallBack(std::function<void(Socket*)>);
};