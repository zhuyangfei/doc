# compile check

## signed check
Using `-Wformat-signedness` together with `-Werror`
[gnu warning](https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html)

## print format check
using keyword **__attribute__(format(printf, x, x))**, for example:
void printWrapper(int logLevel, const char* format, ...) __attribute__((format(printf, 2, 3)))
`2` means the second parameter is the format string.
`3` means the third parameter is the variables arguments

## good practice
`-Wall`, `-Wextra`,`-Werror`, such as error check of "-Wuninitialized（变量未初始化）"
- note
上面的编译选项，要加到每一个编译产物中，比如library，application.
