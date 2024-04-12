# UT practice

- [测试对象](#测试对象)
- [为什么进行UT](#为什么进行ut)
- [UT的关键点](#ut的关键点)
  - [快速，稳定/可靠](#快速稳定可靠)
  - [可维护性](#可维护性)
  - [可测试性](#可测试性)
  - [自动化](#自动化)
- [UT限制](#ut限制)
- [code coverage](#code-coverage)

## 测试对象
Unit通常表示一个`function`或者一个`class`

## 为什么进行UT
1. 能够快速验证代码，从而实现快速的迭代开发
2. 固化已经完成的开发，即当前的开发，都已经完成测试。基于此，进行后续的开发。
3. 使用TDD开发迭代的起点
4. 相比集成测试，UT更加快速，稳定。是测试金字塔模型的最底层。

## UT的关键点
1. 快速，稳定/可靠
2. 可维护性
3. 可测试性
4. 自动化，即集成到CI中

### 快速，稳定/可靠
快速，通常UT cases都能在毫秒执行完成。
稳定及可靠，UT cases需要是环境无关的，比如网络。
因此，影响UT速度，稳定性的对象，需要被mock

### 可维护性
可维护性，体现在：重构时，UT cases应该是不会失败的。修改行为时，UT cases应该会失败，同时，可以快速的修改UT cases。
关于提高可维护性的一些`good practice`:
1. 基于public API
2. 优先选择测试`state`，而不是`interact`
3. 优先选择真实对象，而不是`mock`
4. 测试行为，而不是function
5. good naming
6. 一个test case，测试一个场景
7. test case之间没有依赖
8. 避免复杂逻辑
9. review UT
详见：[ut maintainability](./utMaintainability.md)

### 可测试性
详见：[testability](./testability.md)

### 自动化
UT 需要集成到CI中

## UT限制
1. 只能是单线程的场景
2. 多数情况，都测试单个对象

## code coverage
结论：不要设定`code coverage`的目标。因为
1. code coverage是结果，而不是目标。使用好的开发技巧，自然有比较高的代码覆盖率。
2. 高的code coverage，既不代表高的软件质量，也不代表好的测试。
   软件质量可以使用bug来衡量。
   好的测试，需要遵循UT good practice，在code review时，进行评估。
3. 最求高的code coverage，会导致开发效率下降很大，代码维护的工作量大增
详见: [code coverage targets are harmful](./unit-test-coverage-targets-are-harmful.pdf)
