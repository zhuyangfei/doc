# TDD
test driven development

## 法则
1. 写product code之前，先写test code。
   **一个test case就代表一个待实现的需求**, 这样保证了产品实现，总是跟着需求在走。
2. 每次只写一个简单的test case
   比如，在当前test case基础上变化一点的test case。简单的测试场景对应的test case。通过这些简单测试场景的累积与迭代，来完成开发，同时，避免设计并实现复杂test case难题。
3. 只写**足够当前test code通过**的产品代码。即不写当前测试覆盖不到的产品代码。
   这样才能保证，写的代码都能够被测试覆盖到。

### why
1. 测试，是code能重构的前提
   1. **测试给你重构的信心与勇气**，软件最重要的就是**可改变**
   2. 测试，避免软件的败坏，随着时间的推移
2. unit test是low lever design document
3. testable，代表的是，decouple

## 如何建立可靠的测试？
始终遵循上面的法则，才能建立测试的可靠性

### legacy code 怎么来遵循上面的原则，来达到可靠的测试？

## 如何实践
loop Red-Green->Blue until finish.
1. Red phase：写个failed test case,
   聚焦在设计test case。
    1. initial test case, such as `can create object`
    2. then，测试最简单的产品一些场景, 然后，到复杂场景
2. Green phase：修改代码，让test case pass
   聚焦在，如何让test case pass.
3. Blue phase：refactor both **test code** and **product code** to make code clean.
   1. 只在Green的情况下，开始重构。符合之前说的，测试是code能够重构的前提.
   2. 重构并不一定能在这次都完成。随着开发继续，可能需要进一步的重构。
4. commit a change of test case

### 如何设计test cases
设计test case，从简单到复杂，从特殊到一般，从主要（如正常分支）到可选的（如异常分支）。通过test case的累积与迭代，来完成整个开发，同时，也保证了非常高测试覆盖率与产品质量。

## 创造力的事情
都是需要迭代和rework的，而不是一次成型的

## practice
1. dummy test case 之后，开始写第一个业务相关的test case，coding的难度会上升很快，如何减少难度，继续test case的迭代？
   先重构test case，引入必要的依赖，然后，开始写业务test case
2. 如何写test case？
   1. 思考一种测试场景/输入，每一种场景，就是一个test case
   2. 写代码时，只专注在让当前test case 通过，比如只写正常流程，不写异常分支。当然，如果同时写了异常分支，当你写下一个异常test case时，可能发现不用修改源代码，test case 直接通过了。这样，破坏了TDD的开发流程，可能会让你很不自在。

## reference
https://nokialearn.csod.com/lms/scorm/clientLMS/Scorm12IframeContainer.aspx?aicc_sid=AICCc927QDfM9mPhTock1swmDegJ7u3LZS_CpeNjIm2G5yk&aicc_url=https://nokialearn.csod.com/LMS/scorm/aicc.aspx
https://nokialearn.csod.com/LMS/Video/LaunchVideo.aspx?loid=43a5fb45-f100-4625-98a0-5efb7215bb74
https://nokialearn.csod.com/ui/lms-learning-details/app/video/ca23eb7d-19de-4cb7-9d21-42e74dc09c0a
