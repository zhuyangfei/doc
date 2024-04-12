# syscom
无论在节点内部或者节点之间，应用程序都能够通过统一的syscom API实现消息通信。使用syscom API进行通信，APP不需要考虑底层的实现，只需要知道对方的地址（sicAddress），就能实现消息传递。

syscom基于nokia协议（sicAddress）进行通信，通过在全局范围内唯一sicAddress，实现APP之间的通信通信。

- [SIC address](#sic-address)
  - [CPID：](#cpid)
- [大小端处理](#大小端处理)
- [消息生命周期](#消息生命周期)
- [APP如何使用及syscomAPI内部实现](#app如何使用及syscomapi内部实现)
  - [注册CPID](#注册cpid)
  - [create a message](#create-a-message)
  - [send a message](#send-a-message)
    - [syscomGW](#syscomgw)
  - [receive message](#receive-message)
  - [Destroy message](#destroy-message)
  - [destroy queue](#destroy-queue)
- [Q\&A:](#qa)
- [intra communication](#intra-communication)
- [inter communication](#inter-communication)
  - [路由表的建立](#路由表的建立)
  - [新连接的处理](#新连接的处理)
  - [sctp socket的创建](#sctp-socket的创建)
  - [添加路由](#添加路由)
  - [发送数据](#发送数据)
  - [childSd 的创建](#childsd-的创建)
  - [child stcp socket fd的数据处理](#child-stcp-socket-fd的数据处理)
- [performance](#performance)
- [network service](#network-service)
- [syscom vs zmq](#syscom-vs-zmq)
- [evolution](#evolution)
- [implementation](#implementation)
- [rt](#rt)
  - [core affinity](#core-affinity)


## SIC address
   通信地址，sicad = NOID<<16 | CPID。
   NodeID是统一分配，由配置文件设置，不同Node的NID是唯一的。

### CPID：
    CPID是执行单元（单个线程/单个进程）的ID号，每一执行单元与一个CPID进行对应。
    静态CPID，即提前定义好的，如作为server的线程
    动态CPID，运行时通过API获取，用在如client的线程

## 大小端处理
    sysom消息总是以网络字节序进行传输，APP负责转换字节序。在message header中包含payload的类型，默认是32-bit 字的host大小端。

## 消息生命周期

## APP如何使用及syscomAPI内部实现
### 注册CPID
create a new queue when register EU, i.e. call TAaSysComCpid AaSysComEuUserRegister(const TAaSysComCpid userCpid)
- API：AaSysComEuUserRegister
调用AaSysComEuUserRegister，会在共享内存中创建一个与eu相对应的消息队列，该队列用于存放消息。即：RTOS_msgQueueCreate。
RTOS_msgQueueCreate功能：
// 1. reserve a rtos id
// 2. 把该rtos id，map 到 rtos_core_s->eus[rtos id], 即从rtos_core_s->eus[rtos id] reserve 一个rtos_process_s
// 3. 初始化rtos_process_s相关的数据
RTOS_msgQueueCreate
```cpp
// message queue structure
typedef struct rtos_process_s
{
    u32               euId;
    pid_t             eeId; /* the pointers in this struct are valid only inside the same ee */
    char              name[RTOS_MAX_PROC_NAME_LEN];
    char              eeName[RTOS_MAX_PROC_NAME_LEN];
    pthread_mutex_t   message_queue_mutex;
    u32                  lastMessage_queue_mutexOwner;
    pthread_cond_t    message_queue_cond;
    char              *ipcMemBase; /* if different EE wants to manipulate the queue msg queue
                                      pointers have be dealed as relative offsets to this base */
    u32 rcvq_first;
    u32 rcvq_last;
    u32 owned_first;
    u16 rcvq_len;
    u16 owned_len;
    u16 rcvq_thr;
    u16 owned_thr;

    TBoolean             euSelfCleanup;/*if this flag is true, eu will do self cleanup
                                        (e.g. deregesiter cpid) after receive AASYSCOM_EE_SHUTDOWN_NOTIF_MSG*/
    TBoolean             euZombie;
} rtos_process_s;

typedef struct rtos_core_s
{
    pthread_mutex_t       shmIpcMutex;
    u32                   lastShmIpcMutexOwner;
    rtos_process_s        eus[RTOS_MAX_PROCS]; /*this table will be indexed with the rtos id allocated from rtosIdPool*/
    u32                   rtosIdPool[RTOS_MAX_PROCS];

    TMsgAtomic32          euStatus[RTOS_MAX_PROCS];

    int                   msg_count;
    int                   msg_total_size;
    pthread_mutex_t       analyserMutex;
    u32                   lastAnalyserMutexOwner;

    u32                   max_pid;        /* the highest pid on the system */
} rtos_core_s;
```

### create a message
- API：void *AaSysComMsgCreate(const TAaSysComMsgId messageId,
                        const TAaSysComMsgSize messagePayloadSize,
                        const TAaSysComSicad messageTarget)
AaSysComMsgCreate是底层IPC通信的封装，即分装了RTOS_msgBufAlloc。
URtosMsg* RTOS_msgBufAlloc(TRtosMsgSize size, TRtosMsgId msgNo)功能：
1. call `RTOS_msgBufAlloc` to allocate space(size of payload and syscom message header)
2. assign `messageId`, `target` and `size` to the syscom message
// message id, when and how to use it.

### send a message
- API： void AaSysComMsgSend(void **message)
1. get target Nid
2. send message according to the Nid
   2.1 if targetNid is Local, then convert `targetCpid` to `targetEuid`, then call `rtos_send`
   2.2 if targetNid is not local, then transfer message to syscom gateway.
      1. forward this Message to the syscom gateway receiver. 即通过调用`rtos_send`并把目标`euID`，设置成syscomgw的`euid`(`AASYSCOM_GW_CPID`)

#### syscomGW
syscomGW主要实现了以下几个功能:
1. 接收APP发往外部的消息，然后，将消息forward到sctp 的socket上
2. 建立syscom路由
3. 创建不同协议的socket，并管理相应的连接，如: 创建并管理sctp socket，sctp association。

- sctp one-to-many 方式
syscomGW使用了sctp one-to-many 方式的方式，即server端，不需要调用accept，client端，不需要调用connect，详见：
https://man7.org/linux/man-pages/man7/sctp.7.html

具体实现如下：
A syscom eu always wait to receive message, which need to be sent to other NID. It has a dedicated `AASYSCOM_GW_CPID`.
syscom daemon create this syscom gateway.
how does sycom gateway eu work.
1. receive syscom message, then
2. if it is a own message, handle message.
3. if not an own message, then route out it
   1. get sicAddress of sender and receiver, then find route thorough those.
   2. transfer syscom message to `AaSysComGwSctpControlMsg` message, data passed by pointer, so there is no copy
   3. send sctp `controlMsg` to SCTPCtrlThreadParent through `RTOS_send`, at the same time, notify SCTPCtrlThreadParent to receive message by trigger event.
   4. SCTPCtrlThreadParent read data from Rtos, then change `controlMsg's` euid to child's euid and evfd to child evfd, next call `RTOS_send` and notify SCTPCtrlThreadChild
   5. SCTPCtrlThreadChild read data from Rtos, then save data to a queue. It will read max three messages at one time. next copy data pointer to `epoll_ctl`.
   6. CNMapGwSCTPCtrlThreadChild send message to network(`processOutgoingData`).

### receive message
receive message with timeout or infinite.
call `RTOS_receive` or `RTOS_receiveWithTimeout` directly.

### Destroy message
call `RTOS_msgBufFree`

### destroy queue
destroy rtos queue when `AaSysComExitEe`, which will call `shutdownEUs`.

## Q&A:
1. 何时创建与销毁queue？
   1. 注册CPID时，创建queue
   2. EE 退出时，销毁所有的EUs对应的queue
2. 如何设置queue的大小？动态分配还是固定大小？
   1. queue 不能是固定大小的，因为如果固定大小，多个queue时，queue可能出现不够或者浪费。如果是动态分配，从系统获取，queue的能又会变差。所以，queue不能是固定长度，同时，又不能是从系统获取。因此，queue从提前的，固定分配的内存中申请。这样及保证性能又最大化资源的利用率。
3. 每个message 没有固定大小，要怎么设计？兼顾性能与灵活性？
   [buddy memory allocation](https://en.wikipedia.org/wiki/Buddy_memory_allocation)
4. 如何创建一个message？
5. 发送一个message，需要将message从贝到queue中吗？
   1. 不需要，传的是指针
6.  销毁message，是怎么释放资源地？
7.  message 销毁之后，queue是怎么管理的？如何高效地找到空闲地资源？
8.  为什么有一个ParentThread，一个childThread？
   1. with dynamic cpid, 其他syscom eu怎么知道发到这里来？
9.  收数据时，gw怎么把数据转发个对应的eu的？怎么找到对应的eu的？

## intra communication
由`rtos`实现了同节点上，不同APP之间的通信。

## inter communication
通过`syscomgw`实现了跨Node的通信。详见：[syscomGw](#syscomgw)

### 路由表的建立
详见[syscom route setup](./l1commproxy-syscomsetup.md)

### 新连接的处理
当SCTP_COMM_UP发生时，CNMapGwSCTPCtrlThreadParent的epoll_wait监测到有事件发生，调用读取数据函数，即processIncomingData，然后，调用sctp_recvmsg，来读取数据。如果数据是SCTP_COMM_UP消息，则处理SCTP的新连接，即processNewAssoc(meta, assocId)。

### sctp socket的创建
添加路由的过程中，会创建`sctp socket`。
创建`sctp socket`在函数CNMapGwSCTPCommunication::addConnection中处理。
在函数createAndBindSocket，创建了sctp 的socket，同时，listen在这个sctp socket上，socket的地址为local IP，即本container的IP。
```cpp
   i32 sd = socket(sa_in.ss_family, SOCK_SEQPACKET | SOCK_CLOEXEC, IPPROTO_SCTP)
   SOCKET_NO_ERROR != bind(sd, (struct sockaddr*)&sa_in, addrLen)
   SOCKET_NO_ERROR != listen(sd, MAX_PENDING_CONNECTIONS)
```
然后，创建EAaSysComGwSctpControlAction_AddParentSocket类型的AaSysComGwSctpControlMsg消息，通过进程间通信，传递给CNMapGwSCTPCtrlThreadParent进行处理，即CNMapGwSCTPCtrlThreadParent::processControlMessage。
把sctp sd加到CNMapGwSCTPCtrlThreadParent的epoll fd下，通过epoll fd来处理sctp sd的事件。比如，当有新的连接时即SCTP_COMM_UP时，处理新连接事件。

### 添加路由
CNMapGwRouter::addRoute(CAaSysComGwRouteEntry& entry, TAaSysComGwRouteId& routeID)
1. 在添加路由时，会根据协议，创建对应的协议的socket，如：添加sctp协议的路由，则创建sctp socket。详见：[sctp-socket的创建](#sctp-socket的创建)
2. 把route信息，放到map结构的路由表中

### 发送数据
1. 把syscom消息，缓存到child的发送队列中
2. 在当与对端没有建立连接时，在child里面会调用connect，尝试连接对端。
最终，child，在processOutgoingData函数内处理，发送数据到socket上。
int sendDataLen = sendto(childThreadQueue->childSd, CNMapGwSCTPControlMsg::getRetainPayload(controlMsg), controlMsg->bufferLen, 0, (struct sockaddr*)&controlMsg->sa_in, sizeof(controlMsg->sa_in));

### childSd 的创建
由parent 处理新assocation时，通过sctp_peeloff，从parent的sctp socket分离出来。

### child stcp socket fd的数据处理
child sd从parent sctp socket fd分离后，由parent 通过EAaSysComGwSctpControlAction_AddChildSocket类型的消息，传给child的线程，然后，在child线程里面，加入到child 模块里面的epoll fd，通过epoll来操作child stcp socket fd。详见：[发送数据](#发送数据)。

## performance
详见:[syscom performance](./syscom-performance.md)

## network service
https://github.com/Qihoo360/evpp
## syscom vs zmq
https://confluence.ext.net.nokia.com/display/5GRD/5G+CP+SW+middleware+assesment+-+DRAFT

## evolution
https://nokia.sharepoint.com/:p:/r/sites/PlatformSWArchitectureSteeringGroup/_layouts/15/Doc.aspx?sourcedoc=%7BADBF1718-B080-4E35-991D-CDF1B45F255E%7D&file=Loosely_coupled_sw_architeture_and_SysCom.pptx&wdLOR=c42FAD491-7465-49E7-A651-E3DD41593100&action=edit&mobileredirect=true

## implementation
void RTOS_ipcInitCe(void* address, unsigned long size)
初始化`rtos_ipcZone`
变量`rtos_ipcZone`: 从shared memory中申请的，用来管理并存放syscom message的内存

## rt
1. 哪些硬件上支持Event Machine？

### core affinity
https://confluence.ext.net.nokia.com/display/RCP/CCSRT+core+affinity

- DPM vs EM-ODP
  DPM: data plane middleware
  EM-ODP: Event machine on top of open data plane
  EM-DPDK: EM on top of DPDK
  [compare between DPM and EM-ODP](https://confluence.ext.net.nokia.com/display/RCP/EM-ODP+study?preview=%2F904961020%2F904961015%2FEM-ODP-DPM_Study.pptx)

- SCTP details
  https://nokia.sharepoint.com/:w:/r/sites/btscommsarchandinterfaces/_layouts/15/Doc.aspx?sourcedoc=%7B244D2504-9814-4FDC-99B1-25F2538FCBCE%7D&file=Transport%20layer%20Architecture%20Specification.docx&action=default&mobileredirect=true

- syscom introduction
  https://confluence.ext.net.nokia.com/display/UPHWAPI/AaSysCom+overview?preview=/325583528/1465600688/AaSysComIntroductionTraining.pptx

- reference
https://confluence.ext.net.nokia.com/display/UPHWAPI/AaSysCom+overview
