# shared memory
一种IPC的技术，可以让多个process，像在操作本进程的内存一样，操作共享内存

## how to implement
用户通过系统调用mmap，由OS 将memory-mapped file 映射到进程的virtual space。

### memory-mapped files

## why
高性能的IPC技术，因为data交换，不需要copy到kernel里面

## how to use
