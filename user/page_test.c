#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int global_var = 0x12345678;

int main() {
    printf("\n=== Initial state ===\n");
    show_page_table(0, 0, 3);
    
    printf("\n\n=== After heap allocation (2 pages) ===\n");
    char *heap = sbrk(2*4096);
    show_page_table(0, 0, 3);
    
    printf("\n\n=== After stack variable creation ===\n");
    int stack_var = 0x87654321;
    show_page_table(0, 0, 3);
    
    printf("\n\n=== After clearing all A/D flags ===\n");
    clear_flags(0, 0, 3);
    show_page_table(0, 0, 3);
    
    printf("\n\n=== After large stack array creation ===\n");
    int stack_arr[1024];
    stack_arr[0] = 0;
    show_page_table(0, 0, 3);
    
    printf("\n\n=== After accessing global and stack ===\n");
    global_var++;
    stack_var++;
    stack_arr[0]++;
    show_page_table(0, 0, 3);
    
    printf("\n\n=== After heap access (show affected only) ===\n");
    heap[0] = 0xAA;
    volatile int val = heap[4096];
    (void)val;
    show_page_table(heap, 2*4096, 3);
    
    printf("\n\n=== After heap deallocation ===\n");
    sbrk(-2*4096);
    show_page_table(0, 0, 3);
    
    exit(0);
}