#include "../include/Epoll.hpp"
#include "../include/util.hpp"
#include <sys/epoll.h>
#include <vector>

void Epoll::add_fd(int fd, uint32_t events) {
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.data.fd = fd;
    ev.events = events;
    errif(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll_ctl add error");
}

std::vector<struct epoll_event> Epoll::poll(int timeout) {
    std::vector<epoll_event> activeEvents;
    int nfds = epoll_wait(epfd, events, MAX_EVENTS, timeout);
    errif(nfds == -1, "epoll wait error");
    
    for (int i = 0; i < nfds; i++) {
        activeEvents.push_back(events[i]);
    }
    return activeEvents;
}