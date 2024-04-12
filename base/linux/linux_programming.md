# linux programming

## kill
kill 通过给目标进程发送特定的`信号(signal)`来结束一个进程。
不指定信号时，默认发送`TERM`信号，如，kill pidxxx, 默认的响应是结束（terminate）进程。
因为进程可能有处理`TERM`信号的handler，如资源清理，因此，相比`KILL`信号，优先使用`TERM`信号。

[kill example](https://www.geeksforgeeks.org/kill-command-in-linux-with-examples/)
[man kill](https://man7.org/linux/man-pages/man1/kill.1.html)

## 环境变量(environment variable)
`environment variable`是`shell`的一部分，包含变量。
`environment variable`可以在`system-wide`范围，被用户和进程访问。
### [print env usage](https://linuxconfig.org/how-to-set-and-list-environment-variables-on-linux)
    printenv                    // print all env
    printenv PATH               // print PATH
    printenv HOME SHELL         // print HOME and SHELL
## set env usage
1. scenario1: 临时环境变量, 对当前的session启作用
   $ export MY_SITE="linuxconfig.org"
   // 添加环境变量`MY_SITE`，其值为"linuxconfig.org"
2. scenario2: 长久有效的环境变量
   - ~/.bashrc, user-wide
   - /etc/profile, all users-wide
   - /etc/environment, system-wide
[Environment Variables in Linux/Unix](https://www.geeksforgeeks.org/environment-variables-in-linux-unix/)

## dup2 API
系统调用`dup2`, 用来重定向标志input,output, error到其他的文件，
如
```cpp
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    int fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Duplicate the file descriptor to stdout (file descriptor 1)
    if (dup2(fd, 1) == -1) {
        perror("dup2");
        exit(EXIT_FAILURE);
    }

    // Everything written to stdout (e.g., printf) will now go to the "output.txt" file.
    printf("This will be written to output.txt\n");

    close(fd); // Close the original file descriptor

    return 0;
}
```

## exit status code
0：success, ！0：异常退出
超过255后，退出码 = （status % 256）
https://itsfoss.com/linux-exit-codes/
