# process
进程是可执行文件（程序）的运行时的抽象。对应着一块固定大小的虚拟内存，其地址从0~一个固定大小。其内存结构分成用户空间与内核空间，由低到高分辨是，text段，data段，bss段，堆，栈及内核空间构成。

## 进程抽象的目的
为了提高CPU的利用率或者CPU的复用，比如当一个进程在等待IO输入时，其他进程可以使用CPU

## 创建
由父进程通过调用`fork`创建。fork后，子进程与父进程有一样的镜像。因此，子进程与父进程有相同的文件描述符，指向一样的文件列表。当子进程调用`exec`时，若被指定`O_CLOEXEC`, 则文件被关闭。

## 启动
由kernel调用`exec`接口启动程序。这部分的代码通常在CC的link时插入。

## terminate
most process terminate itself by calling `exit`, such as return from `main`, the compiler will insert `exit` after `return`.

After `exit`, all process related resource will be freed. resource like `virtual memory`, file descriptor will be flushed and closed. Expect for it's kernel stack, `PCB` and `TCB`, it will be exist until parent process handle it.

After `exit`, the process status will be set as `TASK_ZOMBIE`, and Signal `SIGCHLD` may be sent to his parent.

a process exit, all thread terminated.

- note
也可以通过`_exit`退出，区别在于调用`exit`会清理一些资源，如执行注册到`atexit`里面的handle，及flush IO

## init 进程
进程ID = 1。
为所有孤儿进程的父进程。

## 子进程
子进程，从父进程中继承了一些资源，如IO对象（file）及一些文件属性等。
- zombie 状态
  当子进程termination后，父进程没有调用`wait`或`waitpid`，对子进程进行处理（释放子进程占用的资源），则这个进程状态为zombie。

## process status
the process status will saved in `PCB`. It may has some types, such as `ready`, `running`, `blocking` and so on. The OS will schedule process according to their status.

## execv 或exec系列接口
通过`execv`, 将当前进程，替换成新的进程。进程ID保持不变。详见：
`exec`，在执行成功时，不会返回，即在成功时，当前的进程，会收到`SIGTRAP`信号。若执行失败，则返回-1。详见：https://linux.die.net/man/3/execv
`execve`对进程属性的影响，如：
1. signal的处理，恢复到默认。
2. mmap的内存，不在保留。
3. 关闭打开的文件夹等。详见：https://man7.org/linux/man-pages/man2/execve.2.html
4. 文件描述符，依然打开，除了设置`close-on-exec`的。
[linux系統編程](https://www.cntofu.com/book/46/linux_system/linuxxi_tong_bian_cheng_zhi_jin_cheng_ff08_wu_ff09.md)

## 进程间关系
- 进程组
  每个进程，都属于一个进程组。可包含至少一个进程。
- 会话
  一个或者多个进程组的集合。进入session，实现任务的前后台控制。

https://cattail.me/tech/2017/03/02/process-relationship.html
## reference
https://man7.org/linux/man-pages/man3/exit.3.html
[Process Termination](https://www.informit.com/articles/article.aspx?p=370047&seqNum=4)
https://en.wikipedia.org/wiki/Process_management_(computing)
https://www.tutorialspoint.com/process-creation-vs-process-termination-in-operating-
https://pages.cs.wisc.edu/~remzi/OSTEP/
