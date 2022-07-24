# boost usage

[message_queue](#message_queue-usage)

## message_queue usage
### 注意
1. 为了robust考虑，需要在进程启动时，先调用`message_queue::remove`来删掉可能的未清除的资源。例如启动`message_queue`的进程被直接kill掉，那么自身的资源清理函数或者析构函数不能被执行到，导致下次启动时，出现如下异常：
    > terminate called after throwing an instance of 'boost::interprocess::interprocess_exception'
    > what():  File exists
    > Aborted
2. [`~message_queue()`](https://www.boost.org/doc/libs/1_46_0/doc/html/boost/interprocess/message_queue.html)析构函数只能释放，创建该`message_queue`对象的资源，不能释放全局的`message_queue`资源。
3. 复杂数据类型，需要对数据进行`serialization`，然后，再使用`message_queue`
### 设计
针对boost的`message_queue`, 需要通过全局的`message_queue::remove`函数来删除所有资源，所以，应该设计一个daeman进程负责`message_queue`的创建与删除，其他进程只是使用。
### REF
[boost serialization](https://www.boost.org/doc/libs/1_72_0/libs/serialization/doc/tutorial.html)
[concurrent programing](https://developer.ibm.com/articles/au-concurrent_boost/)
[boost message_queue](https://www.boost.org/doc/libs/1_78_0/boost/interprocess/ipc/message_queue.hpp)
[boost message_queue](https://www.boost.org/doc/libs/1_46_0/doc/html/boost/interprocess/message_queue.html)
### 例子：
[boostMegQTEST](./boostMegQTest.cpp)