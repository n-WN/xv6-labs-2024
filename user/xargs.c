// xargs (moderate)
// Write a simple version of the UNIX xargs program for xv6: its arguments
// describe a command to run, it reads lines from the standard input, and it
// runs the command for each line, appending the line to the command's
// arguments. Your solution should be in the file user/xargs.c.

// The following example illustrates xarg's behavior:
//     $ echo hello too | xargs echo bye
//     bye hello too
//     $

// Note that the command here is "echo bye" and the additional arguments are
// "hello too", making the command "echo bye hello too", which outputs "bye
// hello too". Please note that xargs on UNIX makes an optimization where it
// will feed more than one argument to the command at a time. We don't expect
// you to make this optimization. To make xargs on UNIX behave the way we want
// it to for this lab, please run it with the -n option set to 1. For instance

//     $ (echo 1 ; echo 2) | xargs -n 1 echo
//     1
//     2
//     $

// Some hints:

// Use fork and exec to invoke the command on each line of input. Use wait in
// the parent to wait for the child to complete the command. To read individual
// lines of input, read a character at a time until a newline ('\n') appears.
// kernel/param.h declares MAXARG, which may be useful if you need to declare an
// argv array. Add the program to UPROGS in Makefile. Changes to the file system
// persist across runs of qemu; to get a clean file system run make clean and
// then make qemu. xargs, find, and grep combine well:

//   $ find . b | xargs grep hello

// will run "grep hello" on each file named b in the directories below ".".
// To test your solution for xargs, run the shell script xargstest.sh. Your
// solution should produce the following output:

//   $ make qemu
//   ...
//   init: starting sh
//   $ sh < xargstest.sh
//   $ $ $ $ $ $ hello
//   hello
//   hello
//   $ $

// You may have to go back and fix bugs in your find program. The output has
// many $ because the xv6 shell doesn't realize it is processing commands from a
// file instead of from the console, and prints a $ for each command in the
// file.

#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

#define STDIN 0
#define BUF_SIZE 512
#define ERROR_PREFIX "xargs: "

// 错误处理函数
void error(char *msg) {
    fprintf(2, ERROR_PREFIX "%s\n", msg);
    exit(1);
}

// 读取一行输入
int read_line(char *buf, int max_size) {
    int i = 0;
    char c;

    while (i < max_size - 1) {
        if (read(STDIN, &c, 1) != 1) {
            return i > 0 ? i : -1;
        }
        if (c == '\n') {
            break;
        }
        buf[i++] = c;
    }
    buf[i] = '\0';
    return i;
}

// 执行命令
void execute_command(char **argv, int argc, char *line) {
    char *new_argv[MAXARG];
    int new_argc = 0;

    // 复制原始参数
    for (int i = 1; i < argc; i++) {
        new_argv[new_argc++] = argv[i];
    }

    // 添加新参数
    if (line[0] != '\0') {
        new_argv[new_argc++] = line;
    }
    new_argv[new_argc] = 0;

    int pid = fork();
    if (pid < 0) {
        error("fork failed");
    }
    if (pid == 0) {
        exec(argv[1], new_argv);
        error("exec failed");
    }
    wait(0);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        error("usage: xargs command [args...]");
    }

    char buf[BUF_SIZE];
    while (1) {
        int n = read_line(buf, BUF_SIZE);
        if (n < 0)
            break;
        if (n > 0) {
            execute_command(argv, argc, buf);
        }
    }

    exit(0);
}