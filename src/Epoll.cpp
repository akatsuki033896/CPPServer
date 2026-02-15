#include "../include/Epoll.hpp"
#include "../include/util.hpp"
#include <sys/epoll.h>
#include <vector>
#include "../include/Channel.hpp"

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
    std::vector<Channel*> activeEvents;
    int nfds = epoll_wait(epfd, events, MAX_EVENTS, timeout);
    errif(nfds == -1, "epoll wait error");
    
    for (int i = 0; i < nfds; i++) {
        Channel *ch = (Channel*)events[i].data.ptr;
        ch->setRevents(events[i].events);
        activeEvents.push_back(ch);
    }
    return activeEvents;
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