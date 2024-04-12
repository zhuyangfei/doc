# boost usage

[message_queue](#message_queue-usage)

## message_queue usage
用于进程间通信

### 例子：
[message queue for one process](./boostMegQTest.cpp)
[message queue for IPC](https://gitlabe2.ext.net.nokia.com/yanzhu/utexample/-/blob/master/src/boostmq.cpp)

### 注意
1. 为了robust考虑，需要在进程启动时，先调用`message_queue::remove`来删掉可能的未清除的资源(optional)。例如启动`message_queue`的进程被直接kill掉，那么自身的资源清理函数或者析构函数不能被执行到，导致下次启动时，出现如下异常：
    > terminate called after throwing an instance of 'boost::interprocess::interprocess_exception'
    > what():  File exists
    > Aborted
2. [`~message_queue()`](https://www.boost.org/doc/libs/1_46_0/doc/html/boost/interprocess/message_queue.html)析构函数只能释放，创建该`message_queue`对象的资源，不能释放全局的`message_queue`资源。
3. 复杂数据类型，需要对数据进行`serialization`，然后，再使用`message_queue`
### 设计
针对boost的`message_queue`, 需要通过全局的`message_queue::remove`函数来删除所有资源.
1. 方案1：设计一个daeman进程负责`message_queue`的创建与删除，其他进程只是使用。
2. 方案2：队列创建后，不删除。进程以`open_or_create`模式，打开`message_queue`。
### REF
[boost serialization](https://www.boost.org/doc/libs/1_72_0/libs/serialization/doc/tutorial.html)
[concurrent programing](https://developer.ibm.com/articles/au-concurrent_boost/)
[boost message_queue](https://www.boost.org/doc/libs/1_78_0/boost/interprocess/ipc/message_queue.hpp)
[boost message_queue](https://www.boost.org/doc/libs/1_46_0/doc/html/boost/interprocess/message_queue.html)

## I/O services
Programs that use Boost.Asio for asynchronous data processing are based on I/O services and I/O objects.
I/O services abstract the operating system interfaces that process data asynchronously.
I/O objects initiate asynchronous operations.
[ioservice](https://theboostcpplibraries.com/boost.asio-io-services-and-io-objects)
### example
```c++
#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include <chrono>
#include <iostream>

using namespace boost::asio;

int main()
{
  io_service ioservice;

  steady_timer timer{ioservice, std::chrono::seconds{3}};
  timer.async_wait([](const boost::system::error_code &ec)
    { std::cout << "3 sec\n"; });

  ioservice.run();
}
```
1. callback's thread the same as the ioservice.run(); the callback execution will be stalled by the ioservice.run()'s thread, such as timer's callback happens in 50ms, but there is one second sleep before run `ioservice.run()`;
2. when callback executes done, then ioservice.run() return

## named mutex
named_mutex named_mtx{open_or_create, "mtx"};
no robustness, if a app abort without unlock the mutex, then the mutex can't be used by others.
