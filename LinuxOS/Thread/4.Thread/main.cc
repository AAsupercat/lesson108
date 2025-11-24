#include <iostream>
#include <vector>
#include <chrono>
#include <thread>

#include "Thread.hpp"

int main()
{
    std::cout << "启动线程池封装测试..." << std::endl;

    Thread t1([]() {
        std::cout << "[t1] 在线程中打印信息" << std::endl;
    }, "printer");

    Thread t2([]() {
        std::cout << "[t2] 模拟耗时工作开始" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "[t2] 模拟耗时工作结束" << std::endl;
    });

    t1.start();
    t2.start();

    t1.join();
    t2.join();

    std::cout << "创建多个线程，演示 joinable/detach..." << std::endl;
    std::vector<Thread> threads;
    threads.reserve(3);  // 预分配空间，避免扩容时的移动
    for (int i = 0; i < 3; ++i)
    {
        threads.emplace_back([i]() {
            std::cout << "[worker-" << i << "] 在线程 " << pthread_self() << " 中运行" << std::endl;
        });
        threads.back().start();
    }

    for (auto &t : threads)
    {
        if (t.joinable())
        {
            t.join();
        }
    }

    std::cout << "所有线程执行完毕" << std::endl;
    return 0;
}

