# File IO
对文件的操作的集合。unix把外设，socket，文件，都抽象成`fd`，因此，可以使用统一的接口，进行操作。
文件IO，操作时不使用buffer（用户层），即直接使用系统调用。

## 操作集
- open
- read
  ```cpp
  read(fd, buffer, BUFFERSIZE);
  // 1. 当BUFFERSIZE与块长（由st_blksize确定）一样时，系统时间systime，使用最少
  // 2. 通常，通过read ahead技术, 来提升读文件性能
  ```
- write
  1. 通常数据写到内核的缓存区，然后，由内核控制何时真正的写到磁盘上，比如定时刷新，缓冲满了。因此，在异常发生时，如突然断电，可能会出现数据丢失。
  2. 可以通过`fcntl`或者`fsync`等，由用户控制写道磁盘的时刻。比如数据库系统，要求每次写入，数据都到磁盘。可以设置`sync`的文件描述符属性。同时，这样的设置，会影响系统的性能。
- close
- lseek

## 文件系统结构
通常由3部分构成，1. 文件描述符表，2. 文件表，3. V节点表
- 文件描述符表，由文件描述符标志+指向`文件表`指针构成
- 文件表，由文件属性标志，当前文件偏移及指向V节点表指向构成
- V节点表，由V节点信息，i节点信息构成

## TOCTOU
TOCTOU，由于多个操作不是原子造成，比如
```cpp
if (open(pathname, O_WRONLY) < 0)
{
    if(errno == ENOENT)
    {
        create(pathname, mode);
    }
}
// 两个操作open 与 create之间，不是原子的
```
- 解决办法：
  将多个操作，合并到一个接口上，以保证操作的原子性，比如open(pathname, O_WRONLY|O_CREATE), `O_CREATE`,保证打开与新建文件是原子的，open(pathname, O_WRONLY|O_CREATE|O_APPEND), `O_APPEND`, 标志多个进程，同时写文件时，不会产生冲突。

## 标准I/O
以流为操作对象，每个流对应一个文件IO

### why
通过缓冲，减少系统调用次数以及避免用户设置缓存（缓冲大小匹配块大小），通常情况下，可能提升性能。在缓冲大小匹配块大小时，直接操作IO，时钟时间最少。

### 不足
标准IO性能不够，因为存在数据读写过程中，存在2次拷贝。
