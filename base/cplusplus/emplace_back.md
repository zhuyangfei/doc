# vector's emplace_back
- implement
  将args，通过`std::forward<Args>(args)...`方式传给构造函数
```cpp
template< class... Args >
void emplace_back( Args&&... args );
```


- [usage](#usage)
- [code example](#code-example)
- [注意事项](#注意事项)
- [ref:](#ref)


## usage
1. when put a `new object` to a vector, call `emplace_back`, else call `push_back`, such as a temporary or a named variable
```cpp
std::vector<string> vec;
// put a new object
vec.emplace_back(100,'c');

std::string s(100, 'c');
// put a exist object, in this cases, bad performance compare to `emplace_back`
vec.push_back(s);
```

## code example
```cpp
#include <vector>
#include <cassert>
#include <iostream>
#include <string>

struct President
{
    std::string name;
    std::string country;
    int year;

    President(std::string p_name, std::string p_country, int p_year)
        : name(std::move(p_name)), country(std::move(p_country)), year(p_year)
    {
        std::cout << "I am being constructed. " << this << '\n';
    }

    President(const President& other)
        : name(other.name), country(other.country), year(other.year)
    {
       std::cout << "I am being copy constructed. " << this << '\n';
    }


    President(President&& other)
        : name(std::move(other.name)), country(std::move(other.country)), year(other.year)
    {
        std::cout << "I am being moved " << this << '\n';
    }

    ~President()
    {
        std::cout << "I'm being destructed. " << this << '\n';
    }

    President& operator=(const President& other) = default;
};

int main()
{
    std::vector<President> elections;
    std::cout << "emplace_back:\n";
    auto& ref = elections.emplace_back("Nelson Mandela", "South Africa", 1994);
    assert(ref.year == 1994 && "uses a reference to the created object (C++17)");

    elections.emplace_back("bush", "South Africa", 2000);


    std::cout << "emplace_back: a temporary variable\n";
    elections.emplace_back(President("bush", "South Africa", 2000));

    std::cout << "emplace_back: a named variable\n";
    President obama("obama", "South Africa", 2010);
    elections.emplace_back(obama);

    std::vector<President> reElections;
    std::cout << "\npush_back:\n";
    reElections.push_back(President("Franklin Delano Roosevelt", "the USA", 1936));

    std::cout << "\nContents:\n";
    for (President const& president: elections)
        std::cout << president.name << " was elected president of "
                  << president.country << " in " << president.year << ".\n";

    for (President const& president: reElections)
        std::cout << president.name << " was re-elected president of "
                  << president.country << " in " << president.year << ".\n";
}
```
```bash
emplace_back:
I am being constructed. 0x144c030
I am being constructed. 0x144c0c8
I am being copy constructed. 0x144c080
I'm being destructed. 0x144c030
emplace_back: a temporary variable
I am being constructed. 0x7ffeb15df320
I am being moved 0x144c1b0
I am being copy constructed. 0x144c120
I am being copy constructed. 0x144c168
I'm being destructed. 0x144c080
I'm being destructed. 0x144c0c8
I'm being destructed. 0x7ffeb15df320
emplace_back: a named variable
I am being constructed. 0x7ffeb15df2d0
I am being copy constructed. 0x144c1f8

push_back:
I am being constructed. 0x7ffeb15df320
I am being moved 0x144c030
I'm being destructed. 0x7ffeb15df320

Contents:
Nelson Mandela was elected president of South Africa in 1994.
bush was elected president of South Africa in 2000.
bush was elected president of South Africa in 2000.
obama was elected president of South Africa in 2010.
Franklin Delano Roosevelt was re-elected president of the USA in 1936.
I'm being destructed. 0x144c030
I'm being destructed. 0x7ffeb15df2d0
I'm being destructed. 0x144c120
I'm being destructed. 0x144c168
I'm being destructed. 0x144c1b0
I'm being destructed. 0x144c1f8
```
- lesson learn
1. 新增一个新的对象时，使用`emplace_back`效率更好, 其他情况, `emplace_back` 及`push_back`效率一样，比如新增一个a temporary variable, a named variable
2. a temporary variable 调用移动构造，而a named variable 调用拷贝构造，所以，a temporary variable 比 a named variable 效率更好，详见video benchmark数据
3. 当vector空间不足时，vector会析构所有旧元素，然后，拷贝到新的位置
4. move a const object 会让move退化到copy的情况，详见video benchmark数据

## 注意事项
1. 在资源管理对象时，使用temporary对象，可以防止资源泄露，而直接使用emplace_back来构造一个对象，可能导致资源泄露，
推荐做法
```cpp
std::shared_ptr<Widget> spw(new Widget,    // create Widget and
                            killWidget);   // have spw manage it

ptrs.push_back(std::move(spw));            // add spw as rvalue
ptrs.emplace_back(std::move(spw));         // or
```
2. emplace_back，减弱的`explicit`显式构造的功能（有些错误，在编译阶段，没有被检查出来）
```cpp
std::regex r = nullptr;           // error! won't compile
regexes.push_back(nullptr);       // error! won't compile
std::regex r2(nullptr);          // compiles
regexes.emplace_back(nullptr)    // also compiles
```

## ref:
[emplace_back vs push_back](https://www.youtube.com/watch?v=jKS9dSHkAZY)
https://en.cppreference.com/w/cpp/container/vector/emplace_back
https://learning.oreilly.com/library/view/effective-modern-c/9781491908419/ch08.html#consider_emplacement_instead_of_insertio
