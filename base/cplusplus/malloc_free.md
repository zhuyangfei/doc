# malloc

## feature
1. malloc分配地内存分成两块，一块是控制信息，如空间大小，是否使用，另一块，用于给用户存储数据（data），malloc返回指针指向这一块。
2. malloc 与 free的次数要相匹配
3. 操作系统为了减少分配内存，而引起系统调用的开销，malloc的内存，也是从内存池里面获取的。
4. `free`根据控制块的信息，释放内存
5. malloc的最大值，可以设置。当malloc内存大于阈值时，系统调用`mmap`接口，分配内存。即从内存映射段分配内存，而不是从堆里面。

https://jacktang816.github.io/post/mallocandfree/
