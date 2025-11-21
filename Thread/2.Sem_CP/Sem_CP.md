# 基于信号量的环形队列生产者-消费者模型

本文档总结 `Thread/2.Sem_CP` 示例的整体思路、关键逻辑与多线程测试方案，便于复盘和扩展。

## 设计思路
- **核心目标**：让多个生产者和多个消费者在共享缓冲区上安全、高效地协作，避免忙等。
- **数据结构**：使用定长环形队列（`RingQueue<T>`）作为缓冲区，利用取模实现首尾相接。
- **同步原语**：
  - `space_sem_`：空槽位数量，初始为队列容量。生产者在入队前必须先 `P(space_sem_)`，确保不溢出。
  - `data_sem_`：当前可消费的数据数量，初始为 0。消费者在出队前必须先 `P(data_sem_)`，确保不越界。
  - `pmutex_` / `cmutex_`：分别保护生产者下标和消费者下标，减少互斥粒度，避免两类线程互相阻塞。
- **延迟/调度**：生产者中刻意 `sleep(1)`，便于观察调度顺序和信号量阻塞唤醒过程。

## 常用信号量接口速览
| 接口 | 作用 | 备注 |
| --- | --- | --- |
| `int sem_init(sem_t *sem, int pshared, unsigned int value)` | 初始化信号量 `sem`，初值为 `value` | `pshared`=0 表示线程间共享 |
| `int sem_destroy(sem_t *sem)` | 销毁信号量，释放内核资源 | 程序结束前调用 |
| `int sem_wait(sem_t *sem)` | `P` 操作：当值 > 0 时 -- 并继续；否则阻塞 | 可被信号打断，需要重试 |
| `int sem_post(sem_t *sem)` | `V` 操作：++ 并唤醒可能等待的线程 | 无阻塞 |

## RingQueue 关键逻辑
```cpp
template<class T>
class RingQueue
{
private:
    std::vector<T> ring_;    // 环形缓冲区
    int cap_;                // 队列容量
    int producer_pos_;       // 生产者位置
    int consumer_pos_;       // 消费者位置
    sem_t space_sem_;        // 空间信号量（表示可用空间数量）
    sem_t data_sem_;         // 数据信号量（表示数据数量）
    pthread_mutex_t pmutex_; // 生产者互斥锁
    pthread_mutex_t cmutex_; // 消费者互斥锁
};
```

### push：生产者步骤
```cpp
void push(const T& in)
{
    sem_wait(&space_sem_);        // P(space)：等待空槽位
    pthread_mutex_lock(&pmutex_); // 只保护生产者下标和对应槽位

    ring_[producer_pos_] = in;
    producer_pos_ = (producer_pos_ + 1) % cap_;

    pthread_mutex_unlock(&pmutex_);
    sem_post(&data_sem_);         // V(data)：增加可消费数据
}
```

### pop：消费者步骤
```cpp
void pop(T* out)
{
    sem_wait(&data_sem_);         // P(data)：等待可消费数据
    pthread_mutex_lock(&cmutex_); // 只保护消费者下标和对应槽位

    *out = ring_[consumer_pos_];
    consumer_pos_ = (consumer_pos_ + 1) % cap_;

    pthread_mutex_unlock(&cmutex_);
    sem_post(&space_sem_);        // V(space)：释放空槽位
}
```

## 多生产者/多消费者测试方案
- `main.cc` 中创建 **5 个生产者线程**、**3 个消费者线程**，全部共享同一个 `RingQueue<int>`（容量 5）。
- 生产者使用 `std::atomic<int> mydata` 确保多线程下生成的编号唯一：
  ```cpp
  static std::atomic<int> mydata{1};
  int data = mydata.fetch_add(1);
  tp->push(data);
  ```
- 线程创建和等待：
  ```cpp
  std::vector<pthread_t> producers(5), consumers(3);
  for (...) pthread_create(&tid, nullptr, Producer, tp);
  for (...) pthread_create(&tid, nullptr, Consumer, tp);
  for (...) pthread_join(tid, nullptr); // 示例中为死循环，join 不会返回
  ```
- 在输出日志中若看到“先消费后生产”，实际上只是打印顺序被调度打乱——真正的数据流依旧遵循“生产 -> 信号量唤醒 -> 消费”的顺序。

## 可扩展/注意事项
- 如果需要正常退出，可改为“有限次生产/消费”，并在所有线程结束后释放 `RingQueue`。
- 要增加吞吐量，只需调大队列容量或减少 `sleep` 模拟延迟；若要观察竞争，可在消费者侧添加 `sleep`。
- `RingQueue` 目前假设单生产者线程持有 `pmutex_`，单消费者线程持有 `cmutex_`；如果需要更复杂的场景，还可以扩展成同一锁保护整个缓冲区，但吞吐会下降。