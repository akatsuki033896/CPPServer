#include "EventLoop.hpp"
#include "Epoll.hpp"
#include "Channel.hpp"
#include "ThreadPool.hpp"
#include <functional>
#include <vector>

EventLoop::EventLoop() {
    ep = new Epoll();
    threadPool = new ThreadPool();
}

EventLoop::~EventLoop() {
    delete ep;
    delete threadPool;
}

void EventLoop::loop() {
    while (!quit) {
        std::vector<Channel*> chs;
        chs = ep->poll(); // 获取活跃的Channel列表
        for (auto it = chs.begin(); it != chs.end(); it++) {
            (*it)->handleEvent(); // 处理每个活跃的Channel事件
        }
    }
}

void EventLoop::updateChannel(Channel* channel) {
    ep->updateChannel(channel);
}

void EventLoop::addThread(std::function<void()> func) {
    threadPool->add(func);
}