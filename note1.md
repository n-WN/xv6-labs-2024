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

### System call tracing

add prototype in `user.h`

add entry in `usys.pl`

add syscall number in `syscall.h`

add struct proc in `proc.h`

add sys_trace in `sysproc.c`

change `fork` in `proc.c`

lastly, add `trace info` in `syscall.c`

变更的代码见 commit [System call tracing](https://github.com/n-WN/xv6-labs-2024/commit/7d2c0db0f9d19a67628731f90c2d7d782cd1bedc)

### 

## Reference

[riscv-privileged-20211203](https://github.com/riscv/riscv-isa-manual/releases/download/Priv-v1.12/riscv-privileged-20211203.pdf)

