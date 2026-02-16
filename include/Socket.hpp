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
    
    // 管理套接字资源，禁止拷贝
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    void bind(InetAddress* server_addr);
    void listen();
    void setNonblocking();
    int accept(InetAddress* _addr);
    void connect(InetAddress* _addr);
    int get_fd() const;
};

