#include "Acceptor.hpp"
#include "EventLoop.hpp"
#include "InetAddress.hpp"
#include "Socket.hpp"
#include "Channel.hpp"

Acceptor::Acceptor(EventLoop *_loop) : loop(_loop) {
    sock = new Socket();
    addr = new InetAddress("127.0.0.1", 8888);
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
}

Acceptor::~Acceptor() {
    delete sock;
    delete addr;
    delete accept_channel;
}

void Acceptor::acceptConnection() {
    newConnectionCallBack(sock);
}

void Acceptor::setNewConnectionCallBack(std::function<void(Socket*)> _cb) {
    newConnectionCallBack = _cb;
}