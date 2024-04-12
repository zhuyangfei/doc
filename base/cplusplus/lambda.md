# lambda expression
lambda expression 是一个小的，匿名的函数对象，即一小段函数代码，其能够以`value`或者`reference`的方式`capture`周围的变量，其可以赋值给`std::function`使用。
**lambda expression只是生成类与创建类对象的一种简便的方式**
- enclosure
  lambda 创建的运行时对象
- closure class
  编译器为每个lambda表达式，生成一个类对象，即closure class，lambda expression 即类对象的成员函数，即重载了`operator()()`

[why](#why)
[when to use](#when-to-use)
[how to use](#how-to-use)
[liftime cycle](#liftime-cycle)
[rules](#rules)
[类实现与lambda对比](#类实现与lambda对比)
[reference](#reference)


## why
相比inline及a named function，使用lambda expression, 能让代码更简洁，可读性更好，即实现就在当前，不用跳转查找。

## when to use
1. 只被一个地方调用的函数，可以使用lambda 替换

## how to use
[capture](parameters){body} or [capture](parameters)-> return type {body}

## liftime cycle

## rules
- 不使用默认捕获，而使用显式的捕获
- 使用初始化捕获来把对象`move`到closure
  ```cpp
  auto pw = std::make_unique<Widget>();
  auto func = [pw = std::move(pw)](){};
  ```
- prefer lambda to std::bind

## 类实现与lambda对比
```cpp
#include <iostream>
#include <memory>

class Widget
{
public:
    Widget(bool valid, bool digital)
        : valid(valid),
          digital(digital)
    {}
    bool isValid() const
    {
        return valid;
    }
    bool isDigital() const
    {
        return digital;
    }
private:
    bool valid;
    bool digital;
};

class IsWidgetDigital
{
public:
    IsWidgetDigital(std::unique_ptr<Widget>&& ptr)
        : ptr(std::move(ptr))
        {}
    bool operator()() const
    {
        return ptr->isValid() && ptr->isDigital();
    }
private:
    std::unique_ptr<Widget> ptr;
};


int main()
{
    std::cout << "example for lambda init  capture \n";
    auto func = [pw = std::make_unique<Widget>(true, true)](){
        return pw->isValid() && pw->isDigital();
    };
    std::cout << "init lambda data member " << func() << '\n';

    auto wPtr = std::make_unique<Widget>(true, true);

    // auto func3 = [](){return wPtr->isValid() && wPtr->isDigital();}; can't compile
    auto func3 = [pw = std::move(wPtr)](){ return pw->isValid() && pw->isDigital();};
    std::cout << "pass lambda member data pw by move " << func3() << '\n';

    std::cout << "Implement a class instead of lambda\n";
    auto fun2 = IsWidgetDigital(std::make_unique<Widget>(false, true));
    std::cout << fun2() << '\n';
    return 0;
}
```

## reference
https://github.com/CnTransGroup/EffectiveModernCppChinese/blob/master/src/6.LambdaExpressions/item34.md
