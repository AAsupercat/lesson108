# pthread 封装线程类

## 设计目标
- 提供类似 `std::thread` 的 C++ 面向对象接口，隐藏直接调用 `pthread_create/pthread_join` 的细节。
- 线程函数统一使用 `std::function<void()>`，方便接收任意可调用对象。
- 自动管理线程生命周期：忘记 `join` 时自动 `detach`，避免资源泄漏。
- 提供基础接口：`start()`、`join()`、`detach()`、`joinable()`、`id()`、`name()`。

## 核心实现要点
- `Thread` 构造时保存任务和可选线程名，不立即创建 pthread。
- `start()`：调用 `pthread_create`，工作入口统一为静态成员 `ThreadRoutine`，内部回调用户函数。
- `join()`：确保只调一次，否则抛出异常；`detach()` 用于将线程与主线程分离。
- 析构函数若发现线程已启动但尚未 `join/ detach`，自动调用 `pthread_detach`。
- 通过布尔标记 `started_` / `joined_` 追踪状态，防止重复调用。

## 示例用法（`main.cc`）
```cpp
Thread t([] {
    std::cout << "在线程中执行任务" << std::endl;
}, "worker");
t.start();
t.join();
```

同时可以批量创建线程：
```cpp
std::vector<Thread> threads;
for (int i = 0; i < 3; ++i) {
    threads.emplace_back([i] {
        std::cout << "工作线程 " << i << std::endl;
    });
    threads.back().start();
}
for (auto& th : threads) {
    th.join();
}
```


## 可扩展方向
- 支持线程亲和性、优先级设置。
- 在线程启动前注册钩子，比如记录线程创建日志。
- 提供 `ThreadGroup` 或线程池进一步管理多个线程。

