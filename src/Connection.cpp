#include "Connection.hpp"
#include "Channel.hpp"
#include "Socket.hpp"
#include <functional>
#include <iostream>
#include <format>
#include <cstring>

#define READ_BUFFER_SIZE 1024

Connection::Connection(EventLoop* _loop, Socket* _sock) : loop(_loop), sock(_sock) {
    channel = new Channel(loop, sock->get_fd());
    // std::function<void()> cb = std::bind(&Connection::echo, this, sock->get_fd());
    // channel->setCallback(cb);
    // TODO: change std::bind to lambda
    channel->setCallback(
        [this] () {
            this->echo(sock->get_fd());
        }
    );
    channel->enableReading();
}

Connection::~Connection() {
    delete channel;
    delete sock;
}

void Connection::echo(int sockfd) {
    char buf[READ_BUFFER_SIZE]; // buffer to hold incoming data
                    // 非阻塞IO，边缘触发模式需要循环读取直到没有数据可读
    while (true) {
        bzero(&buf, sizeof(buf));
        ssize_t read_bytes = read(sockfd, buf, sizeof(buf)); //从客户端socket读到缓冲区，返回已读数据大小
        if (read_bytes > 0) {
            std::cout << std::format("Received from client {}: {}", sockfd, buf) << '\n';
            write(sockfd, buf, read_bytes); // echo back to client
        }
        // 客户端正常中断
        else if (read_bytes == -1 && errno == EINTR) {
            std::cout << "Continue Reading" << '\n';
            continue; // interrupted by signal, try again
        } 
        // 非阻塞IO，这个条件表示数据全部读取完毕
        else if (read_bytes == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            std::cout << std::format("Finished reading from client {}. No more data to read. errno: {}", sockfd, errno) << '\n';
            break; // no more data to read
        }
        // EOF，客户端关闭连接
        else if (read_bytes == 0) {
            std::cout << std::format("Client {} disconnected.", sockfd) << '\n';
            deleteConnectionCallBack(sock);
            break;
        }
    }
}

void Connection::setDeleteConnectionCallBack(std::function<void(Socket*)> _cb) {
    deleteConnectionCallBack = _cb;
}