#pragma once

#include<iostream>
#include<stdexcept>
#include<functional>
#include<vector>
#include<queue>
#include<pthread.h>

static constexpr size_t DefaultThreads=4;
class ThreadPool
{
    using Task = std::function<void()>;
public:
    explicit ThreadPool(size_t thread_num = DefaultThreads)
    :stop_(false)
    {
        if(thread_num==0)
        {
            throw std::invalid_argument("thread_num must be > 0"); //表示逻辑错误
        }
        pthread_mutex_init(&mutex_,nullptr);
        pthread_cond_init(&cond_,nullptr);
        try
        {
            for(size_t i=0;i<thread_num;i++)
            {
                threads_.resize(thread_num);
                //传入参数为ThreadPool* ，以便于使用其他成员函数
                int ret = pthread_create(&threads_[i],nullptr,ThreadRoutine,this);
                if(ret!=0)
                {
                    throw std::runtime_error("pthread_create failed"); //运行错误
                }
            }
        }
        catch(...)
        {
            //如果启动失败，我们希望删除已经创建的线程，打上标记，销毁mutex和cond
            stop_=true;
            pthread_cond_broadcast(&cond_);
            for(size_t i=0;i<thread_num;i++)
            {
                //WokerLoop中就必须判断是否停止线程池
                pthread_join(threads_[i],nullptr);
            }
            pthread_mutex_destroy(&mutex_);
            pthread_cond_destroy(&cond_);
        }
        
    }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator = (const ThreadPool&) = delete;

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
        tasks_.push(std::move(task));   //右值引用传参
        pthread_cond_signal(&cond_);
        pthread_mutex_unlock(&mutex_);

        return;
    }

    //主动关闭，重置标志位stop_,并且主动释放线程池线程;
    void shutdown()
    {
        pthread_mutex_lock(&mutex_);
        if(stop_)
        {
            pthread_mutex_unlock(&mutex_);
            return;
        }
        //stop_如果没有停止
        stop_=true;
        pthread_mutex_unlock(&mutex_);
        pthread_cond_broadcast(&cond_);
        for(auto& tid:threads_)
        {
            pthread_join(tid,nullptr);
            tid=0;
        }
        return ;
    }

private:
//必须是静态成员函数，因为pthread_create()要求的是void* ThreadRoutine(void* args)函数
    //void* ThreadRoutine(/*this,*/void* args) 
    static void* ThreadRoutine(void* args)
    {
        ThreadPool* pool = static_cast<ThreadPool*>(args);
        pool->WokerLoop();
        return nullptr;
    }

    void WokerLoop()
    {
        while(true)
        {
            Task task;
            pthread_mutex_lock(&mutex_);
            //什么时候需要等待？线程没有结束,并且任务队列不为空
            while(!stop_&&tasks_.empty())
            {
                pthread_cond_wait(&cond_,&mutex_); //多线程等待，等待条件满足,被唤醒时归还锁
            }
            if(stop_)
            {
                pthread_mutex_unlock(&mutex_);
                break;
            }
            //线程池没有停止，就要处理任务
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

    std::vector<pthread_t> threads_;
    std::queue<Task> tasks_;
    pthread_mutex_t mutex_;
    pthread_cond_t cond_;
    bool stop_;
};