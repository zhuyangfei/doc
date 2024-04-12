# daemon process
没有控制终端的进程，称为daemon进程。其生命周期通常比较长，通常用作服务进程。如syslogd进程。
其`TTY`字段显示为`？`, `TPGID`显示为-1。
所有用户层的daemon进程，都是进程组组长进程及会话首进程。
- 进程组组长
  即PGID = PID

daemon process实例，如ministarter, /bin/start_rsyslog, syscom_starter.sh
```bash
bash-5.2$ ps -axj
   PPID     PID    PGID     SID TTY        TPGID STAT   UID   TIME COMMAND
      0       1       1       1 ?             -1 Ss    9999   1:12 /usr/bin/ministarter
      1      11      11       1 ?             -1 Sl    9999  70:01 /usr/bin/6tunnelmanager
      1      12      12       1 ?             -1 S     9999   0:00 /usr/bin/genapilogger-cat --ident 6tunnel /usr/bin/sta
      1      13      13       1 ?             -1 S     9999   0:00 /bin/bash /bin/start_rsyslog
      1      14      14       1 ?             -1 S     9999   0:00 /usr/bin/bash /usr/libexec/syscom/syscom_starter.sh
     13      15      13       1 ?             -1 Sl    9999   4:44 rsyslogd -n -iNONE
     14      16      14       1 ?             -1 S     9999   0:00 logger -t CLOUD -p user info
     14      17      14       1 ?             -1 S     9999   0:00 logger -t CLOUD -p user err
     12      25      12       1 ?             -1 S     9999   0:00 /usr/bin/bash -e /usr/bin/start-6tunnel-for-remote-sto
     14      56      14       1 ?             -1 Sl    9999   0:51 /usr/libexec/syscom/syscomd
      1      65      65       1 ?             -1 Sl    9999  10:52 /usr/bin/adetservice
      1      66      66       1 ?             -1 Sl    9999 105:49 /usr/bin/fhtrsconfigagent
      1      67      67       1 ?             -1 Sl    9999   1:03 /usr/libexec/logrouter/logrouter
      1      68      68       1 ?             -1 Sl    9999   0:36 /opt/nokia/bin/syscomrannicprovider libsyscom /usr/lib
      1      69      69       1 ?             -1 Sl    9999   0:36 /opt/nokia/libexec/syscomrouteproxy/syscomrouteproxy l
     25     755      12       1 ?             -1 S     9999   0:00 /usr/bin/coreutils --coreutils-prog-shebang=sleep /usr
      0     758     758     758 pts/0        759 Ss    9999   0:00 bash
    758     759     759     758 pts/0        759 R+    9999   0:00 ps -axj
```
- ministarter 怎么带起其他daemon进程？
1. fork
2. setupKillSwitch(parentPid); // 当parent退出时，childprocess，收到Kill信号
3. setpgrp 设置PGID = PID
4. exec

## 如何创建
daemon process，有特定的创建流程。
