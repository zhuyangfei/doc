# description

```
top - 01:03:31 up 50 days, 19:57,  3 users,  load average: 4.81, 5.42, 6.10
Tasks: 2796 total,   5 running, 2791 sleeping,   0 stopped,   0 zombie
%Cpu(s):  8.4 us,  1.0 sy,  0.0 ni, 90.2 id,  0.0 wa,  0.1 hi,  0.3 si,  0.0 st
MiB Mem : 257571.0 total, 135721.8 free,  76972.5 used,  44876.8 buff/cache
MiB Swap:      0.0 total,      0.0 free,      0.0 used. 177375.9 avail Mem
PID     USER      PR  NI    VIRT    RES    SHR S  %CPU  %MEM     TIME+    COMMAND
954142  root      20   0   64.8g  32640  25988 R 100.0   0.0   2482:31    DispP2
953872  root      20   0   64.8g  85576  79096 R  99.7   0.0   2482:29    DispP1
1068509 9999      20   0   64.8g  82020  75592 R  99.7   0.0 985:19.51    DispP1
```
默认的采样周期是3s
%CPU：表示进程占用的CPU的比例，**在多核情况下，是进程在每个核上，所有CPU占比的总和**
`load average: 4.81, 5.42, 6.10`:

`%Cpu(s):  8.4 us,  1.0 sy,  0.0 ni, 90.2 id,  0.0 wa,  0.1 hi,  0.3 si,  0.0 st`
合计100%
`8.4 us`: user 空间CPU占比8.5%
`1.0 sy`: 内核 空间CPU占比1.0%
`0.1 hi`: 硬件中断占比，0.1%
`0.3 si`: 软件中断占比，0.3%
`90.2 id`: idle
`0.0 ni`: 用户进程中，修改nice的进程占比
`0.0 wa`: 等待输入输出的CPU占比

`PID     USER      PR  NI    VIRT    RES    SHR S  %CPU  %MEM     TIME+    COMMAND`
`VIRT`: 虚拟内存
`RES`: 使用的物理内存（KB）
`SHR`: 共享内存（KB）
`TIME+`: 进程合计使用CPU时间，`+`表示时间精度的10ms

# REF:
https://manpages.org/top