# 线程池设计与实现概述

## 目标
- 复用固定数量的工作线程执行任意可调用任务，避免频繁创建/销毁线程的开销。
- 通过任务队列 + 条件变量实现「生产者-消费者」式调度，保证任务串行出队、并行执行。
- 在程序退出或显式调用 `shutdown` 时，能够让所有工作线程有序收尾。

## 核心组件
- `ThreadPool::Task`：`std::function<void()>` 封装的任务，可捕获任意上下文。
- `std::queue<Task> tasks_`：等待执行的任务队列。
- `pthread_mutex_t mutex_`：保护队列与状态 `stop_`。
- `pthread_cond_t cond_`：当队列为空时让工作线程休眠，push 时唤醒。
- `std::vector<pthread_t> threads_`：固定数量的工作线程。

## 运行流程
1. **构造函数**
   - 初始化互斥量/条件变量。
   - 按线程数量创建工作线程，每个线程执行 `ThreadRoutine`。
2. **任务入队 (`enqueue`)**
   - 先加锁，检查是否已经停止。
   - 将任务压入队列，解锁并 `pthread_cond_signal` 唤醒一个工作线程。
3. **线程例程 (`workerLoop`)**
   - 在队列为空且未停止时等待条件变量。
   - 若收到停止信号且队列已空，则安全退出循环。
   - 取出任务执行，异常被捕获打印日志，避免线程崩溃。
4. **停止 (`shutdown`)**
   - 设置 `stop_ = true`，`pthread_cond_broadcast` 唤醒所有线程。
   - `pthread_join` 等待线程退出，防止资源泄漏。

## 示例用法（见 `main.cc`）
```cpp
ThreadPool pool(5);
for (int i = 0; i < 15; ++i)
{
    pool.enqueue([i] {
        std::cout << "任务 " << i << " 执行" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    });
}
std::this_thread::sleep_for(std::chrono::seconds(3));
pool.shutdown();
```

## 注意事项
- 任务中若抛出异常会被捕获打印，避免线程退出。
- `enqueue` 在 `shutdown` 之后调用会抛出异常，防止任务丢失。
- 示例里显式调用 `shutdown`，即使不调用析构函数也会自动收尾，但提前停止能让主线程更快结束。

