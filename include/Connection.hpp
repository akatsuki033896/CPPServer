// 类存在于事件驱动EventLoop类中，也就是Reactor模式的main-Reactor
// 类中的socket fd就是客户端的socket fd，每一个Connection对应一个socket fd
// 每一个类的实例通过一个独有的Channel负责分发到epoll，该Channel的事件处理函数handleEvent()会调用Connection中的事件处理函数来响应客户端请求

// Connection 和 Acceptor 是平行关系，由 Server 直接管理，由一个 Channel 分发到 Epoll，通过回调函数处理相应事件
// Accpetor 处理事件函数放在 Server 里，Connection 类处理事件由本身完成

#pragma once

#include <functional>
class EventLoop;
class Socket;
class Channel;
class Buffer;

class Connection {
    EventLoop *loop_;
    Socket *sock_;
    Channel *channel_ = nullptr;
    Buffer *read_buffer_ = nullptr; // 读缓冲
    Buffer *send_buffer_ = nullptr; // 写缓冲

    // 回调函数
    std::function<void(Socket*)> delete_connection_call_back_;
    std::function<void(Connection*)> on_connection_call_back_;

    void ReadNonBlocking();
    void WriteNonBlocking();
    // void ReadBlocking();
    // void WriteBlocking();
public:
    Connection(EventLoop*, Socket*);
    ~Connection();
    
    Connection(const Connection&) = delete;
    Connection operator=(const Connection&) = delete;

    // 对于将服务器内容发送回客户端的 echo 要支持自定义，同时应该被 Server 直接管理
    // void echo(int sockfd);
    
    Buffer* getReadBuffer() const;
    const char *readBuffer();
    
    Buffer* getSendBuffer() const;
    void setSendBuffer(const char*);
    const char *sendBuffer();
    void getlineSendBuffer();

    Socket* getSocket() const;

    enum State {
        Invalid = 1,
        HandShaking,
        Connected,
        Closed,
        Failed
    };
    // 状态
    State state_ = State::Invalid;
    State getState() const;

    void Read();
    void Write();
    void onConnect(std::function<void()>); // 自定义业务
    void setDeleteConnectionCallBack(std::function<void(Socket*)> const&);
    void setOnConnectionCallBack(std::function<void(Connection*)> const&);
    void Close();
};