# 资源管理
资源管理的基本目标是不泄漏资源及不拥有资源超过需要的时间
资源的拥有者负责释放资源
## `资源`
`资源`是任何需要**获取并释放**的对象，比如file handler, memory, socket, lock.
## R1. 通过`资源handles`及`RAII`来自动的管理资源
### `资源handles`
`资源handles`是利用C++语言的`构造`与`析构`的强制性与成对性来管理资源，即在构造时，获取资源，在析构时，释放资源。
### `RAII`
resource acquisition is initialization, 即资源申请后，立即赋值给资源管理对象（`资源handles`）
### 为什么
避免资源泄漏及复杂地、手动地管理资源。C++强制的构造与析构对象，很好的配对资源的获取与释放函数，比如，fopen/fclose,
lock/unlock, 及 new/delete。**在调用资源的获取及释放函数的任何时候，都应该把这两个操作，封装到一个对象的构造与析构中**。
### 举例，bad
```c plus plus
    void send(X* x, string_view destination)
    {
        auto port = open_port(destination);
        my_mutex.lock();
        // ...
        send(port, x);
        // ...
        my_mutex.unlock();
        close_port(port);
        delete x;
    }
```
你需要在每个分支中，都记得去调用`close_port`, `unlock`，同时，在`...`的代码中，如果出现`return`或者`exception`，那么就会出现资源`port`,`lock`及`x`未释放的情况
### 举例，good
``` c++
    void send(unique_ptr<X> x, string_view destination)     // x owns the X
    {
        Port port{destination};         // port owns port handle
        lock_guard<mutex> guard{my_mutex};   // guard owns lock
        // ...
        send(port, x);
        // ...
    } // automatically close_port, unlock, and delete the pointer x
```
这样所有的资源清理都是自动进行，同时，无论是否有异常发生，都只执行一次。
Port对象，是一个简便的资源封装对象
```c++
    class Port
    {
        PortHandle port
    public:
        explict Port(string destination)
            : port(open_port(destination)) {}
        ~ Port() {close_port(port);}
        opeartor PortHandle() {return port;}

        Port(const Port&) = delete;
        Port& operator=(const Port&) = delete;
    };
```
## R2. 优先选择`域对象`（scoped object），避免不必要的heap对象（heap-allocated）
### `scoped object`
scoped object是local 对象，a global对象或者成员对象。
local对象，作用域为{}，生命周期在{}内。
global对象，作用域全局地，生命周期同程序。
成员对象，作用域类内部，生命周期与对象一致（类内部构造的对象）。
### 举例
    void func()
    {
        auto p = new Gadget{n};
        // ...
        delete p;
    }
上述代码效率低（因为没有必要的allocation及deallocation），同时，在...发生异常或return时，有资源泄漏风险。
替代方案,
``` c++
    void func()
    {
        Gadget gadget{n};
        // ...
    }
```
## R3. 一个裸指针（T*）不是一个资源的`拥有者`
### 为什么
裸指针（T*）一般都不是资源的拥有者，而我们需要一个明确的资源拥有者，这样我们才能可靠并有效的释放指针所指向的对象
###
    void func()
    {
        int* p = new int(1);        // 不好，裸指针拥有对象
        auto p2 = make_unique<int>(7);      //好，int 被unique pointer拥有
    }
### 例外

## R4. 避免使用malloc 及 free，同时，不要混用malloc 及delete
### 为什么
因为malloc及free不支持构造与析构

## R5. 避免显式的调用new及delete
### 为什么
new返回的指针应该赋值给资源handle对象（能调用delete），如果new的返回值，赋值给一个裸指针，那么就可能出现资源泄漏。
### 注
一个大型程序中，如果delete出现在application的代码中，而不是资源handle的代码中，那通常是一个bug。因为当你有N个delete后，
你不能保证不需要新增delete，这样就在维护时，可能引入bug，所以，这种情况，通常就是一个bug。

## R6. 显式的分配资源后，立即将资源赋值给资源管理对象
### 为什么
因为不这样做，资源就可能泄漏
### 例子，不好的
    void f(const string& name)
    {
        FILE* f = fopen(name, "r");            // open the file
        vector<char> buf(1024);
        auto _ = finally([f] { fclose(f); });  // remember to close the file
        // ...
    }
buf 分配异常时，会导致file handle没有被close
### 例子，好
    void f(const string& name)
    {
        ifstream f{name};
        vector<char> buf(1024);
        // ...
    }

## R7. 使用shared_ptr或者unique_ptr来表示所有权(ownership)
### 为什么
防止资源泄漏
### 例子
    void func()
    {
        T t;
        auto p2 = make_unique<T>();    // unique ownership
        auto p3 = make_shared<T>();    // shared_ownership
    }

