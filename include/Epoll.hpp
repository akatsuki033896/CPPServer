#pragma once
#include <sys/epoll.h>
#include <unistd.h>
#include <vector>
#include <cstring>

#define MAX_EVENTS 10000

class Channel;

class Epoll {
    int epfd = -1;
    struct epoll_event *events = nullptr;
public:
    Epoll();
    ~Epoll();
    
    // 管理 epoll 禁止拷贝
    Epoll(const Epoll&) = delete;
    Epoll operator=(const Epoll&) = delete;

    std::vector<Channel*> poll(int timeout = -1);
    void updateChannel(Channel*);
    void deleteChannel(Channel*);
};