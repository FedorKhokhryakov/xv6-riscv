#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "file.h"
#include "memlayout.h"
#include "proc.h"
#include "sleeplock.h"
#include "fcntl.h"

#define MUTEX_DEBUG 1

uint64 sys_mutex(void) {
    struct file *f;
    
    if (mutexalloc(&f) < 0) {
#if MUTEX_DEBUG
        printf("sys_mutex: allocation failed\n");
#endif
        return -1;
    }
    
    int fd = fdalloc(f);
    if (fd < 0) {
#if MUTEX_DEBUG
        printf("sys_mutex: fd allocation failed\n");
#endif
        fileclose(f);
        return -1;
    }
    
#if MUTEX_DEBUG
    printf("sys_mutex: created fd %d\n", fd);
#endif
    return fd;
}

uint64 sys_mutex_lock(void) {
    struct file *f;
    int fd;
    
    if (argfd(0, &fd, &f) < 0) {
#if MUTEX_DEBUG
        printf("sys_mutex_lock: invalid fd\n");
#endif
        return -1;
    }
    
    if (f->type != FD_MUTEX) {
#if MUTEX_DEBUG
        printf("sys_mutex_lock: not a mutex fd\n");
#endif
        return -1;
    }
    
    acquiresleep(f->mutex);
#if MUTEX_DEBUG
    printf("sys_mutex_lock: pid %d acquired mutex %d\n", myproc()->pid, fd);
#endif
    return 0;
}

uint64 sys_mutex_unlock(void) {
    struct file *f;
    int fd;
    
    if (argfd(0, &fd, &f) < 0) {
#if MUTEX_DEBUG
        printf("sys_mutex_unlock: invalid fd\n");
#endif
        return -1;
    }
    
    if (f->type != FD_MUTEX) {
#if MUTEX_DEBUG
        printf("sys_mutex_unlock: not a mutex fd\n");
#endif
        return -1;
    }
    
    if (!holdingsleep(f->mutex)) {
#if MUTEX_DEBUG
        printf("sys_mutex_unlock: mutex not held by caller\n");
#endif
        return -1;
    }
    
    releasesleep(f->mutex);
#if MUTEX_DEBUG
    printf("sys_mutex_unlock: pid %d released mutex %d\n", myproc()->pid, fd);
#endif
    return 0;
}