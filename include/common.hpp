#pragma once

class Acceptor;
class TCPConnection;
class EventLoop;
class ThreadPool;
class Channel;
class Buffer;
class Epoller;
class Server;

// 禁止拷贝
#define DISALLOW_COPY(cname) \
    cname(const cname&) = delete; \
    cname& operator=(const cname&) = delete;

// 禁止移动
#define DISALLOW_MOVE(cname) \
    cname(cname &&) = delete;  \
    cname &operator=(cname &&) = delete;

// 禁止拷贝和移动
#define DISALLOW_COPY_AND_MOVE(cname) \
    DISALLOW_COPY(cname);               \
    DISALLOW_MOVE(cname);
