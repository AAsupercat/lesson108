// 基于 RingQueue 的简单生产者-消费者测试程序
#include <iostream>
#include <vector>
#include <atomic>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "RingQueue.hpp"

static std::atomic<int> mydata{1}; // 多生产者下安全递增
void *Producer(void *args)
{
    RingQueue<int> *tp = static_cast<RingQueue<int> *>(args);
    while (true)
    {
        sleep(1);
        int data = mydata.fetch_add(1); // 返回旧值并自增，确保唯一
        tp->push(data);
        std::cout << "生产者生产了一个数据：" << data << std::endl;
    }
    return nullptr;
}

void *Consumer(void *args)
{
    RingQueue<int> *tp = static_cast<RingQueue<int> *>(args);
    while (true)
    {
        int tmp = 0;
        tp->pop(&tmp);
        std::cout << "消费者消费了一个数据：" << tmp << std::endl;
    }
    return nullptr;
}

int main()
{
    constexpr int producer_cnt = 5;
    constexpr int consumer_cnt = 3;

    RingQueue<int> *tp = new RingQueue<int>(5);

    std::vector<pthread_t> producers(producer_cnt);
    std::vector<pthread_t> consumers(consumer_cnt);

    for (auto &tid : producers)
    {
        pthread_create(&tid, nullptr, Producer, tp);
    }

    for (auto &tid : consumers)
    {
        pthread_create(&tid, nullptr, Consumer, tp);
    }

    for (auto &tid : producers)
    {
        pthread_join(tid, nullptr);
    }
    for (auto &tid : consumers)
    {
        pthread_join(tid, nullptr);
    }

    delete tp;
    return 0;
}