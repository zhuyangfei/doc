# syslog
syslog是linux提供的log服务，其由用户接口`syslog`及daemon进程组成，如`rsyslogd` 进程。daemon进程，通过监听位于`/dev/log`文件, 再根据配置文件, 如`/etc/rsyslog.conf`，把日常输出到某个位置，如backend、文件系统等。

## why syslog
syslog 是一种日志标准
把不同设备的日志收集到一个地方，方便troubleshooting，

## how does it work
采用client-server的架构，不同的设备作为client，同时，有一个logger server(也是client)

## Q&A
1. aasyslog 怎么与syslog进行配合？如怎么把syslog的转到aasyslog，反之，怎样？
A: assyslog listen logs send from rsyslog, then save log to buffer(a shared memory buffer).
aasyslog can configure output to a tcp or udp device, so logs can forward to rsyslogd.

2. rcp 的rsyslog的怎么配置的？怎么deployment的？
A：通过configmap, `templates/rsyslog-config.yaml`, 比如，l1commproxy的/etc/rsyslog.conf
```ruby
module(load="imuxsock")
module(load="omstdout")
if prifilt("*.*") then {
    action(type="omstdout" template="RSYSLOG_SyslogProtocol23Format")
}
$EscapeControlCharactersOnReceive off

global(LocalHostName=`echo $RSYSLOG_LOCALHOSTNAME`)


action(type="omfwd" Target="logstorageserver" Port="1025" Protocol="tcp" TCP_Framing="octet-counted" template="RSYSLOG_SyslogProtocol23Format")
```
3. rcp 的rsyslog是由谁启动的？
A: 由ministarter启动
```bash
4 S 9999          18       1  0  80   0 -  1006 -      19:24 ?        00:00:00 /bin/bash /bin/start_rsyslog
4 S 9999          20      18  0  80   0 - 37305 -      19:24 ?        00:00:00 rsyslogd -n -iNONE
```
4. 用户进程与`rsyslogd`是怎么交互的？
A：通过API接口，syslog

5. syslog的性能怎样？
6. 对比aasyslog，有哪些相同点与不同点？

7. kubectl logs 取得是什么log?
A: 对APP而言，取得是APP输到`stdout`得log
https://www.tigera.io/learn/guides/kubernetes-monitoring/kubernetes-logging/

## rsyslog
rsyslog是syslog的另一个实现，其支持很多功能，如通过TCP、UDP协议，接收与转发log。其template功能，可以支持定制不同格式的log，如RFC5424等。
https://www.rsyslog.com/category/guides-for-rsyslog/

## use case
### logservice
logservice pod compose with two containers. `logstorageserver` container is use to deploy rsyslog service.
rsyslog configuration of container `logstorageserver`.

cat /etc/rcp-rsyslog.conf
```ruby
# setting escaping off in order to remove the control characters
global(maxMessageSize="64k" parser.escapeControlCharactersOnReceive="off")

# setting main queue to Disk-Assisted Memory Queue
main_queue(queue.highWatermark="10000" queue.lowWatermark="4000" queue.size="100000" queue.type="LinkedList" queue.filename="syslog.0" queue.spoolDirectory="/var/log/spool" queue.maxDiskSpace="200M")

# Readable only by user
$FileCreateMode 0600

module(load="imtcp"
       MaxSessions="5000"
       KeepAlive="on"
       KeepAlive.time="60"
       KeepAlive.interval="30"
       DiscardTruncatedMsg="on")

module(load="imudp")

include(
   file="/tmp/rsyslog.conf.d/*.conf"
)

include(
   file="/etc/rcp-rsyslog.conf.d/*.conf"
)
```
cat /etc/rcp-rsyslog.conf.d/30-syslog.conf

