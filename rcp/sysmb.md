# SysMb（Message broker）
   SysMb是一种消息中介，不同的服务，在彼此不能直接通信时，通过SysMb的publish及subscribe接口来交换消息。

## 典型应用场景
### 服务发现
    server 发布在线消息
    app 订阅server的online消息
    ![serverDiscover](/severDiscover.png)