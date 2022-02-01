// This file contains a basic implementation of a memory allocator that is similar to a doubly linked list.
// It includes the following functions: mem_init(), my_malloc(), my_free(), mem_get_stats() (and print_stats()).

#include "header.h" // include function declarations and structure definitions
     
// global variables for init 
unsigned int global_mem_size;   // size of memory pool in bytes (available virtual memory)
unsigned char *global_memory;   // pointer to total allocated memory pool

// global variables for memory stats (used and free counters)
int num_free;
int num_used;

// ** This routine is guaranteed to be called before any of the other routines, and can do     **
// ** whatever initialization is needed. The memory to be managed is passed into this routine. **
void mem_init(unsigned char *my_memory, unsigned int my_mem_size)
{
    global_memory = my_memory;  
    global_mem_size = my_mem_size;

    // free a block allocated by malloc
    if (head_block != NULL){
        free(head_block);
    }

    // initialize first block in available memory 
    head_block = (mem_block *)my_memory;
    head_block -> status = 0; // free = 0
    head_block -> mem_size = global_mem_size - ONE_BLOCK_SIZE; // memory block is stored in memory pool
    head_block -> next_block = NULL;                      
    head_block -> prev_block = NULL;
}

// ** This function is functionally equivalent to malloc(),                     **
// ** but allocates a block of memory from the memory pool passed to mem_init() **
void *my_malloc(unsigned size)
{
    // check if num of bytes requested is valid. Return NULL on failure.
    // size requested is too large
    if (size > global_mem_size) // size is too large
    {
        printf("ERROR! You have requested more virtual memory than total allocated memory!\n");
        return NULL;
    }

    // size requested is too small
    if (size <= 0)
    {
        printf("ERROR! Please request a number of bytes larger than zero!\n");
        return NULL;
    }

    // allocate num of bytes requested to the smallest available (free) memory block 
    mem_block *cur_block; 
    mem_block *smallest_free_block;
    unsigned smallest_free_size;

    // begin loop from first block in memory
    cur_block = head_block;

    if (cur_block == NULL)
    {
        printf("ERROR! Current block does not exist!\n");
        return NULL;
    }

    // initialize smallest available memory block
    smallest_free_block = (mem_block *) NULL; 
    smallest_free_size = global_mem_size + 1; // init with invalid size

    // traverse overall memory block and search for smallest available free block 
    // that is greater than or equal to number of bytes requested
    while (cur_block) 
    {
        // is the current memory block the smallest free block? 
        // If so, update values. Otherwise, continue to loop.
        if ( (cur_block -> mem_size >= (size + ONE_BLOCK_SIZE)) &&
             (cur_block -> mem_size <= smallest_free_size)      &&
             (cur_block -> status != 1))
            {
                smallest_free_block = cur_block;
                smallest_free_size = cur_block -> mem_size;
            }
        cur_block = cur_block -> next_block; // continue to loop, searching for smallest available free block.
    }

    // At this point, we have found the smallest available block of appropriate byte size.
    // Confirm that this block exists (not a dangling ptr).
    if (smallest_free_block != NULL){
        mem_block *new_block;

        // decrement size of smallest free block by num of requested bytes that were just allocated. 
        smallest_free_block -> mem_size = smallest_free_block -> mem_size - size - ONE_BLOCK_SIZE;

        // insert new block into allocated memory after current block + new block's size + allocated memory.
        new_block = (mem_block *)(((unsigned char *)smallest_free_block) + ONE_BLOCK_SIZE + smallest_free_block -> mem_size);
       
        new_block -> status = 1; // busy = 1
        new_block -> mem_size = size;
        new_block -> prev_block = smallest_free_block;
        new_block -> next_block = smallest_free_block -> next_block;
        
        // confirm that the next block exists and update smallest free block's next pointer 
        // and new block's prev pointer.
        if (smallest_free_block -> next_block != NULL){

            smallest_free_block -> next_block -> prev_block = new_block;
        }
        smallest_free_block -> next_block = new_block;

        // return pointer to allocated region of memory (after new block)
        return (void *)((unsigned char *) new_block + ONE_BLOCK_SIZE);
    }
    // if request fails (ex: ran out of memory), return NULL
    else
    {
        printf("ERROR! Check available memory!\n");
        return NULL;
    }
}

