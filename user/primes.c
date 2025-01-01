#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void sieve_non_recursive(int start, int end) {
    int current_pipe[2];  // 当前处理的管道

    // 初始化管道，用于传递初始数据
    pipe(current_pipe);
    if (fork() == 0) {
        close(current_pipe[1]);  // 子进程关闭写端
        int p;

        while (read(current_pipe[0], &p, sizeof(int)) > 0) {
            printf("prime %d\n", p);  // 输出素数

            // 创建下一个管道
            int next_pipe[2];
            pipe(next_pipe);

            if (fork() == 0) {
                // 下一层筛选
                close(current_pipe[0]);  // 关闭父级管道
                close(next_pipe[1]);     // 关闭写端
                current_pipe[0] = next_pipe[0];  // 移动到下一个管道
                continue;  // 进入下一循环
            } else {
                close(next_pipe[0]);  // 关闭读端
                int n;
                while (read(current_pipe[0], &n, sizeof(int)) > 0) {
                    if (n % p != 0) {
                        write(next_pipe[1], &n, sizeof(int));
                    }
                }
                close(current_pipe[0]);  // 关闭当前读端
                close(next_pipe[1]);     // 关闭写端
                wait(0);  // 等待子进程
                exit(0);  // 父进程退出
            }
        }
        close(current_pipe[0]);  // 关闭最终管道的读端
        exit(0);
    } else {
        close(current_pipe[0]);  // 父进程关闭读端

        // 生成 2 到 end 的数字并写入管道
        for (int i = start; i <= end; i++) {
            write(current_pipe[1], &i, sizeof(int));
        }
        close(current_pipe[1]);  // 写入完成后关闭写端
        wait(0);  // 等待子进程
    }
}

int main() {
    sieve_non_recursive(2, 269);
    exit(0);
}
