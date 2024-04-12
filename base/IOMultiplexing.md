# IO MultiPlex
IO多路转接（复用）即通过异步通知的方式，来监听多个IO的状态，如可读，可写，以满足同时操作多个IO需求，提升性能。
APP通过调用`MultiPlexing API`,等待fds上的事件发生。当有事件发送时，APP从接口返回，然后，检查事件，处理事件。
IO多路转接（复用），在一个线程上操作，因此，可以简化多线程编程带来的程序复杂性。

- MultiPlexing API
  - `select`
  - `poll`
  - `epoll`

## 使用场景
进程需要同时操作多个IO时，如socket通信，中断处理，文件操作。

## MultiPlex API使用对比
### `select`
  只能监听少于1024个fd，通常，不能满足现代的APP，所以，不推荐使用
### `poll`
API接口，如下：
```cpp
int poll(struct pollfd* fds, nfds_t nfds, int timeout);
```
返回fds事件的个数。APP通过轮询`pollfd`列表的`revents`字段，来确认是哪个fd的事件发生了。
- poll可以是`block`也可以是`non-block` IO，详见https://www.man7.org/linux/man-pages/man2/poll.2.html，的`POLLOUT`上的说明

### [epoll](https://en.wikipedia.org/wiki/Epoll)
- 触发模式
1. `edge-triggered（ET）`：只在被监测的`fd`发生变化时，才触发事件
   调用一次`epoll_wait`后，就消费了该事件.
   使用要点：
   1. 使用`non-blocking`的`file descriptors`
   2. 只在`read`或`write`返回`EAGAIN`时，调用`epoll_wait`
[ET explanation](https://linux.die.net/man/7/epoll)
1. `level-triggered（LT）`：可调用多次调用`epoll_wait`，消费同一个事件，比如分多次读取数据。
- usage
1. 通过`epoll_create1`，创建一个`epoll`实例
2. 通过`epoll_ctl`, 注册需要监测的`file descriptors`
3. 通过`epoll_wait`，等待IO事件发生，同时，阻塞当前线程
   发生中断`EINTR`时，要重新进行wait，详见
```cpp
int System::epoll_wait(int epfd, epoll_event* events, int maxevents, int timeout)
{
    const int ret(::epoll_wait(epfd, events, maxevents, timeout));
    if ((ret == -1) && (errno != EINTR))
        throw std::system_error(errno, std::system_category(), "epoll_wait");
    return ret;
}
```
```cpp
// 使用RAII封装EPOLL
class Epoll
    {
    public:
        explicit Epoll(std::shared_ptr<IsystemCall> ptrSystemCall);
        Epoll();
        ~Epoll();
        Epoll& operator=(const Epoll& obj) = delete;
        Epoll& operator=(Epoll& obj) = delete;
        Epoll(Epoll&& obj) = delete;
        void addFd(int fd, int events);
        void addFd(int fd, int events, void* const ptr);
        void modifyFd(int fd, int events);
        void modifyFd(int fd, int events, void* const ptr);
        void removeFd(int fd);
        int wait(epoll_event* events, int maxEvents, int timeout = EPOLL_TIME_OUT_MS);

    private:
        std::shared_ptr<IsystemCall> m_ptrSystemCall;
        int m_epfd = -1;
    };

Epoll::Epoll(std::shared_ptr<IsystemCall> ptrSystemCall):
        m_ptrSystemCall(ptrSystemCall)
    {
        m_epfd = m_ptrSystemCall->epoll_create1(EPOLL_CLOEXEC);
    }

    Epoll::Epoll():
        Epoll(std::make_shared<systemCallImpl>())
    {
    }

    Epoll::~Epoll()
    {
        m_ptrSystemCall->close(m_epfd);
    }

```
## poll与epoll性能对比
- poll实现机制
  poll流程：
  1. 将fd list，从user空间，拷贝到内核空间
  2. 内核轮询检查fd list，并赋值相应fd的event，时间复杂度为O(n)
  3. 若有事件发生，则event list，从内核空间，拷贝到用户空间，即poll接口返回
  每次调用，循环上面的过程。
- epoll实现机制
  epoll基于事件驱动的方式。
  epoll只在epoll_ctl时，拷贝事件，epoll_wait时，不用拷贝事件。内核使用红黑树的数据结构，来存储事件。
  epoll_wait，只需要检查就绪列表，不需要轮询检查所有fd，所以，性能提升很大。
详见：https://www.cnblogs.com/binarylei/p/11130079.html
https://github.com/muyinchen/woker/blob/master/%E9%9D%A2%E8%AF%95/select%E3%80%81poll%E3%80%81epoll%E4%B9%8B%E9%97%B4%E7%9A%84%E5%8C%BA%E5%88%AB%E6%80%BB%E7%BB%93%5B%E6%95%B4%E7%90%86%5D.md

总结：若fd数量很少时（小于10个），poll性能与epoll查不多。同时，poll的性能随fd数量增加而快速下降，但是，epoll不会。

## 区分`异步IO`与`non-block IO`
`异步IO`，是内核通过`signal(信号)`的方式，告诉APP有IO可以操作。需要操作系统支持的。
`non-block IO`，是IO接口属性，可以在打开一个`fd`时设置

## 如果epoll性能不足时，怎么处理？
[todo]比如同时发生很多事件，event handle来不及处理？

## reference
https://zhuanlan.zhihu.com/p/630753032