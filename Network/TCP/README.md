# TCP协议

## TCP的报头
### 目的端口号和源端口号
解决协议关键问题之一：交付给上层协议的哪一个？目的端口号
### 序号和确认序号

序号就是字节流的下标表示，一共可以表示0~4,294,967,295，每次发送数据段报文都会带有各自的字节流的第一个位置，注意起始位置是随机的，这个应该在建立连接的时候确认起始位置的，确认序号同样也是回应收到最后一个数据段的最后一个下标的下一位，表示请从这个下标开始发送。

一个数据段：seq=1000, len=100; 那期待的下一个数据段就是seq=1100;

### 4位首部长度
解决协议问题关键之一：如何分离报头和有效载荷，这路4位首部长度表示[0~15]，权值位4，就是表示[0~60]字节，除开TCP报头20字节固定大小，还有选项[0~40]字节。
### 6位标记位
6位标记位实际就是TCP发送报文的类型，将要完成怎样的工作；
1. SYN建立连接标志位：请求建立连接；
2. ACK相应报文：对收到消息做回应，以确保单向可靠；
3. FIN结束连接：本端对另一端请求断开连接，事情已经完毕；
4. PSH抓紧读取：我想要给服务端发送消息，但是接收缓冲区已经存有很多数据，接收缓冲区窗口大小不足，没有被上层读取，因此希望提醒上层抓紧读取，我要发送消息；
5. URG紧急指针：紧急指针是否有效，标记之后使用紧急指针(1字节)，发送紧急数据；
6. RST重新连接：当发现之前已成功链接但当前已经其他原因断开的或以为成功连接的客户端发送消息时，服务器会发送RST，告知客户端，然后客户端重新发送链接请求。

### 16位窗口大小
窗口大小表示目前接收缓冲区还能接收多少数据；
### 16位校验和和16位紧急指针
校验和是为了保证数据不会出现错误问题，紧急指针就是发送紧急请求，比如当服务器卡顿，一直在缓慢加载，这是启动URG，启动紧急指针发送探寻信息，服务器返回目前状态信息，以方便程序员维护修正。
## TCP三次握手，四次挥手

### TCP协商

#### 必须协商
- 初始序列号：建立序列号空间
- 窗口大小：流量控制的基础
- MSS：确定最大段大小 

#### 拓展可选协商
- 窗口缩放（Window Scale）
- 选择性确认（SACK）
- 时间戳（Timestamp）
这里有个序列号回环的问题，可以讨论

## TCP的可靠性

## TCP的高效性

## TCP的特点

## TCP底层














## 服务器端管理多个tcp_sock

### 为什么需要管理多个tcp_sock

在客户端-服务器模型中，服务器需要同时处理多个客户端连接：
- 每个客户端连接对应一个独立的`tcp_sock`结构
- 服务器需要维护所有活跃连接的`tcp_sock`
- 需要能够快速查找和管理这些连接

### 服务器端的数据结构管理

**监听socket和连接socket的区别**：

1. **监听socket（Listening Socket）**：
   - 服务器调用`listen()`后创建的socket
   - 用于接受新的连接请求
   - 对应一个`tcp_sock`，但状态是`LISTEN`

2. **连接socket（Connected Socket）**：
   - 每个客户端连接对应一个独立的socket
   - 每个连接有自己独立的`tcp_sock`结构
   - 状态是`ESTABLISHED`

**内核中的管理结构**：

```c
// 监听socket维护的连接队列
struct tcp_sock {
    // 监听socket的字段
    struct request_sock_queue icsk_accept_queue;  // 已完成连接的队列
    // ...
};

// 每个连接socket
struct tcp_sock {
    // 连接相关的字段
    struct sk_buff_head write_queue;   // 发送队列
    struct sk_buff_head receive_queue; // 接收队列
    u32 state;                         // 连接状态
    // ...
};
```

### 连接的管理方式

**哈希表管理**：

Linux内核使用哈希表来管理所有的TCP连接：

```c
// 内核中的连接哈希表（简化表示）
struct inet_hashinfo {
    struct inet_ehash_bucket *ehash;  // 已建立连接的哈希表
    // ...
};

// 每个连接通过四元组（源IP、源端口、目标IP、目标端口）计算哈希值
// 快速查找对应的tcp_sock
```

**连接查找过程**：

```
收到数据包：
1. 提取四元组（源IP、源端口、目标IP、目标端口）
2. 计算哈希值
3. 在哈希表中查找对应的tcp_sock
4. 将数据包放入对应tcp_sock的receive_queue
```

**连接的生命周期管理**：

```
1. 客户端连接请求：
   - 客户端发送SYN
   - 服务器创建新的tcp_sock（状态：SYN_RCVD）
   - 加入半连接队列

2. 连接建立：
   - 三次握手完成
   - tcp_sock状态变为ESTABLISHED
   - 从半连接队列移到已连接队列
   - 加入哈希表

3. 数据传输：
   - 通过哈希表查找对应的tcp_sock
   - 数据放入对应tcp_sock的receive_queue

4. 连接关闭：
   - 四次挥手完成
   - 从哈希表中移除
   - 释放tcp_sock结构
```

### 实际例子

**服务器处理多个客户端**：

```
服务器监听socket：
tcp_sock_listen (状态: LISTEN)
  └─> icsk_accept_queue (等待accept的连接队列)

客户端1连接：
tcp_sock_client1 (状态: ESTABLISHED)
  ├─> write_queue
  └─> receive_queue
  └─> 加入哈希表[hash(client1的四元组)]

客户端2连接：
tcp_sock_client2 (状态: ESTABLISHED)
  ├─> write_queue
  └─> receive_queue
  └─> 加入哈希表[hash(client2的四元组)]

客户端3连接：
tcp_sock_client3 (状态: ESTABLISHED)
  ├─> write_queue
  └─> receive_queue
  └─> 加入哈希表[hash(client3的四元组)]

收到数据包时：
1. 提取四元组
2. 计算哈希值
3. 在哈希表中找到对应的tcp_sock（如tcp_sock_client1）
4. 将数据放入该tcp_sock的receive_queue
```

### 关键数据结构

**服务器端的完整结构**：

```
监听socket：
struct tcp_sock listen_sock {
    state = LISTEN;
    icsk_accept_queue;  // 已建立连接的队列
}

每个客户端连接：
struct tcp_sock client1_sock {
    state = ESTABLISHED;
    write_queue;        // 发送队列
    receive_queue;      // 接收队列
    // ... 其他字段
}

哈希表：
hash_table[hash_value] -> tcp_sock链表
```

### 总结

1. **每个连接一个tcp_sock**：服务器为每个客户端连接创建独立的`tcp_sock`
2. **哈希表管理**：通过四元组计算哈希值，快速查找对应的`tcp_sock`
3. **队列管理**：监听socket维护连接队列，每个连接socket维护数据队列
4. **生命周期**：从连接建立到关闭，`tcp_sock`在哈希表中动态添加和移除

**简单记忆**：
- 服务器 = 一个监听socket + 多个连接socket
- 每个连接socket = 一个独立的`tcp_sock`
- 哈希表 = 快速查找`tcp_sock`的索引
- 四元组 = 连接的唯一标识（源IP、源端口、目标IP、目标端口）
