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
    using Task = std::function<void()>;

    explicit ThreadPool(size_t thread_num = kDefaultThreads)
        : stop_(false)
    {
        if (thread_num == 0)
        {
            throw std::invalid_argument("thread_num must be > 0");
        }
        pthread_mutex_init(&mutex_, nullptr);
        pthread_cond_init(&cond_, nullptr);
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
            stop_ = true;
            pthread_cond_broadcast(&cond_);
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
        shutdown();
        pthread_mutex_destroy(&mutex_);
        pthread_cond_destroy(&cond_);
    }

    void enqueue(Task task)
    {
        pthread_mutex_lock(&mutex_);
        if (stop_)
        {
            pthread_mutex_unlock(&mutex_);
            throw std::runtime_error("enqueue on stopped ThreadPool");
        }
        tasks_.push(std::move(task));
        pthread_mutex_unlock(&mutex_);
        pthread_cond_signal(&cond_);
    }

    void shutdown()
    {
        pthread_mutex_lock(&mutex_);
        if (stop_)
        {
            pthread_mutex_unlock(&mutex_);
            return;
        }
        stop_ = true;
        pthread_mutex_unlock(&mutex_);
        pthread_cond_broadcast(&cond_);

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
    static void *ThreadRoutine(void *arg)
    {
        ThreadPool *pool = static_cast<ThreadPool *>(arg);
        pool->workerLoop();
        return nullptr;
    }

    void workerLoop()
    {
        while (true)
        {
            Task task;
            pthread_mutex_lock(&mutex_);
            while (!stop_ && tasks_.empty())
            {
                pthread_cond_wait(&cond_, &mutex_);
            }
            if (stop_ && tasks_.empty())
            {
                pthread_mutex_unlock(&mutex_);
                break;
            }
            task = std::move(tasks_.front());
            tasks_.pop();
            pthread_mutex_unlock(&mutex_);

            try
            {
                task();
            }
            catch (const std::exception &e)
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

    std::vector<pthread_t> threads_;
    std::queue<Task> tasks_;
    pthread_mutex_t mutex_;
    pthread_cond_t cond_;
    bool stop_;
};
