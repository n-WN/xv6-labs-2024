# Lab: page tables

## What's in this part? [???]

[task - page tables](https://pdos.csail.mit.edu/6.S081/2024/labs/pgtbl.html)

> 将探索页面表并修改它们以实现常见的操作系统功能

> Before you start coding, read Chapter 3 of the xv6 book, and related files:
>
>> kernel/memlayout.h, which captures the layout of memory.
>>
>> kernel/vm.c, which contains most virtual memory (VM) code.
>>
>> kernel/kalloc.c, which contains code for allocating and freeing physical memory.
>>
> It may also help to consult the RISC-V privileged architecture manual.

> To start the lab, switch to the pgtbl branch:

```shell
$ git fetch
$ git checkout pgtbl
$ make clean
```

> 做这 part 之前, 笔者先看了 jyy 的 xv6 代码导读, 里面有讲一些基本的内存结构, 以及一些基本的内存管理的代码, 这些都是很有帮助的. slides 如下:

[slides](https://jyywiki.cn/OS/2022/slides/18.slides.html#/)

更详细的内容依旧参考 [xv6 book]

## 实现

### Inspect a user-process page table (easy)

> 翻译 task

为了帮助您理解 RISC-V 页表, 您的第一个任务是解释用户进程的页表.

运行 make qemu 和运行用户程序 pgtbltest . print_pgtbl 函数使用我们为 xv6 添加的实验室系统调用打印出 pgtbltest 进程的前 10 页和最后 10 页的页表项. 输出如下所示: 

```shell
va 0 pte 0x21FCF45B pa 0x87F3D000 perm 0x5B
va 1000 pte 0x21FCE85B pa 0x87F3A000 perm 0x5B
...
va 0xFFFFD000 pte 0x0 pa 0x0 perm 0x0
va 0xFFFFE000 pte 0x21FD80C7 pa 0x87F60000 perm 0xC7
va 0xFFFFF000 pte 0x20001C4B pa 0x80007000 perm 0x4B
```

对于每个输出中的页表条目 `print_pgtbl` , 解释它逻辑上包含的内容以及它的权限位. xv6 书籍中的图 3.4 可能有所帮助, 尽管请注意, 该图可能包含与这里检查的进程略有不同的页面集.

请注意, xv6 并不会在物理内存中连续放置虚拟页面.

> 这步好像就是个问答题

### Speed up system calls (easy)

一些操作系统 (例如 Linux) 通过在用户空间和内核之间共享一个只读区域的数据来加速某些系统调用 这种方法消除了系统调用时进入内核的开销 

在本实验中, 您将实现该优化, 为 `getpid()` 系统调用加速 

#### **任务要求**

1. **为每个进程映射一个只读页面**:
   - 映射地址为 `USYSCALL` (在 `memlayout.h` 中定义) 
   - 在该页面的起始位置, 存储一个 `struct usyscall` (定义在 `memlayout.h`), 并初始化为当前进程的 PID 

2. **用户态接口**:
   - 在用户态, 提供 `ugetpid()` 函数, 它会自动使用 `USYSCALL` 映射获取进程 PID 

3. **测试目标**:
   - 如果 `pgtbltest` 程序中的 `ugetpid` 测试通过, 则任务成功完成 

#### **提示**

- 选择合适的权限位, 使用户空间只能读取该页面 
- 页面生命周期中涉及多个操作, 可以参考 `kernel/proc.c` 中的 `trapframe` 处理逻辑 
- 思考:还有哪些系统调用可以利用该共享页面加速？请解释 

#### 检查点

```shell
xv6-labs-2024> ./grade-lab-pgtbl
== Test pgtbltest == (3.7s) 
== Test   pgtbltest: ugetpid == 
  pgtbltest: ugetpid: OK 
```

### Print a page table (easy)



### Use superpages (moderate)/(hard)

## Reference

- [xv6 book]
- [xv6 source code]
- [jyy slides](https://jyywiki.cn/OS/2022/slides/18.slides.html#/)