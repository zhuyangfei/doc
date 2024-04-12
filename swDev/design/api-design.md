# API design

## 对问题域建模
- 好的抽象
  API是问题如何解决的逻辑抽象。非软件人员，阅读API文档时，也应该能知道API的功能。
- 建立关键对象
  即面向对象的设计方法。能够描述对象的层次关系。

## 封装/隐藏实现
有两种隐藏的分类，物理隐藏与逻辑隐藏
- 物理隐藏
  即只把对外的接口，发布到安装包里面
- 逻辑隐藏
  即通过定义`private`，`public`来限定component or class的使用

通用的技巧：
- 不直接操作类的成员变量
  改用get/set接口
- 不返回可修改对象的指针或者引用
- function区分为private与public
- 隐藏class，即只暴露对外的class
- 最小职责

## 易用
- Discoverable，即用户第一次使用，就知道怎样用
- 不容易错误使用，即被错误调用
  编程技巧，详见modern effective c++及effective c++
  1. 使用`overload`
  2. 考虑`type property`
- 一致性
  一致性的设计，便于API的记忆与使用，如函数命名与参数顺序的一致性
- 平台独立
  API文件，不能存在关于平台的`#ifdef`。关于平台的编译宏，应该放到cpp文件中。

## 松耦合（松耦合高内聚）
耦合是指两个组件之间的交互程度，内聚指单个组件的功能之间的相关性
- 评估耦合性的方法
  1. Size，即component之间的交互数，包括类的个数，函数个数，函数的参数个数
  2. 可见性
  3. 亲密性，即两个components的直接关系，如继承，组合
  4. 灵活性，即修改components的容易程度

实践技巧：
- class前置声明
  通过class前置声明，来减少耦合
- 优先非成员函数
- 使用Manager class 来避免暴露low-level的类
- 定义convenient API
- 通过Callback，Observer, Notification来解耦

## 稳定，文档，经过测试
API经过测试，通过版本进行发布，同时，具有清晰的文档

https://biendltb.github.io/book/api-design-for-cpp-martin-reddy-part-1/
