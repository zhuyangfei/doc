# move and forward
std::move and std::forward, 不执行任何功能，只做类型转换。在运行时，不做任何事情。他们并不产生任何可执行的代码

- [std::move](#stdmove)
- [std::forward](#stdforward)
- [rules](#rules)
- [使用`universal reference`接口的upsides ans downsides](#使用universal-reference接口的upsides-ans-downsides)

## std::move
把类型转成一个右值

## std::forward
只有实参是右值初始化时，std::forward才执行转换，转换成右值

https://github.com/CnTransGroup/EffectiveModernCppChinese/blob/master/src/5.RRefMovSemPerfForw/item23.md

## rules
- 右值引用时，使用std::move, 通用引用时，使用std::forward
- 避免重载通用引用的函数
  1. 因为通用引用特化后，通常比你的重载函数，优先被匹配，导致自定义重载不会被执行
  2. 构造函数中，不使用通用引用传参。因为通用引用构造，导致overload的构造异常，如copy 构造不会被调用，而直接调用T型构造。这种情况，需要使用`限定模板`或者`Tag dispatcher`技术

## 使用`universal reference`接口的upsides ans downsides
- upsides
  1. 为`rvalue`及`lvalue`提供统一的接口
  2. 利用`std::forward`最大化的提升性能，减少拷贝
- downsides
  1. 通常不支持重载
  2. 相比提供`rvalue`及`lvalue`接口，编译异常时，输出的告警很多，在推导过程，遇到异常出现比较深，更难排查
