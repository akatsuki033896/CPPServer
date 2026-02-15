#include "Acceptor.hpp"
#include "EventLoop.hpp"
#include "InetAddress.hpp"
#include "Socket.hpp"
#include "Channel.hpp"
#include <cstdio>
#include <format>
#include <iostream>

Acceptor::Acceptor(EventLoop *_loop) : loop(_loop) {
    sock = new Socket();
    InetAddress* addr = new InetAddress("127.0.0.1", 8888); // TODO: change to unique_ptr
    sock->bind(addr);
    sock->listen();
    sock->setNonblocking();
    accept_channel = new Channel(loop, sock->get_fd());
    
    // std::function<void()> cb = std::bind(&Acceptor::acceptConnection, this);

    // accept_channel -> setCallback(cb);
    accept_channel -> setCallback(
        [this]() {
            acceptConnection();
        }
    );
    accept_channel -> enableReading(); // 监听socket的Channel只需要关注可读事件
    delete addr;
}

Acceptor::~Acceptor() {
    delete sock;
    delete accept_channel;
}

void Acceptor::acceptConnection() {
    InetAddress *clnt_addr = new InetAddress(); // TODO: change to unique_ptr
    Socket *clnt_sock = new Socket(sock->accept(clnt_addr));
    std::cout << std::format("New Client {}, IP: {}, port: {}", clnt_sock->get_fd(), inet_ntoa(clnt_addr->getAddr().sin_addr), ntohs(clnt_addr->getAddr().sin_port)) << '\n'; // TODO: 地址和端口显示0.0.0.0
    clnt_sock->setNonblocking();
    newConnectionCallBack(clnt_sock);
    delete clnt_addr;
}

void Acceptor::setNewConnectionCallBack(std::function<void(Socket*)> _cb) {
    newConnectionCallBack = _cb;
}