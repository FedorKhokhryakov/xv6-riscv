#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "proc.h"

#define PSEUDO_NULL       0
#define PSEUDO_ZERO       1
#define PSEUDO_URANDOM    2
#define PSEUDO_NULLSTAT   3

#define ZERO_BUF_SIZE 128
static char zerobuf[ZERO_BUF_SIZE] = {0};

struct nullstat_counter {
    uint64 total_bytes;
    struct spinlock lock;
} ns_counter;

uint64 pseudo_rand_seed = 1;
struct spinlock rand_lock;

static uint64 lcg(uint seed) {
    return (515487153 * seed + 981641ULL) % 2286661337ULL;
}

int pseudodev_read(short minor, int user_dst, uint64 dst, int len) {
    char byte;
    switch (minor) {
        case PSEUDO_NULL:
            return 0;

        case PSEUDO_ZERO:
            int remaining = len;
            uint64 off = 0;
            while (remaining > 0) {
                int chunk = remaining > ZERO_BUF_SIZE ? ZERO_BUF_SIZE : remaining;
                if (either_copyout(user_dst, dst + off, zerobuf, chunk) < 0)
                    return -1;
                off += chunk;
                remaining -= chunk;
            }
            return len;

        case PSEUDO_URANDOM:
            for (int i = 0; i < len; ++i) {
                acquire(&rand_lock);
                pseudo_rand_seed = lcg(pseudo_rand_seed);
                byte = pseudo_rand_seed & 0xFF;
                release(&rand_lock);
                if (either_copyout(user_dst, dst + i, &byte, 1) < 0)
                    return -1;
            }
            return len;

        case PSEUDO_NULLSTAT:
            if (len != sizeof(uint64))
                return -1;
            acquire(&ns_counter.lock);
            uint64 total = ns_counter.total_bytes;
            release(&ns_counter.lock);
            if (either_copyout(user_dst, dst, &total, len) < 0)
                return -1;
            return len;

        default:
            return -1;
    }
}

int pseudodev_write(short minor, int user_src, uint64 src, int len) {
    switch (minor) {
        case PSEUDO_NULL:
            return len;

        case PSEUDO_ZERO:
            return -1;

        case PSEUDO_URANDOM:
            if (len != sizeof(uint64))
                return -1;
            uint64 new_seed;
            if (either_copyin(&new_seed, user_src, src, len) < 0)
                return -1;
            acquire(&rand_lock);
            pseudo_rand_seed = new_seed ? new_seed : 1;
            release(&rand_lock);
            return len;

        case PSEUDO_NULLSTAT:
            acquire(&ns_counter.lock);
            ns_counter.total_bytes += len;
            release(&ns_counter.lock);
            return len;

        default:
            return -1;
    }
}

void pseudodev_init(void) {
    initlock(&rand_lock, "randseed");
    initlock(&ns_counter.lock, "nullstat");
    devsw[DEV].read = pseudodev_read;
    devsw[DEV].write = pseudodev_write;
}
