# performance analysis

- [performance analysis](#performance-analysis)
  - [性能问题](#性能问题)
    - [响应慢，throughput不够](#响应慢throughput不够)
    - [资源利用率问题](#资源利用率问题)
      - [CPU利用率](#cpu利用率)
      - [memory利用率](#memory利用率)
      - [IO](#io)
  - [分析流程](#分析流程)

## 性能问题
性能问题包括响应慢，throughput不够及资源（如CPU，Memory及IO ）利用率的问题

### 响应慢，throughput不够
响应慢，throughput不够，可能由于：
- 在I/O上，阻塞等待
  即等待IO可读或者可写
- I/O处理不及时
  处理IO的线程，被其他业务，如decode，computer，encode等，阻塞了
- 网络

处理这类问题，优秀的实践，如non-block IO + IO复用技术，对应的产品，如boost asio，网络框架库
- latency
系统完成一次操作/任务所需要的时间，以时间为单位。如：
Reading 1 MB from RAM: 0.25 ms
Reading 1 MB from SSD: 1 ms
Transfer 1 MB over network: 10 ms
Inter-continental round trip: 150 ms
串行处理请求时，任务的平均latency就会变大。因为任务可能被之前的任务block，而得不到及时处理。可以通过使用多线程来降低latency。

- throughput
系统在一段时间内，能够处理的任务数。如一个server的throughput，可以通过一段时间的请求数来衡量。

### 资源利用率问题
#### CPU利用率
CPU利用率问题，可能在于
- loop 计算
  可以通过优化数据结构与算法，得到改善
- loop 查询
  如：loop 查询，锁的状态，业务标志等。
  优秀的实践，如：条件变量，信号等技术
- 频繁的系统调用
  优秀的实践，如：标准IO库，通过buffer来减少系统调用。linux系统的内存管理。
- 频繁的进程切换
  如：频繁的创建线程，进程。
  优秀的实践，如：线程池。
- 频繁的数据拷贝
  数据频繁的在内核空间和用户空间之间拷贝，值传递时，不断的`对象析构`-》`拷贝构造`。
  优秀的实践，如：IPC的shared memory技术，`std::move`语义及智能指针。

常用的排查工具，如`perf`, `gprof`, `top`。

#### memory利用率
memory利用率问题在于内存泄漏或者内存没有及时被释放。
常用排查工具，如`valgrind`

#### IO
频繁的IO操作。
常用的工具，如：`iotop`，`strace`

## 分析流程
- 建立性能指标，即哪方面的性能优化
- 搭建测试模型
- 针对不同的问题，选择不同的工具，利用工具，定位性能瓶颈
- 优化，如代码
- 测试，验证改善后的性能
- 迭代
