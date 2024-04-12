## aasyslog
ccs base log component. aasyslog can receive APP's log. aasyslog support logs sent form both aasyslog APi and syslog. aasyslog can support outputting logs to different kinds of log devices, such as stdout, tcp, or LFS.

## features
1. 日志可选择输出到多个设备，如filesystem，udp 设备，TCP设备等等
2. 有统一的格式，支持不同的log level
3. 可以压缩日志
4. 支持startup.log及runtime.log，system_xxx.log, Aasyslog_xxx.zip
5. 支持filter，both input and output

## Q&A
1. APP 与 log server 之间，如何通信？
   - 控制消息：APP 的log 通过syscom消息，发送给syslog server，在mcu或者rt上执行
   - 其他消息，通过shared memory。APP 将消息存到ring buffer，位于shared memory中，而logserver 从ring buffer中读取消息
2. syslog 与 aasyslog 如何交互？
aasyslog 通过监测rsyslogd，把APP通过syslog输出的日志，转存在ring buffer，从而打通了aasyslog与syslog之间的路径
3. aasyslog server 启动之前，怎么打印log，如何被收集？
aasyslog server 启动之前，daemon 与 APP 的log 都通过syslog打印，启动之后，把log切到aasyslog。
4. 启动阶段，log从syslog切到aasyslog，APP会感知吗？

## reference
[syslog service API specification](https://nokia.sharepoint.com/:b:/r/sites/ps/ccs/Shared%20Documents/001%20CC%26S/CC-S%20API%20Specifications%20and%20Customer%20Documentation/CC-S%20Service%20API%20Specification%20AaSysLog.pdf?csf=1&web=1&e=VK04G7)
