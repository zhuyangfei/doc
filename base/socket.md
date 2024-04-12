# socket
`socket`是网络通信的接口，可以用于跨主机之间的进程通信。通过`socket`可以统一基于不同协议的网络通信

- [TCP socket](#tcp-socket)
  - [TCP server 工作流程](#tcp-server-工作流程)
  - [TCP client 工作流程](#tcp-client-工作流程)
- [UDP socket](#udp-socket)
- [IO 复用的处理](#io-复用的处理)
- [close socket的处理](#close-socket的处理)
- [client或server，断开，重启探测及处理](#client或server断开重启探测及处理)

## TCP socket
即基于`TCP`协议的socket。
TCP协议是面向连接，即在通信前，client与server之间先要建立连接。同时，传递的是字节流，所以，需要循环的读取数据，直到读完所有数据。

### TCP server 工作流程
1. 通过`getaddInfo`来获取地址
   通过`getaddInfo`来获取地址，这样应用程序，可以不感知`IPv4`与`IPv6`, 同时，调用`bind`接口前，不需要大小端转换
   接口如下：
   ```cpp
   int getaddrinfo(const char *restrict node,
                       const char *restrict service,
                       const struct addrinfo *restrict hints,
                       struct addrinfo **restrict res);
   ```
   通过`hits`来过滤可用的地址，然后，调用``socket`及`bind`,当`bind`成功时，即找到的可用地址。
   - 大小端
    小端，高位存储在高地址，低位存储在低地址，而大端与之相反。
    网络及TCP协议，定义需要大端的数据类型。而通常一般的系统，都是小端，所有需要进行数据转换。
2. 创建socket
   ```cpp
   /**
   * domain: AF_UNIX(local communication), AF_INET, AF_INET6
   * type：SOCK_STREAM,SOCK_DGRAM 如：报文，字节流
   * protocol：TCP, UDP
   * return: socket id
   */
   int socket(int domain, int type, int protocol);
   ```
   在内核中，申请一块sock空间，并返回该空间的一个引用，即返回值socketfd.
3. 设定socket的属性
   socket的属性，如空闲后，什么时候开始heart beat，heart beat间隔, 重启后，允许重复绑定到同一接口。
   ```cpp
   /**
    * protocol
   */
   setsockopt(int socket, int protocol, int option_name, const void* option_value, socklen_t option_len);
   ```
4. 绑定地址
   把socket的数据，如`IP`，`PORT`, 从用户层，拷贝到内核。接口如下
   ```cpp
   int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
   ```
5. 监听`连接`
   通过`listen`API，让server与client建立连接，即三次握手。接口如下：
   ```cpp
   int listen(int sockfd, int maxConnectionsForPending);
   ```
   有多个client连接时，内核会把这些连接缓存到队列里面，等待APP调用`accept`接口。如果缓存的连接数，超过`maxConnectionsForPending`，则后续的client 连接会拒，直到队列有空闲。
   `maxConnectionsForPending`的大小设置，与性能及负载均衡有关。
6. 取出`连接`，然后，基于这个连接(fd)，读写数据
   ```cpp
   int fdConnect = accept(sockfd, address, addrlen);
   int fdConnect2 = accept(sockfd, NULL, NULL);
   // MSG_NOSIGNAL is no SIGPIPE
   system.send(streamDescriptor->native_handle(), data.data(), data.size(), MSG_NOSIGNAL);
   // MSG_DONTWAIT is non-block
   recv(CHILDSOCKET_FILENO, buffer, sizeof(buffer), MSG_DONTWAIT);
   ```
   - `accept`
    1. `fdConnect`相当于是包含对端地址的索引，所以，后续的`send`，`recv`只要使用`fdConnect`，就能与对端进行通信。
    2. 在block sockfd的情况下，如果没有connection，则accept阻塞调用，若为non-block sockfd, accept 返回-1。可用把sockfd，放到poll中，当由connection时，poll返回。
   - `send`
    对于字节流，send会block直到数据发送完毕。send成功返回，仅表示数据发到网卡驱动。
   - `recv`
    如果对端shutdown，则`recv`返回0。


### TCP client 工作流程
与server不同点，在于不需要调用`listen`及`accept`，在`bind`之后，需要调用`connect`,以建立于server的连接。

## UDP socket

## IO 复用的处理
- 场景1：单个线程，处理多个连接，并多个连接(长连接)与读写数据
  即循环执行
  ```
  loop:
    accept
    read or write connect1
    read or write connect2
    ...
  end loop
  ```
  1. 当其中一个连接的读写被block时，那么`accept`及其他connects的读写，也被block
  2. 当没有新连接时，`accept`会block
   所以，并发连接性能及CPU的性能都会变差

- 场景2：多个线程，一个线程处理connects，仍后每个connect一个线程处理
  1. 把sockfd都放到epoll中，由epoll来处理连接
  2. 一个线程，处理connection的读写
  为啥一个线程，只处理一个connect呢，还在每个线程中使用poll呢？
  详见muduo多线程模型

## close socket的处理
当`recv`返回0时，表示对端已经关闭，本端可以关闭socket。
详见，epoll中，怎么处理socket shutdown消息
https://blog.csdn.net/weixin_44834554/article/details/128676703

## client或server，断开，重启探测及处理
