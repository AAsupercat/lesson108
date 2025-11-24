# C++ Linux后端开发 - 网络前置知识大纲

## 一、网络基础概念（必须掌握）

### 1.1 网络分层模型
- **OSI七层模型**：理解各层职责
- **TCP/IP四层模型**：实际开发中的分层
  - 应用层（HTTP/HTTPS/WebSocket/gRPC）
  - 传输层（TCP/UDP）
  - 网络层（IP/ICMP）
  - 链路层（以太网/ARP）
- **五层模型**：学习时的折中模型

### 1.2 网络地址体系
- **IP地址**：IPv4地址分类、子网掩码、CIDR
- **端口号**：知名端口、动态端口、端口范围
- **MAC地址**：物理地址的作用
- **域名系统（DNS）**：域名解析过程、DNS缓存

### 1.3 网络通信基本概念
- **客户端/服务器模型**
- **请求/响应模式**
- **长连接 vs 短连接**
- **同步 vs 异步通信**

---

## 二、传输层协议（核心重点）

### 2.1 TCP协议（必须深入理解）
- **TCP特点**：面向连接、可靠传输、全双工
- **TCP三次握手**：建立连接的过程和状态转换
- **TCP四次挥手**：关闭连接的过程和状态转换
- **TCP状态机**：CLOSED、LISTEN、SYN_SENT、ESTABLISHED等状态
- **TCP可靠传输机制**：
  - 序列号和确认号
  - 超时重传
  - 滑动窗口
  - 流量控制（接收窗口）
  - 拥塞控制（慢启动、拥塞避免、快重传、快恢复）
- **TCP头部格式**：各字段含义（源端口、目标端口、序列号、确认号、窗口大小、标志位等）
- **TCP粘包/拆包问题**：原因和解决方案

### 2.2 UDP协议
- **UDP特点**：无连接、不可靠、低延迟
- **UDP头部格式**
- **UDP适用场景**：DNS查询、视频直播、游戏等
- **UDP可靠性实现**：应用层实现可靠传输

### 2.3 TCP vs UDP选择
- 何时选择TCP
- 何时选择UDP
- 性能对比分析

---

## 三、网络层协议（重要）

### 3.1 IP协议
- **IP地址分类**：A/B/C/D/E类地址
- **子网划分**：子网掩码、CIDR表示法
- **私有IP地址**：10.0.0.0/8、172.16.0.0/12、192.168.0.0/16
- **NAT（网络地址转换）**：原理和应用
- **IP数据报格式**：头部各字段含义
- **IP分片和重组**：MTU、分片过程

### 3.2 ICMP协议
- **ICMP作用**：网络诊断和错误报告
- **常用ICMP消息**：ping（Echo Request/Reply）、TTL超时
- **traceroute原理**

### 3.3 路由基础
- **路由表**：理解路由选择过程
- **默认网关**：作用和使用
- **路由协议**：RIP、OSPF、BGP（了解即可）

---

## 四、应用层协议（实际开发重点）

### 4.1 HTTP/HTTPS协议（必须掌握）
- **HTTP请求格式**：
  - 请求行（方法、URI、版本）
  - 请求头（Host、Content-Type、Content-Length、Connection等）
  - 请求体
- **HTTP响应格式**：
  - 状态行（版本、状态码、状态描述）
  - 响应头
  - 响应体
- **HTTP方法**：GET、POST、PUT、DELETE、PATCH等
- **HTTP状态码**：2xx、3xx、4xx、5xx常见状态码
- **HTTP版本**：HTTP/1.0、HTTP/1.1、HTTP/2、HTTP/3
- **HTTP/1.1特性**：
  - Keep-Alive（长连接）
  - 管道化（Pipelining）
  - 分块传输编码（Chunked）
- **HTTPS**：TLS/SSL握手过程、证书验证
- **HTTP客户端实现**：如何用C++实现HTTP客户端

### 4.2 WebSocket协议
- **WebSocket特点**：全双工通信、低延迟
- **WebSocket握手**：HTTP升级到WebSocket
- **WebSocket帧格式**
- **应用场景**：实时通信、游戏、聊天

### 4.3 gRPC协议（现代后端常用）
- **gRPC特点**：基于HTTP/2、使用Protocol Buffers
- **RPC概念**：远程过程调用
- **gRPC vs RESTful API**

### 4.4 其他协议（了解）
- **DNS协议**：查询过程、记录类型
- **FTP协议**：文件传输
- **SMTP/POP3/IMAP**：邮件协议

---

## 五、Linux网络编程基础（核心技能）

### 5.1 Socket编程基础
- **Socket概念**：网络编程的抽象接口
- **Socket类型**：
  - SOCK_STREAM（TCP）
  - SOCK_DGRAM（UDP）
  - SOCK_RAW（原始套接字）
