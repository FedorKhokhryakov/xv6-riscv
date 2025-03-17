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
		if (kill(pid) < 0) {
			fprintf(2, "Error kill\n");
		}
        int status;
        int wpid = wait(&status);
        if (wpid < 0) {
            fprintf(2, "wait failed\n");
            exit(1);
        }
        printf("The process %d was stopped with a code %d\n", wpid, status & 0xFF);
        exit(0);
    }
    return 0;
}
