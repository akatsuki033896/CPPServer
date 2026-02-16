#include "Socket.hpp"
#include "InetAddress.hpp"
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

void Socket::bind(InetAddress* server_addr) {
    errif(::bind(fd, (struct sockaddr*)&server_addr->addr, server_addr->addr_len) == -1, "bind error");
}

void Socket::listen() {
    errif(::listen(fd, SOMAXCONN) < 0, "listen error");
}

void Socket::setNonblocking() {
    fcntl(fd, F_SETFL, fcntl(this->fd, F_GETFL) | O_NONBLOCK);
}

int Socket::accept(InetAddress* _addr) {
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    bzero(&addr, sizeof(addr));

    int clnt_sockfd = ::accept(fd, (struct sockaddr*)&addr, &addr_len);
    errif(clnt_sockfd == -1, "accept error");
    _addr->setInetAddr(addr, addr_len);
    return clnt_sockfd;
}

void Socket::connect(InetAddress* _addr) {
    struct sockaddr_in addr = _addr->getAddr();
    socklen_t addr_len = _addr->getAddr_len();
    errif(::connect(fd, (struct sockaddr*)&addr, addr_len) == -1, "Socket connect error");
}

int Socket::get_fd() const {
    return fd;
}
