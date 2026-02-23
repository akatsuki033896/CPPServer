#include "Epoller.hpp"
#include "util.hpp"
#include <sys/epoll.h>
#include <vector>
#include "Channel.hpp"

Epoller::Epoller() {
    epfd_ = epoll_create1(0);
    errif(epfd_ == -1, "epoll create error");
    events_ = new struct epoll_event[MAX_EVENTS];
    bzero(events_, sizeof(*events_) * MAX_EVENTS);
}

Epoller::~Epoller() {
    if (epfd_ != -1) {
        close(epfd_);
        epfd_ = -1;
    }
    delete[] events_;
}

// 返回就绪的事件列表
std::vector<Channel*> Epoller::poll(int timeout) const {
    std::vector<Channel*> activeChannels;
    int nfds = epoll_wait(epfd_, events_, MAX_EVENTS, timeout);
    errif(nfds == -1, "epoll wait error");
    for (int i = 0; i < nfds; i++) {
        Channel *ch = (Channel*)events_[i].data.ptr;
        ch->setReadyEvents(events_[i].events);
        activeChannels.push_back(ch);
    }
    return activeChannels;
}

void Epoller::updateChannel(Channel* channel) const{
    int fd = channel->getFd();
    
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.data.ptr = channel;
    ev.events = channel->getListenEvents();
    if (!channel->isInEpoll()) {
        errif(epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll_ctl add error");
        channel->setInEpoll();
    }
    else {
        errif(epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &ev) == -1, "epoll_ctl mod error");
    }
}

void Epoller::deleteChannel(Channel* channel) const {
    int fd = channel->getFd();
    errif(epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, NULL) == -1, "epoll_ctl delete error");
    channel->setInEpoll(false);
}