#pragma once
#include<pthread.h>

class LockGuard
{
public:
    LockGuard(pthread_mutex_t* lock):lock_(lock)
    {
        pthread_mutex_lock(lock_);
    }
    ~LockGuard()
    {
        pthread_mutex_unlock(lock_);
    }
private:
    pthread_mutex_t* lock_;
};