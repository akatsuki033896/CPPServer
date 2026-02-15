#pragma once

#include <cstring>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

class InetAddress {
public:
    struct sockaddr_in addr;
    socklen_t addr_len;
    
    InetAddress();
    InetAddress(const char* ip, uint16_t port);
    ~InetAddress() {}
    void setInetAddr(sockaddr_in _addr, socklen_t _addr_len);
    sockaddr_in getAddr() const;
    socklen_t getAddr_len() const;
};