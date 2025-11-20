#pragma once
#include<iostream>
#include<queue>
#include <pthread.h>
#include<unistd.h>
using namespace std;

// BlockingQueue 是一个面向线程的有界阻塞队列封装，利用 pthread 自旋互斥锁
// 与条件变量实现生产者-消费者之间的同步。
template<class T>
class BlockingQueue
{
    // 当调用方不指定容量时使用默认容量，避免无限制增长
    static const int default_num=10;
public:
    // extremum 表示队列最大容量，并据此计算“低/高水位线”唤醒阈值
    BlockingQueue(int extremum=default_num):extremum_(extremum)
    {
        low_water_=extremum_/5; //低水位线
        height_water_=4*extremum_/5; //高水位线
        pthread_mutex_init(&mutex_,nullptr); //互斥锁
        pthread_cond_init(&c_cond_,nullptr); //消费者条件变量
        pthread_cond_init(&p_cond_,nullptr); //生产者条件变量
    }
    // 消费者接口：当队列为空时阻塞等待，取出元素后若低于低水位线唤醒生产者
    T pop()
    {
        pthread_mutex_lock(&mutex_);
        //如果队列为空，则等待消费者条件变量
        if(q_.size()==0)
        {
            pthread_cond_wait(&c_cond_,&mutex_);
        }
        T task = q_.front();
        q_.pop();
        //如果队列大小小于等于低水位线，则唤醒生产者
        if(q_.size()<=low_water_) pthread_cond_signal(&p_cond_);
        pthread_mutex_unlock(&mutex_);
        return task;
    }

    // 生产者接口：队列已满则阻塞等待，放入元素后达到高水位线唤醒消费者
    void push(const T& task)
    {
        pthread_mutex_lock(&mutex_);
        //如果队列已满，则等待生产者条件变量
        if(q_.size()==extremum_)
        {
            pthread_cond_wait(&p_cond_,&mutex_);
        }
        // 没有达到极值，继续生产并入队
        q_.push(task);
        //如果队列大小大于等于高水位线，则唤醒消费者
        if(q_.size()>=height_water_) pthread_cond_signal(&c_cond_);
        pthread_mutex_unlock(&mutex_);
    }

    ~BlockingQueue()
    {
        pthread_mutex_destroy(&mutex_);
        pthread_cond_destroy(&c_cond_);
        pthread_cond_destroy(&p_cond_);
    }



private:
    queue<T> q_; //队列 
    int extremum_; //队列最大容量
    // mutex_ 保护共享队列，c_cond_ 唤醒消费者，p_cond_ 唤醒生产者
    pthread_mutex_t mutex_; //互斥锁
    pthread_cond_t c_cond_; //消费者条件变量
    pthread_cond_t p_cond_; //生产者条件变量

    int low_water_; //低水位线
    int height_water_; //高水位线

};