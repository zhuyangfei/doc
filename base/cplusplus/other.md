# using
## 用法1：将`ns(namespace)`及`ns::members`引入到另外的`ns`或者块`{}`内
```c++
#include <iostream>
using namespace std;

// dependency code
namespace ns1{
class A
{
public:
    A(){}
    ~A(){}
};
}

// user/application code
namespace ns2
{
using namespace ns1;        // import all ns1's declarations into ns2;
using ns1::A
class B
{
public:
    B(){}
    ~B(){}
private:
    A m_a;
};
}

int main()
{
    cout<<"Hello World";
    using ns2::B;           // import ns2::B to main block
    B b;

    return 0;
}
```

# 匿名namespace
相当于文件中的static用法，匿名namespace内的接口，对外不可见

# teminate, abort, exit

## [abort](https://en.cppreference.com/w/cpp/utility/program/abort)
中止当前进程，表示程序`异常退出`
调用`abort`后，触发`SIGABRT`信号
程序被中止，但不会析构任何对象，也不会调用传给`atexit`与`at_quick_exit`的函数
## [std::exit](https://en.cppreference.com/w/cpp/utility/program/exit)
正常的退出进程，同时，执行`清理`流程。
清理流程包括：
1. 销毁static对象，同时，执行注册到`atexit`的函数
2. 关闭（flush buffer）所有的`C streams`，同时，删掉所有的`tmpfile`, 参考，[exit vs _exit](https://www.geeksforgeeks.org/exit-vs-_exit-c-cpp/)
3. 销毁`thread_local`对象
注：
1. 自动存储的对象不会被销毁，如：
```
A a1;
int main()
{
    A a2;
    exit(EXIT_FAILURE);
}
```
对象`a1` 会销毁，对象`a2`不会销毁
如果对象的析构中，要处理跨进程的资源，那么main函数里面不建议使用`exit`函数。
2. 析构静态对象时，在多线程程序中，可能会因为竞争，而导致退出时，发送coredump。详见,[exit multi thread](https://stackoverflow.com/questions/36094700/how-to-exit-a-program-in-c03-in-a-thread-safe-manner)
   [Data races](https://cplusplus.com/reference/cstdlib/exit/)
## [std::terminate](https://en.cppreference.com/w/cpp/error/terminate)
异常发生，没有被catch时，会触发`terminate`
默认的`terminate_handler`调用`std::abort`

# reinterpret_cast
用于指向不同类型数据的指针之间的转换
不检查指针类型是否与原指针所指的数据类型是否一致
## 使用场景
把其他类型指针，转成原类型的指针
## REF：
https://www.geeksforgeeks.org/reinterpret_cast-in-c-type-casting-operators/

# copy elision
memory copy
