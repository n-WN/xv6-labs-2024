# Lab: system calls

## What's in this part? [???]

[task - syscall](https://pdos.csail.mit.edu/6.S081/2024/labs/syscall.html)

> 在上次实验中，你使用了系统调用来编写了一些实用程序。在本实验中，你将为 xv6 添加一些新的系统调用，这将帮助你理解它们的工作原理，并让你接触到 xv6 kernel 的一些内部机制。你将在后续实验中添加更多系统调用。

> Before you start coding, read Chapter 2 of the xv6 book, and Sections 4.3 and 4.4 of Chapter 4, and related source files:

> To start the lab, switch to the syscall branch:

```shell
$ git fetch
$ git checkout syscall
$ make clean
```

> If you run make grade you will see that the grading script cannot exec trace. Your job is to add the necessary system calls and stubs to make trace work. Furthermore, you will notice attacktest fails.

## 实现

### Using gdb (easy)

已在 note0.md 中介绍了如何使用 gdb 调试 xv6

本 part 并无难点, 但要注意制造 panic 后

- sepc 为出错代码的 `program counter(PC)`, 可以在 gdb 中通过 `b *0x???????` 的方式来设置断点, 接着 `continue` 运行

- scause 表示 trap 的原因, 值 `0xd` 也就是 `13` 表示的错误可以从 `riscv-privileged-20211203` 中查到

- stval 表示 trap 的 faulting address - 导致 trap 的地址

答案在 GitHub Repo 的 syscall 分支 的 `answers-syscall.txt` 文件中

[answers-syscall.txt](https://github.com/n-WN/xv6-labs-2024/blob/7d2c0db0f9d19a67628731f90c2d7d782cd1bedc/answers-syscall.txt)

跑了下 grade, 竟然通过了, 不清楚是如何判断的... 暂时没时间去看评测实现了

### System call tracing

add prototype in `user.h`

add entry in `usys.pl`

add syscall number in `syscall.h`

add struct proc in `proc.h`

add sys_trace in `sysproc.c`

change `fork` in `proc.c`

lastly, add `trace info` in `syscall.c`

变更的代码见 commit [System call tracing](https://github.com/n-WN/xv6-labs-2024/commit/7d2c0db0f9d19a67628731f90c2d7d782cd1bedc)

#### 评测

```shell
$ make grade
== Test answers-syscall.txt == 
answers-syscall.txt: OK 
== Test trace 32 grep == 
$ make qemu-gdb
trace 32 grep: OK (4.3s) 
== Test trace close grep == 
$ make qemu-gdb
trace close grep: OK (0.7s) 
== Test trace exec + open grep == 
$ make qemu-gdb
trace exec + open grep: OK (2.0s) 
== Test trace all grep == 
$ make qemu-gdb
trace all grep: OK (1.1s) 
== Test trace nothing == 
$ make qemu-gdb
trace nothing: OK (1.9s) 
== Test trace children == 
$ make qemu-gdb
trace children: OK (22.2s) 
== Test attack == 
$ make qemu-gdb
attack: FAIL (0.3s) 
    ...
         hart 2 starting
         init: starting sh
         $ attacktest
         FAIL: no/incorrect secret
         $ qemu-system-riscv64: terminating on signal 15 from pid 82894 (<unknown process>)
    MISSING '^OK: secret is'
    QEMU output saved to xv6.out.attack
== Test time == 
time: FAIL 
    Cannot read time.txt
Score: 35/50
make: *** [Makefile:347: grade] Error 1
```

### Attack xv6

通过查看注释, 可以定位到 `kernel/kalloc.c`, 这是用户程序的物理内存分配器

> kernel stacks, page-table pages

`user/secret.c` 从 `kernel/kalloc.c` 中通过 `kalloc` 从栈顶取内存页, 退出时通过 `kfree` 释放内存页(放回栈顶)

而这时就可以复用这个内存页, 从而获取到 `secret` 的值

**注意** `secret` 与 `attack` 分配内存时, 内存页的出栈顺序不一定相同, 要注意按照释放内存页的逆序来获取 secret

#### 评测

```shell
$ make grade
== Test answers-syscall.txt == 
answers-syscall.txt: OK 
== Test trace 32 grep == 
$ make qemu-gdb
trace 32 grep: OK (4.4s) 
== Test trace close grep == 
$ make qemu-gdb
trace close grep: OK (0.7s) 
== Test trace exec + open grep == 
$ make qemu-gdb
trace exec + open grep: OK (1.0s) 
== Test trace all grep == 
$ make qemu-gdb
trace all grep: OK (1.0s) 
== Test trace nothing == 
$ make qemu-gdb
trace nothing: OK (1.1s) 
== Test trace children == 
$ make qemu-gdb
trace children: OK (23.6s) 
== Test attack == 
$ make qemu-gdb
attack: OK (0.4s) 
== Test time == 
time: FAIL 
    Cannot read time.txt
Score: 49/50
make: *** [Makefile:347: grade] Error 1
```

最后一个得分点输出不能读取 `time.txt`, 分析 grade 的代码, 错误来自 `./gradelib.py` 中的 `check_time` 函数

```python
def check_time():
    try:
        print("")
        with open('time.txt') as f:
            d = f.read().strip()
            if not re.match(r'^\d+$', d):
                raise AssertionError('time.txt does not contain a single integer (number of hours spent on the lab)')
    except IOError:
        raise AssertionError('Cannot read time.txt')
```

全部做完后, 再回头看这里吧

## Reference

[riscv-privileged-20211203](https://github.com/riscv/riscv-isa-manual/releases/download/Priv-v1.12/riscv-privileged-20211203.pdf)

[NOSAE's blog](https://nos-ae.github.io/)

