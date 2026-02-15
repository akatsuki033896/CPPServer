#pragma once

class Epoll;
class Channel;

// Reactor模式的核心事件循环类，负责监听事件并分发事件
class EventLoop {
    Epoll *ep = nullptr;
    bool quit = false;
public:
    EventLoop();
    ~EventLoop();
    void loop(); // 开始事件驱动
    void updateChannel(Channel* channel);
};