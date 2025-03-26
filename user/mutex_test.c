#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void print_args(int use_mutex, int mutex_fd, int argc, char *argv[]) {
    char ch[2] = {0, 0}; 
    
    for (int i = 1; i < argc; i++) {
        for (int j = 0; argv[i][j]; j++) {
            if (use_mutex) mutex_lock(mutex_fd);
            
            ch[0] = argv[i][j];
            printf("%d: arg %d, char '%s'\n", getpid(), i, ch);
            
            if (use_mutex) mutex_unlock(mutex_fd);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s text_to_print\n", argv[0]);
        exit(1);
    }

    int mx = mutex();
    if (mx < 0) {
        printf("Mutex creation failed\n");
        exit(1);
    }

    printf("=== Without mutex ===\n");
    if (fork() == 0) {
        print_args(0, 0, argc, argv);
        exit(0);
    }
    print_args(0, 0, argc, argv);
    wait(0);

    printf("\n=== With mutex ===\n");
    if (fork() == 0) {
        print_args(1, mx, argc, argv);
        exit(0);
    }
    print_args(1, mx, argc, argv);
    wait(0);

    close(mx);
    exit(0);
}