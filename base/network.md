# network

## App支持IPv6
1. 协议不依赖编程方式，`getaddrinfo()`和`getnameinfo()`，详见：https://man7.org/linux/man-pages/man3/getaddrinfo.3.html
2. IPv4与IPv6的数据报文格式不一样，IPv4的socket，只能收IPv4的报文，IPv6 socket, 可接收IPv4及IPv6的报文。dual-stack, 可支持收/发IPv4及IPv6的报文。
3. 在双栈的情况下，IPv6 socket接收IPv4的报文，kernel会自动将IPv4地址转成IPv4 mapped IPv6的地址。发送数据时，再将IPv4 mapped IPv6的地址，转成IPv4的地址。
4. UDP中，调用connect，相当于设置socket的address及port，TCP中，调用connect，尝试与server建立链接，即3次握手。详见：https://man7.org/linux/man-pages/man2/connect.2.html
[IPv4 and IPv6 Interoperability](https://zaf1ro.github.io/p/b4e.html)

- reference
Beej's Guide to Network Programming, https://beej.us/guide/bgnet/
https://www.xiaolincoding.com/network/3_tcp/tcp_interview.html#%E6%9C%8D%E5%8A%A1%E5%99%A8%E5%87%BA%E7%8E%B0%E5%A4%A7%E9%87%8F-time-wait-%E7%8A%B6%E6%80%81%E7%9A%84%E5%8E%9F%E5%9B%A0%E6%9C%89%E5%93%AA%E4%BA%9B

## 内核角度看IO模型
从 Linux 内核角度看 IO 模型的演变，
https://zhuanlan.zhihu.com/p/630753032

## route

## debug
1. 数据发送成功，但是，抓包（datalink层）中没有，则检查route，是否正确配置。
