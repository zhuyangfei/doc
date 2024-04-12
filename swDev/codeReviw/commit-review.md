# commit review

- [commit log](#commit-log)
- [code review](#code-review)
  - [头文件](#头文件)
    - [接口设计/类声明](#接口设计类声明)
  - [接口实现](#接口实现)

## commit log
- commit log 规范
  详见：[commit log](../git/commitlog.md)
- [small commit](../git/small-commit.md)
- 是否包含UT

## code review
### 头文件
- 检查编译依赖
  检查是否存在非必要的依赖。通过前置声明，尽可能的减少编译依赖，提升编译速度。

#### 接口设计/类声明
- 设计是否符合SOLID原则
- 代码的可测性
  即是否使用`依赖注入`
- 是否符合接口不容易勿用的原则（最小惊奇）
  - 封装性，如`namespace`，`private`，`public`
  - 包含必要的修饰与限定
    如`noreturn`，`noexcept`，`const`，`nodiscard`等
  - 命名
  - 利用C++类型检查
    如`void*`，`enum`
详见:[api design low level](../design/api-design-lowLevel.md)

### 接口实现
- clean code
  是否符合clean code的要求，如`命名`，`重复代码`，代码可读性
- 性能
  如数据结构与算法，循环查询，循环打印，线程的创建等
- 安全
  如资源管理，buffer的操作，数据竞争
- API的正确使用
  关注robustness的处理
- 业务逻辑