```ruby

lookup_table(name="severity_table" file="/etc/rcp-rsyslog.conf.d/severity_mapping.json" reloadOnHUP="on")

set $.host_name=getenv("HOSTNAME");
set $.severity = lookup("severity_table", $syslogseverity-text);

template(name="non_aasyslog_fmt" type="string"
         string="XX %hostname% <%timestamp:::date-rfc3339%> %procid% %$.severity%/%app-name%, %msg%\n")
# RCP CC&S uses RSYSLOG_SyslogProtocol23Format template to format the AaSyslog, this format will introduce
# a leading space in the MSG field. To strip this leading space character, we should print the MSG feild
# from the second character.
template(name="aasyslog_fmt" type="string" string="%msg:2:$%\n")
template(name="outfmt" type="string" string="%$.out%")

# emerg(0), alert(1), crit(2), err(3), warning(4), notice(5), info(6), debug(7)
if ($syslogseverity < 5 and $syslogfacility == 16) then {
        set $.out = exec_template("aasyslog_fmt");
        call out_syslog_err
}
if ($syslogseverity < 5 and $syslogfacility != 16) then {
        set $.out = exec_template("non_aasyslog_fmt");
        call out_syslog_err
}
ruleset(name="out_syslog_err") {
        :omfile:$syslog_log_err;outfmt
}

# Write everyting we get to output and stop
if ($syslogfacility == 16) then {
        set $.out = exec_template("aasyslog_fmt");
        call out_syslog
}
if ($syslogfacility != 16) then {
        set $.out = exec_template("non_aasyslog_fmt");
        call out_syslog
}
ruleset(name="out_syslog") {
        :omfile:$syslog_log;outfmt
}

template(name="substring" type="string" string="%msg%\n")

if ($msg contains "#LOG_FORWARD_CONTROL_MSG#: Enable logs forward") then {
    set $/forward = "true";
        action(type="omfwd" Target="127.0.0.1" Port="50107" Protocol="udp" template="substring")
}

if ($msg contains "#LOG_FORWARD_CONTROL_MSG#: Disable logs forward") then {
    set $/forward = "false";
        action(type="omfwd" Target="127.0.0.1" Port="50107" Protocol="udp" template="substring")
}

# Forward syslog(exclude headers) to logprocessor to do substring filtering except its own logs
if ($hostname != $.host_name and $/forward == "true") then {
        action(type="omfwd" Target="127.0.0.1" Port="50107" Protocol="udp" template="substring")
}

stop
```

## reference
https://blog.csdn.net/zhizhengguan/article/details/111597389#:~:text=Linux%E7%B3%BB%E7%BB%9F%E6%97%A5%E5%BF%97%201%20syslog%20%E5%B7%A5%E5%85%B7%E6%9C%89%E4%B8%A4%E4%B8%AA%E4%B8%BB%E8%A6%81%E7%BB%84%E4%BB%B6%EF%BC%9Asyslogd%20daemon%20%E5%92%8C%20syslog%20%283%29%E5%BA%93%E5%87%BD%E6%95%B0,%E4%B8%AD%E7%9A%84%E6%B6%88%E6%81%AF%E7%9A%84%E5%8F%A6%E4%B8%80%E4%B8%AA%E6%9D%A5%E6%BA%90%E6%98%AF%20Kernel%20Log%20daemon%20klogd%EF%BC%8C%E5%AE%83%E4%BC%9A%E6%94%B6%E9%9B%86%E5%86%85%E6%A0%B8%E6%97%A5%E5%BF%97%E6%B6%88%E6%81%AF%EF%BC%88%E5%86%85%E6%A0%B8%E4%BD%BF%E7%94%A8%20printk%20%28%29%E5%87%BD%E6%95%B0%E7%94%9F%E6%88%90%E7%9A%84%E6%B6%88%E6%81%AF%EF%BC%89%E3%80%82%20

https://www.linuxfordevices.com/tutorials/remote-syslog-in-linux
https://stackify.com/syslog-101/
https://en.wikipedia.org/wiki/Syslog
