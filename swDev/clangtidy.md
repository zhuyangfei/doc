# clang-tidy

## purpose
clang-tidy is a clang-based C++ “linter” tool. Its purpose is to provide an extensible framework for diagnosing and fixing typical programming errors, like style violations, interface misuse, or bugs that can be deduced via static analysis. clang-tidy is modular and provides a convenient interface for writing new checks.
a developer can learn cpp best practice while coding.

## specific a configure
create a configure file of `.clang-tidy` in a workspace, such as
```json
Checks: 'clang-analyzer-*,clang-diagnostic-*,cppcoreguidelines-*,'
WarningsAsErrors: true
```

## usage
### option 1: vscode integration
1. AnalysisOnActiveFile
    `ctrl+shift+p` -> `C_Cpp.RunCodeAnalysisOnActiveFile`
2. Enable Analysis for a project/workspace
   `ctrl+shift+p` -> `workspace setting` -> `code analysis` -> `c++`, for details, refer to the tutorial.

### option 2: cmd line
```bash
    clang-tidy xxxFile
```

## tutorial
[clang-tidy tutorial](https://learn.microsoft.com/en-us/events/pure-virtual-cpp-2022/clang-tidy-in-visual-studio-code)
[clang-tidy documentation](https://clang.llvm.org/extra/clang-tidy/)

## todo:
1. conde docker's env with clang-tidy
2. a suitable configure to get rid of false positive.
