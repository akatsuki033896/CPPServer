#pragma once
#include "common.hpp"
#include <memory>

// Reactor模式的核心事件循环类，负责监听事件并分发事件
class EventLoop {
    // Epoller *ep = nullptr;
    std::unique_ptr<Epoller> poller_;
    // bool quit = false;
public:
    DISALLOW_COPY_AND_MOVE(EventLoop);
    EventLoop();
    ~EventLoop();
    
    void loop(); // 开始事件驱动
    void updateChannel(Channel* channel) const;
    void deleteChannel(Channel* channel) const;
};

// 每一个EventLoop主要是不断地调用epoll_wait来获取激活的事件，并处理。这也就意味着Epoll是独属于EventLoop的成员变量，随着EventLoop的析构而析构，因此可以采用智能指针