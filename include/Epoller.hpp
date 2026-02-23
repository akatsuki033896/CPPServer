#pragma once
#include "common.hpp"
#include <sys/epoll.h>
#include <unistd.h>
#include <vector>
#include <cstring>

#define MAX_EVENTS 10000

class Epoller {
    int epfd_ = -1;
    struct epoll_event *events_ = nullptr;
public:
    DISALLOW_COPY_AND_MOVE(Epoller); // 禁止拷贝和移动
    Epoller();
    ~Epoller();

    std::vector<Channel*> poll(int timeout = -1) const; // 返回调用完epoll_wait的通道事件
    void updateChannel(Channel*) const;
    void deleteChannel(Channel*) const;
};