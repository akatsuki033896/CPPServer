#pragma once

#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include "util.hpp"

class InetAddress;

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
    void setNonblocking();
    int accept(InetAddress* _addr);
    int get_fd() const;
};

