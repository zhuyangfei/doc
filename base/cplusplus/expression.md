# expression

- [what](#what)
- [value category](#value-category)
- [T\&\&](#t)
- [引用折叠(reference collapsing)](#引用折叠reference-collapsing)


## what
a sequence of a operator and operand

## value category
- xvalue, a variable's life time can transfer
- prvalue, a literal value, such 10
- `lvalue`: 可以引用的对象，如通过名字，指针，左值引用
  判断方法：如对象可以取地址，即可以使用操作符`&`，那一般是左值，否则是右值
- `rvalue`: 从函数返回的temporary variable

## T&&
- 通用引用
需要推导时，是通用引用
```cpp
template<typename T>
void f(T&& )

auto&& var2 = var1;
```
- 右值引用
具体的类型，比如
```cpp
template<typename T>
void f(std::vector<T>&& param);

void f(Widget && );

Widget&& var2 = var1;

template<typename T>
void f(const T&& param);    // const make param to be a rvalue reference

template<class T, class Allocator = allocator<T>>
class vector
{
public:
    void push_back(T&& x);    // it a rvalue reference, as T doesn't deduce
                              // std::vector<int> v; type has already specified after constructing.
                              // so push_back doesn't need to deduce
    …
}
```
可以通过传一个左值，来区分右值还是通用引用，如
```cpp
std::vector<int> v;
f(v);     // compilation error
```

## 引用折叠(reference collapsing)
```cpp
template<typename T>
void f(T&& param);

Widget w1;
f(w1);            // T deduce as Widget&

Widget& w2 = w1;
f(w2)

Widget makeWidget();
f(makeWidget());    // T deduce as Widget

```
- 推导结果
`f`传入左值，`T`被推导成左值引用，传入右值，推导成非引用。
- 如何实现
通过`reference collapsing`, 即仅有右值引用折叠成右值，其他情况，都折叠成左值引用
- 适用场景
  模板推导，`auto`推导，`decltype`, using 及typedef
