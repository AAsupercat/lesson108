#pragma once
#include <pthread.h>

#include <functional>
#include <stdexcept>
#include <string>

class Thread
{
public:
    using ThreadFunc = std::function<void()>;

    explicit Thread(ThreadFunc func, std::string name = "")
        : func_(std::move(func)),
          name_(std::move(name)),
          started_(false),
          joined_(false)
    {
        if (!func_)
        {
            throw std::invalid_argument("Thread func must be valid");
        }
    }

    Thread(const Thread &) = delete;
    Thread &operator=(const Thread &) = delete;

    // 移动构造函数
    // 注意：只能移动未启动的线程，已启动的线程不应该被移动
    Thread(Thread &&other) noexcept
        : func_(std::move(other.func_)),
          name_(std::move(other.name_)),
          tid_(other.tid_),
          started_(other.started_),
          joined_(other.joined_)
    {
        // 如果other已经启动，不应该移动（但为了支持vector扩容，我们允许移动）
        // 这种情况下，移动后的对象不应该再使用
        other.tid_ = 0;
        other.started_ = false;
        other.joined_ = false;
        // 注意：如果other已经启动，func_被移动后，正在运行的线程会出问题
        // 所以实际使用中，应该在启动前完成所有移动操作
    }

    // 移动赋值运算符
    Thread &operator=(Thread &&other) noexcept
    {
        if (this != &other)
        {
            if (started_ && !joined_)
            {
                pthread_detach(tid_);
            }
            func_ = std::move(other.func_);
            name_ = std::move(other.name_);
            tid_ = other.tid_;
            started_ = other.started_;
            joined_ = other.joined_;
            other.tid_ = 0;
            other.started_ = false;
            other.joined_ = false;
        }
        return *this;
    }

    ~Thread()
    {
        if (started_ && !joined_)
        {
            pthread_detach(tid_);
        }
    }

    void start()
    {
        if (started_)
        {
            throw std::runtime_error("Thread already started");
        }
        started_ = true;

        int ret = pthread_create(&tid_, nullptr, &Thread::ThreadRoutine, this);
        if (ret != 0)
        {
            started_ = false;
            throw std::runtime_error("pthread_create failed");
        }
    }

    void join()
    {
        if (!started_)
        {
            throw std::runtime_error("Thread not started");
        }
        if (joined_)
        {
            throw std::runtime_error("Thread already joined");
        }
        joined_ = true;
        pthread_join(tid_, nullptr);
    }

    void detach()
    {
        if (!started_) // 如果线程没有启动,则抛出异常
        {
            throw std::runtime_error("Thread not started");
        }
        if (joined_)  // 如果线程已经join,则抛出异常
        {
            throw std::runtime_error("Thread already joined");
        }
        pthread_detach(tid_); // 分离线程
        joined_ = true;
    }

    bool joinable() const
    {
        return started_ && !joined_; // 如果线程已经启动,并且没有被join,则返回true
    }

    pthread_t id() const
    {
        return tid_;
    }

    const std::string &name() const
    {
        return name_;
    }

private:
    static void *ThreadRoutine(void *arg)
    {
        Thread *self = static_cast<Thread *>(arg);
        self->func_();
        return nullptr;
    }

    ThreadFunc func_;   // 线程函数
    std::string name_;  // 线程名称
    pthread_t tid_;     // 线程id
    bool started_;      // 标记线程是否被启动
    bool joined_;       // 标记线程是否被join
};

