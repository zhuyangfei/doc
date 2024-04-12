# clang-format

## description
code style format tools for c/c++/Json/Java and so on.

## usage
1. setup a format file `.clang-format`, such as
```bash
clang-format -style=llvm -dump-config > .clang-format
```
2. `clang-format` with file(.clang-format), such as
```
clang-format -i --style=file --Wno-error=unknown $FILES
# FILES: file list to process
```

## practice
1. setup code style rules, then to find the right `style options`
2. create or adapt the `.clang-format` file according to style rules
3. format the code style with `.clang-format`

## enable in CI
params.properties
```
runClangFormat=true
```
example:
https://gitlabe2.ext.net.nokia.com/rcp/tools/component-ci-configs/syscom/-/blob/clangformat/params.properties

## usage in conde
1. create a file `.clang-format`
2. conde with format command
```bash
conde clang-format format
```

## script
```bash
function runclangformat() {
    if ! run "command -v clang-format" > /dev/null; then
        build_layer clang-format
    fi
    local FILES=$(find . -iname "*.cpp" -o -iname "*.c" -o -iname "*.hpp" -o -iname "*.h" -o -iname "*.proto")
    if [ -f .clang-format-ignore ]; then
        FILES=$(echo "$FILES" | grep -vFf .clang-format-ignore)
    fi
    FILES=$(echo "$FILES" | xargs)
    local FORMATARGS=
    if [ -f .clang-format ]; then
        FORMATARGS="--style=file"
    fi
    case $1 in
        format)
            run "clang-format -i $FORMATARGS --Wno-error=unknown $FILES"
            ;;
        check)
            run "clang-format $FORMATARGS --dry-run --Werror --Wno-error=unknown $FILES" || {
                echo "FAILED clang-format found errors!"
                exit 1
            }
            ;;
        *)
            echo "ERROR: invalid or missing clang-format command. Use 'conde clang-format format' or 'conde clang-format check'"
            ;;
    esac
}
```

## REF
[introduction](https://clang.llvm.org/docs/ClangFormat.html)
[style options](https://clang.llvm.org/docs/ClangFormatStyleOptions.html)
