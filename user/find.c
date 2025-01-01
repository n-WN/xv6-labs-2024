// find (moderate)
// Write a simple version of the UNIX find program for xv6: find all the files
// in a directory tree with a specific name. Your solution should be in the file
// user/find.c.

// Some hints:

// Look at user/ls.c to see how to read directories.
// Use recursion to allow find to descend into sub-directories.
// Don't recurse into "." and "..".
// Changes to the file system persist across runs of qemu; to get a clean file
// system run make clean and then make qemu. You'll need to use C strings. Have
// a look at K&R (the C book), for example Section 5.5. Note that == does not
// compare strings like in Python. Use strcmp() instead. Add the program to
// UPROGS in Makefile. Your solution should produce the following output (when
// the file system contains the files b, a/b and a/aa/b):

//     $ make qemu
//     ...
//     init: starting sh
//     $ echo > b
//     $ mkdir a
//     $ echo > a/b
//     $ mkdir a/aa
//     $ echo > a/aa/b
//     $ find . b
//     ./b
//     ./a/b
//     ./a/aa/b
//     $

// Run make grade to see what our tests think.
#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "user/user.h"

#define BUF_SIZE 512
#define ERROR_PREFIX "find: "

// 错误处理函数
void error(char *msg, char *path) {
    fprintf(2, ERROR_PREFIX "%s %s\n", msg, path);
}

// 从路径中提取文件名
char* extract_filename(char *path) {
    char *p = path + strlen(path);
    while (p >= path && *p != '/') {
        p--;
    }
    return p + 1;
}

// 构建完整路径
int build_path(char *base, char *name, char *full_path) {
    int len = strlen(base);
    if (len + 1 + strlen(name) + 1 > BUF_SIZE) {
        error("path too long", base);
        return -1;
    }
    strcpy(full_path, base);
    full_path[len] = '/';
    strcpy(full_path + len + 1, name);
    return 0;
}

// 检查是否为特殊目录
int is_special_dir(char *name) {
    return strcmp(name, ".") == 0 || strcmp(name, "..") == 0;
}

// 递归查找文件
void find(char *path, char *target) {
    int fd;
    struct stat st;
    struct dirent de;
    char new_path[BUF_SIZE];

    if ((fd = open(path, O_RDONLY)) < 0) {
        error("cannot open", path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        error("cannot stat", path);
        close(fd);
        return;
    }

    switch (st.type) {
        case T_FILE:
            if (strcmp(extract_filename(path), target) == 0) {
                printf("%s\n", path);
            }
            break;

        case T_DIR:
            while (read(fd, &de, sizeof(de)) == sizeof(de)) {
                if (de.inum == 0 || is_special_dir(de.name)) {
                    continue;
                }
                
                if (build_path(path, de.name, new_path) < 0) {
                    break;
                }
                
                if (stat(new_path, &st) < 0) {
                    error("cannot stat", new_path);
                    continue;
                }
                
                find(new_path, target);
            }
            break;
    }
    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        error("usage: find <path> <filename>", "");
        exit(1);
    }

    find(argv[1], argv[2]);
    return 0;
}