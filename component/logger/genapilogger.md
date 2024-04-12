# genapilogger
genpailogger 给APP提供统一的logger接口，支持多种不同的log方式，如`syslog`, linux systemd journal, remote syslog daemon, stdout/stderr or other user defined log.

## feature
1. common logging API
2. support facility, the same as syslog(3)
3. support levers
4. support sync and async logging

## genapiloggermt
基于genapilogger封装的，提供类似syslog接口的模块

## reference
[genapilogger](https://gitlabe1.ext.net.nokia.com/genapi/genapilogger/)
