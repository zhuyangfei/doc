# IPC
IPC用户进程间通信，常见的有`Pipe`，`FIFO`,`Shared memory`, `消息队列`，`信号量`，`socket`等

- [Pipe](#pipe)
- [`FIFO`](#fifo)
- [消息队列](#消息队列)
- [shared memory](#shared-memory)
- [POSIX 信号量](#posix-信号量)
- [socket](#socket)

## Pipe
- 使用场景
1. 用于父子进程间通信，通常是单向的，一端读，一端写。
2. 连接到另一个进程，如将文件内容，通过`Page`程序，显示到终端。
- `popen`
  `popen`常用于创建与其他进程之间的管道，然后，从管道读数据或者写数据到管道，如`fin = fopen("/bin/ls" '-r')`。`popen`对于`pipe`的连接场景，是这种实现的一种封装。
- 生命周期
  通过引用计数的方式进行管理

## `FIFO`
命名管道，可用于超过两个进程之间的通信。
- 生命周期
  通过引用计数的方式进行管理

## 消息队列
消息收发的抽象，实现时，可以是kernel的消息队列，也可以是，基于mmap实现，如boost message queue

## shared memory
`shared memory`因为数据不需要在进程之间来回复制，所以，是IPC中最快一种。`shared memory`中使用锁，相比`记录锁`及`信号量`的性能更好。
### XSI shared memory
由kernel提供的一种实现，存在内置限制，需要修改内核配置。

### mmap shared memory
通过应用计数的方式来管理生命周期

- 同步及异常退出
  进程异常退出，数据依然存在，所以，使用`pthead_mutex`及`pthead_condition`同步时，需要设置robustness的属性。
- coredump debug
  通过`mmap`方式，使用的`shared memory`，在coredump时，不会dump的core文件。因此，使用`GDB`时，需要core文件与`shared memory`文件都存在，不然，通过`GDB`看到的`shared memory`内容都是`\0`;

## POSIX 信号量
进程之间的同步机制。
使用注意，只能通过`sem_wait`，`sem_post`来操作，不能使用`sem_getvalue`操作信号量。 因为`sem_getvalue`不是原子的，当使用这个值时，这个值可能已经变化了，典型的`TOCTOU`的问题。

## socket
可用于跨主机之间的通信
