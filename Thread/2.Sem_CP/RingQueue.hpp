#pragma once
// 环形队列 + 信号量 实现的生产者消费者模型
// 仅负责并发安全的入队/出队，不关心具体业务逻辑
#include <iostream>
#include <vector>
#include <semaphore.h>
#include <unistd.h>

static const int defaultcap = 5; // 默认队列容量

template <class T>
class RingQueue
{
private:
    // P 操作：申请信号量（--），可能阻塞
    void P(sem_t &sem)
    {
        sem_wait(&sem);
    }

    // V 操作：归还信号量（++），可能唤醒等待线程
    void V(sem_t &sem)
    {
        sem_post(&sem);
    }

    // 互斥锁封装：加锁
    void Lock(pthread_mutex_t &mutex)
    {
        pthread_mutex_lock(&mutex);
    }

    // 互斥锁封装：解锁
    void UnLock(pthread_mutex_t &mutex)
    {
        pthread_mutex_unlock(&mutex);
    }

public:
    // num：环形队列的最大容量（槽位数）
    RingQueue(int num = defaultcap)
        : ring_(num), cap_(num),
          p_pos_(0), c_pos_(0)
    {
        // space_sem_：可用空间的数量，一开始有 cap_ 个空槽
        sem_init(&space_sem_, 0, cap_);
        // data_sem_：已有数据的数量，一开始为 0
        sem_init(&data_sem_, 0, 0);
        // 生产者/消费者各自使用一个互斥锁，只保护各自的下标
        pthread_mutex_init(&pmutex_, nullptr);
        pthread_mutex_init(&cmutex_, nullptr);
    }

    // 生产者接口：向环形队列中放入一个元素
    void push(const T &in)
    {
        // 1. 先申请一个“空槽位”资源；如果队列满了，会在这里阻塞
        P(space_sem_);

        // 2. 只保护 p_pos_ 和 ring_ 对应位置的写操作
        Lock(pmutex_);
        ring_[p_pos_] = in;

        // 3. 循环队列下标向前移动
        p_pos_ = (p_pos_ + 1) % cap_;
        UnLock(pmutex_);

        // 4. 归还一个“数据”资源，唤醒可能在等待数据的消费者
        V(data_sem_);
    }

    // 消费者接口：从环形队列中取出一个元素
    void pop(T *out)
    {
        // 1. 先申请一个“数据”资源；如果当前没有数据，会在这里阻塞
        P(data_sem_);

        // 2. 只保护 c_pos_ 和 ring_ 对应位置的读操作
        Lock(cmutex_);
        *out = ring_[c_pos_];

        // 3. 循环队列下标向前移动
        c_pos_ = (c_pos_ + 1) % cap_;
        UnLock(cmutex_);

        // 4. 归还一个“空槽位”资源，唤醒可能在等待空间的生产者
        V(space_sem_);
    }

    ~RingQueue()
    {
        sem_destroy(&space_sem_);
        sem_destroy(&data_sem_);
        pthread_mutex_destroy(&pmutex_);
        pthread_mutex_destroy(&cmutex_);
    }

private:
    std::vector<T> ring_;     // 真正存放数据的环形缓冲区
    int cap_;                 // 缓冲区容量
    int p_pos_;               // 生产者当前写入位置
    int c_pos_;               // 消费者当前读取位置

    sem_t space_sem_;         // 空间信号量：还剩多少“空槽位”
    sem_t data_sem_;          // 数据信号量：当前有多少“可消费数据”
    pthread_mutex_t pmutex_;  // 保护生产者下标/写入操作
    pthread_mutex_t cmutex_;  // 保护消费者下标/读取操作
};