// ** This function is equivalent to free(),                  **
// ** but returns the memory to the pool passed to mem_init() ** 
void my_free(void *mem_pointer)
{
    // confirm parameter is a pointer to previously allocated region of memory
    if (mem_pointer == NULL)
    {
        printf("ERROR! Pointer does not exist!\n");
        return;
    }

    // point to current block in memory 
    mem_block *cur_mem_block = (mem_block *)((unsigned char *)mem_pointer - ONE_BLOCK_SIZE);

    // confirm that current memory block exists
    if (cur_mem_block == NULL)
    {
        printf("ERROR! Current memory block does not exist!\n");
        return;
    }

    cur_mem_block -> status = 0; // free = 0
    
    // check adjacent next block. If also free, merge current and next blocks.
    if (cur_mem_block -> next_block != NULL)
    {
        if (cur_mem_block -> next_block -> status != 1)
        {
            // add size of next block to update size of merged next block
            cur_mem_block -> mem_size += cur_mem_block -> next_block -> mem_size;
            cur_mem_block -> mem_size += ONE_BLOCK_SIZE;
            
            // delete next block and set current block to point to next's next block
            cur_mem_block -> next_block = cur_mem_block -> next_block -> next_block;

            // confirm that next's next block exists. If so, set next's next block to point to current block.
            if (cur_mem_block -> next_block != NULL) // (current block's next already points to next's next!)
            {
                cur_mem_block -> next_block -> prev_block = cur_mem_block;
            }
        }
    }

    // check prev block. If also free, merge current and prev blocks.
    // (Similar process as above, but this time for prev_block)
    if (cur_mem_block -> prev_block != NULL)
    {
        if (cur_mem_block -> prev_block -> status != 1)
        {
            // add size of freed block to previous block to update size 
            cur_mem_block -> prev_block -> mem_size += cur_mem_block -> mem_size;
            cur_mem_block -> prev_block -> mem_size += ONE_BLOCK_SIZE;
            
            // delete freed block and set prev block to point to next block
            cur_mem_block -> prev_block -> next_block = cur_mem_block -> next_block;
            
            // confirm that next block exists. If so, set next block to prev block.
            if (cur_mem_block -> next_block != NULL)
            {
                cur_mem_block -> next_block -> prev_block = cur_mem_block -> prev_block;
            }
        }
    }
}

// ** This function provides statistics about the current allocation of the memory pool **
void mem_get_stats(mem_stats_ptr mem_stats_ptr)
{
    // set current block to point to first block in memory
    mem_block *cur_block_stats; 
    cur_block_stats = head_block;

    // check that current block exists
     // return error if pointer to head block is NULL
    if (cur_block_stats == NULL) 
    {
        printf("ERROR! Current memory block does not exist!\n");
        return;
    }

    // initialized block variables to hold "smallest so far" or "largest so far" during loop
    mem_block *smallest_block_free = cur_block_stats;
    unsigned smallest_block_free_size = global_mem_size + 1; // smallest size is sure to be < total size of memory + 1

    mem_block *largest_block_free = cur_block_stats;
    unsigned largest_block_free_size = 0;                    // largest size is sure to be > 0 

    mem_block *smallest_block_used = NULL;
    unsigned smallest_block_used_size = global_mem_size + 1; 

    mem_block *largest_block_used = NULL;
    unsigned largest_block_used_size = 0;

    // initialize global variables (free and used counters)
    num_free = 0;
    num_used = 0;

    // loop through overall memory block and gather stats
    while (cur_block_stats)
    {
        // Check if current block is free or used and update counters accordingly
        if (cur_block_stats -> status == 0)       // free = 0
        {
            num_free += 1; // increment counter

            // is current block the smallest free block? If so, assign pointer and update stats.
            if (cur_block_stats -> mem_size <= smallest_block_free_size)
            {
                smallest_block_free = cur_block_stats;
                smallest_block_free_size = cur_block_stats -> mem_size;
            }

            // is current block the largest free block? 
            if (cur_block_stats -> mem_size >= largest_block_free_size)
            {
                largest_block_free = cur_block_stats;
                largest_block_free_size = cur_block_stats -> mem_size;
            }
        }

        else if (cur_block_stats -> status == 1) // used = 1
        { 
            num_used += 1;  // increment counter 

            // is current block the smallest used block? 
            if (cur_block_stats -> mem_size <= smallest_block_used_size)
            {
                smallest_block_used = cur_block_stats;
                smallest_block_used_size = cur_block_stats -> mem_size;
            }

            // is current block the largest used block? 
            if (cur_block_stats -> mem_size >= largest_block_used_size)
            {
                largest_block_used = cur_block_stats;
                largest_block_used_size = cur_block_stats -> mem_size;
            }
        }
        cur_block_stats = cur_block_stats -> next_block; // continue to loop, searching for smallest available used block.
    }

    // At this point, we have traversed every memory block in the total allocated memory.
    // Now, assign these stats to pointers to pass to print_stats().
    mem_stats_ptr -> num_blocks_free = num_free;
    mem_stats_ptr -> num_blocks_used = num_used;

    // acount for special cases
    if (num_free == 0) // memory is completely used
    {
        mem_stats_ptr -> smallest_block_free = 0;
    }
    else
    {
        mem_stats_ptr -> smallest_block_free = smallest_block_free_size;
    }

    if (num_used == 0) // memory is comoletely free 
    {
        mem_stats_ptr -> smallest_block_used = 0;
    }
    else
    {
        mem_stats_ptr -> smallest_block_used = smallest_block_used_size;
    }

    mem_stats_ptr -> largest_block_free  = largest_block_free_size;
    mem_stats_ptr -> largest_block_used =  largest_block_used_size;
}


// ** This function prints statistics about current allocation of the memory pool **
void print_stats(char *prefix)
{
    mem_stats_struct mem_stats;

    mem_get_stats(&mem_stats);
    printf("mem stats: %s: %d free blocks, %d used blocks, free blocks:"
           "smallest=%d largest=%d, used blocks: smallest=%d largest=%d\n",
           prefix,
           mem_stats.num_blocks_free,
           mem_stats.num_blocks_used,
           mem_stats.smallest_block_free,
           mem_stats.largest_block_free,
           mem_stats.smallest_block_used,
           mem_stats.largest_block_used);
}