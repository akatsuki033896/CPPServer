#include <cstdlib>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <format>
#include "InetAddress.hpp"
#include "Buffer.hpp"
#include "Socket.hpp"

#define BUFFER_SIZE 1024

int main() {
    Socket *sock = new Socket();
    InetAddress *addr = new InetAddress("127.0.0.1", 8888);
    sock->connect(addr);

    int sockfd = sock->get_fd();

    Buffer *sendBuffer = new Buffer();
    Buffer *readBuffer = new Buffer();

    while (true) {
        std::cout << "Enter message to send (type 'q' to quit): ";
        sendBuffer->getline(); // 从标准输入读取一行数据到缓冲区
        ssize_t write_bytes = write(sockfd, sendBuffer->c_str(), sendBuffer->size()); // 将缓冲区数据写入socket
        if (strcmp(sendBuffer->c_str(), "q") == 0) {
            break;
        }
        if (write_bytes == -1) {
            std::cout << "Socket already disconnected." << '\n';
            break;
        }

        int already_read = 0;
        char buf[BUFFER_SIZE];
        while (true) {
            bzero(&buf, sizeof(buf));
            ssize_t read_bytes = read(sockfd, buf, sizeof(buf)); // 从socket读取数据到缓冲区，返回已读数据大小
            if (read_bytes > 0) {
                readBuffer->append(buf, read_bytes);
                already_read += read_bytes;
            }
            else if (read_bytes == 0) {
                std::cout << "Server closed the connection." << '\n';
                exit(EXIT_SUCCESS);
            } // EOF
            
            if (already_read >= sendBuffer->size()) {
                std::cout << std::format("Message from server: {}", readBuffer->c_str()) << '\n';
                break;
            }
        }
        readBuffer->clear();
    }
    
    delete sendBuffer;
    delete readBuffer;
    delete addr;
    delete sock;
    return 0;
}