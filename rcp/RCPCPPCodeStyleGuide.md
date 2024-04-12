# RCP C++ Code style guide

- [Tools](#tools)
  - [C++17](#c17)
  - [Boost](#boost)
  - [Google test and Google mock](#google-test-and-google-mock)
  - [Conde](#conde)
- [Coding style](#coding-style)
- [Code Analyzers](#code-analyzers)
  - [Static analyzing with compiler warnings](#static-analyzing-with-compiler-warnings)
  - [clang-tidy](#clang-tidy)
  - [Dynamic analyzing with Valgrind](#dynamic-analyzing-with-valgrind)
- [Good practice](#good-practice)
  - [RAII](#raii)
  - [Namespace](#namespace)
  - [Exception](#exception)
  - [Class vs. struct](#class-vs-struct)
  - [new \& delete operators](#new--delete-operators)
  - [Global variables, environment variables and singletons](#global-variables-environment-variables-and-singletons)
  - [malloc() and free()](#malloc-and-free)
- [Other guide](#other-guide)
  - [c++ core guidelines](#c-core-guidelines)
  - [a good code example](#a-good-code-example)
- [Reference](#reference)

## Tools
### C++17
C++17 was published in 2017 and is widely supported by modern tools, like for example GCC.
We are making modern platform with modern tools, so let's take the full advantage of that.**Use C++17 standard!**
RCP support C++17 at least.

### Boost
Boost is widely known and recognized C++ library. It provides huge amount of ready-made tools for variety of applications. For more information about boost see http://www.boost.org/.

Don't reinvent the wheel. Use boost!

### Google test and Google mock
Google Test and Google Mock are widely known and recognized C++ unit testing and mocking platforms. For more information about Google Test see http://code.google.com/p/googletest/ and for more information about Google Mock see http://code.google.com/p/googlemock/.

**Take full advantage of unit testing. Use Google Test and Google Mock!**

### Conde
The `conde` is tool to simplify software development tasks.
`Conde` provides isolation of build environment from development tools, such as editors, web browsers, file managers.
`Conde` provides build environment to compile and build our components.
`Conde` also integrate some useful tools, such as `clang-format`, `go-format`.
For more information about `Conde` see https://waders-infra.gitlabe2-pages.ext.net.nokia.com/conde/intro.html.

It's used by everyone on everyday in RCP. **Use conde**

## Coding style
When modifying existing code, there is only one rule in terms of coding style: **Always follow the existing coding style!** The same coding style must be used throughout the whole repository.

Some code style details, please see: https://confluence.ext.net.nokia.com/pages/viewpage.action?pageId=546709151#RCPC++StyleGuide-Codingstyle

Personally, I also suggest using `clang-format` to uniform coding style in a repo.

`Conde` has already integrated it. you can use `conde clang-format format` before committing a change. you need to enable format checking in `componentCI`, so coding style violation can be detected. For more information, you can see https://waders-infra.gitlabe2-pages.ext.net.nokia.com/conde/static-analysis.html#using-clang-format.

If you want to uniform coding style of a legacy code, a good practice is committing a change only fixing coding style , such as "Uniform coding style", then commit other changes.

## Code Analyzers
### Static analyzing with compiler warnings
Take full advantage of static analyzing done by your compiler. Use warning options and treat them as errors. The recommended options for GCC are **-Wall -Wextra -Werror** .

This is cheap way to make code warning free and to make sure that the code **stays warning free**.

Personally, I also suggest using `clang-tidy` to check your code.
### [clang-tidy](https://clang.llvm.org/extra/clang-tidy/)
clang-tidy is a clang-based C++ “linter” tool. Its purpose is to provide an extensible framework for diagnosing and fixing typical programming errors, like style violations, interface misuse, or bugs that can be deduced via static analysis. clang-tidy is modular and provides a convenient interface for writing new checks.
`clang-tidy` is a code master at your side.
A short video tutorial, you can see https://learn.microsoft.com/en-us/events/pure-virtual-cpp-2022/clang-tidy-in-visual-studio-code.

### Dynamic analyzing with Valgrind
`Valgrind` is widely known and recognized dynamic analyzer. `Valgrind` provides very good analyzer tools including checks for memory allocations, checks for buffer overflows, checks for using uninitialized variables, etc. For more information about `Valgrind` see http://valgrind.org/.

Take full advantage of `Valgrind` and integrate it into your unit tests so that unit tests are executed with `Valgrind`. This way you can be pretty sure that there are no hidden memory leaks, buffer overflows, use of freed or uninitialized memory etc. (Note: `Valgrind` detects a buffer overflow only if it causes unallocated memory to be accessed, or an uninitialized value to be read.)

Recommended options to be used with Valgrind are --tool=memcheck --error-exitcode=255 --track-fds=yes --leak-check=full --show-reachable=yes

## Good practice
### RAII
Use `RAII`(Resource Acquisition Is Initialization) to manage your resource, and also use `RAII` utility, such as lock_guard, std::unique_ptr, std::shared_ptr.
For more information about `RAII` see https://en.cppreference.com/w/cpp/language/raii.
```cpp
// bad practice
void send(X* x, string_view destination)
{
    auto port = open_port(destination);
    my_mutex.lock();
    // ...
    send(port, x);      // may throw exception
    // ...              // may return beforehand accidentally
    my_mutex.unlock();
    close_port(port);
    delete x;
}
```
```cpp
// good practice
void send(unique_ptr<X> x, string_view destination)     // x owns the X
{
    Port port{destination};         // port owns port handle
    lock_guard<mutex> guard{my_mutex};   // guard owns lock
    // ...
    send(port, x);
    // ...
} // automatically close_port, unlock, and delete the pointer x
```

### Namespace
Namespaces are extremely important when implementing public libraries. **Every symbol in a library must be inside a unique namespace used only by that library or alternatively in an anonymous namespace (which limits the visibility to only that module)**.

The best practice for inventing namespaces is to use the repository name (i.e. subsystem name) as the first level of namespace hierarchy and then logical names as the second, third etc. level hierarchies as needed.

using namespace statements must never be used for other repositories' namespaces. using namespace statements must never be used in a header file. Never use `using namespace std` statement.

Namespaces are good tools to isolate functions and variables, so it can avoid link conflict.

If functions and variables defined as `static` in a cpp file, In this case, you can refactor it with anonymous namespace. For example,
```cpp
// before, designing variables and functions only used in this file by define static.
// such as a.cpp file
{
static int a = 0;
static void foo();
}

// refactor to anonymous namespace
// a.cpp
namespace
{
    int a = 0;
    void foo();
    const int b = 0;
}

namespace SomeComponent
{
void func()
{
    a = 1;
}
}
```

### Exception
Exceptions are the best way to implement error handling in C++, because exceptions allow error handling to happen where it is feasible. **Use exceptions for error handling!**

In public libraries you should have one top level exception inherited from std::exception and all exception classes used in your library should then be inherited from this top level exception (with as many levels of hierarchy as needed).

**Provide unique exception class for every error**, so that just the exception type alone is enough to identify the error. In addition to that you need to provide feasible what() function for your exceptions.
```cpp
// bad practice, throw a system exception directly
class A
{
public:
    A(const std::string& fileName)
    {
        if (open(fileName.c_str(), O_CREAT|O_RDWR|O_CLOEXEC)== -1)
        {
            throw std::runtime_error("create file failed");
        }
    }
}

void foo()
{
    try
    {
        A a("test.txt");
    }
    catch(const std::runtime_error& e)
    {
        std::cout << e.what();    // there is probably other runtime_error exceptions being caught.
    }
}
```
```cpp
// good practice
// define exception inherit from std::runtime_error and
// define specific exception from self-defined exception
class Exception: public std::runtime_error
{
public:
    explicit Exception(const std::string& msg);
    explicit Exception(const char* msg);
    virtual ~Exception() noexcept { }
};

class FDCreateFailed: public Exception
{
public:
    explicit FDCreateFailed(int fd);
};

class A
{
public:
    A(const std::string& fileName)
    {
        int fd = open(fileName.c_str(), O_CREAT|O_RDWR|O_CLOEXEC);
        if ( fd == -1)
        {
            throw FDCreateFailed(fd);
        }
    }
}

{
    try
    {
        A a("test.txt");
    }
    catch(const FDCreateFailed& e)
    {
        std::cout << e.what();    // only catch exception yon can handle
    }
}


```

### Class vs. struct
Distinguish between classes and data structures. Use class keyword for declaring classes and struct keyword for declaring data structures.

Data structures exposes variables but can't be asked to do anything. In other words all member variables in a data structure are public and it doesn't have any functions (except constructors and destructor).

Class contains logic and thus can be told to do something. Class has bunch of public member functions but **no public member variables**.

If you have a class with lot of getter and setter functions and very little anything else, it should probably be a data structure, not a class. Provide as few getter and setter functions for a class as possible.

### new & delete operators
With modern C++ there are very few places where new operator is needed and even fewer places where delete operator is needed. In ideal case all objects are automatically allocated from stack. When that is not possible and you need to allocate object with new, then store the result into a smart pointer (std::unique_ptr, std::shared_ptr, etc.). With std::shared_ptr you can use std::make_shared instead of new operator. If you need to use delete, then you haven't used smart pointers correctly.

### Global variables, environment variables and singletons
Using global variables makes code automatically non-reentrant and it makes the code thread unsafe (unless appropriate locking is used). Global variables also **make unit testing very difficult**, as everything depends on those global variables.

A global variable is really hard to maintain and debug, as you don't who and where will change it.

**Avoid using global variables!** Especially in a shared library global variables must not be used.

Environment variables are global variables, but on a grander scale. Environment variables are also external input for the process, which is a potential security risk.

Singletons are nothing more than global variables in disguise.

### malloc() and free()
Avoid using malloc(), calloc(), realloc(), strdup() and free(). They do not belong to C++ code. The only place where using those functions is acceptable is when you need compatibility with C library or framework (for example if a C library allocates memory but your code has to free that memory).

Memory allocated with new operator must not be freed with free(), but with delete operator. Likewise memory allocated with malloc() must not be freed with delete operator, but with free().

## Other guide
### [c++ core guidelines](https://github.com/isocpp/CppCoreGuidelines)
The C++ Core Guidelines are a collaborative effort led by Bjarne Stroustrup, much like the C++ language itself. They are the result of many person-years of discussion and design across a number of organizations.
The `C++ core guidelines` provides many good practices for reference.
### a good code example
[genapi](https://gitlabe1.ext.net.nokia.com/genapi/genapi/)
[genapi documentation](https://genapi.gitlabe1-pages.ext.net.nokia.com/)

## Reference
https://confluence.ext.net.nokia.com/pages/viewpage.action?pageId=546709151#RCPC++StyleGuide-Staticanalyzingwithcompilerwarnings