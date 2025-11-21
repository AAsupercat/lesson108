#include <iostream>
#include <chrono>
#include <thread>

#include "ThreadPool.hpp"

int main()
{
    ThreadPool pool(5);

    for (int i = 0; i < 15; ++i)
    {
        pool.enqueue([i]() {
            std::cout << "任务 " << i << " 开始执行，线程ID: " << pthread_self() << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(300 + (i % 3) * 200));
            std::cout << "任务 " << i << " 执行完毕" << std::endl;
        });
    }

    std::this_thread::sleep_for(std::chrono::seconds(3));

    pool.shutdown();
    return 0;
}

