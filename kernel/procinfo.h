#ifndef PROCINFO_H
#define PROCINFO_H

#include "types.h"

enum procstate_info {
    PSLEEPING,
    PRUNNABLE,
    PRUNNING,
    PZOMBIE,
};

struct procinfo {
    int pid;
    char name[16];
    enum procstate_info state;
    int ppid;
    char pname[16];
};

#endif
