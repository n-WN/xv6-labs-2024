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