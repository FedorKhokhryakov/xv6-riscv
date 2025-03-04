#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main() {
    int pid = fork();

    if (pid < 0) {
        printf("fork fail\n");
        exit(1);
    }

    else if (pid == 0) { // child
        sleep(10);
        exit(1);
    } 

    else { // parent
        printf("Parent pid: %d\nChild pid: %d\n", getpid(), pid);
        int status;
        wait(&status);
        printf("The process %d was completed with a code %d\n", pid, status);
        exit(0);
    }
    return 0;
}
