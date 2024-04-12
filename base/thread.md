# thread

- [使用场景](#使用场景)
- [线程同步](#线程同步)
- [线程创建及结束](#线程创建及结束)
- [线程间关系](#线程间关系)
- [线程与进程的退出](#线程与进程的退出)
- [detached thread](#detached-thread)
- [exception safe thread](#exception-safe-thread)
- [std::jthread](#stdjthread)

## 使用场景
在多任务场景时，使用线程。比如，我在写文档的同时，等待外卖咖啡。当咖啡送到后，我停下手上的工作去取咖啡。
这个场景中，有两个任务，一个写文档，另一个是，喝咖啡（等咖啡，取咖啡及喝咖啡）。这里，我相当于CPU, 每个任务对应一个线程，相关任务“同时”在进行，因此，CPU的利用率与吞吐量得到提升。
同时，任务之间不相互干扰，便于编程。任务之间，只需要通过通知，进行同步。

## 线程同步
因为线程之间，共享进程的内存，当线程同时访问内存时，要确保内存的一致性，所以，线程之间需要同步。
常用线程同步技术：
- 锁
  有互斥锁，读写锁，用于保护数据免于竞争。通过设置锁属性，包括`是否进程共享`,`是否可重入`，`是否是robustness`，适用于不同的适用场景。
- 条件变量
  用于线程之间，相互通知。可配置属性，包括是否进程共享，`PTHREAD_PROCESS_SHARED`, `CLOCK_MONOTONIC`
- barre
  用于多个线程同时等待某个时刻或者任务。

## 线程创建及结束
- 线程创建
  通过`pthread_create`创建线程。在创建的同时，通过设置线程属性，来创建不同的线程，常见的如：detachable，stack_size，schedule policy.
- 结束
  三种情况，1. 从执行函数返回，2. 执行调用`pthread_exit`，3. 其他线程调用pthread_cancel。
  线程上的资源清理, 线程在退出前，有callback入口，可用于资源清理。

## 线程间关系
通过`pthread_join`得知其他线程是否退出（执行结束）或者等待线程退出

## 线程与进程的退出
If any thread in a process call `exit` or `_exit`, will terminate the entire process.
When the default action is to terminate the process, a signal sent to a thread will terminate the entire process.

## detached thread
detached thread also will terminate when process exit. a detached thread only affect what will do after termination.

Detaching a thread doesn’t make it immune to a call to exit() in another thread or a return in the main thread. In such an event, all threads in the process are immediately terminated, regardless of whether they are joinable or detached. To put things another way, pthread_detach() simply controls what happens after a thread terminates, not how or when it terminates.

[the linux programming interface](https://learning.oreilly.com/library/view/the-linux-programming/9781593272203/xhtml/ch29.xhtml#:-:text=Detaching%20a%20thread,when%20it%20terminates.)
https://www.informit.com/articles/article.aspx?p=2085690&seqNum=5

## exception safe thread
即在线程析构前，先`join`or`detach`线程，然后，再析构。
note：如果exception 发生了，而thread选择`join`，那么会等待`join`返回后，才能继续向上抛出异常。因此，这个地方会block执行。所以，这种设计，可能也不是很好的。
alternative 1: **避免在try {}中，创建std::thread**，即避免在异常回溯的路径上，创建std::thread, 只在主线程启动时，创建std::thread
alternative 2: 不使用std::thread, 而使用pthread

## std::jthread
相比std::thread, std::jthread 析构之前，能够只能`join`