## R8. 优先使用unique_ptr, 而不是shared_ptr，除非你需要共享所有权
### 为什么
unique_ptr概念简单，更容易知道什么时候析构，同时，也更快（不需要引用计数）
#### 举例，不好的
    void func()
    {
        shared_ptr<base> base = make_shared<Derived>();
        // ... use base locally, without copy it
    }
### 举例，好的
    void func()
    {
        unique_ptr<base> base = make_unique<Derived>();
        // use base locally.
    }

## R9. 使用'make_shared()'来产生'shared_ptr'
### 为什么
'make_shared'让构造的语句更简洁，同时，通过把“shared_ptr”的引用计数靠近它的对象，使用有机会消除引用计数分离分配
### 例子
考虑
    share_ptr<T> p1 {new T();};
    auto p2 = make_shared<T>();
'make_shared'语句，其X只出现一次，因此，相较“new”，通常能够更短也更快
## R10. 使用'make_unique()'来产生'unique_ptr'
### ref [同make_shared](#使用make_shared来产生shared_ptr)
## R11. 仅在显示的表达生命周期语义时，才把智能指针作为参数
### 为什么
1. 传递一个智能指针来转移或者共享所有权，应该只是在包含`所有权`语义时才被使用。若一个函数，并不操作生命周期，那么应该使用裸指针或者引用来传参。
2. 使用智能指针传参，迫使调用者，需要智能智能对象
3. 函数需要一个`widget`，应该能接受任何的`widget`对象，而不是一个生命周期被智能指针管理的对象
4. 传递一个智能指针暗含着运行时开销
### 例子
    // accepts any int*
    void f(int*);

    // can only accept ints for which you want to transfer ownership
    void g(unique_ptr<int>);

    // can only accept ints for which you are willing to share ownership
    void g(shared_ptr<int>);

    // doesn't change ownership, but requires a particular ownership of the caller
    void h(const unique_ptr<int>&);

    // accepts any int
    void h(int&);
### Example, 不好的

    // callee
    void f(shared_ptr<widget>& w)
    {
        // ...
        use(*w); // only use of w -- the lifetime is not used at all
        // ...
    };

    // caller
    shared_ptr<widget> my_widget = /* ... */;
    f(my_widget);

    widget stack_widget;
    f(stack_widget); // error

### Example, 好的

    // callee
    void f(widget& w)
    {
        // ...
        use(w);
        // ...
    };

    // caller
    shared_ptr<widget> my_widget = /* ... */;
    f(*my_widget);

    widget stack_widget;
    f(stack_widget); // ok -- now this works

## R12. 在函数中需要重置`widget`时，才使用`unique_ptr<widget>`作为参数
### 为什么
使用`unique_ptr`作为参数，一定要包含`重置`的语义
### 注：
`重置`表示产生一个智能指针或者智能指针指向一个新的对象
### 例子：
    void sink(unique_ptr<widget>); // takes ownership of the widget

    void uses(widget*);            // just uses the widget
### 例子，不好的
    void thinko(const unique_ptr<widget>&); // usually not what you want
### 注：
使用`shared_ptr`作为参数时，有同样类似的规则。当函数需要共享所有权时，才使用。

## R13. 不要传递从一个智能指针的别名获取指针或者引用
### 为什么
违反这条规则，导致丢失引用计数及空悬指针的第一大原因。
调用者，通过智能指针，来获取指针或者引用，要保证`object`对象活着。同时，确保智能指针不会在底层的调用链上被无意的`reset`或者`重新赋值`。
### 注：
通过把智能指针，拷贝给一个本地临时的智能指针变量，能解决上面的问题。临时的智能指针，保证了对象不会被释放。
### 例子：
```c++
shared_ptr<widget> g_p = ...;

void func(widget& w)
{
    g();
    use(w);
}

void g()
{
    g_p = ...;    // if this was the last shared_ptr to the widget, destorys the widget
}

void my_code()
{
    // bad, the widget will be destoryed, when call g();
    // so error happens when the use() was called.
    func(*g_p);

    // bad the widget is destoryed.
    g_p->func2();
}

// to fix this problem
void my_code()
{
    // copy to a local variable to keep object alive
    auto localgp = g_p;
    func(*localgp);

    localgp->func2();
}
```