- **Socket地址结构**：
  - `sockaddr`、`sockaddr_in`、`sockaddr_in6`
  - 字节序转换：`htonl`、`htons`、`ntohl`、`ntohs`
  - 地址转换：`inet_addr`、`inet_ntoa`、`inet_pton`、`inet_ntop`

### 5.2 TCP Socket编程API
- **服务器端流程**：
  ```cpp
  socket() -> bind() -> listen() -> accept() -> recv()/send() -> close()
  ```
- **客户端流程**：
  ```cpp
  socket() -> connect() -> send()/recv() -> close()
  ```
- **关键函数详解**：
  - `socket()`：创建套接字
  - `bind()`：绑定地址和端口
  - `listen()`：监听连接
  - `accept()`：接受连接
  - `connect()`：发起连接
  - `send()/recv()`：发送/接收数据
  - `close()`：关闭套接字
- **错误处理**：errno、错误码含义

### 5.3 UDP Socket编程API
- **服务器端**：`socket()` -> `bind()` -> `recvfrom()`/`sendto()`
- **客户端**：`socket()` -> `sendto()`/`recvfrom()`
- **UDP特点**：无连接、需要指定目标地址

### 5.4 Socket选项设置（重要）
- **SO_REUSEADDR**：地址复用，解决TIME_WAIT问题
- **SO_REUSEPORT**：端口复用（Linux 3.9+）
- **SO_KEEPALIVE**：TCP保活机制
- **TCP_NODELAY**：禁用Nagle算法
- **SO_RCVBUF/SO_SNDBUF**：接收/发送缓冲区大小
- **setsockopt()/getsockopt()**：设置/获取选项

---

## 六、Linux网络I/O模型（高并发核心）

### 6.1 I/O模型分类
- **阻塞I/O（Blocking I/O）**：默认模式
- **非阻塞I/O（Non-blocking I/O）**：`fcntl()`设置O_NONBLOCK
- **I/O多路复用（I/O Multiplexing）**：
  - `select()`：跨平台，但性能有限
  - `poll()`：改进的select
  - `epoll()`：Linux高性能方案（**重点掌握**）
- **信号驱动I/O（Signal-driven I/O）**：SIGIO
- **异步I/O（AIO）**：`aio_read()`/`aio_write()`

### 6.2 epoll详解（必须深入掌握）
- **epoll优势**：高性能、支持大量连接
- **epoll API**：
  - `epoll_create()`：创建epoll实例
  - `epoll_ctl()`：添加/修改/删除文件描述符
  - `epoll_wait()`：等待事件
- **epoll工作模式**：
  - **LT模式（Level Triggered）**：水平触发，默认模式
  - **ET模式（Edge Triggered）**：边缘触发，高性能模式
- **epoll事件类型**：EPOLLIN、EPOLLOUT、EPOLLERR、EPOLLHUP、EPOLLET、EPOLLONESHOT
- **epoll编程模式**：
  - Reactor模式
  - 事件循环（Event Loop）
  - 非阻塞I/O + epoll

### 6.3 多线程网络编程
- **线程模型**：
  - 单线程 + I/O多路复用（推荐）
  - 每连接一线程（Thread-per-Connection）
  - 线程池模型
- **线程安全**：互斥锁、条件变量、原子操作
- **C++11线程库**：`std::thread`、`std::mutex`、`std::condition_variable`

---

## 七、高并发网络编程（实战重点）

### 7.1 高并发架构设计
- **C10K问题**：如何支持1万并发连接
- **C100K/C1000K问题**：更大规模并发
- **架构模式**：
  - Reactor模式
  - Proactor模式
  - 主从Reactor模式

### 7.2 性能优化技巧
- **连接池**：复用TCP连接
- **内存池**：减少内存分配开销
- **零拷贝技术**：`sendfile()`、`splice()`
- **缓冲区设计**：环形缓冲区、链式缓冲区
- **定时器设计**：时间轮、最小堆

### 7.3 常见问题与解决方案
- **TIME_WAIT问题**：原因和解决方案
- **半关闭连接**：`shutdown()`的使用
- **粘包/拆包**：定长、分隔符、长度字段、TLV格式
- **心跳机制**：Keep-Alive、应用层心跳
- **优雅关闭**：如何正确关闭连接

---


## 八、网络安全基础（了解）

### 8.1 常见网络攻击
- **DDoS攻击**：分布式拒绝服务
- **SYN Flood**：TCP SYN洪水攻击
- **中间人攻击**：MITM
- **SQL注入**：应用层安全

### 8.2 安全编程实践
- **输入验证**：防止缓冲区溢出
- **TLS/SSL**：加密传输
- **防火墙规则**：iptables基础
- **权限控制**：最小权限原则

---


### 在线资源
- Linux man pages：`man 2 socket`、`man 7 epoll`
- RFC文档：RFC 793（TCP）、RFC 2616（HTTP/1.1）
- 开源项目：muduo、libevent、nginx源码

