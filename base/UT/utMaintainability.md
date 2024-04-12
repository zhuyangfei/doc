# test maintainability

- [brittle Tests](#brittle-tests)
- [guideline for test maintainability](#guideline-for-test-maintainability)
  - [1. Test via Public APIs](#1-test-via-public-apis)
  - [2. Test State, Not Interactions](#2-test-state-not-interactions)
    - [什么时候使用`Interaction Testing`是合适的](#什么时候使用interaction-testing是合适的)
    - [Best Practices for Interaction Testing](#best-practices-for-interaction-testing)
  - [3. Test Behaviors, Not Methods](#3-test-behaviors-not-methods)
  - [4. 隔离对象与真实对象之间，优先选中真实对象(Prefer Realism Over Isolation)](#4-隔离对象与真实对象之间优先选中真实对象prefer-realism-over-isolation)
    - [怎么判断什么时候使用`真实对象`](#怎么判断什么时候使用真实对象)
    - [Faking](#faking)
    - [stub](#stub)
  - [避免太过细节（具体）](#避免太过细节具体)
    - [example: Overspecified interaction tests](#example-overspecified-interaction-tests)
    - [example: Well-specified interaction tests](#example-well-specified-interaction-tests)
- [Test doubles](#test-doubles)
  - [`Test doubles`对软件开发的影响](#test-doubles对软件开发的影响)
  - [1. faking](#1-faking)
  - [2. stubbing](#2-stubbing)
  - [3. Interaction Testing](#3-interaction-testing)
  - [Test Doubles at Google](#test-doubles-at-google)
  - [seams](#seams)
  - [mocking frameworks](#mocking-frameworks)
- [reference](#reference)


## brittle Tests
**brittle Tests**, a brittle test is one that fails in the face of an unrelated change to production code that does not introduce any real bugs.
什么情况下，修改代码会影响到Test case？
代码的修改，可以归为4种情形：
1. Pure refactorings
   内部实现变化，API接口不变。原有的test case不会被影响。
2. New features
   增加功能。需要增加test case，原有的test case不会被影响。
3. Bug fixes
   修复问题。原先的test case 没有覆盖的情况，需要新增test case进行覆盖。
4. Behavior changes
   功能或者行为发生了变化。原先的test case需要被修改。
上述4种情形，只有`Behavior changes`一种情形，需要修改`原先Test Case`.

## guideline for test maintainability
### 1. Test via Public APIs
When we say “public API” in this context, we’re really talking about the API exposed by that unit to third parties outside of the team that owns the code. This doesn’t always align with the notion of visibility provided by some programming languages; for example, classes in Java might define themselves as “public” to be accessible by other packages in the same unit but are not intended for use by other parties outside of the unit.

关于什么是`Public APIs`及`Unit`范围，更多是艺术的而不是科学定义的，但是，也有一些不错的规则可以被遵循。
1. If a method or class exists only to support one or two other classes (i.e., it is a “helper class”), it probably shouldn’t be considered its own unit, and its functionality should be tested through those classes instead of directly.

2. If a package or class is designed to be accessible by anyone without having to consult with its owners, it almost certainly constitutes a unit that should be tested directly, where its tests access the unit in the same way that the users would.

3. If a package or class can be accessed only by the people who own it, but it is designed to provide a general piece of functionality useful in a range of contexts (i.e., it is a “support library”), it should also be considered a unit and tested directly. This will usually create some redundancy in testing given that the support library’s code will be covered both by its own tests and the tests of its users. However, such redundancy can be valuable: without it, a gap in test coverage could be introduced if one of the library’s users (and its tests) were ever removed.

### 2. Test State, Not Interactions
- Prefer state Testing Over Interaction Testing
[state Testing](http://xunitpatterns.com/State%20Verification.html): 就是执行后，验证系统前后状态的变化。如黑盒测试
- 例子
```
@Test public void sortNumbers() {
  NumberSorter numberSorter = new NumberSorter(quicksort, bubbleSort);
  // Call the system under test.
  List sortedList = numberSorter.sortNumbers(newList(3, 1, 2));
  // Validate that the returned list is sorted. It doesn’t matter which
  // sorting algorithm is used, as long as the right result was returned.
  assertThat(sortedList).isEqualTo(newList(1, 2, 3));
}
```
[Interaction Testing](https://martinfowler.com/articles/mocksArentStubs.html): 通过函数怎么被调用或有没有被调用进行验证，比如mock framework.
- 例子
```
@Test public void sortNumbers_quicksortIsUsed() {
  // Pass in test doubles that were created by a mocking framework.
  NumberSorter numberSorter =
      new NumberSorter(mockQuicksort, mockBubbleSort);

  // Call the system under test.
  numberSorter.sortNumbers(newList(3, 1, 2));

  // Validate that numberSorter.sortNumbers() used quicksort. The test
  // will fail if mockQuicksort.sort() is never called (e.g., if
  // mockBubbleSort is used) or if it’s called with the wrong arguments.
  verify(mockQuicksort).sort(newList(3, 1, 2));
}
```

At Google, we’ve found that emphasizing `state testing is more scalable`; it reduces test brittleness, making it easier to change and maintain code over time.

`The primary issue with interaction testing is that it can’t tell you that the system under test is working properly; it can only validate that certain functions are called as expected`. It requires you to make an assumption about the behavior of the code

Another downside of interaction testing is that `it utilizes implementation details of the system under test—to validate that a function was called`, you are exposing to the test that the system under test calls this function.

because they fail in response to any change to the production code, even if the behavior of the system under test remains unchanged.

There is often no exact answer regarding whether to use a real implementation or a test double, or which test double technique to use. An engineer might need to make some trade-offs when deciding the proper approach for their use case.

#### 什么时候使用`Interaction Testing`是合适的
1. 没办法进行`state Testing`，如不能使用真实对象也没有fake对象。有`Interaction Testing`测试，要比完全没有测试更可靠。
2. 函数调用的顺序不同与次数不一样，会导致异常行为。比如，cache 存数据，只能被调用一次。
`Interaction Testing`不是`state Testing`的替代方案。假如不能进行`state Testing`，你或许可以把验证，放到更上层的测试中进行。

#### Best Practices for Interaction Testing
当被测对象，调用依赖的接口时，会出现两种情况：
1. 状态变化(state-changing)
   调用后，影响了**依赖对象的状态**，如
   sendEmail(), saveRecord(), logAccess().
2. 没有状态变化（Non-state-changing）
   调用后，依赖对象的状态没有变化，被测对象通过返回值，获取了依赖对象的信息。如：
   getUser(), findResults(), readFile().
一般来说，`Interaction Testing`仅用于测试`state-changing`的场景。测试`Non-state-changing`, 通常没有意义，因为被测对象会使用返回值，所以，你能够通过`被测对象`的进行验证。

### 3. Test Behaviors, Not Methods

### 4. 隔离对象与真实对象之间，优先选中真实对象(Prefer Realism Over Isolation)
在测试中，过度使用mock framework，导致**refactor**及**scale**非常困难。
使用真实对象进行测试，让在测试情况下，系统能很好工作，变得更可靠、更可信。如果在测试中过多的利用`Test Double`, 你需要通过集成测试或手动测试，才能验证系统或者`Feature`，达到一样的可信。

#### 怎么判断什么时候使用`真实对象`
1. 执行时间(Execution time)
   通常来说，单个test执行时间小于10ms
2. 确定性(Determinism)
   确定性，即被测系统（对象）不变的情况下，测试结果不会变化。
   非确定性，导致测试变得古怪(flakiness)-系统没有变化，测试也偶尔出现失败，进而导致大家不再信任测试系统。比如pipeline case failed。
   测试时，几乎不会出现`flakiness`，可能还好。经常出现，那么肯定不能再使用了。
   最常见引起`nondeterminism`的原因是，代码不是`封闭的`-依赖外部service来控制输出结果。比如，一个测试，需要访问网站来进行测试。一个可行的办法是，引入一个封闭的HTTP服务，其生命周期被测试所控制。
3. 构造链依赖(Dependency construction)
   被测对象构造时，需要构造其依赖的所有对象。
    ```
    Foo foo = new Foo(new A(new B(new C()), new D()), new E(), ..., new Z());
    ```
    除了使用mock对象来避免构造依赖之外，一个更好的办法时，产品代码中使用`工厂方法`及`自动的依赖注入`
##### note:
1. 优先选用真实对象的测试方式，又称为古典测试([classical testing](https://martinfowler.com/articles/mocksArentStubs.html))。与之对应的是，利用mock进行测试(mockist testing).
google 工程师写代码的风格[strict guidelines when designing the system under test](http://jmock.org/oopsla2004.pdf)，更适合古典测试。
2. 为了不过度使用mock，google会在API上写`DoNotMock`的注释
```
@DoNotMock("Use SimpleQuery.create() instead of mocking.")
public abstract class Query {
  public abstract String getQueryValue();
}
```

#### Faking
当真实对象不合适时，第一考虑是fake对象。
If using a real implementation is not feasible within a test, the best option is often to use a fake in its place.
- 比如一个文件系统的Fake
```
// This fake implements the FileSystem interface. This interface is also
// used by the real implementation.
public class FakeFileSystem implements FileSystem {
  // Stores a map of file name to file contents. The files are stored in
  // memory instead of on disk since tests shouldn’t need to do disk I/O.
  private Map<String, String> files = new HashMap<>();
  @Override
  public void writeFile(String fileName, String contents) {
    // Add the file name and contents to the map.
    files.add(fileName, contents);
  }
  @Override
  public String readFile(String fileName) {
    String contents = files.get(fileName);
    // The real implementation will throw this exception if the
    // file isn’t found, so the fake must throw it too.
    if (contents == null) { throw new FileNotFoundException(fileName); }
    return contents;
  }
}
```
##### 什么时候写Fake
A fake requires more effort and more domain experience to create because it needs to behave similarly to the real implementation. A fake also requires maintenance: whenever the behavior of the real implementation changes, the fake must also be updated to match this behavior. Because of this, **the team that owns the real implementation should write and maintain a fake**.

If a team is considering writing a fake, a trade-off needs to be made on whether the productivity improvements that will result from the use of the fake outweigh the costs of writing and maintaining it. If there are only a handful of users, it might not be worth their time, whereas if there are hundreds of users, it can result in an obvious productivity improvement.

##### fake对象真实度
API 层面上（API的行为），要保持一致。
Primarily, however, a fake should maintain fidelity to the API contracts of the real implementation. For any given input to an API, a fake should return the same output and perform the same state changes of its corresponding real implementation.

##### Fakes Should Be Tested

#### stub

##### 什么时候使用stubbing
当你需要一个函数返回一个特定值，让被测对象进到一个确定的状态。
因为函数的行为是在test中被定义的，所以，stubbing可以模拟多种返回值，同时，可以触发使用`真实对象`很难达到的条件。
确保测试的清晰性，每个stub的返回值应该与测试断言直接相关。
一个test，通常只需要stub很少的函数，如果所有函数都stub，那么就是stub过度使用了。

##### The Dangers of Overusing Stubbing

### 避免太过细节（具体）
avoiding overspecifying which functions and arguments are validated.
#### example: Overspecified interaction tests
```
@Test public void displayGreeting_renderUserName() {
  when(mockUserService.getUserName()).thenReturn("Fake User");
  userGreeter.displayGreeting(); // Call the system under test.

  // The test will fail if any of the arguments to setText() are changed.
  verify(userPrompt).setText("Fake User", "Good morning!", "Version 2.1");

  // The test will fail if setIcon() is not called, even though this
  // behavior is incidental to the test since it is not related to
  // validating the user name.
  verify(userPrompt).setIcon(IMAGE_SUNSHINE);
}
```
#### example: Well-specified interaction tests
```
@Test public void displayGreeting_renderUserName() {
  when(mockUserService.getUserName()).thenReturn("Fake User");
  userGreeter.displayGreeting(); // Call the system under test.
  verify(userPrompter).setText(eq("Fake User"), any(), any());
}
@Test public void displayGreeting_timeIsMorning_useMorningSettings() {
  setTimeOfDay(TIME_MORNING);
  userGreeter.displayGreeting(); // Call the system under test.
  verify(userPrompt).setText(any(), eq("Good morning!"), any());
  verify(userPrompt).setIcon(IMAGE_SUNSHINE);
}
```

## [Test doubles](https://martinfowler.com/articles/mocksArentStubs.html)
`Test doubles`是在测试时，能够替代`真实实现`的对象或者函数。
### `Test doubles`对软件开发的影响
1. Testability
   要使用`Test doubles`，代码设计时，就需要考虑可测性-测试时，替换真实的对象。
2. Applicability（适用性）
   不合理的过度使用`Test doubles`，让测试变得脆弱、低效，减低开发效率。
3. Fidelity
   `Fidelity`用来评估`Test doubles`与`真实实现`之间的相似度。如果`Test doubles`与`真实实现`的行为不一致，那么测试没什么价值。
   Unit tests that use test doubles often need to be supplemented by larger-scope tests that exercise the real implementation

主要的三种使用`Test doubles`的技巧
### 1. faking
基于一样的API，`fake`对象是真实对象的轻量化实现。`faking`就是测试时，使用`fake对象`替换`真实对象`

### 2. stubbing
让函数返回特定的值。`stubbing`可以通过mocking framework来实现。

### 3. [Interaction Testing](http://xunitpatterns.com/Behavior%20Verification.html)
用来验证函数如何被调用，却不用真正的调用它。
如果函数没被调用或者调用的次数不对，或者参数不对，那么测试就会失败。
`Interaction Testing`有时被成为`mocking`.
Interaction testing is a way to validate how a function is called without actually calling the implementation of the function. A test should fail if a function isn’t called the correct way—for example, if the function isn’t called at all, it’s called too many times, or it’s called with the wrong arguments.

### Test Doubles at Google
It wasn’t until several years and countless tests later that we began to realize the cost of such tests: though these tests were easy to write, we suffered greatly given that they required constant effort to maintain while rarely finding bugs.

### seams
让代码变的可测试的一种技巧，如`依赖注入`是一种常见的引入seams的方式.

### mocking frameworks
可以方便的构造mock对象的软件库，如gmock

## reference
[Software Engineering at Google](https://learning.oreilly.com/library/view/software-engineering-at/9781492082781/ch12.html#donapostrophet_put_logic_in_tests)
