// This file contains a main function to display a sample output of the memory allocator functions
#include "header.h"

// The following test code was written by Professor Shamash
int main(int argc, char **argv)
{  
     unsigned int global_mem_size = 1024 * 1024;
     unsigned char *global_memory = malloc(global_mem_size);

     mem_init(global_memory, global_mem_size);
     print_stats("init");

     unsigned char *ptr_array[10];
     unsigned int sizes[] = {50, 20, 20, 20, 50, 0};

     for (int i = 0; sizes[i] != 0; i++) {
       char buf[1024];
       ptr_array[i] = my_malloc(sizes[i]);
       
       sprintf(buf, "after iteration %d size %d", i, sizes[i]);
       print_stats(buf);
     }

     my_free(ptr_array[1]);  print_stats("after free #1"); 

     my_free(ptr_array[3]);  print_stats("after free #3");

     my_free(ptr_array[2]);  print_stats("after free #2");

     my_free(ptr_array[0]);  print_stats("after free #0");

     my_free(ptr_array[4]);  print_stats("after free #4");


     // ** Unit Tests **
     printf("-------------------UNIT TESTS-----------------\n");
     printf("-- NOTE: Any error messages that follow are generated due to the tests. --\n");

    // TEST: Request negative bytes of memory to allocate 
    // Expected: Return NULL pointer and error message 
    mem_block *negative_bytes = my_malloc(0);

    if(negative_bytes == NULL)
    {
        printf("Negative bytes test passed!\n");
    }
    else
    {
        printf("Negative bytes test failed!\n");

    }

    // TEST: Request memory when out of available memory 
    // Expected: Return NULL pointer and error message 
    mem_block *out_of_memory = my_malloc(global_mem_size + 1); // request more than total available memory

    if(out_of_memory == NULL)
    {
        printf("Out of memory test passed!\n");
    }
    else
    {
        printf("Out of memory test failed!\n");
    }

    // 2 TESTS: 1. Call my_free() on a NULL pointer, 2. Memory stats do not change before and after this occurs
    // Expected: Error message "ERROR! Pointer does not exist!"
    print_stats("Memory Before:\n");

    mem_block *null_ptr = NULL;
    my_free(null_ptr);

    print_stats("Memory After:\n");

    // TEST that memory allocator properly allocates specified size
    int size = 10;
    my_malloc(size);
    print_stats("Allocate Size 10:\n");


  return 0;  
}

// To run the code in the terminal: (press 'Enter' after typing each command)
// Command 1: gcc main.c my_mem.c -o main
// Command 2: ./main