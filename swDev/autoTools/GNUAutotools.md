# GNU autotools
GNU autotools是GNU automake, GNU autoconf 及GNU libtool的合称

## 工作步骤
1. ./configure
   autotools，扫描host机器并发现默认配置。默认设置包括支持库所在的位置，以及新软件应放在系统上的位置。
2. make
   把代码编译成机器指令
3. make install
   将软件或者脚本，安装到指定的目录

## autotools的项目结构
必须包含的文件及文件夹
- src：源文件，如代码、脚本
- README
- NEWS
- AUTHORS
- ChangeLog
- configure.ac
- Makefile.am
### 注：
1. src是默认的源文件路径，如果是其他路径，需要修改配置

## 编写一个autotools项目
step1: [创建autotools项目结构](#autotools的项目结构)
step2: 编写configure.ac，如：
    AC_INIT([penguin], [0.0.1])
    AM_INIT_AUTOMAKE
    AC_CONFIG_FILES([Makefile])
    AC_PROG_CXX
    AC_OUTPUT
step3: 编写Makefile.am，如：
    bin_PROGRAMS = penguin
    penguin_SOURCES = penguin.cpp
step4: 生成构建脚本，编译并安装
    `autoreconf --install`
    `./configure`
    `make`
    `make install`
step5: 发布
    `make dist`    // 将你的工程打包成tar.gz
- 注：
1. `./configure`过程异常，通过`config.log`文件查看
- 引用
[introduction to autotools](https://opensource.com/article/19/7/introduction-gnu-autotools)
[autotools介绍](https://linux.cn/article-11218-1.html)

## configure.ac
configure.ac文件，被autoconf用来产生“configure”脚本。这个脚本在build之前，运行。
其中必需包含字段(M4 宏)
    **AC_INIT([penguin], [2019.3.6], [seth@example.com])**
    **AC_OUTPUT**
### configure.ac解析
- AC_INIT
    宏AC_INIT，用于初始化configure。每个configure脚本，都要先调用这个宏。
    AC_INIT定义的一些变量, 如：AC_PACKAGE_NAME，AC_PACKAGE_VERSION
- 格式
    AC_INIT (package, version, [bug-report], [tarname], [url])
- 注：
[AC_INIT](https://www.gnu.org/software/autoconf/manual/autoconf-2.67/html_node/Initializing-configure.html)

- Macro: AC_CONFIG_AUX_DIR (dir)
用到的build工具，如install-sh等，所在的路径。这些工具被configure使用。
常用如:
AC_CONFIG_AUX_DIR (build-aux)
- Macro: AC_OUTPUT
用来生成“config.status”脚本并进行该文件。该宏放到configue.ac的结尾，只要调一次。
- AX_PTHREAD
该宏指出如何来构建使用POSIX 线程的C Programs。该宏会输出PTHREAD_LIBS及PTHREAD_CFLAGS变量，
用于编译与链接。
[AX_PTHREAD](https://www.gnu.org/software/autoconf-archive/ax_pthread.html)
- LT_INIT
libtool初始化宏
- AC_CONFIG_FILES
AC_CONFIG_FILES宏，通过AC_OUTPUT宏，产生一个拷贝于“.in文件”的文件。
这是一个安装宏。
e.g:
AC_CONFIG_FILES([Makefile])

## makefile.am
Makefile.am文件，被automake用来产生Makefile脚本。autotools能够根据，在./configure时检测到的配置选项，
同时，结合Makefile.am，自动产生Makefile。
Makefile.am的语法与Makefile的一样。
### 格式解析
- libexecdir
该路径，用于存放被其他程序启动的可执行文件

- noinst_LTLIBRARIES
不需要安装库，又称为`Convenience Libraries`。链接时`，不需要`rpath`flag。详见：https://www.gnu.org/software/automake/manual/html_node/Libtool-Convenience-Libraries.html

## GNU M4
Unix marco 处理器
- refs
[Autoconf macros](./autoconf_macros.pdf)

## 路径宏的使用
如何让路径宏，如$(top_srcdir)，在脚本中被使用？
- option1：利用autotool，xxx.sh.in 使用`@top_srcdir@`, 在执行configure时，即xxx.sh.in->xxx.sh，工具自动会用$(top_srcdir)值来替换`@top_srcdir@`
- option2: 在makefile中，通过sed命令进行替换，典型的例子：
  ```bash
    bin_SCRIPTS = yourscript.sh
    yourscript.sh: $(top_srcdir)/yourscript.sh
        sed 's|@top_srcdir@|$(top_srcdir)|g' $< > $@
  ```

## libtool for library
`libtool`用来方便地创建library。其与automake适配性很好，支持版本管理，自动识别目标平台等，因此，能方便地，智能地创建library。

`libtool`默认同时编译出static及shared library。默认情况下，shared library需要安装到执行环境。若使用`noinst_LTLIBRARIES`语法，则`libtool`会自动以static 的方式，把library链接到可执行文件下。详见：https://openbooks.sourceforge.net/books/wga/dealing-with-libraries.html

- 如何使用
```bash
# in file configue.ac
# add libtool macro
AM_PROG_LIBTOOL

# in file makefile.am
# 关键字有`LT`的前缀，库名称的扩展名称为`.la`
bin_PROGRAMS = dummy
dummy_LDADD = libdump.la
# ...

lib_LTLIBRAIES = libdump.la
libdump_la_LDFLAGS=-version-info 0:0:0
```
- note
1. `.la`并不是可链接文件，但在`libtool`下，可用作（看作）一般的binary文件，进行连接。
2. `-version-info`，即libtool 用来管理shared library的`ABI` 版本

- `_LDADD`与`_LIBADD`区别
`_LDADD`只能使用-l,-L的标志，添加目标文件或者库到目标的binary的`link line`
`_LIBADD`，能够传递目标文件或库到目标库
