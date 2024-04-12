# event machine

## EM-DPM
- dispatcher
  用于从event queue中取事件，i.e 调用eo_receive 接口。
- queue group(QG)
  queue 的集合，在queue之前被创建。queue group，与core关联，可以配置到不同的core上。类似process，通过affinity，配置到不同的core上。
- event queue
  事件队列，由用户创建，可以设置不同的优先级。优先级高，先被调度。
- eo
  event object, 由用户创建。
- master core
  运行在dip0上，负责处理来自外部的event，然后，将events 送给各个APP, 或者接收来自APP的event，并将event发送到外部去。dispatcher 与 core 一一对应
- work core
  分配给APP运行的资源
- event scheduler
  em-scheduler or event scheduler, 负责将event queue调度到queue group关联的core上。event scheduler，不是一个process。
  一次调度，dispatcher并发地取event。

- 外部与内部，怎么区分与设计的？
  以主机区分吗，经过网卡的是外部数据？

## 典型应用场景
master core：接收外部的数据，如packet，然后，封装成event，根据不同的event类型，放到不同的event queue中，同时，触发event queue scheduler, 由event-scheduler 把queue调度到不同queue group所关联的不同的core上。在queue被调到某个core时，core上执行的dispatcher，处理event。

## APP启动创建并启动em
1. Eo create
2. Queue create
3. Attach queue to EO
4. Enable the Queue
5. Start EO

## implement
- master/primary core
  1. initial primary core
     1. rte_eal_init
     2. 创建ring,
   dpm_hal_ring_create(tp_up_rx_rings[ring_cnt].ring_name,TP_UP_RING_QUEUE_SIZE,SOCKET_ID_ANY,RING_F_SC_DEQ)
     3.
      dpm_alloc_name_block(PROC_MODE_CONF_ZONE,sizeof(dpm_proc_mode_conf_t),64)
     4. initial dpdk
      dpdk_init(DPDK_AUTO);
  2. get network interface of CLI_APP_IFACE and APP_CLI_IFACE, then create APP_CLI_IFACE socket
  3. start primary work loop
     1. receive packet(event data packet) from cli
     2. fetch data from packet and copy to a new event, then send to eo
      ```cpp
         local_ptr = em_alloc(size,event_type,NW_DATA_POOL);
         void* new_buf = em_event_pointer(local_ptr);
         memcpy(new_buf,data_buf->data,size);
         int ret = em_send((void*)local_ptr,data_buf->emo_id);
      ```
      3. fetch events from ring, then send to cli by socket
      4. usleep(100);

- second core
  1. rte_eal_init
  2. configure em and initialize em
     - em_init(&evm_conf)
  3. if the first rt process, then
     - 3.1. create em memory, em_create_mem
     - 3.2. register user handlers, em_reg_trs_handle(handle_shared_ring_io_data);em_reg_interface_buff_handle(handle_shared_ring_io_data,4);
     - 3.3. create master process
       1. create eo with user defined handler
       2. create queue
       3. add eo to queue
       4. enable queue
       5. start eo
     - 3.4. create egress handler(another eo, just like master process)
     - 3.5. event_bundle_init()
  4. event_bundle_attach
  5. dpm_secondary_proc_work_loop
     - 1. master core loop
         ```cpp
         while(1)
         {
             poll_timestamp = rte_get_timer_cycles();
             em_dispatch_timestamp = rte_get_timer_cycles();
             em_dispatch(1); /** Passing 0 to em_dispatcher, control never returns*/
             timer_manage_timestamp = rte_get_timer_cycles();
             /** Call DPM Timer Routine*/
             em_timers_managed();
             em_ext_bundle_flush();
         }
         ```
     - 2. other core
       - call em_dispatch
       - do business
       - em_ext_bundle_flush
        ```cpp
        while (1){
			   em_dispatch(1);
			   em_cpuload_generate();
            em_ext_bundle_flush();
		      }
        ```
- nrt_core mask
  1. set coremask and schedule affinity
  2. dpm_secondary_proc_init
  3. event dispatcher

## Q&A:
1. app 的handler event是怎样串起来的？
   创建EO时，会传入event handler
2. app 启动一个dispatcher，意味的什么？
3. eo 与 dispatcher的关系
4. core 与 dispatcher的关系
5. ccs在dpm之上怎么理解，ccs怎么与dpm进行交互的？
6. QG 由谁创建？为啥要引入QG的概念？

## reference
[DPM_OpenEventMachine_In_Detail](./em-dpm.pdf)
