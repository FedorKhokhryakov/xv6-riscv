#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int global_var = 0x12345678;

int main() {
    printf("\n=== Initial state (all pages) ===\n");
    show_page_table(0, 0, SHOW_PT_ALL);
    
    printf("\n\n=== Only pages with A/D flags set ===\n");
    show_page_table(0, 0, SHOW_PT_BOTH);
    
    printf("\n\n=== After heap allocation (2 pages) ===\n");
    char *heap = sbrk(2*4096);
    show_page_table(0, 0, SHOW_PT_ALL);
    
    printf("\n\n=== Only accessed heap pages ===\n");
    show_page_table(heap, 2*4096, SHOW_PT_ACCESSED);
    
    printf("\n\n=== After stack variable creation ===\n");
    int stack_var = 0x87654321;
    show_page_table(0, 0, SHOW_PT_ALL);
    
    printf("\n\n=== After clearing all A/D flags ===\n");
    clear_flags(0, 0, CLEAR_PT_BOTH);
    show_page_table(0, 0, SHOW_PT_BOTH);
    
    printf("\n\n=== After large stack array creation ===\n");
    int stack_arr[1024];
    stack_arr[0] = 0;
    show_page_table(0, 0, SHOW_PT_ALL);
    
    printf("\n\n=== Only dirty pages after access ===\n");
    global_var++;
    stack_var++;
    stack_arr[0]++;
    show_page_table(0, 0, SHOW_PT_DIRTY);
    
    printf("\n\n=== After heap access (show affected only) ===\n");
    heap[0] = 0xAA;
    volatile int val = heap[4096];
    (void)val;
    show_page_table(heap, 2*4096, SHOW_PT_BOTH);
    
    printf("\n\n=== Clear only dirty flag on heap ===\n");
    clear_flags(heap, 4096, CLEAR_PT_DIRTY);
    show_page_table(heap, 2*4096, SHOW_PT_BOTH);
    
    printf("\n\n=== After heap deallocation ===\n");
    sbrk(-2*4096);
    show_page_table(0, 0, SHOW_PT_ALL);
    
    exit(0);
}