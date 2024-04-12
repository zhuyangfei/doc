
# concurrency

- [protect data](#protect-data)
- [synchronize actions on separate threads](#synchronize-actions-on-separate-threads)
- [区分atomic与volatile的使用](#区分atomic与volatile的使用)
- [shared\_ptr](#shared_ptr)

## protect data
One concept that’s widely used to help programmers reason about their code is invariants—statements that are always true about a particular data structure, such as “this variable contains the number of items in the list.” These invariants are often broken during an update, especially if the data structure is of any complexity or the update requires modification of more than one value.

a race condition is anything where the outcome depends on the relative ordering of execution of operations on two or more threads.
It’s when the race condition leads to broken invariants that there’s a problem, such as with the doubly linked list example mentioned

The C++ Standard also defines the term data race to mean the specific type of race condition that arises because of concurrent modification to a single object.

Problematic race conditions typically occur where completing an operation requires modification of two or more distinct pieces of data, such as the two link pointers in the example. Because the operation must access two separate pieces of data, these must be modified in separate instructions, and another thread could potentially access the data structure when only one of them has been completed.

## synchronize actions on separate threads
### a generic thread-safe queue
```c++
#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>
template<typename T>
class threadsafe_queue
{
private:
    mutable std::mutex mut;           1
    std::queue<T> data_queue;
    std::condition_variable data_cond;
public:
    threadsafe_queue()
    {}
    threadsafe_queue(threadsafe_queue const& other)
    {
        std::lock_guard<std::mutex> lk(other.mut);
        data_queue=other.data_queue;
    }
    void push(T new_value)
    {
        std::lock_guard<std::mutex> lk(mut);
        data_queue.push(new_value);
        data_cond.notify_one();
    }
    void wait_and_pop(T& value)
    {
        std::unique_lock<std::mutex> lk(mut);
        data_cond.wait(lk,[this]{return !data_queue.empty();});
        value=data_queue.front();
        data_queue.pop();
    }
    std::shared_ptr<T> wait_and_pop()
    {
        std::unique_lock<std::mutex> lk(mut);
        data_cond.wait(lk,[this]{return !data_queue.empty();});
        std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
        data_queue.pop();
        return res;
    }
    bool try_pop(T& value)
    {
        std::lock_guard<std::mutex> lk(mut);
        if(data_queue.empty())
            return false;
        value=data_queue.front();
        data_queue.pop();
        return true;
    }
    std::shared_ptr<T> try_pop()
    {
        std::lock_guard<std::mutex> lk(mut);
        if(data_queue.empty())
            return std::shared_ptr<T>();
        std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
        data_queue.pop();
        return res;
    }
    bool empty() const
    {
        std::lock_guard<std::mutex> lk(mut);
        return data_queue.empty();
    }
};
```

## 区分atomic与volatile的使用
- `std::atomic`
  可以保证，其操作（read-modify-write）的原子性, 同时，保证执行顺序（避免重排执行顺序），用在并发编程上。
  如：
  ```cpp
  std::atomic<bool> valueValid(false);
  auto inputValue = calculateResult();
  valueValid = true;
  ```
  定义`std::atomic`类型的valueValid，可以保证`inputValue` 一定在valueValid = true之前能被执行到。注：不相关变量的操作，编译器及硬件执行时，可以重新排序。如
  ```cpp
  y = x;
  a = b;
  // could be reordered by compile or machine
  a = b;
  y = x;
  ```
- `volatile`
  即不能保证操作原子性，也不能保证执行顺序。只是告诉编译器，需要处理特殊的内存，不要优化重复地读取与写入。用在与硬件IO（比如，传感器）映射的内存操作上。
  如：
  ```cpp
  volatile x = 0;
  volatile y = x;
  volatile y = x;
  x = 10;
  x = 20;
  ```
  定义`volatile`, 避免了编译器将上述重复的赋值，优化为
  ```cpp
  volatile x = 0;
  volatile y = x;
  x = 20;
  ```

  ## shared_ptr
  多线程中，对象生命周期，比较难管理。使用`shared_ptr`，可以比较好的防止对象被提前析构，特别是不能拷贝的对象。
