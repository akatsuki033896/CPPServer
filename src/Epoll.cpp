#include "Epoll.hpp"
#include "util.hpp"
#include <sys/epoll.h>
#include <vector>
#include "Channel.hpp"

Epoll::Epoll() {
    epfd = epoll_create1(0);
    errif(epfd == -1, "epoll create error");
    events = new struct epoll_event[MAX_EVENTS];
    bzero(events, sizeof(*events) * MAX_EVENTS);
}

Epoll::~Epoll() {
    if (epfd != -1) {
        close(epfd);
        epfd = -1;
    }
    delete[] events;
}

// 将文件描述符fd添加到epoll实例中，监听events指定的事件
void Epoll::add_fd(int fd, uint32_t events) {
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.data.fd = fd;
    ev.events = events;
    errif(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll_ctl add error");
}

// 返回就绪的事件列表
std::vector<Channel*> Epoll::poll(int timeout) {
    std::vector<Channel*> activeChannels;
    int nfds = epoll_wait(epfd, events, MAX_EVENTS, timeout);
    errif(nfds == -1, "epoll wait error");
    for (int i = 0; i < nfds; i++) {
        Channel *ch = (Channel*)events[i].data.ptr;
        ch->setRevents(events[i].events);
        activeChannels.push_back(ch);
    }
    return activeChannels;
}

void Epoll::updateChannel(Channel* channel) {
    int fd = channel->getFd();
    
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.data.ptr = channel;
    ev.events = channel->getEvents();
    if (!channel->getInEpoll()) {
        errif(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll_ctl add error");
        channel->setInEpoll();
    }
    else {
        errif(epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev) == -1, "epoll_ctl mod error");
    }
}