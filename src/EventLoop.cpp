#include "EventLoop.hpp"
#include "Epoller.hpp"
#include "Channel.hpp"
#include <memory>

EventLoop::EventLoop() {
    poller_ = std::make_unique<Epoller>();
}

EventLoop::~EventLoop() {}

void EventLoop::loop() {
    while (true) {
        for (Channel* active_ch : poller_->poll()) {
            active_ch->handleEvent();
        }
    }
}

void EventLoop::updateChannel(Channel* channel) const {
    poller_->updateChannel(channel);
}

void EventLoop::deleteChannel(Channel* channel) const {
    poller_->deleteChannel(channel);
}