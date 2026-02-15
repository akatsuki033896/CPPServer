#include <cerrno>
#include <cstdlib>
#include <iostream>
#include <format>
#include <unistd.h>
#include "include/Epoll.hpp"
#include "include/InetAddress.hpp"
#include <fcntl.h>
#include "include/Socket.hpp"
#include <cstring>
#include "include/Channel.hpp"

#define READ_BUFFER_SIZE 1024

void set_nonblocking(int fd) {
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

void handleReadEvent(int sockfd) {
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

int main() {
    Socket *serv_sock = new Socket();
    InetAddress *serv_addr = new InetAddress("127.0.0.1", 8888);
    serv_sock->bind(serv_addr);
    serv_sock->listen();
    std::cout << "Server listening on 127.0.0.1:8888" << '\n';

    Epoll *ep = new Epoll();
    Channel *serv_channel = new Channel(ep, serv_sock->get_fd());
    serv_channel->enableReading();
    
    while (true) {
        std::vector<Channel*> activeChannels = ep->poll(); // 等待并获取就绪事件
        int nfds = activeChannels.size();
        for (int i = 0; i < nfds; i++) {
            int chfd = activeChannels[i]->getFd();

            // 新客户端连接
            if (chfd == serv_sock->get_fd()) {
                InetAddress *client_addr = new InetAddress();
                Socket *clnt_sock = new Socket(serv_sock->accept(&client_addr->addr, &client_addr->addr_len));
                std::cout << std::format("Client {} connected. IP: {}, port: {}", clnt_sock->get_fd(), inet_ntoa(client_addr->addr.sin_addr), ntohs(client_addr->addr.sin_port)) << '\n';
                
                clnt_sock->set_nonblocking();
                Channel *clnt_channel = new Channel(ep, clnt_sock->get_fd());
                clnt_channel->enableReading();
            }
            // 可读事件
            else if (activeChannels[i]->getRevents() & EPOLLIN) {
                // 处理客户端数据
                handleReadEvent(activeChannels[i]->getFd());
            }
            else {
                // 其他事件（如错误事件）可以在这里处理
                std::cout << std::format("Unexpected event for fd {}: events: {}", activeChannels[i]->getFd(), activeChannels[i]->getEvents()) << '\n';
            }
        }
    }
    delete serv_addr;
    delete serv_sock;
    return 0;
}