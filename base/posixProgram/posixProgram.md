# posixProgram

[POSIX](#posix)
[pthread mutex](#pthread-mutex)

## POSIX
POSIX（Portable Operating System Interface）是关于规则与指令的一系列`标准`，定义了application与UNIX操作系统的接口。
为了实现UNIX 程序能在不同的UNIX平台上运行。
### REF
[What is POSIX? Why Does it Matter to Linux/UNIX Users?](https://itsfoss.com/posix/)

## pthread Mutex
### attribute protocol
1. PTHREAD_PRIO_INHERIT
   拥有锁（一个或者多个锁）的线程，可提升线程优先级到，被阻塞的（等在该锁上）的其他线程所具有的最大优先级。这样保证，
   拥有锁的线程，能优先被执行，而不被打断。
2. PTHREAD_PRIO_NONE
   线程优先级及调度，不因拥有锁而改变
3. PTHREAD_PRIO_PROTECT
   拥有锁（一个或者多个）的线程，可以提升线程优先级到，所有拥有锁的最大优先级，无论有没有其他线程在等待锁。
### attribute robust
    在拥有锁的线程挂了，同时，没有释放锁的情况下，其他线程可以有机会重新拥有锁
1. PTHREAD_MUTEX_RECURSIVE
   拥有锁的线程，可以重新lock，而不会死锁。几次lock（），要对应一样数量的unlock（）。
2. PTHREAD_MUTEX_ERRORCHECK
   拥有锁的线程，重新lock()时，会返回错误。
3. PTHREAD_MUTEX_NORMAL
   默认的类型，当线程拥有锁后，同一个线程再次lock，会发生死锁。

### mutex的owner
1. mutex ower 字段存储了tid的信息
2. mutex ower thread 挂掉之后，mutex ower id 被设置成7fffffff
3. 释放mutex之后，mutex ower id  = 0

### recursive mutex
`recursive mutex`允许mutex owner重新获取到锁，非owner获取不到，e.g
```c++
#include <iostream>
#include <pthread.h>
#include <thread>
#include <assert.h>
#include <chrono>
#include <thread>
using namespace std;
void setMutexAttr(pthread_mutex_t& mutex)
{
    pthread_mutexattr_t attr;
    int ret = pthread_mutexattr_init(&attr);
    ret |= pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    ret |=pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);
    ret |=pthread_mutexattr_setrobust(&attr, PTHREAD_MUTEX_ROBUST);
    ret |=pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    ret |=pthread_mutex_init(&mutex, &attr);
    ret |=pthread_mutexattr_destroy(&attr);
    assert(ret == 0);
}
void robustlock(pthread_mutex_t& mutex)
{
    int ret = pthread_mutex_lock(&mutex);
    if (ret == 0)
    {
        cout << "thread " << std::this_thread::get_id()<< " success to get lock, ";
        cout << "mutex ower is " << mutex.__data.__owner << "\n";
    }
    else if (ret == EOWNERDEAD)
    {
        cout << "thread " << std::this_thread::get_id() << " to consistent themutex\n";
        cout << "mutex ower thread die, the ower is " << mutex.__data.__owner << "\n";
        ret = pthread_mutex_consistent(&mutex);
    }
    else
    {
        cout << "pthread mutex lock failed, the return is " << ret << "\n";
    }
    assert(ret == 0);
}
void robustunlock(pthread_mutex_t& mutex)
{
    cout << "thread " << std::this_thread::get_id()<< " release to get lock\n";
    assert(pthread_mutex_unlock(&mutex) == 0);
}
void funcGetLock(pthread_mutex_t& mutex)
{
    robustlock(mutex);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    // robustunlock(mutex);
}
int main()
{
    cout<<"main thread id is " << std::this_thread::get_id() << "\n";
    pthread_mutex_t mutex;
    setMutexAttr(mutex);
    std::thread t(funcGetLock, std::ref(mutex));
    t.join();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    funcGetLock(mutex);
    robustunlock(mutex);
    return 0;
}
```
[mutex debug](https://linux.m2osw.com/debugging-mutex-deadlock-linux-gdb)
#### REF
https://linux.die.net/man/3/pthread_mutexattr_setprotocol
