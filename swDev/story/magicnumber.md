# magic number

- [背景](#背景)
- [探索问题](#探索问题)
- [难点](#难点)
- [问题分析](#问题分析)
- [问题原因：](#问题原因)
- [如何修改](#如何修改)
- [详细分析过程](#详细分析过程)

## 背景
syscom开发过程中，有一个最大路由条数的宏，AASYSCOM_GW_MAX_NUMB_OF_ROUTES(398)。听大家说，如果添加路由数目比较多时，就有可能出现coredump。因为只是测试环境中会添加很多路由，所以，大家都没有去解这个问题。测试用例中，路由的条数，也限定在这个里面。

## 探索问题
设置路由时，不会出现coredump，但是，获取route时，大概率出现coredump。有时候是cli进程挂，有时候，是其他进程挂。
异常日志显示：shared memory的mutex已经坏了，锁持有者挂了，然后，触发了mutex 的assert判断。
可能的，异常日志如下：
```bash
Crash Scenarios1: fhtrsconfigagent
<15>1 2023-08-31T17:44:04.167608+08:00 ahm-0-l1commproxy syscomd - - -  AaSysComMsgCreateReply(): Reply message created. Id 0x118b, target 0xe5001508, sender 0xe500031e, payload size 6324
fhtrsconfigagent[103]: Fatal glibc error: ../nptl/pthread_mutex_lock.c:450 (__pthread_mutex_cond_lock_full): assertion failed: e != ESRCH || !robust

crash scenarios2: syscomd
<11>1 2023-08-28T17:36:54.924518+08:00 ahm-0-l1commproxy CLOUD - - -  /usr/libexec/syscom/syscom_starter.sh: line 8:    89 Segmentation fault      (core dumped) /usr/libexec/syscom/syscomd

Crash scenarios3: syscomcli
```
初步分析，shared memory被写坏了

## 难点
1. syscom 通信，多个进程，多个线程都会读写数据。
2. shared memory，不会在coredump数据中被收集

## 问题分析
异常出现：与添加路由，获取路由相关，所以，在这两个业务过程加打印。查看消息的申请与释放正常

## 问题原因：
创建`恢复路由的消息`时，u16类型的payload overflow，导致申请的空间不够，所以，路由信息把附近的shared memory数据都踩掉了，最终，导致其他进程在访问这部分内存时，出现了异常。
根本原因：随着产品功能变化，消息的结构体扩展，导致内存变大，所以，u16的类型的payload，放不下之前设定的最大路由条数。

## 如何修改
u16的payload减去消息头，再除以每条路由的消息大小。
```cpp
+            constexpr u32 SYSCOM_GW_MAX_NUMB_OF_ROUTES =
+                (AASYSCOM_MAXPAYLOADSIZE - sizeof(AaSysComGwRouteInfoReplyExt)) / sizeof(AaSysComGwRouteInfoExt);
 if (replyRouteCount > SYSCOM_GW_MAX_NUMB_OF_ROUTES)
                 replyRouteCount = SYSCOM_GW_MAX_NUMB_OF_ROUTES;
            replySize = (TAaSysComMsgSize)(sizeof(AaSysComGwRouteInfoReplyExt) +
                                            sizeof(AaSysComGwRouteInfoExt) * replyRouteCount);
```

## 详细分析过程
```bash
When the number of syscom route exceeds AASYSCOM_GW_MAX_NUMB_OF_ROUTES(398), if a user/APP get route information, such as `syscomcli -r`, `aasyscomgw -r` or through syscom message AASYSCOM_GW_ROUTE_INFO_EXT_REQ_MSG , then syscomd will crash. Actually, a route number exceeds 350 may cause the crash.

The reason has been found, as the sum of (TAaSysComMsgSize)(sizeof(AaSysComGwRouteInfoReplyExt) + sizeof(AaSysComGwRouteInfoExt) * replyRouteCount), has exceeded the max value of type uint16, when the route numbers exceed AASYSCOM_GW_MAX_NUMB_OF_ROUTES, so the `replySize` overflows. Then syscomMsgCreateReply will create a message size less than expected, so the following code will overwrite the message memory. Finally, it causes syscomd or CCSMCUDaemonExe crash, such kind of crashes

Crash Scenarios1: fhtrsconfigagent
<15>1 2023-08-31T17:44:04.167608+08:00 ahm-0-l1commproxy syscomd - - -  AaSysComMsgCreateReply(): Reply message created. Id 0x118b, target 0xe5001508, sender 0xe500031e, payload size 6324
fhtrsconfigagent[103]: Fatal glibc error: ../nptl/pthread_mutex_lock.c:450 (__pthread_mutex_cond_lock_full): assertion failed: e != ESRCH || !robust
crash scenarios2: syscomd
<11>1 2023-08-28T17:36:54.924518+08:00 ahm-0-l1commproxy CLOUD - - -  /usr/libexec/syscom/syscom_starter.sh: line 8:    89 Segmentation fault      (core dumped) /usr/libexec/syscom/syscomd
Crash scenarios3: syscomcli

######异常代码的逻辑
if (replyRouteCount > SYSCOM_GW_MAX_NUMB_OF_ROUTES)
                replyRouteCount = SYSCOM_GW_MAX_NUMB_OF_ROUTES;
size_t routePayload = sizeof(AaSysComGwRouteInfoExt) * replyRouteCount;
# 增加打印
syslog(LOG_INFO, "InfoReplyExt %lu, replyCount %u, payloadSize = %lu", sizeof(AaSysComGwRouteInfoReplyExt), replyRouteCount, routePayload);
## replySize（u16）overflow
TAaSysComMsgSize replySize = (TAaSysComMsgSize)(sizeof(AaSysComGwRouteInfoReplyExt) +
                                           sizeof(AaSysComGwRouteInfoExt) * replyRouteCount);

##############打印日志###############
#payload size长度不对，是被异常修改还是算错了？
<15>1 2023-08-28T17:36:53.682087+08:00 ahm-0-l1commproxy syscomd - - -  AaSysComMsgCreateReply(): Reply message created. Id 0x118b, target 0xe5001508, sender 0xe500031e, payload size 6324
<15>1 2023-08-28T17:36:53.682614+08:00 ahm-0-l1commproxy syscomd - - -  AaSysComMsgCreateReply(): Reply message created. Id 0x118b, target 0xe5000039, sender 0x33322e31, payload size 19660

## 分析异常时，在同时操作shared memory的线程

 Id   Target Id                      Frame
* 1    Thread 0x7f41a3c2c6c0 (LWP 91) RTOS_ipcAlloc (size=size@entry=396) at services/msgrtos/MsgRTOS_ipc.c:173

thread : 91 -> 5b
所以，是同一个线程，先destory message，然后，走到coredump
<12>1 2023-08-28T17:36:53.682783+08:00 ahm-0-l1commproxy syscomd - - -  RTOS_destroyMessageValidate: invalid message prefix value
<12>1 2023-08-28T17:36:53.682790+08:00 ahm-0-l1commproxy syscomd - - -  RTOS_destroyMessage: Thread (currentRtosId=0x3, euId=0x5b, name=AaSysComGw) tries to destroy already destroyed message; msg=0x7f41a8e32d00

#5  0x0000556e465b922f in CNMapGwRouter::routeOutgoingMsg (needRelease=<synthetic pointer>: false, msgHandle=0x7f41a8e32a30, this=0x7f41a3c2bc70)

注册的route信息是正确的
<14>1 2023-08-28T17:29:45.181611+08:00 ahm-0-l1commproxy syscomd - - -  <ROUTE> Route info: ID243, fd:22, LSic/LIP=0xe500ffff/127.0.0.1:4446, RSic/RIP=0x0539ffff/16/1.1.1.237:4446, SCTP/YES/0x1
<14>1 2023-08-28T17:29:45.181616+08:00 ahm-0-l1commproxy syscomd - - -  <ROUTE> 243 ROUTES

syscomcli 打印出来的信息不对，
1. syscomd 发过来数据有问题
2. syscomcli, 处理接收的数据有问题

# ================thread info================================

(gdb) bt
#0  RTOS_ipcAlloc (size=size@entry=396) at services/msgrtos/MsgRTOS_ipc.c:173
#1  0x00007f41a9ee97cf in RTOS_msgBufAlloc (size=344, msgNo=31424) at services/msgrtos/MsgRTOS.c:126
#2  0x0000556e465be1c6 in CNMapGwSCTPControlMsg::create(unsigned int, EAaSysComGwSctpControlAction, unsigned int, int, int, sockaddr_storage const*, char const*, int, int, unsigned int, unsigned int, ERetainMsgHeader) [clone .constprop.0] (lengthData=52, action=EAaSysComGwSctpControlAction_AddDataToQueue, receiverEuid=92, receiverEvfd=10,
    sd=20, remoteAddr=0x7f41a3c2bbd0, data=0x7f41a8e32a30 "", parentSd=20, senderEvfd=-1, retainMsgHeader=ERetainMsgHeader_Yes, subscriber=0, routeId=0)
    at src/msggw/CNMapGwSCTPControlMsg.cpp:36
#3  0x0000556e465b9fab in CNMapGwSCTPCommunication::sendData (retainSysComHeader=true, to=0x7f41a3c2bbd0, bufLen=<optimized out>, buffer=<optimized out>, socketId=20,
    this=0x7f41a3c2bdb8) at src/msggw/CNMapGwSCTPCommunication.cpp:186
#4  CNMapGwSCTPCommunication::sendData (this=0x7f41a3c2bdb8, socketId=20, buffer=<optimized out>, bufLen=<optimized out>, to=0x7f41a3c2bbd0, retainSysComHeader=true)
    at src/msggw/CNMapGwSCTPCommunication.cpp:178
#5  0x0000556e465b922f in CNMapGwRouter::routeOutgoingMsg (needRelease=<synthetic pointer>: false, msgHandle=0x7f41a8e32a30, this=0x7f41a3c2bc70)
    at src/msggw/CNMapGwRouter.cpp:832
#6  CNMapGw::handleMessages (msg=<optimized out>, this=0x7f41a3c2bc70) at src/msggw/CNMapGw.cpp:90
#7  CNMapGw::run (this=<optimized out>) at src/msggw/CNMapGw.cpp:67
#8  CNMapGw::startup (createMsgHandle=<optimized out>, this=0x7f41a3c2bc70) at src/msggw/CNMapGw.cpp:54
#9  aaSysComGw (arg=<optimized out>) at src/msggw/nMapGw.cpp:62
#10 0x00007f41a9ee7b28 in MsgProEuInit (arg=0x556e4674ccf0) at services/msgpro/MsgPro.c:292
#11 0x00007f41a9aaa886 in start_thread () from /lib64/libc.so.6
#12 0x00007f41a9b305e4 in clone () from /lib64/libc.so.6
(gdb) p zone
$1 = (TRtos_ipcZone * const) 0x7f41a4e3a000
(gdb) p *zone
$2 = {size = 0, total_pages = 0, free_pages = 0, page_offset = 0, free_area = {{free_list = {prefix = 0, next = 0, prev = 0, suffix = 0},
      buddy_bitmap_offset = 0, buddy_bitmap_size = 0, alloc_bitmap_offset = 0, alloc_bitmap_size = 0}, {free_list = {prefix = 0, next = 0,
        prev = 0, suffix = 0}, buddy_bitmap_offset = 0, buddy_bitmap_size = 0, alloc_bitmap_offset = 0, alloc_bitmap_size = 0}, {
      free_list = {prefix = 0, next = 0, prev = 0, suffix = 0}, buddy_bitmap_offset = 0, buddy_bitmap_size = 0, alloc_bitmap_offset = 0,
      alloc_bitmap_size = 0}, {free_list = {prefix = 0, next = 0, prev = 0, suffix = 0}, buddy_bitmap_offset = 0, buddy_bitmap_size = 0,
      alloc_bitmap_offset = 0, alloc_bitmap_size = 0}, {free_list = {prefix = 0, next = 0, prev = 0, suffix = 0}, buddy_bitmap_offset = 0,
      buddy_bitmap_size = 0, alloc_bitmap_offset = 0, alloc_bitmap_size = 0}, {free_list = {prefix = 0, next = 0, prev = 0, suffix = 0},
      buddy_bitmap_offset = 0, buddy_bitmap_size = 0, alloc_bitmap_offset = 0, alloc_bitmap_size = 0}, {free_list = {prefix = 0, next = 0,
        prev = 0, suffix = 0}, buddy_bitmap_offset = 0, buddy_bitmap_size = 0, alloc_bitmap_offset = 0, alloc_bitmap_size = 0}, {
      free_list = {prefix = 0, next = 0, prev = 0, suffix = 0}, buddy_bitmap_offset = 0, buddy_bitmap_size = 0, alloc_bitmap_offset = 0,
      alloc_bitmap_size = 0}, {free_list = {prefix = 0, next = 0, prev = 0, suffix = 0}, buddy_bitmap_offset = 0, buddy_bitmap_size = 0,
      alloc_bitmap_offset = 0, alloc_bitmap_size = 0}}}
(gdb) p order
$3 = 1
(gdb) info th
  Id   Target Id                      Frame
* 1    Thread 0x7f41a3c2c6c0 (LWP 91) RTOS_ipcAlloc (size=size@entry=396) at services/msgrtos/MsgRTOS_ipc.c:173
  2    Thread 0x7f41a382a6c0 (LWP 93) 0x00007f41a9b30ae2 in epoll_wait () from /lib64/libc.so.6
  3    Thread 0x7f41a3e2e6c0 (LWP 90) 0x00007f41a9aa7189 in __futex_abstimed_wait_common () from /lib64/libc.so.6
  4    Thread 0x7f41a9448780 (LWP 89) 0x00007f41a9b30ae2 in epoll_wait () from /lib64/libc.so.6
  5    Thread 0x7f41a34276c0 (LWP 95) 0x00007f41a9b30ae2 in epoll_wait () from /lib64/libc.so.6
  6    Thread 0x7f41a36286c0 (LWP 94) 0x00007f41a9aa7189 in __futex_abstimed_wait_common () from /lib64/libc.so.6
  7    Thread 0x7f41a3a2b6c0 (LWP 92) 0x00007f41a9b30ae2 in epoll_wait () from /lib64/libc.so.6
  8    Thread 0x7f41a32266c0 (LWP 96) 0x00007f41a9b30ae2 in epoll_wait () from /lib64/libc.so.6
  9    Thread 0x7f41a30256c0 (LWP 97) 0x00007f41a9b30ae2 in epoll_wait () from /lib64/libc.so.6
(gdb) t 2
[Switching to thread 2 (Thread 0x7f41a382a6c0 (LWP 93))]
#0  0x00007f41a9b30ae2 in epoll_wait () from /lib64/libc.so.6
(gdb) bt
#0  0x00007f41a9b30ae2 in epoll_wait () from /lib64/libc.so.6
#1  0x0000556e465bec8a in CNMapGwSCTPCtrlThreadChild::run (this=0x7f41a3829d40) at src/msggw/CNMapGwSCTPCtrlThreadChild.cpp:176
#2  0x0000556e465b29d6 in AaSysComGwSctpCtrlChild (arg=0x7f4194020c70) at src/msggw/CNMapGwSCTPCtrlThreadParent.cpp:1071
#3  0x00007f41a9ee7b28 in MsgProEuInit (arg=0x7f4194020c90) at services/msgpro/MsgPro.c:292
#4  0x00007f41a9aaa886 in start_thread () from /lib64/libc.so.6
#5  0x00007f41a9b305e4 in clone () from /lib64/libc.so.6
(gdb) t 3
[Switching to thread 3 (Thread 0x7f41a3e2e6c0 (LWP 90))]
#0  0x00007f41a9aa7189 in __futex_abstimed_wait_common () from /lib64/libc.so.6
(gdb) bt
#0  0x00007f41a9aa7189 in __futex_abstimed_wait_common () from /lib64/libc.so.6
#1  0x00007f41a9aa9b09 in pthread_cond_wait@@GLIBC_2.3.2 () from /lib64/libc.so.6
#2  0x00007f41a9ee4924 in RTOS_receiveMessage (rtosId=1, sigSelPtr=sigSelPtr@entry=0x7f41a3e2de00, msgSize=msgSize@entry=0x7f41a3e2ddf4,
    timeout=timeout@entry=0, use_timeout=use_timeout@entry=0) at services/msgrtos/MsgRTOS_internal.c:476
#3  0x00007f41a9ee5cf9 in RTOS_receive (msgSelPtr=msgSelPtr@entry=0x7f41a3e2de00, msgSize=msgSize@entry=0x7f41a3e2ddf4)
    at services/msgrtos/MsgRTOS.c:196
#4  0x00007f41a9eeb2ad in AaSysComMsgReceive (maxTimeToWait=maxTimeToWait@entry=4294967295) at src/api/IfAaSysCom.c:721
#5  0x00007f41a9eeb3ca in AaSysComControl (arg=<optimized out>) at src/api/AaSysComControl.c:82
#6  0x00007f41a9ee7b28 in MsgProEuInit (arg=0x556e46734750) at services/msgpro/MsgPro.c:292
#7  0x00007f41a9aaa886 in start_thread () from /lib64/libc.so.6
#8  0x00007f41a9b305e4 in clone () from /lib64/libc.so.6
(gdb) t 4
[Switching to thread 4 (Thread 0x7f41a9448780 (LWP 89))]
#0  0x00007f41a9b30ae2 in epoll_wait () from /lib64/libc.so.6
(gdb) bt
#0  0x00007f41a9b30ae2 in epoll_wait () from /lib64/libc.so.6
#1  0x00007f41a9ead4ca in genapi::System::epoll_wait(int, epoll_event*, int, int) () from /lib64/libgenapi.so.4
#2  0x00007f41a9ea2baf in genapi::Engine::epollWait(int) () from /lib64/libgenapi.so.4
#3  0x0000556e465afdf2 in main () at src/syscomd/syscomDaemon.cpp:78
(gdb) t 5
[Switching to thread 5 (Thread 0x7f41a34276c0 (LWP 95))]
#0  0x00007f41a9b30ae2 in epoll_wait () from /lib64/libc.so.6
(gdb) bt
#0  0x00007f41a9b30ae2 in epoll_wait () from /lib64/libc.so.6
#1  0x0000556e465bec8a in CNMapGwSCTPCtrlThreadChild::run (this=0x7f41a3426d40) at src/msggw/CNMapGwSCTPCtrlThreadChild.cpp:176
#2  0x0000556e465b29d6 in AaSysComGwSctpCtrlChild (arg=0x7f4194020ef0) at src/msggw/CNMapGwSCTPCtrlThreadParent.cpp:1071
#3  0x00007f41a9ee7b28 in MsgProEuInit (arg=0x7f4194020f10) at services/msgpro/MsgPro.c:292
#4  0x00007f41a9aaa886 in start_thread () from /lib64/libc.so.6
#5  0x00007f41a9b305e4 in clone () from /lib64/libc.so.6
(gdb) t 6
[Switching to thread 6 (Thread 0x7f41a36286c0 (LWP 94))]
#0  0x00007f41a9aa7189 in __futex_abstimed_wait_common () from /lib64/libc.so.6
(gdb) bt
#0  0x00007f41a9aa7189 in __futex_abstimed_wait_common () from /lib64/libc.so.6
#1  0x00007f41a9aa9b09 in pthread_cond_wait@@GLIBC_2.3.2 () from /lib64/libc.so.6
#2  0x00007f41a9ee4924 in RTOS_receiveMessage (rtosId=5, sigSelPtr=sigSelPtr@entry=0x7f41a3627e10, msgSize=msgSize@entry=0x7f41a3627e04,
    timeout=timeout@entry=0, use_timeout=use_timeout@entry=0) at services/msgrtos/MsgRTOS_internal.c:476
#3  0x00007f41a9ee5cf9 in RTOS_receive (msgSelPtr=msgSelPtr@entry=0x7f41a3627e10, msgSize=msgSize@entry=0x7f41a3627e04)
    at services/msgrtos/MsgRTOS.c:196
#4  0x00007f41a9eeb2ad in AaSysComMsgReceive (maxTimeToWait=maxTimeToWait@entry=4294967295) at src/api/IfAaSysCom.c:721
#5  0x00007f41a9eed882 in MsgTraceControllerProcess (arg=<optimized out>) at services/msgtrace/MsgTraceController.c:966
#6  0x00007f41a9ee7b28 in MsgProEuInit (arg=0x556e46737f10) at services/msgpro/MsgPro.c:292
#7  0x00007f41a9aaa886 in start_thread () from /lib64/libc.so.6
#8  0x00007f41a9b305e4 in clone () from /lib64/libc.so.6
(gdb) t 7
[Switching to thread 7 (Thread 0x7f41a3a2b6c0 (LWP 92))]
#0  0x00007f41a9b30ae2 in epoll_wait () from /lib64/libc.so.6
(gdb) bt
#0  0x00007f41a9b30ae2 in epoll_wait () from /lib64/libc.so.6
#1  0x0000556e465b2af6 in CNMapGwSCTPCtrlThreadParent::run (this=this@entry=0x7f41a3a2ad20) at src/msggw/CNMapGwSCTPCtrlThreadParent.cpp:152
#2  0x0000556e465b3076 in AaSysComGwSctpCtrlParent (arg=<optimized out>) at src/msggw/CNMapGwSCTPCommunication.cpp:510
#3  0x00007f41a9ee7b28 in MsgProEuInit (arg=0x7f419c000df0) at services/msgpro/MsgPro.c:292
#4  0x00007f41a9aaa886 in start_thread () from /lib64/libc.so.6
#5  0x00007f41a9b305e4 in clone () from /lib64/libc.so.6
(gdb) t 8
[Switching to thread 8 (Thread 0x7f41a32266c0 (LWP 96))]
#0  0x00007f41a9b30ae2 in epoll_wait () from /lib64/libc.so.6
(gdb) bt
#0  0x00007f41a9b30ae2 in epoll_wait () from /lib64/libc.so.6
#1  0x0000556e465bec8a in CNMapGwSCTPCtrlThreadChild::run (this=0x7f41a3225d40) at src/msggw/CNMapGwSCTPCtrlThreadChild.cpp:176
#2  0x0000556e465b29d6 in AaSysComGwSctpCtrlChild (arg=0x7f4194021140) at src/msggw/CNMapGwSCTPCtrlThreadParent.cpp:1071
#3  0x00007f41a9ee7b28 in MsgProEuInit (arg=0x7f4194021160) at services/msgpro/MsgPro.c:292
#4  0x00007f41a9aaa886 in start_thread () from /lib64/libc.so.6
#5  0x00007f41a9b305e4 in clone () from /lib64/libc.so.6
(gdb) t 9
[Switching to thread 9 (Thread 0x7f41a30256c0 (LWP 97))]
#0  0x00007f41a9b30ae2 in epoll_wait () from /lib64/libc.so.6
(gdb) bt
#0  0x00007f41a9b30ae2 in epoll_wait () from /lib64/libc.so.6
#1  0x0000556e465bec8a in CNMapGwSCTPCtrlThreadChild::run (this=0x7f41a3024d40) at src/msggw/CNMapGwSCTPCtrlThreadChild.cpp:176
#2  0x0000556e465b29d6 in AaSysComGwSctpCtrlChild (arg=0x7f4194021390) at src/msggw/CNMapGwSCTPCtrlThreadParent.cpp:1071
#3  0x00007f41a9ee7b28 in MsgProEuInit (arg=0x7f41940213b0) at services/msgpro/MsgPro.c:292
#4  0x00007f41a9aaa886 in start_thread () from /lib64/libc.so.6
#5  0x00007f41a9b305e4 in clone () from /lib64/libc.so.6
```
