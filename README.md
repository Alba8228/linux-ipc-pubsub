# Linux IPC 发布订阅消息系统

一个基于 Linux 系统编程实现的发布/订阅（Pub/Sub）消息中间件项目。

## 功能特性

- **本地进程间通信（IPC）**：使用 System V 共享内存、信号量和消息队列实现本地高效通信
- **跨主机网络通信**：基于 TCP Socket 和 I/O 多路复用（select）支持多客户端并发连接
- **自定义线程池**：用于高效处理并发任务
- **哈希表管理**：用于主题订阅关系的高效存储和查找
- **配置文件支持**：可动态调整参数，实现本地/网络双模式灵活切换

## 项目结构

```
.
├── Makefile          # 顶层 Makefile
├── README.md         # 项目说明文档
├── client1/          # 客户端 1
│   ├── Makefile
│   ├── client.c
│   ├── client.conf
│   ├── client.h
│   └── main.c
├── client2/          # 客户端 2
│   ├── Makefile
│   ├── client.c
│   ├── client.conf
│   ├── client.h
│   └── main.c
├── common/           # 公共模块
│   ├── config.c/h    # 配置文件解析
│   ├── msg.c/h       # 消息队列操作
│   ├── net.c/h       # 网络通信
│   ├── protocol.h    # 协议定义
│   ├── sem.c/h       # 信号量操作
│   ├── shm.c/h       # 共享内存操作
│   └── shmfifo.c/h   # 共享内存队列
└── server/           # 服务端
    ├── Makefile
    ├── hashmap.c/h   # 哈希表实现
    ├── main.c
    ├── server.c/h
    ├── server.conf
    └── thread.c/h    # 线程池实现
```

## 编译运行

### 编译

```bash
make
```

### 清理

```bash
make clean
```

### 运行

1. 启动服务端：
```bash
cd server
./server
```

2. 启动客户端 1：
```bash
cd client1
./client
```

3. 启动客户端 2：
```bash
cd client2
./client
```

## 技术亮点

- 使用 System V IPC（共享内存、信号量、消息队列）实现本地进程间通信
- 基于 TCP Socket 和 select I/O 多路复用实现网络通信
- 生产者-消费者模式实现共享内存队列同步
- 固定长度协议+魔数校验解决 TCP 粘包问题
- 自定义线程池提高并发处理能力
- 哈希表（链地址法）管理主题订阅关系

## 工作模式

### 本地模式
客户端通过共享内存队列发送消息 → 服务端读取 → 通过消息队列分发给订阅者

### 网络模式
客户端通过 TCP Socket 发送消息 → 服务端处理 → 通过 Socket 分发给网络订阅者

## 系统要求

- Linux 操作系统
- GCC 编译器
- pthread 库支持

## 许可证

本项目仅供学习交流使用。
