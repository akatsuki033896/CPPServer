#pragma once

#include <unistd.h>
#include "InetAddress.hpp"
#include <fcntl.h>
#include <sys/socket.h>
#include "util.hpp"


class Socket {
    int fd = -1;
public:
    Socket() {
        fd = socket(AF_INET, SOCK_STREAM, 0); // TCP socket
        errif(fd == -1, "socket create error");
    }

    Socket(int _fd) : fd(_fd) {
        errif(fd == -1, "socket create error");
    }

    ~Socket() {
        if (fd != -1) {
            close(fd);
            fd = -1;
        }
    }

    void bind(InetAddress* server_addr);
    void listen();
    void set_nonblocking();
    int accept(struct sockaddr_in* client_addr, socklen_t* client_addr_len);
    int get_fd() const;
};

