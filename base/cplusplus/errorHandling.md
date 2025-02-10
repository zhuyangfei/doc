# 异常处理error handling

    异常处理包括：
    - 检测错误
    - 传递错误的信息给异常处理代码
    - 维护程序的有效状态
    - 避免资源泄漏
    程序不可能从所有错误中恢复，如果不能从一个错误中恢复，那么快速的以一种被“很好定义"的方式退出就很重要。
    异常处理的策略必须简单，否则，就是更严重的问题（bug）的源头。
    一个未经测试并很难运行到的错误处理代码，其本身就可能包含很多错误。
    接下来的编码规范/规则，用于避免如下错误：
    - 类型违反（e.g. 误用union及cast）
    - 资源泄漏 （包括内存泄漏）
    - 边界错误 (Bounds errors)
    - 生命周期错误 (e.g. 在一个对象被释放后，访问该对象)
    - 复杂错误（Complexity errors）（因为复杂的表达式而引入的逻辑错误）
    - 接口错误 （e.g., 传给接口一个不期望的值）

## 1. 在早期的设计阶段，开发一种异常处理的策略
- 为什么
    一种处理异常及资源泄漏的固定不变及完整的策略很难再适用到一个系统
## 2. 抛出异常(Exception)来表明一个函数不能完成分配（预设）的功能
- 为什么
    让异常处理是系统的、robust的及不重复的
- 例子
  ```cpp
  struct Foo {
        vector<Thing> v;
        File_handle f;
        string s;
    };
    void use()
    {
        Foo bar {{Thing{1}, Thing{2}, Thing{monkey}},   {"my_file", "r"}, "Here we go!"};
        // ...
    }
  ```
vector 及string 的构造时没有足够的内存，vector的不能完成列表的拷贝初始化，File_handle不能打开一个所需的文件，
上述的每一种情况，都会抛异常给use()。如果use()可以处理这些异常，那么use()可以使用try/catch。
**任何情况下，在把控制交给Foo的创建者之前，Foo的构造能够正确的析构成员。**
注：这里没有返回值来表示异常值。
File_handle 的构造可以这样定义：
    File_handle::File_handle(const string& name, const string& mode)
            : f(open(name.c_str(), mode.c_str()))
    {
        if (!f) throw runtime_error{"File_handle: could not open" + name + "as" + mode};
    }
- 注:
1. 用Exception表明例外的事件或者错误，如
- 一个前置条件没有满足
- 不能构造对象
- 越界错误（e.g. v[v.size()] = 7）
- 请求不到资源(e.g. 网络掉线了)
相反的，从一个正常的循环（除了无限的循环）中退出不是一种异常，这种退出是正常及被期望的。
- 注：
2. 不要用throw作为函数返回值的一种替代方案
- 注：
3. 在决定不使用或者不喜欢基于exception的异常处理之前，同样去检视下替代方案的复杂性与问题。
请尽可能的先测试，然后，再确认效率怎样。

## 表明接口的前置条件
前置条件是前提条件，在函数参数有限制的情况下，保证参数被正确使用，从而使被调用函数正常工作
- 为什么
    参数所具有的实际含义，限定了如何被正确的使用
- 举例
    double sqrt(double x);
    这里的X，需要大于等于0，函数sqrt才能正常工作。而参数x的类型double，并不能涵盖这个限定。

    一种办法是接口上加注释，如：
    double sqrt(double x);    // x must be non-negative

    或者使用if，进行判断，如：
    double sqrt(double x) { if(x < 0>) my_error(); }    // obscure

**更好的方式是使用断言，如:**
    double sqrt(double x) { Experts(x > 0); /*...*/ }

- 注：
    1. [Experts ](https://github.com/Microsoft/GSL)是断言的一种实现，比系统函数assert更好。
    2. 使用unsigned int，不是“确保一个变量非负”的好办法

## 表明接口的后置条件
    后置条件是函数调用后，结果需要满足的条件，在检测可能的实现错误及预期结果上，保证函数功能正常
- 为什么
    用于检查结果及可能的实现错误
- 举例，
**不好的实现**
    int area(int height, int width) {return height * width; }  // bad
    第一点，没有检查前置条件（Expect(height >0 && width > 0)）, 第二点，没有检查后置条件，height * width的
    结果可能会导致整型溢出。
**更好的实现**
    int area(int height , int width)
    {
        Expect(height >0 && width > 0);
        auto area = height * width;
        Ensure(area > 0);
        return area;
    }
### 举例，
    //一个有名的安全问题
    void f()    // problematic
    {
        char buffer[MAX];
        // ...
        memset(buffer, 0, sizeof(buffer));
    }
    memset之后，没有确认buffer是否已经被清除。
**更安全的实现**，
    void f()    // better
    {
        char buffer[MAX];
        // ...
        memset(buffer, 0, sizeof(buffer));
        Ensures(buffer[0] == 0);
    }
- 注：
    1. [Ensures](https://github.com/Microsoft/GSL)是断言的一种实现，让后置条件的检查，更加的系统，可见及可检查。
    2. 表明后置条件，有很多方式，如接口注释、if、assert，但是，相比Ensures，这些方式，都不能在代码很好的被识别，很难更新，很难使用工具维护或者存在语义的问题。
    3. 在结果不直接反映到返回值的情况下，后置条件更加重要，如，数据结构的状态。
- 举例
    用户使用锁，保证数据安全。假如我们忘记要在manipulate中进行解锁，用户不知道不解锁是错误还是正常。若
    我们使用“m is unlocked upon exit”来表明接口的后置条件，如：
    ```cpp
    void manipulate(Record& r)    // postcondition: m is unlocked upon exit
    {
        m.lock();
        // ... no m.unlock() ...
    }
    ```

**有了后置条件，就说明，我们没有释放锁是一种错误实现。**
    若后置条件中包含“资源需要释放”这类字眼，**使用RAII，表示后置条件是更好的方式**
    void manipulate(Record& r)    // best
    {
        lock_guard<mutex> _ {m};
        // ...
    }

## 异常处理策略
策略1：将error分成由调用者处理（be handled locally）或者只能在调用链上处理；
be handled locally：通过返回值（错误代码）实现；
在调用链上处理：通过`exception`实现；
https://cacm.acm.org/blogcacm/21st-century-c/
