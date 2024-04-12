# small commit
`small commit`即一个commit只包含一个逻辑改动，通常一个commit改动小于200行。

- [Motivations](#motivations)
- [Guidelines](#guidelines)
  - [The first commit in a new repository](#the-first-commit-in-a-new-repository)
  - [Add one class per commit](#add-one-class-per-commit)
  - [Add one test double per commit](#add-one-test-double-per-commit)
  - [Fix one bug per commit](#fix-one-bug-per-commit)
  - [Do one refactoring per commit](#do-one-refactoring-per-commit)
  - [Do not mix format changes with functional changes](#do-not-mix-format-changes-with-functional-changes)
  - [Update version numbers in a separate commit](#update-version-numbers-in-a-separate-commit)
  - [Using `git revert`](#using-git-revert)
  - [Using `git cherry-pick`](#using-git-cherry-pick)

## Motivations
1. 方便写commit log，log内容可以与改动一致
2. 方便快速review，快速提交
3. 方便`git revert`
4. 方便`git cherry-pick`

## Guidelines

### The first commit in a new repository
When creating a new code repository, the first commit should add a compilation framework. The first commit should not contain any real code, only dummy code to prove that the compilation framework works (i.e. dummy code can be compiled and dummy unit test cases can be executed).

Setting up the initial compilation framework may require more than 200 lines of code. That is not a problem, because most of those lines are trivial and can be reviewed quickly.

### Add one class per commit
When implementing new code, try to add one class (or equivalent) per commit. Avoid adding multiple classes in the same commit even if the classes are related to each others. Ideally the commit should add one header file, one implementation file, one unit test file and modify `Makefile.am` or `CMakeLists.txt`

- Always include unit test code into the same commit with the production code

If one class (together with unit tests) is too big to implement in one commit (considerable more than 200 lines), then you can implement the class in several commits. Mark unimplemented functions or branches with TODO-comments. For example:
```cpp
void MyClass:func(int param)
{
    /** @todo IMPLEMENT ME! */
    static_cast<void>(param);
}
```

### Add one test double per commit
When implementing test doubles (e.g. mocks), try to add one test double per commit. Avoid implementing any functional changes or unit test cases in the same commit. The motivation is to make code reviews quick and to reduce noise in actual real code commits.

### Fix one bug per commit
When fixing a bug in existing code, fix one bug in one commit. Do not make any other changes in the same commit. This has the following benefits:

1. Explaining the bug and the fix in the commit message is easy
2. Reviewing the commit is easy
3. Cherry-picking the commit to maintenance branches is easy
4. If the fix turns out to be incorrect, reverting the commit is easy

### Do one refactoring per commit
When refactoring existing code, do one refactoring in one commit. Do not make any other changes in the same commit. This has the same benefits as [Fix one bug per commit](#fix-one-bug-per-commit).

### Do not mix format changes with functional changes
When fixing or changing source code format, always do that in a separate commit. Never do any functional changes in the same commit. If the whole repository is reformatted (for example with clang-format), then the commit size may exceed 200 lines. That is not a problem if there are no functional changes. It is enough that reviewers agree about the new format.

### Update version numbers in a separate commit
If the repository contains library version or component version numbers in `configure.ac`, `Makefile.am` or `CMakeLists.txt`, then update those in a separate commit. Do not do any other changes in the same commit. This is because if code changes needs to be reverted or cherry-picked, version number changes must not be reverted or cherry-picked along with the code changes.

If the repository contains CHANGELOG.md (or equivalent), then that can be changed in the same commit with the version numbers.

### Using `git revert`
Do not change the commit that is generated auto by git, and Add `Why` in the commit body.

### Using `git cherry-pick`
When the above guidelines are followed, using git cherry-pick -x is easy. By cherry-picking one commit from master to maintenance branch you can be sure you cherry-pick only the needed bug fix. Avoid doing any manual changes in the same commit (except for fixing possible conflicts).

Remember to use the -x option to get the automatic (cherry picked from commit xxx) text appended to the commit message. Avoid changing the commit message manually.

Do not push the cherry-pick commit for review before the original commit has been reviewed and merged. This is because:

The SHA-1 hash of the original commit is not certain until the commit has been merged

There is no point to review the same change twice.
