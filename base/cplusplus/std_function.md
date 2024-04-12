# std::functional
a function wrapper template. 其实例对象，可以store、copy and invoke 任何可调用的对象，如functions , lambda expressions, member functions

## how to use
1. 通过构造或者`=`来传入可调用对象
2. 通过`()`来调用可调用的对象
例子
```cpp
#include <functional>
#include <iostream>
int main()
{
    std::function<void(int)> f = [](int i){std::cout << i << std::endl;};
    f(2);
    return 0;
}
```
## reference
https://en.cppreference.com/w/cpp/utility/functional/function
