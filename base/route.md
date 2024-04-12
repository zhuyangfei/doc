# route

## Network route
网络之间连接的虚拟路径
- 不在一个子网上的设备，怎么通过路由发送数据的? 比如，如下的网络：hostA（192.168.1.3/24）->RouterA->RouterB->hostC(192.168.2.1/24)
1. step1, hostA 判断HostC的地址不在一个子网内，所以，通过gateway发送数据，即把目标Mac地址替换成路由的地址，
数据报文格式：RA_Mac+HostA_MAC+HostAIP+HostCIP
2. step2: RouteA 查路由表，发现目标地址需要forward 给RouterB
3. step3: RouteB 查路由表，发小目标地址在自己的子网内，将目标Mac地址替换成hostC的Mac地址，发送给HostC
4. HostC: 发现Mac与IP都是自己，开始解包，将数据传给应用层

https://www.cnblogs.com/xysgo/p/16155591.html
https://www.youtube.com/watch?v=KTKgueDtIdw
https://blackyau.cc/27.html


## Host route
主机与route之间的路径

## subnet
子网：同一子网内的数据转递，不需要经过路由或者gateway。每个接口都可以设置自己的子网，比如,
在接口`mplane`与`traceplane`上的子网
```bash
2a00:8a00:8000:134::e:5800/120 dev mplane proto kernel metric 256 pref medium
2a00:8a00:8000:134::e:cb00/120 dev traceplane proto kernel metric 256 pref medium
```

子网掩码，决定子网中主机的数量，如
```bash
[yanzhu@espoofedora38 ~]$ ip route
default via 192.168.0.1 dev eth0 proto dhcp src 192.168.0.95 metric 100
169.254.169.254 via 192.168.0.1 dev eth0 proto dhcp src 192.168.
192.168.0.0/24 dev eth0 proto kernel scope link src 192.168.0.95 metric 100
```
`192.168.0.0/24`中`/24`表示24bit的掩码，剩余8bit为主机数，即子网中主机数量范围0~255, 所以，该子网为192.168.0.0~192.168.0.255.

## ip route
用于显示及管理内核的路由表
```bash
$ ip route
default via 192.168.0.1 dev eth0 proto dhcp src 192.168.0.95 metric 100
169.254.169.254 via 192.168.0.1 dev eth0 proto dhcp src 192.168.0.95 metric 100
172.17.0.0/16 dev docker0 proto kernel scope link src 172.17.0.1 linkdown
192.168.0.0/24 dev eth0 proto kernel scope link src 192.168.0.95 metric 100
$ ip -6 route
fe80::/64 dev eth0 proto kernel metric 256 pref medium
fe80::/64 dev fhm proto kernel metric 256 pref medium
```
1. `default via 192.168.0.1 dev eth0 proto dhcp src 192.168.0.95 metric 100 `: 默认路由，任何地址未知的数据，通过eth0接口，发送到192.168.0.1，即网关的地址。`proto dhcp`: 该路由通过`dhcp`添加。`metric 100`: 相比其他路由，该路由的优先级为100。
2. `169.254.169.254 via 192.168.0.1 dev eth0 proto dhcp src 192.168.0.95 metric 100`: IP地址为`169.254.169.254`, 通过eth0接口，发送到192.168.0.1，即网关的地址。
3. `172.17.0.0/16 dev docker0 proto kernel scope link src 172.17.0.1 linkdown`: `docker0`网络的本地路由，子网`172.17.0.0/16`内的数据，通过该接口发送。`linkdown`，当前该接口关闭或未使用。
4. `192.168.0.0/24 dev eth0 proto kernel scope link src 192.168.0.95 metric 100`: `eth0`本地路由，子网`192.168.0.0/24`内数据，由eth0发送。
5. `fe80::/64 dev eth0 proto kernel metric 256 pref medium`: `eth0`上IPv6的本地子网，`fe80::`为常见的本地子网前缀。`pref medium`中等优先级
6. `fe80::/64 dev fhm proto kernel metric 256 pref medium`: 在`fhm`上的本地子网
