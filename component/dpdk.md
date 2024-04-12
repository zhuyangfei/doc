# DPDK
Data plane development kit 用来实现高性能的网络编程，其包括Library及网络接口驱动

## 为什么高性能
1. APP能够在用户态与NIC进行数据交互，避免了中断的开销及内核态到用户态切换的开销
2. 使用huge page，较少缺页中断
3. 使用优化的内存管理技术

## 使用场景
1. 虚拟化
2. 电信业务

## 参考：
https://en.wikipedia.org/wiki/Data_Plane_Development_Kit
[DPDK Introduction](https://www.youtube.com/watch?v=eqRUBXhpkFc)
