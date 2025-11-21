#pragma once
#include <pthread.h>
#include <unistd.h>

#include <functional>
#include <iostream>
#include <queue>
#include <stdexcept>
#include <vector>

class ThreadPool
{
public:
    using Task = std::function<void()>; // 统一封装任意可调用对象（lambda/函数/仿函数）

    // thread_num：预先创建的工作线程数量
    explicit ThreadPool(size_t thread_num = kDefaultThreads) //禁止隐式转换
        : stop_(false)
    {
        if (thread_num == 0)    // 防止创建0个线程
        {
            throw std::invalid_argument("thread_num must be > 0");
        }
        pthread_mutex_init(&mutex_, nullptr); // 保护任务队列与 stop_ 状态
        pthread_cond_init(&cond_, nullptr);   // 队列为空时阻塞工作线程
        try
        {
            threads_.resize(thread_num);
            for (size_t i = 0; i < thread_num; ++i)
            {
                int ret = pthread_create(&threads_[i], nullptr, ThreadRoutine, this);
                if (ret != 0)
                {
                    throw std::runtime_error("pthread_create failed");
                }
            }
        }
        catch (...)
        {
            stop_ = true;                 // 标记为停止，避免新任务入队
            pthread_cond_broadcast(&cond_); // 唤醒已阻塞线程，使其尽快退出
            for (size_t i = 0; i < threads_.size(); ++i)
            {
                if (threads_[i])
                {
                    pthread_join(threads_[i], nullptr);
                }
            }
            pthread_mutex_destroy(&mutex_);
            pthread_cond_destroy(&cond_);
            throw;
        }
    }

    ThreadPool(const ThreadPool &) = delete;
    ThreadPool &operator=(const ThreadPool &) = delete;

    ~ThreadPool()
    {
        shutdown(); // 主动关闭线程池,防止资源泄漏,具体原因见shutdown函数
        pthread_mutex_destroy(&mutex_);
        pthread_cond_destroy(&cond_);
    }

    // 将任务压入队列，交由线程池调度执行
    void enqueue(Task task)
    {
        pthread_mutex_lock(&mutex_);
        if (stop_)
        {
            pthread_mutex_unlock(&mutex_);
            throw std::runtime_error("enqueue on stopped ThreadPool");
        }
        tasks_.push(std::move(task));   // 将任务压入队列,这里使用std::move是因为task是一个右值引用,可以避免拷贝构造
        pthread_mutex_unlock(&mutex_);
        pthread_cond_signal(&cond_); // 唤醒一个等待线程去执行任务
    }

    // 主动关闭线程池（示例中 main 也会调用）
    void shutdown()
    {
        pthread_mutex_lock(&mutex_);
        if (stop_)  // 如果线程池已经停止,则直接返回
        {
            pthread_mutex_unlock(&mutex_);
            return;
        }
        stop_ = true;  // 标记线程池为停止状态
        pthread_mutex_unlock(&mutex_); // 解锁,防止死锁
        pthread_cond_broadcast(&cond_); // 唤醒所有等待线程去执行任务

        for (auto &tid : threads_)
        {
            if (tid)
            {
                pthread_join(tid, nullptr);
                tid = 0;
            }
        }
    }

private:
    // pthread_create 需要的静态函数，将执行权转发给成员函数
    static void *ThreadRoutine(void *arg)
    {
        ThreadPool *pool = static_cast<ThreadPool *>(arg);
        pool->workerLoop();
        return nullptr;
    }

    // 线程主循环：不断从队列取任务执行，直到 stop_ 并且队列为空
    void workerLoop()
    {
        while (true)
        {
            Task task;
            pthread_mutex_lock(&mutex_);
            while (!stop_ && tasks_.empty())  // 如果线程池没有停止,并且队列为空,则阻塞等待
            {
                pthread_cond_wait(&cond_, &mutex_); // 队列空时阻塞，自动释放锁
            }
            if (stop_ && tasks_.empty()) // 如果线程池已经停止,并且队列为空,则退出线程
            {
                pthread_mutex_unlock(&mutex_);
                break;
            }
            // 如果线程池没有停止,并且队列不为空,则取出任务
            task = std::move(tasks_.front());
            tasks_.pop();
            pthread_mutex_unlock(&mutex_);

            try
            {
                task();
            }
            catch (const std::exception &e) // 捕获任务异常，防止线程直接退出
            {
                std::cerr << "[ThreadPool] task exception: " << e.what() << std::endl;
            }
            catch (...)
            {
                std::cerr << "[ThreadPool] task unknown exception" << std::endl;
            }
        }
    }

private:
    static constexpr size_t kDefaultThreads = 4;

    std::vector<pthread_t> threads_; // 固定线程集合
    std::queue<Task> tasks_;         // 待执行任务队列
    pthread_mutex_t mutex_;          // 保护 tasks_ 与 stop_ 的互斥量
    pthread_cond_t cond_;            // 通知工作线程有新任务
    bool stop_;                      // 标记线程池是否停止接收新任务
};
