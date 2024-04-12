# modern cpp

## 优先使用`enum class`
  `enum class`限定了enum的作用域，避免了变量被遮蔽的情况，同时，默认大小为int，所以，可以使用前置申明，减少编译依赖。
- usage
```cpp
enum class Color {WHITE, RED, YELLOW};
auto color = Color::WHITE;
```

## 优先使用`using`而不是`typedef`来声明类型别名
  别名可以简化代码，同时，提高代码的可读性。通过`using`定义的类型别名，适用范围更广，同时，代码更简洁。比如，`using`的类型别名，可以直接在模板类中使用。
  举例：
  ```cpp
  // code readability, code more clean
  using xxxCallBack = std::function<void()>;
  using Key = int
  using Paths = std::vector<std::string>;
  void add(Key key);
  ```
- note
  在c的头文件，需要继续使用`typedef`,如 `typedef void* xxxHandle`

## 优先使用`delete`而不是`undefined private`成员函数来禁用函数
`delete`即可以用在成员函数，也可以用在全局函数，同时，通过`delete`，可以限定模板的特化版本。
举例
```cpp
class Widget
{
public:
  Widget& Widget(const Widget& widget) = delete;
}
void isDigital(int number);
void isDigital(char number) = delete;
```

## 派生类，重写接口时，使用override
`override`关键字，强制编译器检查，派生类重写的接口，是否与基类完全一样。

## 尽可能使用`noexcept`
[noexcept](./noexcept.md)

## 尽可能得使用`constexpr`
[constexpr](./constexpr.md)
