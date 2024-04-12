# pkg-config
`pkg-config`工具，可以从`.pc`文件中获取库的编译（Cflags），链接（Libs）及package版本等信息。
在库文件变更时，如路径，编译符号变更，使用`pkg-config`，可以让APP的编译不需要进行适配，因此，代码的适配性更强，开发者也更容易使用库文件。详见：https://people.freedesktop.org/~dbn/pkg-config-guide.html

在大多数linux版本中，`pkg-config`都适用。

## 工作原理
在指定的路径下，搜索与`libxxx`相应`.pc`文件,即`libxxx.pc`, 然后，从文件中，提取库的编译，链接信息。

## pc file
`.pc`文件包含`APP`如何编译与链接库的元数据，供`pkg-config`工具使用。
`.pc`通常由package安装工具，安装到`pkg-config`指定的路径。`pkg-config`工具，在指定的路径下，搜索相应的`.pc`文件。
- `.pc`文件，在开发阶段使用，所以，打包到devel文件夹下

- 如何编写`pc`
示例，详见链接。
```bash
prefix=/usr/local
exec_prefix=${prefix}
includedir=${prefix}/include
libdir=${exec_prefix}/lib

Name: foo
Description: The foo library
Version: 1.0.0
Cflags: -I${includedir}/foo
Libs: -L${libdir} -lfoo
```

## 如何使用
- 与`autoconf`的结合
```bash
# in file configure.ac
PKG_CHECK_MODULES([LIBXXX], [libxxx])

# in Makefile.am
bin_PROGRAMS = dummy
dummy_CFLAGS = $(LIBXXX_CFLAGS)
dummy_LIBADD = $(LIBXXX_LIBS)
```
配置这个宏，`autoconf`就会导出变量`LIBXXX_CFLAGS`及`LIBXXX_LIBS`，用于编译与链接。

- 与`SPEC`文件的结合
```bash
BuildRequires:  pkgconfig(libxxx)
```
利用`pkg-config`检查依赖`libxxx`是否存在，即`libxxx.pc`是否存在

## 与libtool的比较
https://en.wikipedia.org/wiki/Pkg-config#:~:text=pkg%2Dconfig%20is%20a%20computer,of%20detailed%20library%20path%20information.
