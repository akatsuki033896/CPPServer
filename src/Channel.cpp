#include "Channel.hpp"
#include "EventLoop.hpp"

void Channel::enableReading() {
    events = EPOLLIN | EPOLLET; // 监听可读事件，使用边缘触发模式
    loop->updateChannel(this); // 将当前Channel对象添加到事件循环中
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

void Channel::setCallback(std::function<void()> _cb) {
    callback = _cb;
}

void Channel::handleEvent() {
    callback(); // 调用回调函数处理事件
}
