# split large commit

## git usage
[split commit](https://confluence.ext.net.nokia.com/display/RCP/Code+Review+with+Small+Change)

# UT review
## tools for review
### 通过CQG检查，修改的代码(逻辑)是否被覆盖
通过componentCI访问：
componentCI->Test Coverage Report.

### CQG(code quality guardian)
CQG 工具用于收集静态分析的结果，同时，检查这些数据是否达标。当前，支持的功能：
- compile warning
- complexity
- duplexity
- coverity
- ut test and code coverage
#### how to use CQG?
#### 静态分析怎么实现的？
### 利用gtest检查，case之间是否依赖
1. run case with random order
   ./testrunner --gtest_shuffle
note
    1. testrunner is the name of a UT program
    2. [run testcases](https://rcp.gitlabe2-pages.ext.net.nokia.com/rcp-docs/development/reviews/utguidelines.html)
2. check test report
    run-tests.sh.log
[ref](https://rcp.gitlabe2-pages.ext.net.nokia.com/rcp-docs/development/reviews/utguidelines.html)

## keywords
General:

REVIEW-LOOKS-GOOD: LGTM, perfect (If there are other comments for this MR, then it will be ignored)

REVIEW-CHANGE-TOO-BIG: Too many changes in one MR

REVIEW-LANG-ERROR: Spelling and grammatical errors (language issue)

Commit log:

REVIEW-COMMIT-GOOD: The commit log was good, nothing needs to be updated. (Commit log which was already commented does not count)

REVIEW-COMMIT-MISS-WHY: The commit log had no why at all, or had why issues (fake why, incorrect why…)

REVIEW-COMMIT-NEED-SPLIT: The commit should be split

REVIEW-COMMIT-FORMAT-ERROR: The commit log did not follow No.1~6 rules in https://chris.beams.io/posts/git-commit

Source code:

REVIEW-CODE-BUG: Bugs, code errors, wrong algorithms

REVIEW-FORMAT-ERROR: Formatting and indentation errors

REVIEW-DUPLICATED-CODE: Duplicated code

REVIEW-UT-MISSING: Missing Unit test cases

REVIEW-UT-DESIGN: A problem in UT design

REVIEW-UT-NO-CHECKS: There are no or inadequate checkpoints

REVIEW-DESIGN-ISSUE: The design is not right, for example prevents changes in future

REVIEW-DEAD-CODE: Change contains dead code

REVIEW-VERSION-UPDATE-MISSING: Version needs to be updated, but it has not been done. For example do not update helm chart version or shared library version.

Spec file:

REVIEW-UPDATE-TO-ISSUE: The commit log did not use ‘Update to version’ or should not use ‘Update to version’ for release updating

REVIEW-CHANGELOG-ERROR: The rpmdev-bumpspec was not used for changelog updating

REVIEW-SPEC-ERROR: Spec errors

Documentation:

REVIEW-DOC-MISSING: Missing documentation, for example do not update ReadMe when add new parameter into helm chart.
