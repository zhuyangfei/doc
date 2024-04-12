# signal
信号是一种软件中断, 是一种异步的事件，因此，提供了异步处理事件的方法。

## 功能
1. 可以实现进程间通信或者控制，如child process退出后，parent进程能收到`SIGCHLD`, 通过`kill`结束其他进程
2. 资源清理
3. 实现临界区（原子操作）
4. 提供异步处理事件的方法

## 如何产生
1. 进程调用特定的接口，如`abort`，
2. 由硬件触发，如`除0`，`无效内存的访问`
3. 通过`kill(1)`，给其他进程发信号
4. 同组的进程，通过`kill(2)`系统调用，给其他进程发信号

## 如何处理
3种默认的处理方式，
- ignore
- [signal handle](./signalHandle.md)
- 系统默认行为

## 中断系统调用
信号会中断慢速的系统调用，如read,readv, ioctl, readv, write, writev, wait及waitpid。因此，代码上通常的处理为
```cpp
if (n = read(fd, buf, BUFFSIZE) < 0)
{
    if(errno == EINTR)
        // continue read
    else
        // handle error
}
```
部分系统支持自动重启系统调用，来简化APP的编程