# unique_ptr
智能指针`unique_ptr`，通过一个指针拥有并管理另外一个对象，当`unique_ptr`离开`scope`时，释放对象。
## 释放所管理对象的情形：
1. `unique_ptr`对象析构
2. `unique_ptr`对象被赋值给另一个指针通过`operator=`或者`reset()`
## 使用场景
1. 为类对象及包含资源清理的函数，提供`exception`安全的释放资源的保证
2. 传递对象（具有动态生命周期）的唯一所有权（`ownership`）给函数
3. 通过函数的返回值，获取对象的所有权
4. 作为容器的元素指向动态分配的对象
## note
1. `unique_ptr`删除了`拷贝构造`及`拷贝赋值`, 只支持`移动构造`与`移动赋值`
2. 只有`non-const`的`unique_ptr`能转移所有权，`const unique_ptr`限定了`所管理的对象`生命周期在`unique_ptr`创建的范围。举例：
   ```c++
    #include <cassert>
    #include <cstdio>
    #include <fstream>
    #include <iostream>
    #include <memory>
    #include <stdexcept>

    // helper class for runtime polymorphism demo below
    struct B
    {
        virtual ~B() = default;

        virtual void bar() { std::cout << "B::bar\n"; }
    };

    struct D : B
    {
        D() { std::cout << "D::D\n"; }
        ~D() { std::cout << "D::~D\n"; }

        void bar() override { std::cout << "D::bar\n"; }
    };

    // a function consuming a unique_ptr can take it by value or by rvalue reference
    std::unique_ptr<D> pass_through(std::unique_ptr<D> p)
    {
        p->bar();
        return p;
    }

    void noOwnershipTransfer(const std::unique_ptr<D>& p)
    {
        p->bar();
        // const std::unique_ptr<D> q = std::move(p);  compile error as the operator is deleted.
    }

    void ownershipTransferInFunc(std::unique_ptr<D>& p)
    {
        p->bar();
        const std::unique_ptr<D> q = std::move(p);
    }

    int main()
    {
        std::cout << "1) Unique ownership semantics demo\n";
        {
            // Create a (uniquely owned) resource
            std::unique_ptr<D> p = std::make_unique<D>();

            // Transfer ownership to `pass_through`,
            // which in turn transfers ownership back through the return value
            std::unique_ptr<D> q = pass_through(std::move(p));

            // `p` is now in a moved-from 'empty' state, equal to `nullptr`
            assert(!p);
        }

        std::cout << "\n" "1.1) Unique ownership semantics demo\n";
        {
            // Create a derived resource and point to it via base type
            const std::unique_ptr<D> p = std::make_unique<D>();

            noOwnershipTransfer(p);
            std::cout << "noOwnershipTransfer end\n";

        }

        std::cout << "\n" "1.2) Unique ownership semantics demo\n";
        {
            // Create a derived resource and point to it via base type
            std::unique_ptr<D> p = std::make_unique<D>();

            ownershipTransferInFunc(p);
            std::cout << "ownershipTransferInFunc end\n";
        }
    }

   ```
   ```
    // 执行结果
    1) Unique ownership semantics demo
    D::D
    D::bar
    D::~D

    1.1) Unique ownership semantics demo
    D::D
    D::bar
    noOwnershipTransfer end
    D::~D

    1.2) Unique ownership semantics demo
    D::D
    D::bar
    D::~D
    ownershipTransferInFunc end
    ```
3. 定义`std::unique_ptr<D>`，会隐式的转换成`std::unique_ptr<B>`（D继承与B)。`unique_ptr`使用`B`的delete进行析构，所以，如果`B`的析构为非虚的，那么会导致未定义的行为。shared_ptr<B>使用`D`的delete(即使B析构为非虚的函数)，所以不会存在问题。
## REF
[unique_ptr](https://en.cppreference.com/w/cpp/memory/unique_ptr)

# lifetime
## reference或pointer指向了一个临时对象，而临时对象释放了，导致异常
## auto使用，引入了临时变量，导致异常
## 指针作了一次拷贝，源指针所指对象释放了，导致拷贝后的指针空悬了
## 规则1 解引用一个无效的指针是错误的
### 规则1.a 使用一个移动的对象是错误的
### 规则1.b 给一个声明一个non-null的指针，赋值可能为null的指针是错误的
### 规则1.c 给non-local指针赋值一个可能指向某对象的（除了null或者global）是错误的
### 规则 裸指针的算数运算是不允许的，如裸指针`p`及变量`x`，这些运算是错误的，`p+x, x+p, p[x],p+=x, p++, p-x, --p, p--`
## 规则2 给一个`const`(如一个引用)或者一个`non-local`指针p拷贝一个无效的指针是错误的
## 规则3 除了一个`指针构造`的`this`参数，把一个无效的指针传给一个函数是错误的，或者如果指针违法的前置条件，或者指针指向一个`non-const`全局`Owner`，一个本地的owner通过引用传参给同一函数的一个non-const， 或者一个的本地的Owner被另一个指针的`pset`引用，被传到了同一个函数。
## 规则4 函数退出后，若指针所指向的`函数输出`是无效或者违反后置条件，是错误的



