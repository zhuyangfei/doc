# split commit
- **step1**. 调整commits结构
   通过`git rebase`，把需要split的commit，调整到`当前编辑`状态。如：倒数第二个单子，需要split。`rebase`时，把单子由`pick`改成`e(edit)`，如下：
   git rebase -i HEAD~2，
    ```
        e 4b7e78e Add an empty syscomrannicprovider process
        pick dc903c3 Start up syscomrannicprovider by ministarter

        # Rebase 500c048..dc903c3 onto 500c048 (2 commands)
        #
        # Commands:
    ```
    `rebase`后，`HEAD`就指向了倒数第二个单子。
    注：
    1. `4b7e78e`对应第二个单子
    2. 如拆分最后一个单子，那么可以省略这个步骤
    3. 如果多个单子需要拆分，那么把对应的单子都修改成`edit`
- **step2**. 回退当前commit
   通过`git reset HEAD~1`, 回退当前commit,
    git reset HEAD~1
    ```
        Unstaged changes after reset:
        M       Makefile.am
        M       configure.ac
    ```
    该commit的改动，就都处在`没有commit状态`。如：
    git status
    ```
    Changes not staged for commit:
      (use "git add <file>..." to update what will be committed)
      (use "git restore <file>..." to discard changes in working directory)
            modified:   Makefile.am
            modified:   configure.ac
    ```
- **step3**. 拆分commit
   通过`git add -p filename`，选择性的进行commit，如：
    git add -p Makefile.am
    ```
    diff --git a/Makefile.am b/Makefile.am
    index fe56c0a..ce805b7 100755
    --- a/Makefile.am
    +++ b/Makefile.am
    @@ -2,6 +2,7 @@ ACLOCAL_AMFLAGS = -I m4
     NAME = syscomrouteproxy
    
     pkglibexec_PROGRAMS = syscomrouteproxy
    +bin_PROGRAMS = syscomrannicprovider
    
     syscomrouteproxy_SOURCES = \
                         src/RouteProcessService.cpp \
    (1/3) Stage this hunk [y,n,q,a,d,j,J,g,/,e,?]? 
    ```
    通过选择不同的命令，如`y`，接受所有改动，`e`，编辑该改动，来拆分。
    拆分后，同一个文件，就有既有staged的部分，又有unstaged的部分。如：
    ```
    Changes to be committed:
        (use "git restore --staged <file>..." to    unstage)
            modified:   Makefile.am

    Changes not staged for commit:
        (use "git add <file>..." to update what will be     committed)
        (use "git restore <file>..." to discard changes     in working    directory)
              modified:   Makefile.am
              modified:   configure.ac
    ```
## note
1. (1/3): 表示`Makefile.am`改动被分成了3块
2. 命令[y,n,q,a,d,j,J,g,/,e,?]，参考：[git-add](https://git-scm.com/docs/git-add)
3. `新增`的文件，直接修改文件，分几次提交
- **step4**. 提交commit
   `git commit -e`
- **step5**. 重复步骤4，5，直到所有改动都提交，然后，继续rebase，`git rebase --continue`
## note
1. 使用`gerrite`工具，需要替换commit`changeID`，这样原单子的就不会被`abandon`了。修改方法，`git commit --amend`。

