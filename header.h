// This file is the header file for the memmory allocator program. It contains function declarations 
// and macro definitions that are shared between source files.

// Include guard to ensure compiler processes header file contents only once
#ifndef  HEADER_FILE 
#define  HEADER_FILE

// ** System header files **
#include <stdio.h>   // standard I/O library header file
#include <stdlib.h>
#include <assert.h> // unit tests


// ** Structure for memory block (linked list) **
typedef struct mem_block_struct{
    int status;                      // free = 0, busy = 1
    int mem_size;                   // number of bytes of memory requested
    struct mem_block_struct *prev_block;   // pointer to previous block
    struct mem_block_struct *next_block;   // pointer to next block
} mem_block;


// ** Macro definitions for memory block structure **
//#define TOTAL_SIZE (1024 * 1024)         // size of memory pool in bytes (available virtual memory)
#define ONE_BLOCK_SIZE sizeof(mem_block)  // size of one memory node structure
static mem_block *head_block;             // head_block points to beginning of overall memory block


// ** Structure definition for memory statistics ** 
typedef struct{
    int num_blocks_used;
    int num_blocks_free;
    int smallest_block_free;
    int smallest_block_used;
    int largest_block_free;
    int largest_block_used;
} mem_stats_struct, *mem_stats_ptr;


// ** Function declarations for "my_mem.c" **
void mem_init(unsigned char *my_memory, unsigned int my_mem_size);
void *my_malloc(unsigned size);
void my_free(void *mem_pointer);
void mem_get_stats(mem_stats_ptr mem_stats_ptr);
void print_stats(char *prefix);


// ** Function declaration for "main.c" ** 
int main(int argc, char **argv);


#endif // End of include guard