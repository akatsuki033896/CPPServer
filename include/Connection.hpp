// 类存在于事件驱动EventLoop类中，也就是Reactor模式的main-Reactor
// 类中的socket fd就是客户端的socket fd，每一个Connection对应一个socket fd
// 每一个类的实例通过一个独有的Channel负责分发到epoll，该Channel的事件处理函数handleEvent()会调用Connection中的事件处理函数来响应客户端请求

// Connection 和 Acceptor 是平行关系，由 Server 直接管理，由一个 Channel 分发到 Epoll，通过回调函数处理相应事件
// Accpetor 处理事件函数放在 Server 里，Connection 类处理事件由本身完成

#pragma once

#include <functional>
class EventLoop;
class Socket;
class Channel;

class Connection {
    EventLoop *loop;
    Socket *sock;
    Channel *channel;
    std::function<void(Socket*)> deleteConnectionCallBack;
public:
    Connection(EventLoop* _loop, Socket* _sock);
    ~Connection();
    
    void echo(int sockfd);
    void setDeleteConnectionCallBack(std::function<void(Socket*)> _cb);
};