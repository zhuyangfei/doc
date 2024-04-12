# vTable layout in gcc

## virtual class
包含一个指向虚表地虚表指针，即vptr -> vTable

## vTable layout
1. virtual func
2. object destructor
3. deleting destructor

## typeinfo  ahead of vTable in gcc
[virtual function](https://www.avabodh.com/cxxin/virtualfunction.html#:~:text=There%20is%20a%20vtable%20and%20typeinfo%20generated%20for,of%20all%20virtual%20functions%20defined%20by%20the%20class.)

## two destructors in virtual functions
[C++: Deleting destructors and virtual operator delete](https://eli.thegreenplace.net/2015/c-deleting-destructors-and-virtual-operator-delete/)
[two destructor implementations](https://stackoverflow.com/questions/44558119/why-do-i-have-two-destructor-implementations-in-my-assembly-output)

## virtual destructor
`virtual destructor`有配对的`entries`，一个是`object destructor`，另一个是`delete destructor`。
在`Itanium C++ ABI`进行规定，由编译器实现。
为什么要这么进行规定？

## delete expression
delete object created by new and release obtained memory.
[delete](https://en.cppreference.com/w/cpp/language/delete)
```cpp
base b = new derived();
delete b;
```
`delete b`,背后的实现：编译器，通过`vTable`找到`delete constructor`, 在`deleting constructor`中，先调用`object destructor`,再调用`::operate delete`。
