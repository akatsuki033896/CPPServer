#include "ThreadPool.hpp"
#include <iostream>
#include <chrono>
#include <atomic>
#include <thread>

int main() {
    ThreadPool pool(std::thread::hardware_concurrency());

    // 压力测试
    // std::atomic<int> sum{0};

    // for (int i = 0; i < 10000; ++i) {
    //     pool.add([&sum]() {
    //         sum++;
    //     });
    // }
    
    // for (int i = 0; i < 100; ++i) {
    //     pool.add([i]() {
    //         std::cout << "Processing socket event " << i << "\n";
    //         std::this_thread::sleep_for(std::chrono::milliseconds(50));
    //     });
    // }

    // 并发正确性    
    std::atomic<int> sum{0};
    {
        ThreadPool pool(std::thread::hardware_concurrency());

        for (int i = 0; i < 100000; ++i) {
            pool.add([&sum]() {
                sum.fetch_add(1, std::memory_order_relaxed);
            });
        }
    } // 在这里析构，等待所有任务完成

    std::cout << "Final sum = " << sum << std::endl; // 应 = 100000
    return 0;
}