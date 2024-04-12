# deducing

## template deducing rule
原型
```cpp
template<typename T>
void f(ParamType param);

f(expr);
```
编译器通过expr，来推导ParamType及T的类型

- scenario 1: ParamType is reference or pointer, i.e T& or T*
  ```cpp
    template<typename T>
    void f(T& param);
    int x
    const int x1 = x ;
    const int& x2 = x ;
    f(x);       // T is int, param is int&
    f(x1);      // T is const int, param is const int&
    f(x2);      // T is const int, reference be removed , param is const int&
    const int* x3 = &x;
    f(x3);      // T is const int*, param is const int* &
    template<typename T>
    void f(T* param);
  ```
- scenario 2: ParamType is universal reference
  实参是右值时，param推断为右值，其他情况，param推断为左值引用，保留const等限定
  ```cpp
    template<typename T>
    void fu(T&& param)  // f pass by universal
  ```
- scenario 3: ParamType is pass by value
  T及Param推断时，消除实参所有的const，volatile限定，因为param只是一个副本
  1. 数组实参，退化成指针
  2. 函数类型，退化成函数指针

```cpp
// array deduce
template<typename T>
void f(T& param);

const char[] name = "joey";

f(name);    // T is const char[5]
```

## auto deducing
与模板推导规则一致，除了`auto`支持`std::initializer_list<T>`
```cpp
auto v = {1, 2, 3};  // type is std::initializer_list<int>

template<typename T>
void f(T param);
f({1, 2, 3});   // not be allowed

template<typename T>
void f(std::initializer_list<T> param);

f({1, 2, 3});
```

## decltype deducing
用于返回expression或者variable的类型
- 当推导expression时，会推导成T的引用, i.e T&
```cpp
decltype(auto)  // 以decltype的规则进行推导

int x;
decltype(x);    // int
decltype((x));  // int&, deducing expression (x), return T&

Widget w;

const Widget& cw = w;

auto myWidget1 = cw;                    //type myWidget1 is Widget

decltype(auto) myWidget2 = cw;          // type myWidget2 is widget reference

```
- 为什么要使用decltype(auto)？
  当你要保留表达式的类型，即左值引用时，使用decltype(auto)
  ```cpp
    namespace
    {
        template<typename FuncType,
             typename MuxType,
             typename PtrType>
        decltype(auto) lockAndCall(FuncType func,       //C++14
                                   MuxType& mutex,
                                   PtrType ptr)
        {
            std::lock_guard g(mutex);
            return func(ptr);
        }

        class Person
        {};

        static int i = 0;

        int& f1(const std::shared_ptr<Person> p)
        {
            return i;
        }

    } // namespace name

    int main()
    {
        std::mutex mtx;

        lockAndCall(f1, mtx, nullptr) = 2;

        return 0;
    }
  ```
