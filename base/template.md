# template

## 使用场景
代码结构相同，只是类型不一样，比如，STL的algorithm库，通过template，能够精简代码。

## 模板函数
```cpp
template <typename T>
T max(T a, T b)
{
    return b < a ? a : b;
}

// usage
std::cout << max(1, 2);
std::cout << max<double>(1, 2.1);
```

## 模板类
```cpp
template <typename T>
class Stack
{
private:
    std::vector<T> elements;

public:
    void push(const T& e);
};

// usage
Stack<int> intStack;
```

## instance
实例化即把特定地类型传给模板参数,

## 特化
显式地给模板赋值特定类型

## template trait（模板特征）
`template trait`是模板的属性，`template trait` 与模板搭配使用，实现了对原有模板功能扩展。比如，char 数据的累积，其和类型应该是大于char，如int。
`template trait`的功能，也可以通过多参数的模板来替换

## Q&A

## reference
[](https://github.com/xiaoweiChen/Cpp-Templates-2nd)
http://www.tmplbook.com/