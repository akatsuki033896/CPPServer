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

    // 原则上只能有一个，禁止拷贝
    Acceptor(const Acceptor&) = delete;
    Acceptor operator=(const Acceptor&) = delete;
    
    void acceptConnection();
    void setNewConnectionCallBack(std::function<void(Socket*)> cb);
};

//对于Acceptor，接受连接的处理时间较短、报文数据极小，并且一般不会有特别多的新连接在同一时间到达，所以Acceptor没有必要采用epoll ET模式，也没有必要用线程池。由于不会成为性能瓶颈，为了简单最好使用阻塞式socket，故今天的源代码中做了以下改变：

// 1. Acceptor socket fd（服务器监听socket）使用阻塞式
// 2. Acceptor使用LT模式，建立好连接后处理事件fd读写用ET模式
// 3. Acceptor建立连接不使用线程池，建立好连接后处理事件用线程池