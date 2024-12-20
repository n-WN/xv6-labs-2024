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
    int time;
    // If the user forgets to pass an argument, sleep should print an error
    // message.
    char *err_info = "Usage: sleep {time}\n";
    if (argc != 2) {
        write(1, err_info, strlen(err_info));
        
        exit(0);
    }

    // The command-line argument is passed as a string; you can convert it to an
    // integer using atoi (see user/ulib.c).
    time = atoi(argv[1]);

    sleep(time); // system call

    exit(0); // sleep's main should call exit(0) when it is done.
}