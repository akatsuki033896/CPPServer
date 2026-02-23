#pragma once
#include "common.hpp"
#include <cstdint>
#include <functional>
#include <sys/epoll.h>

class Channel {
    EventLoop *loop_;
    int fd_;

    uint32_t listen_events_ = 0;
    uint32_t ready_events_ = 0;
    bool inEpoll_ = false;
    
    // 事件发生时的回调函数
    std::function<void()> read_callback_;
    std::function<void()> write_callback_;

public:
    DISALLOW_COPY_AND_MOVE(Channel);
    Channel(EventLoop *loop, int fd);
    ~Channel();
    
    void handleEvent() const; // 处理事件的函数，根据revents的值来调用相应的事件处理函数
    void enableReading();
    void enableWriting();
    void enableET();
    // void disableWriting();
    
    int getFd() const;
    uint32_t getListenEvents() const;
    bool isInEpoll() const;
    uint32_t getReadyEvents() const;
    
    void setReadyEvents(uint32_t);
    void setInEpoll(bool in = true);

    void setReadCallback(std::function<void()> const&); // 设置读事件发生时的回调函数，参数是一个std::function对象，可以绑定任何可调用对象（如函数指针、lambda表达式、成员函数等）
    void setWriteCallback(std::function<void()> const&); // 设置写事件发生时的回调函数，参数同上
};