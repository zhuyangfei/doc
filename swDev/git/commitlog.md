# 为什么一个好的commit log是重要的 #
- 一个关于修改的好的沟通资料
- log上能看到修改的原因, git diff中只能看到怎么做
- 提高项目的可维护性
- 从log上可以看出一个开发者是不是一个好的合作者

# 规则 #
- 标题规则
  1. 少于50个字符
  2. 首字母大写
  3. 结尾不使用'.'
  4. 标题与内容之间空一行.
  5. 使用祈使句的方式

        好的例子：
            Refactor subsystem X for readability
            Update getting started documentation

        不好的例子：
            ~Update the documentation of getting started~
            ~Refactor for readability of subsystem X~
            ~Fixed bug with Y~
- 内容规则
  1. 解释做什么（what）和为什么（why），而不是怎么做（how)

        日志内容需要包含，修改前，代码是怎样的，修改后，代码是怎样的，同时，解释这么修改的原因。例如：

            commit eb0b56b19017ab5c16c745e6da39c53126924ed6
            Author: Pieter Wuille <pieter.wuille@gmail.com>
            Date:   Fri Aug 1 22:57:55 2014 +0200

                Simplify serialize.h's exception handling

                Remove the 'state' and 'exceptmask' from serialize.h's stream
                implementations, as well as related methods.

                As exceptmask always included 'failbit', and setstate was always
                called with bits = failbit, all it did was immediately raise an
                exception. Get rid of those variables, and replace the setstate
                with direct exception throwing (which also removes some dead
                code).

                As a result, good() is never reached after a failure (there are
                only 2 calls, one of which is in tests), and can just be replaced
                by !eof().

                fail(), clear(n) and exceptions() are just never called. Delete
                them.

  2. 每行内容控制在72个字符

# 参考资料 #
[cbea.ms/git-commit](https://cbea.ms/git-commit/)