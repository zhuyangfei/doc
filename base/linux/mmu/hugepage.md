# huge page
huge page 即将进程的虚拟内存与物理内存映射的分页（page）变大。这样可以减少MMU管理内存的层级数量，因此，程序能够更快的找到物理内存，减少缺页中断的发生，使得程序性能得到提升。

https://cloud.tencent.com/developer/article/1816836
https://blog.netdata.cloud/understanding-huge-pages/#:~:text=Huge%20pages%20are%20a%20memory,with%20large%20amounts%20of%20RAM.
