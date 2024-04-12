1. gdb cannot open shared object file: No such file or directory
   1. 在conde 的build image对应的container使用gdb时
2. message thread why can't use thread_join
3. coredump 中能获取到哪些信息？
   1. 如何分析coredump文件
4. 如何把wsl里面的文件，直接upload到cloud的服务器中？
5. 条件变量的使用及注意事项？
   [](https://en.cppreference.com/w/cpp/thread/condition_variable/wait)
   [](https://en.cppreference.com/w/cpp/thread/condition_variable)
   1. 子线程，执行到wait时，notify才有意义
6. define empty or default constructor in hpp, compare to cpp
7. 编译器，能够检查构造的依赖吗？
8. objdump 使用场景？
   https://tuttlem.github.io/2015/01/12/a-simple-example-with-gcc-and-objdump.html
9.  为啥是这个写法, boost http的用法

void HttpClientImp::connect(boost::system::error_code ec) {
  if (ec) {
    return fail(ec, "connect");
  }

  http::async_write(socket, req,
                    std::bind(&HttpClientImp::write, shared_from_this(),
                              std::placeholders::_1, std::placeholders::_2));
}
10. componentCI，如何进行json的check
11. https://gerrite1.ext.net.nokia.com/c/scm_rcp/adetservice/+/1439092/7/src/common/HttpClientImp.cpp#29
lambad un user param rule
12. rpm build require dependency
    编译依赖怎么定？
    https://gitlabe1.ext.net.nokia.com/rcp_rpm_specs/SS_RCPCCSRT/-/merge_requests/148#note_7447916
    If including public header files provided by x-devel requires also public header files from y-devel, the this dependency must be visible in the *.pc file provided by x-devel. rpmbuild automatically recognizes that dependency and adds it to the x-devel rpm- No need to explicitly list the dependency in spec file.
One exception to this rule is Requires: boost-devel, which is needed if public header files provided by x-devel #include boost header files. This is because boost does not provide *.pc file.
1.