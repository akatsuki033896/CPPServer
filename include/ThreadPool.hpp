// 当Channel类有事件需要处理时，将这个事件处理添加到线程池，主线程EventLoop就可以继续进行事件循环，而不在某个socket fd上的事件处理
#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <type_traits>
#include <utility>
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
    
    // void add(std::function<void()> func);
    
    // 使用右值移动、完美转发等阻止拷贝，使用模板往任务队列添加不同类型的函数
    template<class F, class... Args>
    auto addTask(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>> {
        
        // using return_type = typename std::result_of<T(Args...)>::type; // 返回值类型     
        // auto task = std::make_shared<std::packaged_task<return_type()> > (
        //     std::bind(std::forward<T>(t), std::forward<Args>(args)...)
        // ); // 完美转发参数
        // std::future<return_type> res = task->get_future(); // 使用期约

        // typename std::result_of<T(Args...)>::type 已被Cpp20删除, 应使用 std::invoke_result_t

        using return_type = std::invoke_result_t<F, Args...>;
        auto task = std::make_shared<std::packaged_task<return_type()> > (
            [func = std::forward<F>(f), ...params = std::forward<Args>(args)] () mutable {
                return std::invoke(std::move(func), std::move(params)...);
            }
        );
        std::future<return_type> res = task->get_future(); // 使用期约
        {
            std::unique_lock<std::mutex> lock(tasks_mtx);
            if (stop) {
                throw std::runtime_error("ThreadPool already stop, can't add task any more");
            }
            tasks.emplace([task]() {(*task)();} ); // 添加到任务队列
        }
        cv.notify_one(); // 通知一次条件变量
        return res; // 返回一个期约
    }
};