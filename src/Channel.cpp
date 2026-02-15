#include "../include/Channel.hpp"

void Channel::enableReading() {
    events = EPOLLIN | EPOLLET; // 监听可读事件，使用边缘触发模式
    ep->updateChannel(this); // 更新Channel在Epoll中的事件设置
}

int Channel::getFd() const {
    return fd;
}

uint32_t Channel::getEvents() const {
    return events;
}

uint32_t Channel::getRevents() const {
    return revents;
}

bool Channel::getInEpoll() const {
    return inEpoll;
}

void Channel::setInEpoll() {
    inEpoll = true;
}

void Channel::setRevents(uint32_t _revents) {
    revents = _revents;
}