#include <cstdlib>
#include <functional>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <format>
#include "InetAddress.hpp"
#include "Buffer.hpp"
#include "Socket.hpp"
#include "ThreadPool.hpp"

#define BUFFER_SIZE 1024

void client(int msgs, int wait) {
    Socket *sock = new Socket();
    InetAddress *addr = new InetAddress("127.0.0.1", 8888);
    sock->connect(addr);

    int sockfd = sock->get_fd();

    Buffer *sendBuffer = new Buffer();
    Buffer *readBuffer = new Buffer();

    sleep(wait);
    int count = 0;

    while (count < msgs) {
        // std::cout << "Enter message to send (type 'q' to quit): ";
        sendBuffer->setBuf("I'm client."); // 从标准输入读取一行数据到缓冲区
        ssize_t write_bytes = write(sockfd, sendBuffer->c_str(), sendBuffer->size()); // 将缓冲区数据写入socket
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
    delete addr;
    delete sock;
    return;
}

int main(int argc, char* argv[]) {
    int threads = 100;
    
    ThreadPool* poll = new ThreadPool(threads);
    std::function<void()> func = [] () {
        client(100, 0); // msgs = 100, wait = 0
    };
    for (int i = 0; i < threads; i++) {
        poll->add(func);
    }
    delete poll;
    return 0;
}