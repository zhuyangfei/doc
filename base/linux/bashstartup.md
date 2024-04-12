# bash startup

## interactive shell
A shell can use to interactive with a user, such as with a termination.

## start up
### login shell
启动后，首先，执行`/etc/profile`脚本，然后，查找并执行`~/.bash_profile`，`~/.bash_login`,`~/.profile`。
shell退出时，执行`~/.bash_logout`

### non-login
执行`~/.bashrc`
详见：https://www.gnu.org/software/bash/manual/html_node/Bash-Startup-Files.html
