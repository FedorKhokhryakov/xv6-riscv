#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void test_read_write() {
    int m = mutex();
    printf("Testing read/write on mutex...\n");
    
    char buf[1];
    if (write(m, buf, 1) >= 0) {
        printf("ERROR: write to mutex succeeded!\n");
    } else {
        printf("OK: write to mutex correctly failed\n");
    }
    
    if (read(m, buf, 1) >= 0) {
        printf("ERROR: read from mutex succeeded!\n");
    } else {
        printf("OK: read from mutex correctly failed\n");
    }
    
    close(m);
}

void test_close_locked() {
    int m = mutex();
    printf("\nTesting close of locked mutex...\n");
    
    mutex_lock(m);
    
    int pid = fork();
    if (pid == 0) {
        sleep(10);
        if (close(m) >= 0) {
            printf("ERROR: child closed parent's mutex!\n");
        } else {
            printf("OK: child failed to close parent's mutex\n");
        }
        exit(0);
    } else {
        wait(0);
        mutex_unlock(m);
        close(m);
    }
}

void test_process_exit() {
    printf("\nTesting process exit with locked mutex...\n");
    
    int pid = fork();
    if (pid == 0) {
        int m = mutex();
        mutex_lock(m);
        printf("Child locked mutex and exiting...\n");
        exit(0);
    } else {
        wait(0);
        printf("OK: child exited with locked mutex\n");
    }
}

void test_unlock_foreign() {
    printf("\nTesting unlocking foreign mutex...\n");
    
    int m = mutex();
    mutex_lock(m);
    
    int pid = fork();
    if (pid == 0) {
        if (mutex_unlock(m) >= 0) {
            printf("ERROR: child unlocked parent's mutex!\n");
        } else {
            printf("OK: child failed to unlock parent's mutex\n");
        }
        exit(0);
    } else {
        wait(0);
        mutex_unlock(m);
        close(m);
    }
}

int main() {
    test_read_write();
    test_close_locked();
    test_process_exit();
    test_unlock_foreign();
    
    printf("\nAll tests completed\n");
    exit(0);
}