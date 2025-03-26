#include "types.h"
#include "param.h"
#include "sleeplock.h"
#include "file.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"
#include "proc.h"

#define MUTEX_DEBUG 1

int
mutexalloc(struct file **f)
{
    *f = filealloc();
    if (!*f) {
#if MUTEX_DEBUG
        printf("mutexalloc: filealloc failed\n");
#endif
        return -1;
    }

    (*f)->mutex = (struct sleeplock *)kalloc();
    if (!(*f)->mutex) {
        fileclose(*f);
#if MUTEX_DEBUG
        printf("mutexalloc: kalloc failed\n");
#endif
        return -1;
    }

    initsleeplock((*f)->mutex, "mutex");
    (*f)->type = FD_MUTEX;
    (*f)->readable = 0;
    (*f)->writable = 0;

#if MUTEX_DEBUG
    printf("mutexalloc: success (pid=%d)\n", myproc()->pid);
#endif
    return 0;
}

int
mutexclose(struct file *f)
{
    if (!f || f->type != FD_MUTEX || !f->mutex) {
#if MUTEX_DEBUG
        printf("mutexclose: invalid mutex\n");
#endif
        return -1;
    }

    if (f->mutex->locked && f->mutex->pid == myproc()->pid) {
        releasesleep(f->mutex);
    }

    kfree((char *)f->mutex);
    f->mutex = 0;

#if MUTEX_DEBUG
    printf("mutexclose: success (pid=%d)\n", myproc()->pid);
#endif
    return 0;
}