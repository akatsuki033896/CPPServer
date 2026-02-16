#include "ThreadPool.hpp"
#include <functional>
#include <mutex>
#include <stdexcept>
#include <thread>

ThreadPool::ThreadPool(int size) {
    for (int i = 0; i < size; i++) {
        // 在 vector 尾部就地构造，效率比构造后 push_back 高
        threads.emplace_back(std::thread([this] () {
            while (true) {
                std::function<void()> task;
                // 该作用域用于 unique_lock
                {
                    std::unique_lock<std::mutex> lock(tasks_mtx);
                    cv.wait(lock, [this] () {
                        return stop || !tasks.empty();
                    }); //等待条件变量，条件为任务队列不为空或线程池停止
                    if (stop && tasks.empty()) {
                        return;
                    } //任务队列为空并且线程池停止，退出线程
                    
                    // 从队列取出任务
                    task = tasks.front();
                    tasks.pop();
                }
                task(); // 执行任务
            }
        }));
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(tasks_mtx);
        stop = true;
    }
    cv.notify_all();
    for (std::thread& th : threads) {
        if (th.joinable()) {
            th.join();
        }
    }
}

void ThreadPool::add(std::function<void()> func) {
    {
        std::unique_lock<std::mutex> lock(tasks_mtx);
        if (stop) {
            throw std::runtime_error("ThreadPool already stop, can't add task any more");
        }
        tasks.emplace(func);
    }
    cv.notify_one();
}