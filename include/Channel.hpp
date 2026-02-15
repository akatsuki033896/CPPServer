#pragma once
#include "Epoll.hpp"

class Channel {
    Epoll *ep;
    int fd;
    uint32_t events = 0;
    uint32_t revents = 0;
    bool inEpoll = false;
public:
    Channel(Epoll *_ep, int _fd) : ep(_ep), fd(_fd) {};
    ~Channel() {};

    void enableReading();

    int getFd() const;
    uint32_t getEvents() const;
    uint32_t getRevents() const;
    bool getInEpoll() const;
    void setInEpoll();

    void setRevents(uint32_t _revents);
};