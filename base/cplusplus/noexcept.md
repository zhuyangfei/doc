# noexcept

## how to use
将接口尽可能的标记成noexcept，尤其是`move`and`sweep`接口
- note
  增加或删除noexcept，为非兼容改动。因为APP代码，需要修改异常处理的策略

## why
- 提升软件性能，因为很多STL的优化操作，都是基于`noexcept`进行的, 比如`move`,`sweep`，当接口申明成`noexcept`后，编译器才能进行优化

## 异常中立
接口没有`noexcept`限定，即，可以继续传递异常

## reference
https://github.com/CnTransGroup/EffectiveModernCppChinese/blob/master/src/3.MovingToModernCpp/item14.md
