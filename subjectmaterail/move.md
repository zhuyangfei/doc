# move semantics

## why need to add std::move in this scenario
```cpp
vector<T> v;
void foo(T&& t)
{
    v.push_back(std::move(t));      // also need to adding move, as you neet to tell the compiler, that t will not be used further.
}
```

## virtual deconstruct disable move semantics
how to verify?
```
```

## Forwarding

### scenario
```cpp
class X
{};

// all kinds of declarations
void foo(const X&);
void foo(X&);
void foo(X&&);

// if user define a non-const x
X x;
foo(x);
foo(X{});
foo(std::move(x));

// if user define a const cx
const X cx;
foo(x);
```
A API designer need to implement all kinds of declarations to support kinds of scenarios. So the code expend fast and huge.

we can use `std::forward` to solve this issue.
```cpp
<typename T>
void foo(T&& x)
{

}
```

### why
1. make a wrapper function be concise and universal, you can define a function with forwarding instead of all kinds of functions.

### universal/forwarding reference


## reference
https://www.youtube.com/watch?v=Bt3zcJZIalk