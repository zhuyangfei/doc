
# wsl usage

## enable wsl in windows
1. step 1: win10配置wsl，并安装ubuntu
   1. Enable wsl
   2. Install ubuntu
[How to Enable WSL on Windows 10 Home (Turn on WSL/WSL2)](https://windowsloop.com/enable-wsl-windows-10-home/#:~:text=Steps%20to%20Enable%20WSL%20on%20Windows%2010%20Home,files%20and%20enables%20the%20feature.%20More%20items...%20)

2. step2：启动ubuntu，添加与公司git 上一样名字的用户

3. step3： 设置`新增的用户`为启动ubuntun的默认用户

4. step4：重启电脑

[wsl document](https://docs.microsoft.com/en-us/windows/wsl/)

## configure `conde` path in wsl remote
编辑启动脚本：`~/.bashrc`,
`export PATH=~/workbench/conde:$PATH`
VS code 通过`interactive`方式，WSL remote，因此，配置的启动脚本是`~/.bashrc`
[Unix shell initialization](https://github.com/rbenv/rbenv/wiki/unix-shell-initialization)
[Configure the environment for the remote extension host](https://code.visualstudio.com/docs/remote/troubleshooting#_configure-the-environment-for-the-remote-extension-host)

## 解决使用VPN后，网络不通的问题
1. step1: set Interface Metrics
   连上VPN后，以administrator方式，打开`powershell`, 在`powershell`中输入，如下命令：
   `Get-NetAdapter | Where-Object {$_.InterfaceDescription -Match "Cisco AnyConnect"} | Set-NetIPInterface -InterfaceMetric 6000`

2. step2: set DNS Servers in linux
   2.1 获取DNS server IP, 在`powershell`中输入：
    `(Get-NetAdapter | Where-Object InterfaceDescription -like "Cisco AnyConnect*" | Get-DnsClientServerAddress).ServerAddresses`
    把返回的IP地址，配置到`WSL`。
   2.2 在`wsl`中, 配置DNS Servers，如下：
   sudo vi /etc/resolv.conf
        nameserver 10.10.0.124
        nameserver 10.10.0.132

[WSL 2 Cisco AnyConnect Networking Workaround](https://gist.github.com/pyther/b7c03579a5ea55fe431561b502ec1ba8)
### workaround 2: configure wsl network automatically.
run script in powershell:
./vpnnet.ps1
[vpnnet.ps1 script](./vpnnet.ps1)

#### note:
1. fix authority of running vpnnet.ps1
   Set-ExecutionPolicy -ExecutionPolicy Bypass -Scope CurrentUser
   for details, refer to : [powershell execution policies](https://docs.microsoft.com/en-us/powershell/module/microsoft.powershell.core/about/about_execution_policies?view=powershell-7.2)
   **error message**
   ```
   .\vpnnet.ps1
   CategoryInfo          : SecurityError: (:) [], PSSecurityException
   FullyQualifiedErrorId : UnauthorizedAccess
   ```

## wsl permission deny
[Fix Windows Subsystem for Linux (WSL) File Permissions · Brian's Blog (turek.dev)](https://www.turek.dev/posts/fix-wsl-file-permissions/)

[File Permissions for WSL | Microsoft Docs](https://docs.microsoft.com/en-us/windows/wsl/file-permissions)

[Configure Linux distributions | Microsoft Docs](https://docs.microsoft.com/en-us/windows/wsl/wsl-config#per-distribution-configuration-options-with-wslconf)

	# Enable extra metadata options by default
	[automount]
	enabled = true
	root = /windir/
	options = "metadata,umask=22,fmask=11"
	mountFsTab = false

	# Enable DNS – even though these are turned on by default, we'll specify here just to be explicit.
	[network]
	generateHosts = true
	generateResolvConf = true

```
// mask 设置值
The mask permissions are not like the octal permission codes passed to the chmod command, however thistable is really helpful understanding how the masks permissions work :
    0   1   2   3   4   5   6   7
r   +   +   +   +   -   -   -   -
w   +   +   -   -   +   +   -   -
x   +   -   +   -   +   -   +   -
It works as the normal octal permissions but subtracted from 7, and use the absolute value. forinstance if you want to set the permissions to 0777 you will need to set it 0000 in the umask (e.g.umask=0000), if you want to set it to 0755 you will set it to 0022
- The first character represents that its an octal permissions
- The second is for the owner
- The third is the group
- The fourth is for other, i.e any other user

From <https://askubuntu.com/questions/429848dmask-and-fmask-mount-options#:%7E:text=fmask%20and%20dmask%20are%20mountfiles%20and%20dmask%20to%20directories>

umask=value
       Set the umask (the bitmask  of  the  permissions  that  are  not
       present).  The default is the umask of the current process.  The
       value is given in octal.
dmask=value
       umask of the current process.  The value is given in octal.
fmask=value
       Set the umask applied to regular files only.  The default is the
       umask of the current process.  The value is given in octal.

From <https://askubuntu.com/questions/429848/dmask-and-fmask-mount-options#:%7E:text=fmask%20and%20dmask%20are%20mount,files%20and%20dmask%20to%20directories>
```

## VS code with WSL
[ Remote - WSL extension documentation](https://code.visualstudio.com/docs/remote/wsl)

## wsl file system
wsl -> windows: /mnt/c
windows -> wsl: \\wsl$

## reference
1. https://confluence.ext.net.nokia.com/display/NACCI/Docker+Development+in+WSL2#DockerDevelopmentinWSL2-WSL2workaroundwithNokiaVPN

## 通过window 代理服务器访问web
```bash
# 获取windows的内部ip
 cat /etc/resolv.conf | grep nameserver | awk '{print $2}'

 #配置代理地址
 export http_proxy=http://<proxy_address>:<proxy_port>
 export https_proxy=http://<proxy_address>:<proxy_port>

 #e.g
 export https_proxy=http://172.28.16.1:7897

 # 测试是否正常工作
 curl -I https://www.google.com

 # 长期有效
cat ~/.bashrc
export http_proxy="http://$(cat /etc/resolv.conf | grep nameserver | awk '{print $2}'):7897"
export https_proxy="http://$(cat /etc/resolv.conf | grep nameserver | awk '{print $2}'):7897"
export no_proxy="localhost,127.0.0.1,::1
```

### 待机后cpu飙升
- 异常
  wsl --shutdown 卡住
- 原因
  wsl kernel bug
- 如何解决
  替换指定版本的kernel

```bash
cat ~\.wslconfig
[wsl2]
kernel=C:\\Users\\burke\\.wsl-kernels\\vmlinuz-msft-5.4.72
```
参考：https://github.com/carlfriedrich/wsl-kernel-build/issues/1
https://github.com/microsoft/WSL/issues/8696#issuecomment-1372751462


## Docker usage

### Failed to start service Docker

```bash
Error starting daemon: Error initializing network controller: error obtaining controller instance: failed to create NAT chain DOCKER: iptables failed: iptables --wait -t nat -N DOCKER: iptables v1.8.2 (nf_tables): CHAIN_ADD failed (No such file or directory): 

```

原因及解决办法，参考：https://github.com/WhitewaterFoundry/Pengwin/issues/485
```bash
The docker installer uses iptables for nat. Unfortunately Debian uses a modified version of nftables. You can convert the entries over to nftables or just setup Debian to use the legacy iptables.

sudo update-alternatives --set iptables /usr/sbin/iptables-legacy
sudo update-alternatives --set ip6tables /usr/sbin/ip6tables-legacy

dockerd, should start fine after switching to iptables-legacy.

sudo service docker start

# 测试
docker ps
```

### 启动docker 服务不识别
```bash
sudo service docker start
docker: unrecognized service
```
【原因】：WSL2 instance is using the default init system instead of systemd；
【解决】：wsl --update
【检查】：`ps -p 1 -o comm=`

### 自启动docker服务
~/.bashrc
```bash
# service docker start
if ! pgrep -x "dockerd" > /dev/null; then
    sudo service docker start
fi
```


### docker hub
解决办法：1. 配置daemon proxy，2. 配置repo mirror
参考：
https://gist.github.com/y0ngb1n/7e8f16af3242c7815e7ca2f0833d3ea6
- 方法1：配置daemon proxy
step1: 编辑daemon配置文件
```bash
vi /etc/docker/daemon.json
{
    "registry-mirrors": [
        "https://dockerproxy.com",
        "https://docker.mirrors.ustc.edu.cn",
        "https://docker.nju.edu.cn"
    ],
    "proxies": {
        "http-proxy": "http://172.30.32.1:7897",
        "https-proxy": "http://172.30.32.1:7897",
        "no-proxy": "localhost,127.0.0.1,::1"
    }
}

docker info    #查看docker信息，验证是否生效
```
step2: 重启docker服务
```bash
sudo systemctl restart docker
```

### docker 运行权限问题
异常信息：
`Got permission denied while trying to connect to the Docker daemon socket at`

解决办法：
docker守护进程启动的时候，会默认赋予名字为docker的用户组读写Unix socket的权限，因此只要创建docker用户组，
并将当前用户加入到docker用户组中，那么当前用户就有权限访问Unix socket了，进而也就可以执行docker相关命令。
```
sudo groupadd docker     #添加docker用户组
sudo gpasswd -a $USER docker     #将登陆用户加入到docker用户组中
newgrp docker     #更新用户组
docker ps    #测试docker命令是否可以使用sudo正常使用
```
[docker 运行权限](https://www.cnblogs.com/Li-JT/p/13994704.html)

### conde build 与docker GID 999冲突
   修改docker的GID
[GID](https://www.cnblogs.com/peida/archive/2012/12/05/2802419.html)
