#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "kernel/procinfo.h"

int main() {
  int ret = 0;
  int limit = 2;
  struct procinfo *proclist;

  while (1) {
    proclist = malloc(sizeof(struct procinfo) * limit);
    if (!proclist) {
        fprintf(2, "Memory allocation error\n");
        exit(1);
    }

    ret = ps_listinfo(proclist, limit);
    if (ret == -1) {
        fprintf(2, "Buffer is too small.\n");
        free(proclist);
        limit *= 2;
        continue;
    } 
    else if (ret == -2) {
        fprintf(2, "Incorrect buffer address\n");
        free(proclist);
        exit(1);
    } 
    else if (ret < 0) {
        fprintf(2, "Other error\n");
        free(proclist);
        exit(1);
    }
    break;
    }

  for (int i = 0; i < ret; i++) {
    const char *state_str;
    switch (proclist[i].state) {
        case PSLEEPING: state_str = "Sleeping"; break;
        case PRUNNABLE: state_str = "Runnable"; break;
        case PRUNNING:  state_str = "Running";  break;
        case PZOMBIE:   state_str = "Zombie";   break;
        default:        state_str = "Unknown";  break;
    }

    printf("PID: %d\n", proclist[i].pid);
    printf("Name: %s\n", proclist[i].name);
    printf("State: %s\n", state_str);
    printf("Parent PID: %d\n", proclist[i].ppid);
    if (proclist[i].pname[0] != '\0') {
        printf("Parent Name: %s\n", proclist[i].pname);
    } 
    else {
        printf("Parent Name: -\n");
    }
    printf("\n");
  }

  free(proclist);
  exit(0);
}
