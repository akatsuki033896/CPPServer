#pragma once

#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

class InetAddress {
public:
    struct sockaddr_in addr;
    socklen_t addr_len;
    
    // 内联的构造函数
    InetAddress() : addr_len(sizeof(addr)) {
        bzero(&addr, sizeof(addr));
    }
    InetAddress(const char* ip, uint16_t port) {
        bzero(&addr, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(ip);
        addr.sin_port = htons(port);
        addr_len = sizeof(addr);
    }
    ~InetAddress() {}
};