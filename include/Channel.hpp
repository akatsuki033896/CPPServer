#pragma once

#include <cstdint>
#include <functional>
#include <sys/epoll.h>

class EventLoop;
class ThreadPool;

class Channel {
    EventLoop *loop;
    int fd;
    uint32_t events = 0;
    uint32_t ready = 0;
    bool inEpoll = false;
    bool useThreadPool = true;
    
    // 事件发生时的回调函数
    std::function<void()> read_callback;
    std::function<void()> write_callback;

public:
    Channel(EventLoop *_loop, int _fd) : loop(_loop), fd(_fd) {};
    ~Channel() {};

    Channel(const Channel&) = delete;
    Channel operator=(const Channel&) = delete;
    
    void handleEvent(); // 处理事件的函数，根据revents的值来调用相应的事件处理函数
    void enableReading();

    int getFd() const;
    uint32_t getEvents() const;
    bool getInEpoll() const;
    uint32_t getReady() const;
    void setReady(uint32_t);
    void setInEpoll(bool _in = true);
    void useET();

    void setReadCallback(std::function<void()>); // 设置读事件发生时的回调函数，参数是一个std::function对象，可以绑定任何可调用对象（如函数指针、lambda表达式、成员函数等）
    void setUseThreadPool(bool use = true); // 是否使用线程池
};