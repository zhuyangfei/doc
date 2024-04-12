# syscom performance
- pps
  pps定义为，在一定的payload下，在一秒的时间内，APP发送syscom消息到对端接收消息的数量
- latency
  latency定义为APP(eu/eo)发送消息，到对端收到消息的间隔

## 测试结果
1. 在490Bytes的payload下，pps约为19K
2. latency 约50~70us

## 性能优化
1. 在cpu busy（到达100%）的情况下，减少最小调度粒度（`kernel.sched_min_granularity_ns`），使能HyperThread，可以减少大的latency（RTT）跳跃值（jitter），但是，均值会提高。因为调度更多，CPU不会被一个进程独占，所以，大的Jitter值会减少，同时，更多的调度，增加了CPU的开销，使得latency均值变大。
- note
  1. kernel.sched_min_granularity_ns，即进程被调度后，最少的运行时间
2. 在RR的调度策略下，调整nice值，即进程调度优先级，并不会影响性能。

- Q
1. 为什么给l2rt，增加shared的core，不能改善性能，但是，变成dedicate core，能够显著的提升性能？
2. 为啥开启HT后，使用性能监测工具，测试到的cpu，就从busy到有空闲了？
3. 如果在cpu 不busy的情况下，减少最小调度粒度（`kernel.sched_min_granularity_ns`），使能HyperThread，性能会变好吗？

## 性能测试工具
- top
- sar，可以看每个core上的数据，如:`sar -P 7 1`
- perf，可以查看进程调度，运行时间，等待时间等。如：
  ```bash
  ```
- taskset，可以查看进程运行到哪个core上。
