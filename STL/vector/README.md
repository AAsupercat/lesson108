# 手撕 Vector 模板

## 概述
这是一个手撕 STL `vector` 容器的完整实现模板，适用于面试准备。

## 核心特性

### 1. 基本功能
- ✅ 构造函数（默认、指定大小、拷贝、移动、初始化列表）
- ✅ 析构函数
- ✅ 赋值运算符（拷贝、移动）
- ✅ 元素访问（`[]`, `at`, `front`, `back`）

### 2. 迭代器
- ✅ `begin()`, `end()`
- ✅ `cbegin()`, `cend()`
- ✅ 支持范围for循环

### 3. 容量管理
- ✅ `size()`, `capacity()`, `empty()`
- ✅ `reserve()` - 预分配内存
- ✅ `resize()` - 调整大小
- ✅ **动态扩容** - 2倍扩容策略

### 4. 修改操作
- ✅ `push_back()` - 支持左值和右值
- ✅ `pop_back()`
- ✅ `insert()` - 在指定位置插入
- ✅ `erase()` - 删除元素
- ✅ `clear()` - 清空
- ✅ `swap()` - 交换

### 5. 异常安全
- ✅ `at()` 边界检查
- ✅ 异常安全保证

## 面试重点

### 1. 动态扩容机制
```cpp
// 当 size_ >= capacity_ 时，容量翻倍
if (size_ >= capacity_) {
    reserve(capacity_ == 0 ? 1 : capacity_ * 2);
}
```

### 2. 内存管理
- 使用 `operator new` 和 `operator delete`
- 使用 placement new 进行构造
- 显式调用析构函数

### 3. 移动语义
- 移动构造函数和移动赋值运算符
- 避免不必要的拷贝

### 4. 迭代器失效
- `push_back` 可能导致迭代器失效（扩容时）
- `insert` 和 `erase` 会导致迭代器失效

## 编译运行

```bash
make
./main
```

## 常见面试问题

1. **为什么扩容是2倍？**
   - 平衡内存使用和性能
   - 避免频繁扩容
   - 时间复杂度均摊 O(1)

2. **迭代器失效的场景？**
   - `push_back` 导致扩容
   - `insert` 操作
   - `erase` 操作

3. **如何优化？**
   - 使用 `reserve()` 预分配
   - 使用移动语义
   - 考虑 shrink_to_fit（本实现未包含）

## 注意事项

- 本实现是简化版，实际 STL 更复杂
- 未实现 `allocator` 分离
- 未实现 `shrink_to_fit`
- 迭代器类型简化（实际 STL 有更复杂的迭代器类型）

