# UT issues

- [不知道check点是什么？(有三段式的形，但没有神)](#不知道check点是什么有三段式的形但没有神)
  - [note](#note)
- [应该测试行为而不是function](#应该测试行为而不是function)
  - [什么是测试function，什么是测试行为](#什么是测试function什么是测试行为)
- [测试内部实现](#测试内部实现)
  - [note](#note-1)
- [not UT for bug fix](#not-ut-for-bug-fix)

## 不知道check点是什么？(有三段式的形，但没有神)
   TEST(UT_AaFileUpdateEncryptedFileKeys, AaFileUpdateEncryptedFileKeys_CreateLocalEncryptionKeyReturnFalse_Fail)
   如果没有返回值，你可以检查系统状态的变化（如数据结构）, 或者检查某个交互没有被执行到的，如AaFileUpdateEncryptedFileKeys fail，那么UpdateEncryptedFileKeys动作肯定是不会被执行的。
### note
1. 上面的状态的变化，是指文件系统中状态的变化。
详见：https://gerrite1.ext.net.nokia.com/c/scm_rcp/SS_RCPCCSMCU/+/1418972

## 应该测试行为而不是function
```cpp
   TEST(UT_AaFileUpdateEncryptedFileKeys, AaFileUpdateEncryptedFileKeys_CreateMemoryEphemeralKeysReturnFalse_Fail)
```
### 什么是测试function，什么是测试行为
一个`function`，内部会实现多种逻辑，可能包含多个边界条件，如果测试`function`, 那么测试的case会变得不清晰。比如：
```cpp
// one test case test all logic of the function AaFileUpdateEncryptedFileKeys
TEST(UT_AaFileUpdateEncryptedFileKeys, TestAaFileUpdateEncryptedFileKeys)
```
在`function`扩展的时候，原来的case描述的就不准确了。
`given/when/then`，就是一种很好的测试行为的一种表现方式

## 测试内部实现
```cpp
TEST(UT_AaFileUpdateEncryptedFileKeys, UpdateEncryptedFileKeysImpl_AaFileListDirReturnNumberReturnError_Fail)
```
### note
1. 测试内部实现存在什么问题?（Test through public API）
   1. 导致ut非常脆弱，任何重构(如：函数名称变化，内部实现变化)都会导致ut case失败
   2. 破坏了封装性，把内部的接口或者私有的接口，暴露出去了，可能会导致接口被误用
   所以，应该通过`public API`进行测试
2. 什么是才是`public API`?
   这里的`public API`是比较上层的一个概念，而不是实际代码中的`public`属性。
   为什么要这么来定义？
   为了代码的可维护性与重构。试想这样的场景，从领域（domain）角度，可能只有一个类，但是，在实现时，你可能会拆成多个类。而这几个类，很可能只是一些helper的对象或者接口，同时，不会被复用。如果对这些接口单独进行测试，那么你重构这些内部类时，就会导致test case 失败，导致ut的可维护性变差。
   关于这个点，google给了几点建议，供大家权衡。
   1. 如果某个对象或者函数（helper class）存在，只是给某个或某两个对象使用的，那么这些对象与函数不应该被认为是一个`unit`，应该通过接口类进行测试，而不是直接进行测试。
   2. 如果函数或者对象，被设计成，可以给任何其他类及其他任何人使用时，那么这些对象应该被认为是一个`unit`, 需要直接进行测试。
   3. 如果对象与函数，只能被部分人使用，但是，提供的是通用的功能，如lib，那么这些对象也应该被认为是一个`unit`, 需要直接进行测试。
[test via public apis](https://learning.oreilly.com/library/view/software-engineering-at/9781492082781/ch12.html#test_via_public_apis)

## not UT for bug fix
出现bug时，是一个很好的机会，去**反思为什么test case（包括UT及TA）没有起作用**？
1. 测试的场景遗漏，导致bug没有被测试出来？
2. 有测试场景，但是test case写的不够完善或者不正确，导致bug没有测试出来？
所以，需要增加test case或者修复原来的test case，来覆盖遗漏场景，**验证你修改的代码**。
详见：https://gerrite1.ext.net.nokia.com/c/scm_rcp/SS_RCPCCSMCU/+/1421828/3/src/CCS_Services/AaFile/src/AaFileUpdateEncryptedFileKeys.cpp
