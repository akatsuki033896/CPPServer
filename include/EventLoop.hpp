#pragma once

#include <functional>
class Epoll;
class Channel;
class ThreadPool;

// Reactor模式的核心事件循环类，负责监听事件并分发事件
class EventLoop {
    Epoll *ep = nullptr;
    ThreadPool *threadPool;
    bool quit = false;
public:
    EventLoop();
    ~EventLoop();

    EventLoop(const EventLoop&) = delete;
    EventLoop operator=(const EventLoop&) = delete;
    
    void loop(); // 开始事件驱动
    void updateChannel(Channel* channel);
    void addThread(std::function<void()>);
};