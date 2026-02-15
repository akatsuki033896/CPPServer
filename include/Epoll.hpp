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
    
    void add_fd(int fd, uint32_t events);
    std::vector<Channel*> poll(int timeout = -1);
    void updateChannel(Channel* channel);
};