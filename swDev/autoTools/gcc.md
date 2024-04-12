# GCC

## GCC support C++
since GCC11, the default C++ standard is C++17
https://gcc.gnu.org/projects/cxx-status.html

## compilation error
- `ld` failed with tag [abi:cxx11]
  ```bash
  `syscomrannicprovider::getRanNicHostName[abi:cxx11]()':
    /home/chuagao/code/MSG/syscomrouteproxy/tst/../src/RannicInfoReceiver.cpp:41: undefined reference to `syscomrannicprovider::uname(utsname*)'
    /usr/bin/ld: ../src/UT_RannicInfoReceiver-RannicInfoReceiver.o: in function
  ```
  可以因为你要`ld`对象，使用不一样的`_GLIBCXX_USE_CXX11_ABI `宏值进行编译。缺少引用头文件时，也会出现上述的问题。
  https://gcc.gnu.org/onlinedocs/libstdc++/manual/using_dual_abi.html
