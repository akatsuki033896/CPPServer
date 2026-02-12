#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include "util.hpp"

#define BUFFER_SIZE 1024

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0); // TCP socket
    errif(sockfd == -1, "socket create error");

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(8888);

    errif(connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1, "connect error");

    while (true) {
        char buf[BUFFER_SIZE];
        bzero(&buf, sizeof(buf));
        std::cout << "Enter message to send (type 'q' to quit): ";
        std::cin.getline(buf, sizeof(buf)); // 从标准输入读取一行数据到缓冲区
        ssize_t write_bytes = write(sockfd, buf, sizeof(buf)); // 将缓冲区数据写入socket
        if (strcmp(buf, "q") == 0) {
            break;
        }
        if (write_bytes < 0) {
            errif(write_bytes < 0, "write error");
            break;
        }
        
        bzero(&buf, sizeof(buf));
        ssize_t read_bytes = read(sockfd, buf, sizeof(buf)); // 从socket读取数据到缓冲区，返回已读数据大小
        if (read_bytes > 0) {
            std::cout << "Received from server: " << buf << '\n';
        }
        else if (read_bytes == 0) {
            std::cout << "Server closed the connection." << '\n';
            break;
        } // EOF
        else if (read_bytes == -1) {
            close(sockfd);
            errif(read_bytes == -1, "read error");
            break;
        }
    }
    close(sockfd);
    return 0;
}