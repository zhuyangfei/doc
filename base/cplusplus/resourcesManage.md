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
