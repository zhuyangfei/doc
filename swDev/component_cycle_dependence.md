# component cycle dependence

## how to solve
- option 1:
  让其中一个componentA，作为另一个componentB的submodule，通过链接源代码的方式，在一个工程上进行编译。
  编译componentA的so时，不设置“--no-undefined”，这样componentA，所依赖componentB的接口，可以在编译时，不去检查未定义的情形。等到编译bin时，才检查所有符号表是否都有被定义。
