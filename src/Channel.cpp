#include "Channel.hpp"
#include "EventLoop.hpp"
#include <cstdint>
#include <sys/epoll.h>

Channel::Channel(EventLoop *loop, int fd) : loop_(loop), fd_(fd) {}

Channel::~Channel() {}

int Channel::getFd() const {
    return fd_;
}

uint32_t Channel::getListenEvents() const {
    return listen_events_;
}

bool Channel::isInEpoll() const {
    return inEpoll_;
}

uint32_t Channel::getReadyEvents() const {
    return ready_events_;
}

void Channel::setInEpoll(bool in) {
    inEpoll_ = in;
}

void Channel::setReadyEvents(uint32_t _ev) {
    ready_events_ = _ev;
}

void Channel::setReadCallback(std::function<void()> const& callback) {
    read_callback_ = std::move(callback);
}

void Channel::setWriteCallback(std::function<void()> const& callback) {
    write_callback_ = std::move(callback);
}

void Channel::handleEvent() const {
    if (ready_events_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
        if (read_callback_) {
            read_callback_();
        }
    }
    if (ready_events_ & EPOLLOUT) {
        if (write_callback_) {
            write_callback_();
        }
    }
}

void Channel::enableReading() {
    listen_events_ |= EPOLLIN | EPOLLPRI; // 监听可读事件，使用边缘触发模式
    loop_->updateChannel(this); // 将当前Channel对象添加到事件循环中
}

void Channel::enableWriting() {
    listen_events_ |= EPOLLOUT; // 监听可写事件
    loop_->updateChannel(this); // 将当前Channel对象添加到事件循环中
}

void Channel::enableET() {
    listen_events_ |= EPOLLET;
    loop_->updateChannel(this);
}