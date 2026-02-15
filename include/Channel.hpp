#pragma once
#include <cstdint>
#include <functional>
#include <sys/epoll.h>

class EventLoop;

class Channel {
    EventLoop *loop;
    int fd;
    uint32_t events = 0;
    uint32_t revents = 0;
    bool inEpoll = false;
    std::function<void()> callback; // 事件发生时的回调函数

public:
    Channel(EventLoop *_loop, int _fd) : loop(_loop), fd(_fd) {};
    ~Channel() {};

    void enableReading();

    int getFd() const;
    uint32_t getEvents() const;
    uint32_t getRevents() const;
    bool getInEpoll() const;
    void setInEpoll();

    void setRevents(uint32_t _revents);
    void handleEvent(); // 处理事件的函数，根据revents的值来调用相应的事件处理函数
    void setCallback(std::function<void()> _cb); // 设置事件发生时的回调函数，参数是一个std::function对象，可以绑定任何可调用对象（如函数指针、lambda表达式、成员函数等）
};