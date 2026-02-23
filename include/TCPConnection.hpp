// 类存在于事件驱动EventLoop类中，也就是Reactor模式的main-Reactor
// 类中的socket fd就是客户端的socket fd，每一个Connection对应一个socket fd
// 每一个类的实例通过一个独有的Channel负责分发到epoll，该Channel的事件处理函数handleEvent()会调用Connection中的事件处理函数来响应客户端请求

// TCPConnection 和 Acceptor 是平行关系，由 Server 直接管理，由一个 Channel 分发到 Epoll，通过回调函数处理相应事件
// Accpetor 处理事件函数放在 Server 里，Connection 类处理事件由本身完成

#pragma once

#include <functional>
#include <memory>
#include "common.hpp"

class TCPConnection {
public:
    enum ConnectionState {
        Invalid = 1,
        Connected,
        Disconnected
    };

private:
    EventLoop *loop_;
    // Socket *sock_;
    // 绑定的socket
    int connid_;
    int connfd_;

    ConnectionState state_;

    std::unique_ptr<Channel> channel_;
    std::unique_ptr<Buffer> read_buffer_; // 读缓冲
    std::unique_ptr<Buffer> send_buffer_; // 写缓冲
    
    // 回调函数
    std::function<void(int)> on_close_;
    std::function<void(TCPConnection*)> on_message_;

    void ReadNonBlocking();
    void WriteNonBlocking();

public:
    DISALLOW_COPY_AND_MOVE(TCPConnection);
    TCPConnection(EventLoop* loop, int connid, int connfd);
    ~TCPConnection();

    void set_close_callback(std::function<void(int)> const& fn); // 关闭的回调函数
    void set_message_callback(std::function<void(TCPConnection*)> const& fn);  // 接受到信息的回调函数    

    void setSendBuffer(const char*);
    Buffer* read_buff();
    Buffer* send_buff();

    void Read();
    void Write();
    void Send(const std::string& msg);
    void Send(const char* msg, size_t len);
    void Send(const char* msg);

    void handleMessage(); // 当接收到信息时，进行回调
    void handleClose(); // 当TcpConnection发起关闭请求时，进行回调，释放相应的socket

    // 状态
    ConnectionState getState() const;
    EventLoop* loop() const;
    int fd() const;
    int id() const;
};