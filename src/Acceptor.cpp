#include "Acceptor.hpp"
#include "EventLoop.hpp"
#include "Channel.hpp"
#include <cassert>
#include <cstdio>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "util.hpp"

Acceptor::Acceptor(EventLoop *loop, const char *ip, const int port) : loop_(loop), listen_fd_(-1){
    createSocket();
    bindSocket(ip, port);
    listenSocket();
    accept_channel_ = std::make_unique<Channel>(loop, listen_fd_); // 构造
    accept_channel_ -> setReadCallback(
        [this]() {
            acceptConnection();
        }
    );
    accept_channel_ -> enableReading(); // 监听socket的Channel只需要关注可读事件
}

Acceptor::~Acceptor() {
    loop_->deleteChannel(accept_channel_.get());
    ::close(listen_fd_);
}

void Acceptor::createSocket() {
    assert(listen_fd_ == -1);
    listen_fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
    errif(listen_fd_ == -1, "socket error");
}

void Acceptor::bindSocket(const char* ip, const int port) {
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
    errif(::bind(listen_fd_, (struct sockaddr*)&addr, sizeof(addr)) == -1, "bind error");
}

void Acceptor::listenSocket() {
    assert(listen_fd_ == -1);
    errif(::listen(listen_fd_, SOMAXCONN) < 0, "listen error");
}

void Acceptor::acceptConnection() {
    struct sockaddr_in clnt_addr;
    socklen_t clnt_len = sizeof(clnt_addr);
    assert(listen_fd_ == -1);
    int clnt_sockfd = ::accept4(listen_fd_, (struct sockaddr*)&clnt_addr, &clnt_len, SOCK_NONBLOCK | SOCK_CLOEXEC);
    errif(clnt_sockfd == -1, "accept error");

    if (new_connection_callback_) {
        new_connection_callback_(clnt_sockfd);
    }
}

void Acceptor::setNewConnectionCallBack(std::function<void(int)> const& callback) {
    new_connection_callback_ = std::move(callback);
}