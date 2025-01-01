# Lab util: Unix utilities [1/2]

## What's in this part? [???]

---

## 本章会涉及到

LEC 1 (rtm): Introduction and examples (handouts: xv6 book)

> 简介和示例 (讲义: xv6 书籍)
> 

Preparation: Read chapter 1 (for your amusement: Unix)

> 准备: 阅读第 1 章 (供您娱乐: Unix)
> 

Assignment: [Lab util: Unix utilities](https://pdos.csail.mit.edu/6.S081/2024/labs/util.html)

> 作业：实验室工具: Unix 工具
> 

---

按照本节 [作业链接](https://pdos.csail.mit.edu/6.S081/2024/labs/util.html) 的指引, 来到 [QEMU/RISC-V tools chain](https://pdos.csail.mit.edu/6.S081/2024/tools.html) 配置页面按照 OS 选择配置

## 笔者的环境

> OS: macOS 14.7
> 
> CPU: Apple M1
> 
> Shell: zsh
> 
> Course: 6.S081
> 
> Term: 2024
> 

## 配置 QEMU/RISC-V tools chain

如果你没有安装过 `xcode-select` 或不知道是什么, 可以在终端输入以下命令安装:

```shell
xcode-select --install
```

接着, 安装一个包管理工具 `homebrew`:

```shell
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

接着, 安装 RISC-V compiler toolchain:

```shell
brew tap riscv/riscv
brew install riscv-tools
```

> check installation
>
> run `riscv64-unknown-elf-gcc` in terminal, if you see `command not found`, you need to add the path to your shell profile
>
> Example: add `PATH=$PATH:/usr/local/opt/riscv-gnu-toolchain/bin` in `~/.zshrc` or `~/.bashrc`, then restart terminal or run `source ~/.zshrc` or `source ~/.bashrc`

最后, 安装 QEMU(Quick Emulator):

```shell
brew install qemu
```

## clone xv6 repository and run

```shell
$> git clone git://g.csail.mit.edu/xv6-labs-2024
正克隆到 'xv6-labs-2024'...
remote: Enumerating objects: 7578, done.
remote: Counting objects: 100% (7578/7578), done.
remote: Compressing objects: 100% (3597/3597), done.
remote: Total 7578 (delta 4115), reused 7287 (delta 3953), pack-reused 0
接收对象中: 100% (7578/7578), 17.27 MiB | 3.35 MiB/s, 完成.
处理 delta 中: 100% (4115/4115), 完成.
$> cd xv6-labs-2024 
$> make qemu
```

## 简单使用

xv6 has no `ps` command, but, if you type `Ctrl-p`, the kernel will print information about each process. If you try it now, you'll see two lines: one for init, and one for sh.

To quit qemu type: `Ctrl-a x` (press `Ctrl` and `a` at the same time, followed by `x`).

### 启动后退出的 Example

```shell
xv6-labs-2024> make qemu                  
qemu-system-riscv64 -machine virt -bios none -kernel kernel/kernel -m 128M -smp 3 -nographic -global virtio-mmio.force-legacy=false -drive file=fs.img,if=none,format=raw,id=x0 -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0

xv6 kernel is booting

hart 2 starting
hart 1 starting
init: starting sh
$ QEMU: Terminated
```

## 配置 clangd (Optional)

```shell
brew install bear
```

```shell
make clean && bear --make qemu  
```

这时在当前目录下已经生成 `compile_commands.json` 文件, 用于配置 `clangd` 的编译信息

从当前目录启动 VS Code, 安装 `clangd` 插件, 即可享受到代码补全和跳转的便利

由于代码 formatter 不符合我的审美, 我继续编辑 `.clang-format` 文件, 使其符合我的审美. 又因为头文件导入顺序如果错乱会导致 Intellisense 异常, 加入 `SortIncludes: false` 使其不会格式化头文件导入顺序

> `.clang-format`

```yaml
BasedOnStyle: LLVM
IndentWidth: 4
SortIncludes: false
```

## 总结

已经模拟运行起来了, 熟练且网络通畅的情况下只需要几分钟即可, 慢一点也没关系. 如果觉得学习起来有阻力, 推荐先把上面所有 shell cmd Run 一遍, 体验到成功的喜悦, 然后再去了解每个命令的作用, 以及为什么要这么做.

下一步我们将继续进行 [Lab util: Unix utilities](https://pdos.csail.mit.edu/6.S081/2024/labs/util.html) 的实验: `sleep`, 在这之前 我们需要先阅读 [Chapter 1: Unix utilities](https://pdos.csail.mit.edu/6.S081/2024/xv6/book-riscv-rev4.pdf)

---

# Lab util: Unix utilities [2/2]

## What's in this part?

<!-- - [ ] [作业概览: Lab util: Unix utilities](https://pdos.csail.mit.edu/6.S081/2024/labs/util.html)
- [ ] [导读: Chapter 1: Unix utilities](https://pdos.csail.mit.edu/6.S081/2024/xv6/book-riscv-rev4.pdf) -->

---

刚开始以为是让自己实现一个 `sleep`, 寻思还挺有难度的, 不应该是 easy, 但题目提示 `kernel/sysproc.c` 中已经实现了, 我们只需要在 `user` 空间实现一个 `sleep` 的 `syscall` 即可, 于是我就开始了我的实验

## 实现 

### `user/sleep.c`

```c
/*
 * target: Use the system call sleep.
 * Add your sleep program to UPROGS in Makefile; once you've done that, make
 * qemu will compile your program and you'll be able to run it from the xv6
 * shell.
 */

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    // If the user forgets to pass an argument, sleep should print an error
    // message.
    char *err_info = "Usage: sleep {time}\n";
    if (argc != 2) {
        write(1, err_info, strlen(err_info));
    }

    // The command-line argument is passed as a string; you can convert it to an
    // integer using atoi (see user/ulib.c).
    int time = atoi(argv[1]);

    sleep(time); // system call

    exit(0); // sleep's main should call exit(0) when it is done.
}
```

#### 评测

```shell
xv6-labs-2024> ./grade-lab-util sleep
make: 'kernel/kernel' is up to date.
== Test sleep, no arguments == sleep, no arguments: OK (3.7s) 
== Test sleep, returns == sleep, returns: OK (0.4s) 
== Test sleep, makes syscall == sleep, makes syscall: OK (0.8s)
```

pass!

### `user/pingpong.c`

```c
/*
 * Write a user-level program that uses xv6 system calls to ''ping-pong'' a byte
 * between two processes over a pair of pipes, one for each direction.
 *
 * The parent should send a byte to the child; the child should print "<pid>:
 * received ping", where <pid> is its process ID, write the byte on the pipe to
 * the parent, and exit; the parent should read the byte from the child, print
 * "<pid>: received pong", and exit.
 *
 * Your solution should be in the file
 * user/pingpong.c.
 *
 * Some hints:
 *
 * Add the program to UPROGS in Makefile.
 *
 * You'll need to use the pipe, fork, write, read, and getpid system calls.
 *
 * User programs on xv6 have a limited set of library functions available to
 * them. You can see the list in user/user.h; the source (other than for system
 * calls) is in user/ulib.c, user/printf.c, and user/umalloc.c.
 */

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define PIPE_READ 0
#define PIPE_WRITE 1

int main() {
    int parent_to_child[2], child_to_parent[2];
    char one_byte = 'a';

    // init pipe
    if (pipe(parent_to_child) < 0 || pipe(child_to_parent) < 0) {
        fprintf(2, "pipe() failed\n");
        exit(1);
    }

    if (pipe(parent_to_child) < 0 || pipe(child_to_parent) < 0) {
        fprintf(2, "pipe() failed\n");
        exit(1);
    }

    // in parent process - return sub process PID
    // in sub process - return 0

    int fork_pid = fork();
    if (fork_pid > 0) {
        // parent process
        close(parent_to_child[PIPE_READ]);  // only write
        close(child_to_parent[PIPE_WRITE]); // only read

        write(parent_to_child[PIPE_WRITE], &one_byte, 1);

        read(child_to_parent[PIPE_READ], &one_byte, 1);
        printf("%d: received pong\n", getpid());

        close(parent_to_child[PIPE_WRITE]);
        close(child_to_parent[PIPE_READ]);
    } else if (fork_pid == 0) {
        // sub process
        close(parent_to_child[PIPE_WRITE]);
        close(child_to_parent[PIPE_READ]);

        read(parent_to_child[PIPE_READ], &one_byte, 1);
        printf("%d: received ping\n", getpid());
        write(child_to_parent[PIPE_WRITE], &one_byte, 1);

        close(parent_to_child[PIPE_READ]);
        close(child_to_parent[PIPE_WRITE]);
    } else {
        printf("%d: error, fork_pid < 0\n", getpid());
    }

    exit(0);
}
```

#### 评测

```shell
xv6-labs-2024> ./grade-lab-util pingpong
make: 'kernel/kernel' is up to date.
== Test pingpong == pingpong: OK (0.7s) 
```

## 调试

### GDB CLI

> terminal 1

```shell
xv6-labs-2024> make qemu-gdb
*** Now run 'gdb' in another window.
qemu-system-riscv64 -machine virt -bios none -kernel kernel/kernel -m 128M -smp 3 -nographic -global virtio-mmio.force-legacy=false -drive file=fs.img,if=none,format=raw,id=x0 -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0 -S -gdb tcp::25501
```

> terminal 2

```shell
xv6-labs-2024> riscv64-elf-gdb                                                                        util-!?
GNU gdb (GDB) 15.2
Copyright (C) 2024 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.
Type "show copying" and "show warranty" for details.
This GDB was configured as "--host=aarch64-apple-darwin23.6.0 --target=riscv64-elf".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<https://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
    <http://www.gnu.org/software/gdb/documentation/>.

For help, type "help".
Type "apropos word" to search for commands related to "word".
The target architecture is set to "riscv:rv64".
warning: No executable has been specified and target does not support
determining executable automatically.  Try using the "file" command.
0x0000000000001000 in ?? ()
(gdb) 
```

GDB 会查找当前目录下的 `.gdbinit` 文件, 如果有则自动加载, 而 `.gdbinit` 文件中有 `target remote` 命令, 会自动连接到 QEMU 的 GDBstub, 以及执行一些初始化操作

显示 `0x0000000000001000 in ?? ()` 说明 GDB 已经连接到了 QEMU 的 GDBstub

> 如果不习惯 GDB 的 CLI, 可以使用 GDB Dashboard 插件
>

[GDB Dashboard - GitHub](https://github.com/cyrus-and/gdb-dashboard)

#### 效果图

![](file:///Users/lov3/Documents/Blog/Gridea/post-images/1735065146439.png)

### VS Code

配置 `launch.json` 和 `tasks.json` 文件, 实现 `F5` 启动调试

#### 效果图

![](file:///Users/lov3/Documents/Blog/Gridea/post-images/1735065113481.png)

### CLion

加载项目时选择 Makefile, 并修改参数为 qemu, clean 动作参数不变

配置远程调试, 选择远程调试, 而不是远程 GDB 服务器

#### 效果图

![](file:///Users/lov3/Documents/Blog/Gridea/post-images/1735065106914.png)

### 总结

可惜目前并不懂如何在调试结束后向 qemu 发送退出信号, 所以只能手动退出

等等, 信号? 那岂不是在 GDB 结束后跟随向进程发送信号即可退出? 试试看

问题是如何无感完成这个操作呢

...


## 遇到的问题

### 1. `make qemu` 后无法找到 `sleep` 命令

#### Add your sleep program to UPROGS in Makefile; once you've done that, make qemu will compile your program and you'll be able to run it from the xv6 shell.

打开项目根目录下的 `Makefile`, 找到 `UPROGS` 变量, 添加 `user/sleep` 即可

需要注意的是, 每个程序之间需要用 `\` 分隔, 并用 `$U/` 前缀来指定路径

> Makefile 中的 UPROGS 变量的 example, 在 200 行附近

```shell
UPROGS=\
	$U/_cat\
	$U/_echo\
	$U/_forktest\
	$U/_grep\
	$U/_init\
	$U/_kill\
	$U/_ln\
	$U/_ls\
	$U/_mkdir\
	$U/_rm\
	$U/_sh\
	$U/_stressfs\
	$U/_usertests\
	$U/_grind\
	$U/_wc\
	$U/_zombie\
	$U/_sleep\
```

### 2. `make grade` 报错

#### 找不到 `pipes` 模块

```shell
xv6-labs-2024> make grade
make clean
make[1]: Entering directory '/xv6/xv6-labs-2024'
rm -rf *.tex *.dvi *.idx *.aux *.log *.ind *.ilg *.dSYM *.zip *.pcap \
*/*.o */*.d */*.asm */*.sym \
user/initcode user/initcode.out user/usys.S user/_* \
..................................................
riscv64-unknown-elf-objdump -t kernel/kernel | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$/d' > kernel/kernel.sym
make[1]: Leaving directory '/Users/user/Documents/Code/lab/xv6/xv6-labs-2024'
== Test sleep, no arguments == Traceback (most recent call last):
  File "/Users/user/Documents/Code/lab/xv6/xv6-labs-2024/./grade-lab-util", line 105, in <module>
    run_tests()
    ~~~~~~~~~^^
  File "/Users/user/Documents/Code/lab/xv6/xv6-labs-2024/gradelib.py", line 136, in run_tests
    test()
    ~~~~^^
  File "/Users/user/Documents/Code/lab/xv6/xv6-labs-2024/gradelib.py", line 56, in run_test
    fn()
    ~~^^
  File "/Users/user/Documents/Code/lab/xv6/xv6-labs-2024/./grade-lab-util", line 10, in test_sleep_no_args
    r.run_qemu(shell_script([
    ~~~~~~~~~~^^^^^^^^^^^^^^^
        'sleep'
        ^^^^^^^
    ]))
    ^^^
  File "/Users/user/Documents/Code/lab/xv6/xv6-labs-2024/gradelib.py", line 451, in run_qemu
    self.qemu = QEMU(target_base + "-gdb", *make_args)
                ~~~~^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  File "/Users/user/Documents/Code/lab/xv6/xv6-labs-2024/gradelib.py", line 311, in __init__
    show_command(("make",) + make_args)
    ~~~~~~~~~~~~^^^^^^^^^^^^^^^^^^^^^^^
  File "/Users/user/Documents/Code/lab/xv6/xv6-labs-2024/gradelib.py", line 241, in show_command
    from pipes import quote
ModuleNotFoundError: No module named 'pipes'
make: *** [Makefile:346: grade] Error 1
```

原因是当前环境中没有 `pipes` 模块, 切换到低版本 `python` 环境解决

### 3. GDBstub

一个终端执行 `make qemu-gdb`, 另一个终端执行 `gdb` 进行调试

> terminal 1

```shell
xv6-labs-2024> make qemu-gdb
*** Now run 'gdb' in another window.
qemu-system-riscv64 -machine virt -bios none -kernel kernel/kernel -m 128M -smp 3 -nographic -global virtio-mmio.force-legacy=false -drive file=fs.img,if=none,format=raw,id=x0 -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0 -S -gdb tcp::25501
```

> terminal 2

```shell
xv6-labs-2024> riscv64-elf-gdb
```

但如果想用更方便的 `VS Code` 进行调试, 参考网上的资料配置后, 按下调试, 终端此前的 `make qemu-gdb` 会自动退出

> terminal 1

```shell
qemu-system-riscv64: QEMU: Terminated via GDBstub
```

搜索关键词, 找到了 [stackoverflow Solution](https://stackoverflow.com/a/5572860/20184956)

链接中的内容表明是 `target remote` 两次造成 GDBstub 退出, ~~但我并没有在 `VS Code` 中设置两次 `target remote`~~, 此时反应上来是自动 `source .gdbinit` 文件, 里面有 `target remote` 命令, 于是我将文件删除, 问题解决

这个文件可能会在 `make qemu-gdb` 时自动生成, 所以需要注意不要 `target remote` 两次

在终端下的复现:

> terminal 1

```shell
xv6-labs-2024> make qemu-gdb                                                                                                           util-!?
*** Now run 'gdb' in another window.
qemu-system-riscv64 -machine virt -bios none -kernel kernel/kernel -m 128M -smp 3 -nographic -global virtio-mmio.force-legacy=false -drive file=fs.img,if=none,format=raw,id=x0 -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0 -S -gdb tcp::25501
```

> terminal 2

```shell
> riscv64-elf-gdb 
GNU gdb (GDB) 15.2
Copyright (C) 2024 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.
Type "show copying" and "show warranty" for details.
This GDB was configured as "--host=aarch64-apple-darwin23.6.0 --target=riscv64-elf".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<https://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
    <http://www.gnu.org/software/gdb/documentation/>.

For help, type "help".
Type "apropos word" to search for commands related to "word".
(gdb) target remote 127.0.0.1:25501
Remote debugging using 127.0.0.1:25501
warning: No executable has been specified and target does not support
determining executable automatically.  Try using the "file" command.
0x0000000000001000 in ?? ()
(gdb) target remote 127.0.0.1:25501
A program is being debugged already.  Kill it? (y or n) y
could not connect: Operation timed out.
(gdb) 
```

此时终端 1 输出如下

> terminal 1

```shell
qemu-system-riscv64: QEMU: Terminated via GDBstub
```

以此可以证明, `target remote` 两次会导致 GDBstub 退出

---

## 杂七杂八

macOS 下的 CLion 默认格式化整个文件的快捷键是 `Cmd + Option + L`

---

# Reference

[csdiy](https://csdiy.wiki/%E6%93%8D%E4%BD%9C%E7%B3%BB%E7%BB%9F/MIT6.S081/#_4)

[gitbook](https://mit-public-courses-cn-translatio.gitbook.io/mit6-s081)

[jyy xv6 代码导读](https://jyywiki.cn/OS/2022/slides/18.slides.html#/)

[在 VSCode 中调试 xv6](https://www.zixiangcode.top/article/how-to-debug-xv6-in-vscode)

[MIT 6.S081 lab0：配置xv6环境+vscode调试 by 江欣婷](https://acmicpc.top/2024/02/08/MIT-6.S081-lab0-%E9%85%8D%E7%8E%AF%E5%A2%83/)

[6.1810 Lab1 util: Unix utilities by LRL52](https://lrl52.top/1213/6-1810-lab1-util-unix-utilities/)

[guidance](https://pdos.csail.mit.edu/6.1810/2024/labs/guidance.html)

[VS Code - cpp launch-json-reference](https://code.visualstudio.com/docs/cpp/launch-json-reference)
