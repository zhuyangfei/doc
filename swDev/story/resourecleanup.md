# coredump when resource clean up

- [背景](#背景)
- [初步分析](#初步分析)
- [难点](#难点)
- [问题排查](#问题排查)
- [如何解决](#如何解决)
- [详细分析过程](#详细分析过程)

## 背景
robust测试，kill syscomd时，发现偶会出现coredump。

## 初步分析
从打印上看，异常出现在当线程退出前，清理syscom资源时，触发了assert，导致coredump发生。从assert上，初步分析时，数据竞争引起的异常。

## 难点
1. 退出的业务逻辑很复杂，多个线程，都在访问该shared memory
2. 其他进程，可能还在使用syscom业务，即访问shared memory

## 问题排查
1. 用gdb 解析coredump，发现shared memory相关的内存数据都是0
   因为打印显示，退出过程，shared memory数据，还是存在的，但是，coredump解析的内容都是0。一开始认为：coredump抓取的同时，shared memory的数据刚好被线程清理。
   后来发现，coredump时，不会抓取通过mmap的内存结构的。
2. 通过日志及gbd线程信息，检查多个线程在退出时，是如何交替执行的
   最终，排查出主线程清理了syscom的资源之后，其他线程还在访问的该资源，导致coredump发生

## 如何解决
开始资源清理前，设定退出标志，其他线程，看到该标志后，不再使用该资源。

## 详细分析过程
```bash
Syscomd coredump原因，已经排查清楚。
多线程操作rtos_core->eus[myRtosId]，导致异常发生。详细分析过程如下：
// coredump timing
// thread main 开始退出
start_rsyslog[11]: <14>1 2022-10-11T15:19:02.946096+08:00 ahm-0-l1commproxy syscomd - - -  AaSysComCleanupEe: notify EU to exit
start_rsyslog[11]: <15>1 2022-10-11T15:19:02.946105+08:00 ahm-0-l1commproxy syscomd - - -  AaSysComMsgQueueCreate: creating message queue for EU 0x2f
start_rsyslog[11]: <15>1 2022-10-11T15:19:02.946114+08:00 ahm-0-l1commproxy syscomd - - -  RTOS_msgQueueCreate: EE: 0x2f-syscomd, EU: 0x2f-syscomd, rtosId 15
start_rsyslog[11]: <14>1 2022-10-11T15:19:02.946128+08:00 ahm-0-l1commproxy syscomd - - -  AaSysComShutdownEUs: Shutdown message sent to cpid=0x31e (euid=0x33, rtosid=0x3)
start_rsyslog[11]: <14>1 2022-10-11T15:19:02.946138+08:00 ahm-0-l1commproxy syscomd - - -  AaSysComShutdownEUs: Shutdown message sent to cpid=0x610 (euid=0x32, rtosid=0x1)
start_rsyslog[11]: <14>1 2022-10-11T15:19:02.946142+08:00 ahm-0-l1commproxy syscomd - - -  AaSysComShutdownEUs: Shutdown message sent to cpid=0x612 (euid=0x32, rtosid=0x1)
// thread main schedule out, thread 0x33 schedule in
// EU 0x33 开始退出线程前，清理消息的动作
start_rsyslog[11]: <15>1 2022-10-11T15:19:02.946167+08:00 ahm-0-l1commproxy syscomd - - -  AaSysComMsgQueueDelete: destroying message queue for EU 0x33
start_rsyslog[11]: <14>1 2022-10-11T15:19:02.946175+08:00 ahm-0-l1commproxy syscomd - - -  RTOS_msgQueueDestroy: EU name: AaSysComGw, euId: 0x33, rtosid: 3
start_rsyslog[11]: <15>1 2022-10-11T15:19:02.946182+08:00 ahm-0-l1commproxy syscomd - - -  RTOS_msgQueueCleanup: rcvq_len 0, owned_len 1
// 因为owned_len > 0, 代码处理时，线程会sleep 1秒
// thread 0x33 schedule out, thread 0x32 schedule in
start_rsyslog[11]: <15>1 2022-10-11T15:19:02.946206+08:00 ahm-0-l1commproxy syscomd - - -  AaSysComMsgQueueDelete: destroying message queue for EU 0x32
start_rsyslog[11]: <14>1 2022-10-11T15:19:02.946216+08:00 ahm-0-l1commproxy syscomd - - -  RTOS_msgQueueDestroy: EU name: AaSysComControl, euId: 0x32, rtosid: 1
start_rsyslog[11]: <15>1 2022-10-11T15:19:02.946224+08:00 ahm-0-l1commproxy syscomd - - -  RTOS_msgQueueCleanup: rcvq_len 1, owned_len 0
start_rsyslog[11]: <15>1 2022-10-11T15:19:02.946233+08:00 ahm-0-l1commproxy syscomd - - -  RTOS_msgQueueDestroy: EU queue with euId: 0x32, rtosid: 1 was destroyed
// thread main schedule in
start_rsyslog[11]: <14>1 2022-10-11T15:19:02.946242+08:00 ahm-0-l1commproxy syscomd - - -  AaSysComShutdownEUs: Shutdown message sent to cpid=0x1501 (euid=0x34, rtosid=0x4)
start_rsyslog[11]: <14>1 2022-10-11T15:19:02.946247+08:00 ahm-0-l1commproxy syscomd - - -  AaSysComShutdownEUs: Shutdown message sent to cpid=0x1502 (euid=0x35, rtosid=0x5)
start_rsyslog[11]: <14>1 2022-10-11T15:19:02.946251+08:00 ahm-0-l1commproxy syscomd - - -  AaSysComShutdownEUs: Shutdown message sent to cpid=0x1503 (euid=0x36, rtosid=0x2)
start_rsyslog[11]: <14>1 2022-10-11T15:19:02.946255+08:00 ahm-0-l1commproxy syscomd - - -  AaSysComShutdownEUs: Shutdown message sent to cpid=0x1504 (euid=0x37, rtosid=0x6)
start_rsyslog[11]: <14>1 2022-10-11T15:19:02.946258+08:00 ahm-0-l1commproxy syscomd - - -  AaSysComShutdownEUs: Shutdown message sent to cpid=0x1505 (euid=0x38, rtosid=0x7)
start_rsyslog[11]: <15>1 2022-10-11T15:19:02.946269+08:00 ahm-0-l1commproxy syscomd - - -  AaSysComMsgQueueDelete: destroying message queue for EU 0x2f
start_rsyslog[11]: <14>1 2022-10-11T15:19:02.946273+08:00 ahm-0-l1commproxy syscomd - - -  RTOS_msgQueueDestroy: EU name: syscomd, euId: 0x2f, rtosid: 15
start_rsyslog[11]: <15>1 2022-10-11T15:19:02.946276+08:00 ahm-0-l1commproxy syscomd - - -  RTOS_msgQueueDestroy: EU queue with euId: 0x2f, rtosid: 15 was destroyed
start_rsyslog[11]: <15>1 2022-10-11T15:19:02.946834+08:00 ahm-0-l1commproxy syscomd - - -  AaSysComCleanupCpids: Starting Cpid garbage collector
start_rsyslog[11]: <15>1 2022-10-11T15:19:02.946843+08:00 ahm-0-l1commproxy syscomd - - -  AaSysComUserDeregister: EU 0x2f  is deregistering CPID 0x31e for another EU 0x33 AaSysComGw
start_rsyslog[11]: <15>1 2022-10-11T15:19:02.946846+08:00 ahm-0-l1commproxy syscomd - - -  FreeUserCpid: Deregistered user Cpid 0x31e, euId 0x33
start_rsyslog[11]: <12>1 2022-10-11T15:19:02.946852+08:00 ahm-0-l1commproxy syscomd - - -  RTOS_euQueryEeId: Thread (rtosId=0x0, euId=0x32) doesn't exists.
start_rsyslog[11]: <12>1 2022-10-11T15:19:02.946855+08:00 ahm-0-l1commproxy syscomd - - -  RTOS_euQueryEeId: Thread (rtosId=0x0, euId=0x32) doesn't exists.
start_rsyslog[11]: <15>1 2022-10-11T15:19:02.946869+08:00 ahm-0-l1commproxy syscomd - - -  AaSysComUserDeregister: EU 0x2f  is deregistering CPID 0x1501 for another EU 0x34 syscom_sctp
start_rsyslog[11]: <15>1 2022-10-11T15:19:02.946872+08:00 ahm-0-l1commproxy syscomd - - -  FreeUserCpid: Deregistered user Cpid 0x1501, euId 0x34
start_rsyslog[11]: <15>1 2022-10-11T15:19:02.946875+08:00 ahm-0-l1commproxy syscomd - - -  AaSysComUserDeregister: EU 0x2f  is deregistering CPID 0x1502 for another EU 0x35 syscom_sctp_0
start_rsyslog[11]: <15>1 2022-10-11T15:19:02.946878+08:00 ahm-0-l1commproxy syscomd - - -  FreeUserCpid: Deregistered user Cpid 0x1502, euId 0x35
start_rsyslog[11]: <15>1 2022-10-11T15:19:02.946881+08:00 ahm-0-l1commproxy syscomd - - -  AaSysComUserDeregister: EU 0x2f  is deregistering CPID 0x1503 for another EU 0x36 syscom_sctp_1
start_rsyslog[11]: <15>1 2022-10-11T15:19:02.946884+08:00 ahm-0-l1commproxy syscomd - - -  FreeUserCpid: Deregistered user Cpid 0x1503, euId 0x36
start_rsyslog[11]: <15>1 2022-10-11T15:19:02.946887+08:00 ahm-0-l1commproxy syscomd - - -  AaSysComUserDeregister: EU 0x2f  is deregistering CPID 0x1504 for another EU 0x37 syscom_sctp_2
start_rsyslog[11]: <15>1 2022-10-11T15:19:02.946889+08:00 ahm-0-l1commproxy syscomd - - -  FreeUserCpid: Deregistered user Cpid 0x1504, euId 0x37
start_rsyslog[11]: <15>1 2022-10-11T15:19:02.946892+08:00 ahm-0-l1commproxy syscomd - - -  AaSysComUserDeregister: EU 0x2f  is deregistering CPID 0x1505 for another EU 0x38 syscom_sctp_3
start_rsyslog[11]: <15>1 2022-10-11T15:19:02.946895+08:00 ahm-0-l1commproxy syscomd - - -  FreeUserCpid: Deregistered user Cpid 0x1505, euId 0x38
start_rsyslog[11]: <15>1 2022-10-11T15:19:02.946909+08:00 ahm-0-l1commproxy syscomd - - -  AaSysComCleanupCpids: Cpid garbage collector - done
start_rsyslog[11]: <14>1 2022-10-11T15:19:02.946924+08:00 ahm-0-l1commproxy syscomd - - -  AaSysComCleanupEe: Detaching shared memory
start_rsyslog[11]: <14>1 2022-10-11T15:19:02.946937+08:00 ahm-0-l1commproxy syscomd - - -  RTOS_ExitEe: in
start_rsyslog[11]: <15>1 2022-10-11T15:19:02.946942+08:00 ahm-0-l1commproxy syscomd - - -  RTOS_cleanupEe: marking eu for cleanup (rtosId=0x2, euId=0x36, name=syscom_sctp_1)
start_rsyslog[11]: <15>1 2022-10-11T15:19:02.946945+08:00 ahm-0-l1commproxy syscomd - - -  RTOS_cleanupEe: marking eu for cleanup (rtosId=0x3, euId=0x33, name=AaSysComGw)
start_rsyslog[11]: <15>1 2022-10-11T15:19:02.946948+08:00 ahm-0-l1commproxy syscomd - - -  RTOS_cleanupEe: marking eu for cleanup (rtosId=0x4, euId=0x34, name=syscom_sctp)
start_rsyslog[11]: <15>1 2022-10-11T15:19:02.946950+08:00 ahm-0-l1commproxy syscomd - - -  RTOS_cleanupEe: marking eu for cleanup (rtosId=0x5, euId=0x35, name=syscom_sctp_0)
start_rsyslog[11]: <15>1 2022-10-11T15:19:02.946953+08:00 ahm-0-l1commproxy syscomd - - -  RTOS_cleanupEe: marking eu for cleanup (rtosId=0x6, euId=0x37, name=syscom_sctp_2)
start_rsyslog[11]: <15>1 2022-10-11T15:19:02.946955+08:00 ahm-0-l1commproxy syscomd - - -  RTOS_cleanupEe: marking eu for cleanup (rtosId=0x7, euId=0x38, name=syscom_sctp_3)
start_rsyslog[11]: <14>1 2022-10-11T15:19:02.946972+08:00 ahm-0-l1commproxy syscomd - - -  RTOS_cleanupZombieEus: EE: 0x2f, cleanup own zombies only
start_rsyslog[11]: <14>1 2022-10-11T15:19:02.946976+08:00 ahm-0-l1commproxy syscomd - - -  RTOS_cleanupZombieEus: EE: 0x2f, cleanup zombie eu (rtosId=2, euId=54, name=syscom_sctp_1, rcvq_len=1, owned_len=0)
start_rsyslog[11]: <15>1 2022-10-11T15:19:02.946979+08:00 ahm-0-l1commproxy syscomd - - -  RTOS_msgQueueCleanup: rcvq_len 1, owned_len 0
// main thread 开始清理euId=51(0x33)
start_rsyslog[11]: <14>1 2022-10-11T15:19:02.946983+08:00 ahm-0-l1commproxy syscomd - - -  RTOS_cleanupZombieEus: EE: 0x2f, cleanup zombie eu (rtosId=3, euId=51, name=AaSysComGw, rcvq_len=0, owned_len=1)
start_rsyslog[11]: <15>1 2022-10-11T15:19:02.946985+08:00 ahm-0-l1commproxy syscomd - - -  RTOS_msgQueueCleanup: rcvq_len 0, owned_len 1
// 因为owned_len > 0, 代码处理时，线程会sleep 1秒
// thread main schedule out, thread 0x33 in
// memset(&rtos_core->eus[myRtosId], 0, sizeof(rtos_process_s));
start_rsyslog[11]: <15>1 2022-10-11T15:19:02.947247+08:00 ahm-0-l1commproxy syscomd - - -  RTOS_msgQueueDestroy: EU queue with euId: 0x33, rtosid: 3 was destroyed
// thread 0x33 schedule out(exit), thread main in
start_rsyslog[11]: <11>1 2022-10-11T15:19:02.948045+08:00 ahm-0-l1commproxy syscomd - - -  RTOS_ipcFree: Destroying msg=0x7f5350000000 at invalid page index=-385
start_rsyslog[11]: <11>1 2022-10-11T15:19:02.948052+08:00 ahm-0-l1commproxy syscomd - - -  RTOS_ipcFree: ipc pool info: pool STARTADR=0x7f5350000000, size=0, total pages=261759, page offset=98560, page addr=0x7f5350000020
start_rsyslog[11]: <11>1 2022-10-11T15:19:02.948055+08:00 ahm-0-l1commproxy syscomd - - -  RTOS_ipcFree: line 616 Assertion error page_idx < zone->total_pages occurred.
start_rsyslog[11]: <11>1 2022-10-11T15:19:02.948106+08:00 ahm-0-l1commproxy CLOUD - - -  syscomd: services/msgassert/MsgAssertion.c:18: MsgAssert: Assertion `0' failed.
start_rsyslog[11]: <11>1 2022-10-11T15:19:02.948122+08:00 ahm-0-l1commproxy CLOUD - - -  [Stack trace] Stack trace will report to log server
start_rsyslog[11]: <11>1 2022-10-11T15:19:03.144517+08:00 ahm-0-l1commproxy CLOUD - - -  /usr/libexec/syscom/syscom_starter.sh: line 8:    47 Aborted

// thread 0x33 正在执行
void RTOS_msgQueueDestroy(void)
{
    if(GLO_TRUE == MsgAtomicRead32(&eeShutdownFlag))
    {
        syslog(LOG_INFO, "%s: the application is going down. Message queue will be cleaned up by the zombie cleaner", __FUNCTION__);
        return;
    }
    u32 myRtosId = RTOS_getOwnRtosId();
    if(0 != myRtosId && myRtosId < RTOS_MAX_PROCS)
    {
        u32 euId = rtos_core->eus[myRtosId].euId;
        syslog(LOG_INFO, "%s: EU name: %s, euId: 0x%x, rtosid: %u",
                    __FUNCTION__, rtos_core->eus[myRtosId].name, euId, myRtosId);
        RTOS_euWaitWhileBusy(myRtosId);
        RTOS_msgQueueCleanup(myRtosId);
        memset(&rtos_core->eus[myRtosId], 0, sizeof(rtos_process_s));
        RTOS_releaseRtosId(myRtosId);
        RTOS_setOwnRtosId(0);
        RTOS_setRtosIdToPidMap(euId, 0);
        syslog(LOG_DEBUG, "%s: EU queue with euId: 0x%x, rtosid: %u was destroyed",
                     __FUNCTION__, euId, myRtosId);
    }
}

// thread main 正在在执行
void RTOS_msgQueueCleanup(u32 rtosId)
{
    u32 msgOffset = 0;
    int ret = 0;
    u32 waitCounter = 0;
    const u32 maxWaitTime = 10000; // msec

    // Check in case shared memory was already deattached
    if (GLO_NULL == rtos_core)
    {
        syslog(LOG_WARNING, "%s: the shared memory was already deattached", __FUNCTION__);
        return;
    }

    if (0 != rtos_core->eus[rtosId].rcvq_len || 0 != rtos_core->eus[rtosId].owned_len)
    {
        syslog(LOG_DEBUG, "%s: rcvq_len %d, owned_len %d", __FUNCTION__,
                     rtos_core->eus[rtosId].rcvq_len,
                     rtos_core->eus[rtosId].owned_len);

        if (rtos_core->eus[rtosId].owned_len > 0)     // yanzhu,  线程schedule
        {
            usleep(1000);
        }

        /* free all thread's messages */
        RTOS_robustMutexLock(&(rtos_core->eus[rtosId].message_queue_mutex), &(rtos_core->eus[rtosId].lastMessage_queue_mutexOwner));
        /* first the one in the receive queue*/
        msgOffset = rtos_core->eus[rtosId].rcvq_first;    // 线程thread 0x33 把rtos_core->eus[myRtosId]内存清成0, 导致msgOffset = 0;
        while (msgOffset != RTOS_MSG_NULL)
        {
            rtos_message_s *m = GLO_NULL;
            rtos_message_suffix_s *suffix = GLO_NULL;

            AARTOS_MSGPOOL_LOCK();
            m = RTOS_MSG_ADDRESS_GET(ipcMsgPool, msgOffset);     //

            msgOffset = m->next;
            m->prefix.value = 0;
            suffix = AARTOS_GET_MSG_SUFFIX(m);
            suffix->value = 0;
            RTOS_ipcFree((void*) m);      // msgOffset = 0, 导致m地址与ipcMsgPool相等，而触发message assert（与日志打印吻合了。）
            rtos_core->eus[rtosId].rcvq_len--;
            AARTOS_MSGPOOL_UNLOCK();
        }

start_rsyslog[11]: <11>1 2022-10-11T15:19:02.948045+08:00 ahm-0-l1commproxy syscomd - - -  RTOS_ipcFree: Destroying msg=0x7f5350000000 at invalid page index=-385
start_rsyslog[11]: <11>1 2022-10-11T15:19:02.948052+08:00 ahm-0-l1commproxy syscomd - - -  RTOS_ipcFree: ipc pool info: pool STARTADR=0x7f5350000000, size=0, total pages=261759, page offset=98560, page addr=0x7f5350000020

```