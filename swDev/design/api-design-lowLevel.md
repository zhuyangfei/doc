# api design

## principle
make your API hard to use wrong.

## good practice
- use bettering naming
- use [[nodiscard]] with reason liberally.
- use `noexcept` to help indicate what kind of error handling is being used
- never return a raw pointer
- 一致的不能忽略地异常处理，同时，通过当前接口，在本次调用时，就能获取到异常信息
- 参数强类型校验，相邻的两个参数类型设成不同类型，避免default的类型转换
- delete 不希望的隐式转换
- 不要传指针，可以替换成引用
- 尽可能地使用`constexpr`
- 用`fuzzer`测试API
- `[[noreturn]]`属性修饰不返回的函数，如abort，std::terminate，exit
  编译器，可以根据`noreturn`来优化代码，如资源回收

## reference
[cppcon c++ API design](https://youtu.be/zL-vn_pGGgY)