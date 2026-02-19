#include "Connection.hpp"
#include "Channel.hpp"
#include "Socket.hpp"
#include "Buffer.hpp"
#include <cassert>
#include <cerrno>
#include <functional>
#include <iostream>
#include <format>
#include <cstring>
#include <sys/types.h>
#include <unistd.h>

#define READ_BUFFER_SIZE 1024

Connection::Connection(EventLoop* loop, Socket* sock) : loop_(loop), sock_(sock) {
    if (loop_ != nullptr) {
        channel_ = new Channel(loop, sock->get_fd());
        channel_->enableReading();
        channel_->useET();
        // channel_->setReadCallback(
        //     [this] () {
        //         this->echo(sock->get_fd());
        //     }
        // );
        // channel_->setUseThreadPool(true);
    }
    read_buffer_ = new Buffer();
    send_buffer_ = new Buffer();
    state_ = State::Connected;
}

Connection::~Connection() {
    if (loop_ != nullptr) {
        delete channel_;
    }
    delete sock_;
    delete read_buffer_;
    delete send_buffer_;
}

// 清空 read_buffer_，然后将TCP缓冲区内的数据读取到读缓冲区
void Connection::Read() {
    assert(state_ = State::Connected);
    read_buffer_->clear();
    ReadNonBlocking();
}

// 将 write_buffer_里的内容发送到该Connection的socket，发送后会清空写缓冲区
void Connection::Write() {
    assert(state_ = State::Connected);
    send_buffer_->clear();
    WriteNonBlocking();
}

void Connection::ReadNonBlocking() {
    int sockfd = sock_->get_fd();
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
            std::cout << "Continue Reading" << '\n';
            continue; // interrupted by signal, try again
        } 
        // 非阻塞IO，这个条件表示数据全部读取完毕
        else if (read_bytes == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            break;
        }
        // EOF，客户端关闭连接
        else if (read_bytes == 0) {
            std::cout << std::format("EOF: Client {} disconnected.", sockfd) << '\n';
            state_ = State::Closed;
            // deleteConnectionCallBack(sock);
            break;
        }
        else {
            std::cout << "Other errors occurred" << '\n';
            state_ = State::Closed;
            // deleteConnectionCallBack(sock);
            break;
        }
    }
}

void Connection::WriteNonBlocking() {
    int sockfd = sock_->get_fd();
    char buf[send_buffer_->size()]; // FIXME: readBuffer->size() 不是编译期常量，buf 变成 VLA
    memcpy(buf, send_buffer_->c_str(), send_buffer_->size());

    int data_size = send_buffer_->size();
    int data_left = data_size;
    while (data_left > 0) {
        ssize_t byte_write = write(sockfd, buf + data_size - data_left, data_left);
        if (byte_write == -1 && errno == EINTR) {
            std::cout << "Continue writing." << '\n';
            continue;
        }
        if (byte_write == -1 && errno == EAGAIN) {
            break;
        }
        if (byte_write == -1) {
            std::cout << std::format("Other error on client fd {}", sockfd);
            state_ = State::Closed;
            break;
        }
        data_left -= byte_write;
    }
}

void Connection::Close() {
    delete_connection_call_back_(sock_);
}

Connection::State Connection::getState() const {
    return state_;
}

Buffer* Connection::getReadBuffer() const {
    return read_buffer_;
}

const char* Connection::readBuffer() {
    return read_buffer_->c_str();
}

Buffer* Connection::getSendBuffer() const {
    return send_buffer_;
}

const char* Connection::sendBuffer() {
    return send_buffer_->c_str();
}

void Connection::setSendBuffer(const char* str) {
    send_buffer_->setBuf(str); // TODO: modify Buffer class
}

Socket* Connection::getSocket() const {
    return sock_;
}

void Connection::setDeleteConnectionCallBack(std::function<void(Socket*)> const& callback) {
    delete_connection_call_back_ = callback;
}

void Connection::setOnConnectionCallBack(std::function<void(Connection*)> const& callback) {
    on_connection_call_back_ = callback;
    channel_->setReadCallback([this] () {
        on_connection_call_back_(this);
    });
}

void Connection::getlineSendBuffer() {
    send_buffer_->getline();
}