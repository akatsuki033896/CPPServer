#pragma once
#include <sys/epoll.h>
#include "util.hpp"
#include <unistd.h>
#include <vector>
#include <cstring>

#define MAX_EVENTS 10000

class Epoll {
    int epfd = -1;
    struct epoll_event *events = nullptr;
public:
    Epoll() {
        epfd = epoll_create1(0);
        errif(epfd == -1, "epoll create error");
        events = new struct epoll_event[MAX_EVENTS];
        bzero(events, sizeof(struct epoll_event) * MAX_EVENTS);
    }

    ~Epoll() {
        if (epfd != -1) {
            close(epfd);
            epfd = -1;
        }
        delete[] events;
    }
    
    void add_fd(int fd, uint32_t events);
    std::vector<struct epoll_event> poll(int timeout = -1);
};