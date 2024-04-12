# mandatory copy elision for temporaries

- [what](#what)
- [bad practice](#bad-practice)
- [why or what's the benefit？](#why-or-whats-the-benefit)
- [how to use](#how-to-use)
- [why could happens?](#why-could-happens)
- [ref](#ref)
- [NRVO](#nrvo)

## what
mandatory copy elision when a temporary pass to a parameter or use a temporary to initialize a variable. ex.

### example

```cpp
class X
{};
X getX()
{
    return X{}; // return a temporary
}
void f(X x) // x is initialized by passed argument
{

}
void f(X&&);

// useage
X x = getX();   // use return temporary to initialize x;
                // only default constructor be called
f(X{});         // pass temporary to initialize para x
f(getX{});      // use return temporary to initialize param x
```

### note
this is optional before c++17, i.e `copy` and `move` constructor have to exist, even those functions not be called.

## bad practice
```cpp
// scenario 1:
X getX()
{
    X x;
    // do something
    return x;   // still need copy/move constructor
}

// scenario 2: define a variable, then pass it into a function
X x;
f(x);

// scenario 3:
// declare a interface pass by value,
// you need to stick to prefer pass `const &` to pass value
void f(X x);
```

## why or what's the benefit？
1. minimize the copy to improve the performance
2. easy to use,
   1. an object can be passed to a value even there aren't a copy assignment and move assignment.
   2. return a value instead of passing a out-parameters
3. easy to supply a factory method
## how to use

## why could happens?

## ref
[c17 the complete guide](https://github.com/MeouSker77/Cpp17)
[copy elision](https://en.cppreference.com/w/cpp/language/copy_elision)

## NRVO
name return value optimization
