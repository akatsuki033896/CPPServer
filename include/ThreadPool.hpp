// 当Channel类有事件需要处理时，将这个事件处理添加到线程池，主线程EventLoop就可以继续进行事件循环，而不在某个socket fd上的事件处理
#pragma once

#include <condition_variable>
#include <functional>
#include <queue>
#include <vector>
#include <thread>


class ThreadPool {
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;
    std::mutex tasks_mtx;
    std::condition_variable cv;
    bool stop = false;
public:
    ThreadPool(int size = std::thread::hardware_concurrency()); 
    ~ThreadPool();

    // 管理线程资源，禁止拷贝
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    
    void add(std::function<void()> func);
};