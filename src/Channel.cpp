#include "Channel.hpp"
#include "EventLoop.hpp"
#include <cstdint>
#include <sys/epoll.h>

int Channel::getFd() const {
    return fd;
}

uint32_t Channel::getEvents() const {
    return events;
}

bool Channel::getInEpoll() const {
    return inEpoll;
}

uint32_t Channel::getReady() const {
    return ready;
}

void Channel::setInEpoll(bool _in) {
    inEpoll = _in;
}

void Channel::setReady(uint32_t _ev) {
    ready = _ev;
}

void Channel::setReadCallback(std::function<void()> _cb) {
    read_callback = _cb;
}

void Channel::handleEvent() {
    if (ready & (EPOLLIN | EPOLLPRI)) {
        read_callback();
    }
    if (ready & (EPOLLOUT)) {
        write_callback();
    }
}

void Channel::enableReading() {
    events |= EPOLLIN | EPOLLPRI; // 监听可读事件，使用边缘触发模式
    loop->updateChannel(this); // 将当前Channel对象添加到事件循环中
}

void Channel::useET() {
    events |= EPOLLET;
    loop->updateChannel(this);
}

void Channel::setUseThreadPool(bool use) {
    useThreadPool = use;
}