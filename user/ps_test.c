#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "kernel/procinfo.h"

void test_insufficient_buffer() {
  struct procinfo proclist[1];
  int ret = ps_listinfo(proclist, 1);
  
  if (ret == -1) {
    printf("Test insufficient_buffer passed\n");
  } 
  else {
    printf("Test insufficient_buffer failed: expected -1, but %d\n", ret);
  }
}

void test_invalid_address() {
  struct procinfo *bad_ptr = (struct procinfo *)0xFFFFFFFF;
  int ret = ps_listinfo(bad_ptr, 1);

  if (ret == -2) {
    printf("Test invalid_address passed\n");
  } 
  else {
    printf("Test invalid_address failed: expected -2, but %d\n", ret);
  }
}

void test_ok() {
  int bufsize = 10;
  struct procinfo *proclist = malloc(sizeof(struct procinfo) * bufsize);
  int ret = ps_listinfo(proclist, bufsize);

  if (ret > 0) {
    printf("Test ok passed\n");
  } 
  else {
    printf("Test ok failed: expected positive number, but %d\n", ret);
  }

  free(proclist);
}

int main() {
  test_insufficient_buffer();
  test_invalid_address();
  test_ok();
  exit(0);
}
