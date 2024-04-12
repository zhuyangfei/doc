# genapi
genapi is a build-in event loop engine and a set of abstract use cases API.

## purpose
genapi makes APP separate from runtime environment. loosely couple between user cases and implementation(runtime dependency).
APP building depend on the abstract API, i.e. APP compile and link against on this shared library.
APP runtime depend on some kinds of implementation plugins, which implement against on those API.

### for APP developer
Use the same API, so APP code needn't adaption when runtime environment changed.

### for plugin developer
only force on his own runtime environment, so easy for maintainability.

## when to use
easy integrate to a synchronous select/poll based APP.

## how to use

### how to integrate into poll or select
1. add genapi fd to poll
2. call genapi->handleEvent when event happen.
please see, src/adetservice/Mapping.cpp
Q: all genapi events can be monitored by genapi fd? or how many events can be monitored.

## features or functionality
1. support event call back
2. build-in event loop engine

## implementation
using DIP to make user cases separate from implementation.

## Q&A
1. genapi与plugin是怎么联系在一起的？
   1. 通过调用`dlsym`等动态库的接口，从动态库中读取接口。
2. 如何基于genapi，开发specific use case API, 比如genapicontrollable process?
3. genapi 是如何实践，统一使用exception来表示异常的？
4. controllable process 比较ccs base与genapi的cons and pros, 如果在这个基础上扩展echo server的功能，代码又会是怎样的？
5. how to document or how to use doxygen

## reference
https://genapi.gitlabe1-pages.ext.net.nokia.com/

Q: doxgen上看到的类关系都很简单，
A：因为都是接口类，所有看不到实现的细节
