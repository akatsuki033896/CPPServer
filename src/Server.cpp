#include "Server.hpp"
#include "EventLoop.hpp"
#include "Socket.hpp"
#include "InetAddress.hpp"
#include "Channel.hpp"
#include <functional>
#include <iostream>
#include <format>

Server::Server(EventLoop *_loop) : loop(_loop) {
    Socket *serv_sock = new Socket();
    InetAddress *serv_addr = new InetAddress("127.0.0.1", 8888);
    serv_sock->bind(serv_addr);
    serv_sock->listen();
    std::cout << "Server listening on 127.0.0.1:8888" << '\n';
    serv_sock->setNonblocking();

    Channel *serv_channel = new Channel(loop, serv_sock->get_fd());
    std::function<void()> cb = std::bind(&Server::newConnection, this, serv_sock);
    serv_channel->setCallback(cb);
    serv_channel->enableReading(); // 监听socket的Channel只需要关注可读事件
}

void Server::handleReadEvent(int sockfd) {
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
            close(sockfd); // 关闭客户端socket
            break;
        }
    }
}

void Server::newConnection(Socket *serv_sock) {
    InetAddress *client_addr = new InetAddress();
    Socket *clnt_sock = new Socket(serv_sock->accept(&client_addr->addr, &client_addr->addr_len));
    std::cout << std::format("Client {} connected. IP: {}, port: {}", clnt_sock->get_fd(), inet_ntoa(client_addr->addr.sin_addr), ntohs(client_addr->addr.sin_port)) << '\n';
    
    clnt_sock->setNonblocking();
    Channel *clnt_channel = new Channel(loop, clnt_sock->get_fd());
    std::function<void()> cb = std::bind(&Server::handleReadEvent, this, clnt_sock->get_fd()); // 绑定成员函数和参数
    clnt_channel->setCallback(cb);
    clnt_channel->enableReading();
}