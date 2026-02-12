// #include <cstdint>
#include <cstdlib>
#include <strings.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <format>
#include <unistd.h>
#include <sys/epoll.h>
#include <cstring>
#include "util.hpp"
#include <fcntl.h>

#define MAX_EVENTS 1024
#define READ_BUFFER_SIZE 1024

void set_nonblocking(int fd) {
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int main() {
    // create a TCP socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0); // TCP socket
    errif(sockfd == -1, "socket create error");

    // bind the socket to an address and port
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(8888);
    errif(bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1, "bind error");

    // listen for incoming connections
    errif(listen(sockfd, SOMAXCONN) < 0, "listen error");
    std::cout << std::format("Server is listening on {}:{}", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port)) << '\n';

    // epoll
    int epollfd = epoll_create1(0);
    errif(epollfd == -1, "epoll create error");

    struct epoll_event events[MAX_EVENTS], ev;
    bzero(&events, sizeof(events));
    
    // 初始化监听socket的事件
    bzero(&ev, sizeof(ev));
    ev.data.fd = sockfd;
    ev.events = EPOLLIN | EPOLLET; // 监听可读事件，使用边缘触发模式
    set_nonblocking(sockfd);
    epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev); // 将监听socket添加到epoll实例中

    while (true) {
        int nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1); // 等待并获取就绪事件
        errif(nfds == -1, "epoll wait error");
        
        // 多个事件处理
        for (int i = 0; i < nfds; i++) {
            // accept a new client sock connection
            if (events[i].data.fd == sockfd) {
                struct sockaddr_in client_addr;
                bzero(&client_addr, sizeof(client_addr));
                socklen_t client_addr_len = sizeof(client_addr);

                int client_sockfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_addr_len);
                errif(client_sockfd == -1, "accept error");
                std::cout << std::format("Client {} connected. IP: {}, port: {}", client_sockfd, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port)) << '\n';

                bzero(&ev, sizeof(ev));
                ev.data.fd = client_sockfd;
                ev.events = EPOLLIN | EPOLLET; // 监听可读事件，使用边缘触发模式
                set_nonblocking(client_sockfd);
                epoll_ctl(epollfd, EPOLL_CTL_ADD, client_sockfd, &ev); // 将客户端socket添加到epoll实例中
            }
            // 可读事件
            else if (events[i].events & EPOLLIN) {
                char buf[READ_BUFFER_SIZE]; // buffer to hold incoming data
                // 非阻塞IO，边缘触发模式需要循环读取直到没有数据可读
                while (true) {
                    bzero(&buf, sizeof(buf));
                    ssize_t read_bytes = read(events[i].data.fd, buf, sizeof(buf)); //从客户端socket读到缓冲区，返回已读数据大小
                    if (read_bytes > 0) {
                        std::cout << std::format("Received from client {}: {}", events[i].data.fd, buf) << '\n';
                        write(events[i].data.fd, buf, read_bytes); // echo back to client
                    }
                    // 客户端正常中断
                    else if (read_bytes == -1 && errno == EINTR) {
                        std::cout << "Continue Reading" << '\n';
                        continue; // interrupted by signal, try again
                    } 
                    // 非阻塞IO，这个条件表示数据全部读取完毕
                    else if (read_bytes == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
                        std::cout << std::format("Finished reading from client {}. No more data to read. errno: {}", events[i].data.fd, errno) << '\n';
                        break; // no more data to read
                    }
                    // EOF，客户端关闭连接
                    else if (read_bytes == 0) {
                        std::cout << std::format("Client {} disconnected.", events[i].data.fd) << '\n';
                        close(events[i].data.fd); // 关闭客户端socket
                        break;
                    }
                }
            }
            else {
                std::cout << std::format("Unexpected event for client {}. events: {}", events[i].data.fd, events[i].events) << '\n';
            }
        }
    }
    close(sockfd);
    return 0;
}