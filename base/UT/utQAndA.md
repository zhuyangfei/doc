# Q&A

- [Q&A](#qa)
  - [1. 写UT时，发现很难进行测试？](#1-写ut时发现很难进行测试)
    - [注：](#注)
    - [依赖注入Dependency Injection](#依赖注入dependency-injection)
    - [注：](#注-1)
  - [2. 基类的构造函数，需要重新实现吗？](#2-基类的构造函数需要重新实现吗)
    - [注：](#注-2)
  - [3. 对象B只使用对象A的部分接口，如何mockA？](#3-对象b只使用对象a的部分接口如何mocka)
  - [4. 类之间组合关系、聚合关系、依赖关系，如何设计与选择？](#4-类之间组合关系聚合关系依赖关系如何设计与选择)
    - [注：](#注-3)
  - [5. 一个module/class一个test bin，还是所有模块，一个test bin？](#5-一个moduleclass一个test-bin还是所有模块一个test-bin)
  - [6. 如何注入mock对象(适用已经发布的模块)，进行测试？](#6-如何注入mock对象适用已经发布的模块进行测试)
    - [例子](#例子)
  - [7. 如何进行mock，才能共享mock对象？](#7-如何进行mock才能共享mock对象)
  - [同问题：被测对象的所有依赖，放到一个mock对象里面，还是每个依赖一个mock对象?](#同问题被测对象的所有依赖放到一个mock对象里面还是每个依赖一个mock对象)
  - [8. 库接口的设计者，为什么要提供mockable测试对象？](#8-库接口的设计者为什么要提供mockable测试对象)

## 1. 写UT时，发现很难进行测试？
这个问题，同UT框架及编程方式直接相关。
比如，gtest，其`mock对象`能够实现替换`真实对象`主要技术手段就是`依赖注入`与`运行时多态`。gmock例子：
```c++
// an interface class
// Turtle.hpp
class Turtle {
  ...
  virtual ~Turtle() {}
  virtual void PenUp() = 0;
  virtual void PenDown() = 0;
};

// mock API with gmock in test
// MockTurtle.hpp
class MockTurtle : public Turtle {
  public:
  ...
  MOCK_METHOD(void, PenUp, (), (override));
  MOCK_METHOD(void, PenDown, (), (override));
};
```
从上面可以看出来几个要点，
1. 需要一个接口类（纯虚）或者包含virtual接口（需要mock）的类
2. mock类继承于接口类
[A Case for Mock Turtles](https://google.github.io/googletest/gmock_for_dummies.html#a-case-for-mock-turtles)

### 注：
`gtest`假设编程者都是使用`依赖注入(Dependency Injection)`及`基于接口编程`原则进行编程。
如果基于`Dependency Injection`与`基于接口编程`原则来编程，那么user类，通常的设计，如下：
```c++
// class Turtle User
class TurtleUser
{
public:
   TurtleUser(Turtle& turtle);      // interface turtle be injected to Turtle user
   void drawTurtle()
   {
        m_turtle.PenUp();
        m_turtle.PenDown();
   }
private:
   Turtle& m_turtle;
};

```
在TurtleUser构造时，传入`接口Turtle`。对这样设计的`TurtleUser`类进行UT，就变得比较轻松。
完整代码结构，通常如下：
```c++
// turtle implementation(concrete turtle)
class TurtleA : public Turtle
{
    void PenUp() override;
    ...
};

// in main.cpp
void main()
{
    TurtleA turtleA;
    TurtleUser turtleUser(turtleA);
    turtleUser.drawTurtle();
    ...
}
```

### 依赖注入[Dependency Injection](https://en.wikipedia.org/wiki/Dependency_injection)
`Dependency Injection`一种设计模式，用于把`对象的创建`与`对象使用`解耦。使用者，不需要创建依赖对象。简单的说，就是`传参`。
同时，为了有更好的灵活性，通常传入参数是`接口`，而不是`实现类(concrete implementations)`，与之对应设计原则就是`基于接口编程`，比如，`依赖倒置`就是其实现形式。

违反这些编程原则，导致UT变得很困难。
常见例子，如
1. 直接调用系统函数，如open，syslog等等
```c++
class A
{
public:
    void openStream(const std::string& filename)
    {
        int fd = open(filename.c_str()， O_RDONLY | O_CREAT);
    }
};
```
可测性代码的实践技巧，
将系统函数，封装到一个接口类中，如class system。
详细实现可参考：
[mock systemcall](https://rcp.gitlabe2-pages.ext.net.nokia.com/rcp-docs/development/testing/unittest/mocksyscalls.html)
[genapi system call](https://gitlabe1.ext.net.nokia.com/genapi/genapi/-/blob/master/include/private/System.hpp)

2. 直接调用三方库的C接口或者非虚接口
```c++
class A
{
public:
    void func(void* receiveMsg)
    {
        AaSysComMsgGetId(receiveMsg);
    }
};
```
可测性代码的实践技巧，
在自己的工程中，定义一个三方模块的接口类。其他模块使用该接口类，进行编程。
详细实现可参考：
[syscom in trbl](https://gerrite1.ext.net.nokia.com/admin/repos/scm_rcp/trbl)
### 注：
其可测性技巧与`直接调用系统函数`的可测序技巧，是一样的。

3. 基于实体类编程
```c++
class B
{
public:
    void func();
};
class A
{
public:
    A(B b){};
private:
    B m_b;
};
```
可测性代码的实践技巧,
```c++
class B
{
public:
    virtual ~B();
    virtual void func();
};
class A
{
public:
    A(const B& b){};
private:
    const B m_b;
};
```

4. 在函数内，直接使用单例对象进行调用
同在函数中，直接创建依赖对象进行调用
```c++
class B
{
public:
    void func();
    static B& instance();
private:
    B();
    ~B();
};
class A
{
public:
    void funcA()
    {
        B::instance().func();
    }
};
```
可测性代码的实践技巧,
通过`依赖注入`的方式，使用依赖对象，如：
```c++
// TimerFD.hpp
class TimerFD: public TimerService
{
public:
    explicit TimerFD(FDMonitor& fdMonitor);
    // 1. 可注入systemmock的构造
    TimerFD(System& system, FDMonitor& fdMonitor);
private:
    System& system
}

// TimerFD.cpp
TimerFD::TimerFD(FDMonitor& fdMonitor):
TimerFD(System::getSystem(), fdMonitor)     // call default system object
{}

TimerFD::TimerFD(System& system, FDMonitor& fdMonitor):
    system(system),
    fd(system.timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC), system)
{
    fdMonitor.addFD(fd, FDMonitor::EVENT_IN, std::bind(&TimerFD::handleEvents, this));
}

```
详细实现，可参考：
[genapi TimerFD](https://gitlabe1.ext.net.nokia.com/genapi/genapi/-/blob/master/src/TimerFD.cpp)

一方面，熟悉UT框架，能够帮你写出`可测试性`更好的代码。
另一方面，好的编程方式，同样能实现高`可测试`代码的目标。比如使用TDD（Test driven development）方式进行开发，那基本不存在代码可测性的问题。UT代码与product代码是一起完成的。
TDD的实践，大家可以参考:[Tuomo TDD video tutorial](https://web.yammer.com/main/org/nokia.com/threads/eyJfdHlwZSI6IlRocmVhZCIsImlkIjoiMTY1MzI5MzIwNjg3MjA2NCJ9)


## 2. 基类的构造函数，需要重新实现吗？
假设类之间的依赖关系，C依赖B, B依赖A
如下：
```c++
// A.hpp
class A
{
private:
    /* data */
    int m_a;
public:
    explicit A (int i);
    virtual ~A() = default;
    virtual int getValue() const;
    virtual void unMockFunc();
};

// B.hpp
class B
{
private:
    const A& m_a;
public:
    explicit B(const A& a);
    virtual ~B();
    virtual int getValue() const;
};

// C.hpp
class C
{
public:
    explicit C(const B& b);
    virtual ~C();
    virtual int getValue() const;
private:
    const B& m_b;
};
```
ut bin通过链接A.cpp、B.cpp、C.cpp, 并不需要重新实现构造函数，如A::A, 也不需要mock未使用的接口。
详细例子，请参考：
[ut example](https://gitlabe2.ext.net.nokia.com/yanzhu/utexample)

### 注：
1. UT时，能否链接三方库？
   A：可以链接三方库，但是，被调用三方库的接口，需要被mock。
2. 更好的实践，可以把所有源cpp文件编译成一个内部的la, 这样test code与product code的Makefile可以更简洁。
3. 通用技巧：tst链接源实现，只mock使用的接口

## 3. 对象B只使用对象A的部分接口，如何mockA？
只mock A的被使用的接口，通过链接源实现，未调用到的接口不需要mock。
通过这种方式，可以提高开发效率。
详细例子，请参考：
[ut example](https://gitlabe2.ext.net.nokia.com/yanzhu/utexample)

## 4. 类之间组合关系、聚合关系、依赖关系，如何设计与选择？
假如有A、B两个对象，设计时，你会考量对象之间的关系，
```
组合关系，即B包含A，A的生命周期由B决定。B析构，A也析构。如：人(B)与手脚(A)的关系
代码实现如下：
class A
{
public:
    void funcA();
};
class B
{
private:
    A a;        // object a created in B
public:
    void funb() {a.funcA();}
}

聚合关系，即B拥有A，A的生命周期与B独立。B析构，A还可以存在。如：房子（B）与沙发（A）的关系
代码实现如下：
class B
{
public：
    B(const A& a);
    void funb() {a.funcA();}
private:
    const A& a;
}

依赖关系，即B使用A或者B知道A，两者关联的时间很短，如：人(B)与锤子(A)的关系
代码实现如下：
class B
{
    void func() {A a; a.funcA()};             // method 1
    void func2() {getInstanceA()->funcA();}  // method 2, getInstanceA to get a singleton objectA
    void func3(const A& a){a.funcA();}      // method 3, ok for ut
}
```
组合关系与依赖关系，大多数情况，都是很难进行UT。
一种比较讨巧的做法，在test code中，重新实现A对象（如mockA），然后，链接mockA。比如：BTest_SOURCES = B.cpp mockA.cpp, 与之对应的一般情况是，BTest_SOURCES = B.cpp mockA.cpp A.cpp。但是，这种方法，需要mock所有的A接口（即使B对象没有使用），因此，开发效率相对较低。同时，也因为基于`实现编程`，所以,灵活性也不够。
聚合关系，就是`依赖注入`原则的一种体现，所以，类之间使用聚合关系，方便UT。
### 注：
1. 类关系说明，请参考: 《UML2.0 in a Nutshell》 charpter 2.6 relationships
2. 依赖关系中的`method3`与聚合关系，在使用上（都是传参）是一样的

## 5. 一个module/class一个test bin，还是所有模块，一个test bin？
这个问题取决于怎么编码，与UT无关。
如果使用`基于接口`的方式进行编程，那么所有的文件都可以编到一个bin进行测试（符号并不会冲突）。无论是gtest，还是cpputest，都可以这么做。
如果在tst里面，通过重新实现接口的方式mock接口，那么就不能把所有文件（如A.cpp与MockA.cpp会冲突）编到一个bin里进行测试。

## 6. 如何注入mock对象(适用已经发布的模块)，进行测试？
   类接口新增传参（传入mock对象）的构造函数，原先构造函数调用新构造函数, 这样app代码不需要修改。
```c++
    // previous TimerFD design
    // TimerFD.hpp
    class TimerFD: public TimerService
    {
    public:
        explicit TimerFD(FDMonitor& fdMonitor);
    }；

    // refactor design for UT
    // TimerFD.hpp
    class TimerFD: public TimerService
    {
    public:
        explicit TimerFD(FDMonitor& fdMonitor);
        // 1. 可注入systemmock的构造
        TimerFD(System& system, FDMonitor& fdMonitor);
    private:
        System& system
    }

    // TimerFD.cpp
    TimerFD::TimerFD(FDMonitor& fdMonitor):
    TimerFD(System::getSystem(), fdMonitor)     // call default system object
    {}

    TimerFD::TimerFD(System& system, FDMonitor& fdMonitor):
        system(system),
        fd(system.timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC), system)
    {
        fdMonitor.addFD(fd, FDMonitor::EVENT_IN, std::bind(&TimerFD::handleEvents, this));
    }
```
app/user 调用`TimerFD(FDMonitor& fdMonitor)`创建TimerFD对象，UT中，通过`TimerFD::TimerFD(System& system, FDMonitor& fdMonitor)`创建TimerFD对象。
设计上，满足使用者，或许对`TimerFD`实现上依赖的`system`对象并不关心的需求，同时，又提高代码的可测试性。
### 例子
[Tuomo injecting mock video tutorial](https://web.yammer.com/main/org/nokia.com/threads/eyJfdHlwZSI6IlRocmVhZCIsImlkIjoiMTc3NTAzMzU2OTc2MzMyOCJ9)
[genapi TimerFD](https://gitlabe1.ext.net.nokia.com/genapi/genapi/-/blob/master/src/TimerFD.cpp)

## 7. 如何进行mock，才能共享mock对象？
## 同问题：被测对象的所有依赖，放到一个mock对象里面，还是每个依赖一个mock对象?
应该每个依赖，一个mock对象，这样mock对象，才能在测试不同模块时被复用。
```
不好的例子
    yanzhu@N-20S1PF2NEE9X:~/workbench/syscom/tst/src$ grep -rn RTOS_getEuName
    UT_AaSysComUserRegister_stub.hpp:26:    virtual u32 RTOS_getEuName(u32 euId, char *buffer, u32 length) = 0;
    UT_AaSysComUserRegister_stub.hpp:42:    MOCK_METHOD(u32, RTOS_getEuName, (u32 euId, char *buffer, u32 length));
    UT_MsgRTOSPoolAnalyer_stub.hpp:24:    virtual TBoolean RTOS_getEuName_mock(u32 euId, char *buffer, u32 length) = 0;
    UT_MsgRTOSPoolAnalyer_stub.hpp:38:    MOCK_METHOD(TBoolean, RTOS_getEuName_mock, (u32 euId, char *buffer, u32 length));
    UT_CNMapGwSCTPCtrlThreadParent_stub.cpp:86:TBoolean RTOS_getEuName(u32 euId, char *buffer, u32 length)
    UT_AaSysComUserRegister_stub.cpp:24: u32 RTOS_getEuName(u32 euId, char *buffer, u32 length)
    UT_MsgRTOSPoolAnalyer_stub.cpp:9:TBoolean RTOS_getEuName(u32 euId, char *buffer, u32 length)
    UT_MsgRTOSPoolAnalyer_stub.cpp:11:    return RtosPoolAnalyer_mock->RTOS_getEuName_mock(euId, buffer, length);
    UT_MsgRTOS.cpp:79:    RTOS_getEuName(rtos_core->eus[1].euId, euName, sizeof(euName));
```
测试代码中有很多的重复定义与实现，开发者，要使用`RTOS_getEuName`的mock接口，不知道要引用`UT_AaSysComUserRegister_stub.hpp`或者`UT_MsgRTOSPoolAnalyer_stub.hpp`, 同时，在引用的文件中，可能存在当前被测对象接口的`重定义`，导致mock的对象不能复用。
```
好的例子，定义一个MsgRTOSMock.hpp，如
    class MsgRTOSMock : public MsgRTOS
    {
    public:
        ...
        MOCK_METHOD(u32, RTOS_getEuName, (u32 euId, char *buffer, u32 length));
        ...
    };
```
`MsgRTOS`模块，仅存在一个`MsgRTOSMock.hpp`，其他使用了MsgRTOS模块的被测对象，都引用`MsgRTOSMock.hpp`对象进行注入。
一开始，`MsgRTOSMock.hpp`只会mock，当前被使用的接口。随着开发的深入，`MsgRTOSMock.hpp`就会累积支持所有`MsgRTOS`的接口，后续UT时，就能复用前人的成果了。

## 8. 库接口的设计者，为什么要提供mockable测试对象？
如果不提供一个mockable的测试对象，那么在库接口更新时，用户的测试代码会因为继承接口强耦合性而出现编译问题。
使用场景如下：
```c++
//====== source code
// 3rd party API definitions
namespace ministarter
{
    class Connection
    {
    public:
        /**
         * Heartbeat callback function type.
         *
         * @see setHeartbeatCb
         */
        using HeartbeatCb = std::function<void()>;

        virtual ~Connection();

        /**
         * Notify ministarter that this service is ready to serve other services.
         * Subsequent calls are ignored.
         */
        virtual void notifyReady() = 0;
        ...
    }
}

// application code
// class Plugin depends on ministarter::Connection
class Plugin : public std::enable_shared_from_this<Plugin>
{
public:
    using HeartbeatCb = std::function<void()>;
    explicit Plugin(std::shared_ptr<ministarter::Connection> connection);
    ...
}
```
```c++
//====== test code
// in ConnectionMock.hpp
class ConnectionMock: public ministarter::Connection
{
public:
    MOCK_METHOD0(notifyReady, void());
    ...
};
```
因为需求变化，`ministarter::Connection`新增了一个接口`virtual uint64_t getWatchdogMicroSec() const = 0;`, 这会导致application的UT代码，出现编译失败异常（虚接口`getWatchdogMicroSec`没有被重载）。
通用的解决办法，
库设计者提供一个`mockable对象`，来解耦`ConnectionMock`与`ministarter::Connection`, `mockable对象`实现如下：
```c++
// mockable API
// include/ministarter/tst/mockableconnection.hpp
namespace ministarter
{
    namespace tst
    {
        class MockableConnection : public Connection
        {
        public:
            MockableConnection() {}

            virtual void notifyReady() override { logAndAbort(__PRETTY_FUNCTION__); }
            virtual uint64_t getWatchdogMicroSec() const override { logAndAbort(__PRETTY_FUNCTION__); }     // the new added API
            ...
        private:
            static void logAndAbort(const char *function) noexcept __attribute__((__noreturn__))
            {
                std::cerr << "MockableConnection: calling not-mocked function "
                          << function << ", aborting" << std::endl;
                abort();
            }
        };
    }
}
```
`MockableConnection`继承于`Connection`, 并提供了默认的实现。该默认实现，并不依赖特定的UT框架，所以，用户使用的UT框架不会被限制。
库维护者，在新增接口后，需要在`MockableConnection`实现新接口。
该`MockableConnection`仅用在application的UT代码，product代码保持不变。
在用户的测试代码中，继承于`MockableConnection`，替换之前的`ministarter::Connection`, 新的测试代码，如下：
```c++
class ConnectionMock: public ministarter::tst::MockableConnection
{
public:
    // API mock keep the same, need not to mock the unused API getWatchdogMicroSec
    MOCK_METHOD0(notifyReady, void());
    ...
}
```
代码的详细实现，可以参考：
[ccsplugin-ministarter about commit id 616e101 and b6d58f9](https://gerrite1.ext.net.nokia.com/admin/repos/scm_rcp/ccsplugin-ministarter)
[mockableconnection.hpp](https://gitlabe1.ext.net.nokia.com/tuturune/ministarter/-/blob/master/include/ministarter/tst/mockableconnection.hpp)
