# user and group

## user
every user have a unique name and according to a unique ID. a user can belong to multi-group.

## group
every group has a unique name and unique ID.

## /etc/passwd
a file saves user's information. such as root:x:0:0:root:/root:/bin/bash

## /etc/shadow
enhance of /etc/passwd to protect password attack, only access by privilege process.

## /etc/group
a file save group information.

## knows
1. linux 通过user与group进行权限的控制，只有对应的user或者更高权限的user，才能操作相应的文件。
2. 每个进程或文件都有属于他的user与group
3. API接口：
    AddUser，Add Group，chmod等
4. root 用户有最高权限
5. 文件系统生成的文件，怎么看属于哪个user，还是能看到的文件，都是属于当前user？
6. 与Cgroup的关系是啥？
Cgroup借鉴了user与group的设计？
Cgroup对系统资源进行管理

