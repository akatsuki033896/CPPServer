#include "Socket.hpp"
#include "InetAddress.hpp"

void Socket::bind(InetAddress* server_addr) {
    errif(::bind(fd, (struct sockaddr*)&server_addr->addr, server_addr->addr_len) == -1, "bind error");
}

void Socket::listen() {
    errif(::listen(fd, SOMAXCONN) < 0, "listen error");
}

void Socket::setNonblocking() {
    fcntl(fd, F_SETFL, fcntl(this->fd, F_GETFL) | O_NONBLOCK);
}

int Socket::accept(struct sockaddr_in* client_addr, socklen_t* client_addr_len) {
    int client_sockfd = ::accept(fd, (struct sockaddr*)client_addr, client_addr_len);
    errif(client_sockfd == -1, "accept error");
    return client_sockfd;
}

int Socket::get_fd() const {
    return fd;
}
