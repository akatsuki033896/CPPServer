#include "TCPConnection.hpp"
#include "Channel.hpp"
#include "Buffer.hpp"
#include "EventLoop.hpp"
#include <cassert>
#include <cerrno>
#include <functional>
#include <iostream>
#include <format>
#include <cstring>
#include <memory>
#include <sys/types.h>
#include <unistd.h>

#define READ_BUFFER_SIZE 1024

TCPConnection::TCPConnection(EventLoop* loop, int connfd, int connid) : loop_(loop), connfd_(connfd), connid_(connid) {
    if (loop_ != nullptr) {
        channel_ = std::make_unique<Channel>(loop_, connfd_);
        channel_->enableET();
        channel_->setReadCallback(
            [this] () {
                this->handleMessage();
            }
        );
        channel_->enableReading();
    }
    read_buffer_ = std::make_unique<Buffer>();
    send_buffer_ = std::make_unique<Buffer>();
    state_ = ConnectionState::Connected;
}

TCPConnection::~TCPConnection() {
    ::close(connfd_);
}

void TCPConnection::set_close_callback(std::function<void(int)> const& fn) {
    on_close_ = std::move(fn);
}

void TCPConnection::set_message_callback(std::function<void(TCPConnection*)> const& fn) {
    on_message_ = fn;
}

void TCPConnection::handleMessage() {
    Read();
    if (on_message_) {
        on_message_(this); // 传递当前连接对象指针，方便回调函数获取连接相关信息
    }
}

void TCPConnection::handleClose() {
    if (state_ != ConnectionState::Disconnected) {
        state_ = ConnectionState::Disconnected;
        if (on_close_) {
            on_close_(connid_); // 传递连接ID，方便回调函数识别哪个连接关闭了
        }
    }
}

EventLoop* TCPConnection::loop() const {
    return loop_;
}

int TCPConnection::fd() const {
    return connfd_;
}

int TCPConnection::id() const {
    return connid_;
}

TCPConnection::ConnectionState TCPConnection::getState() const {
    return state_;
}

void TCPConnection::setSendBuffer(const char* str) {
    send_buffer_->setBuf(str); // TODO: modify Buffer class
}

Buffer* TCPConnection::read_buff() {
    return read_buffer_.get();
}

Buffer* TCPConnection::send_buff() {
    return send_buffer_.get();
}

void TCPConnection::Send(const std::string& msg) {
    setSendBuffer(msg.c_str());
    Write();
}

void TCPConnection::Send(const char* msg) {
    setSendBuffer(msg);
    Write();
}

// 清空 read_buffer_，然后将TCP缓冲区内的数据读取到读缓冲区
void TCPConnection::Read() {
    assert(state_ = ConnectionState::Connected);
    read_buffer_->clear();
    ReadNonBlocking();
}

// 将 write_buffer_里的内容发送到该Connection的socket，发送后会清空写缓冲区
void TCPConnection::Write() {
    assert(state_ = ConnectionState::Connected);
    send_buffer_->clear();
    WriteNonBlocking();
}

void TCPConnection::ReadNonBlocking() {
    int sockfd = connfd_;
    char buf[READ_BUFFER_SIZE]; // 该缓冲区大小无所谓
    // 非阻塞IO，边缘触发模式需要循环读取直到没有数据可读
    while (true) {
        memset(buf, 0, sizeof(buf));
        ssize_t read_bytes = read(sockfd, buf, sizeof(buf)); //从客户端socket读到缓冲区，返回已读数据大小
        if (read_bytes > 0) {
            read_buffer_->append(buf, read_bytes);
        }
        // 客户端正常中断，继续读取
        else if (read_bytes == -1 && errno == EINTR) {
            continue; // interrupted by signal, try again
        } 
        // 非阻塞IO，这个条件表示数据全部读取完毕
        else if (read_bytes == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            break;
        }
        // EOF，客户端关闭连接
        else if (read_bytes == 0) {
            handleClose();
            break;
        }
        else {
            handleClose();
            break;
        }
    }
}

void TCPConnection::WriteNonBlocking() {
    char buf[send_buffer_->getSize()]; // FIXME: readBuffer->size() 不是编译期常量，buf 变成 VLA
    memcpy(buf, send_buffer_->c_str(), send_buffer_->getSize());

    int data_size = send_buffer_->getSize();
    int data_left = data_size;
    while (data_left > 0) {
        ssize_t byte_write = write(connfd_, buf + data_size - data_left, data_left);
        if (byte_write == -1 && errno == EINTR) {
            std::cout << "Continue writing." << '\n';
            continue;
        }
        if (byte_write == -1 && errno == EAGAIN) {
            break;
        }
        if (byte_write == -1) {
            std::cout << std::format("Other error on client fd {}", connfd_);
            state_ = ConnectionState::Disconnected;
            break;
        }
        data_left -= byte_write;
    }
}
