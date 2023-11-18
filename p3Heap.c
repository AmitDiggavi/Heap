///////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020-2023 Deb Deppeler based on work by Jim Skrentny
// Posting or sharing this file is prohibited, including any changes/additions.
// Used by permission FALL 2023, CS354-deppeler
//
///////////////////////////////////////////////////////////////////////////////
//// Main File:        p3.Heap.c
//// This File:        p3.Heap.c
//// Other Files:      N/A
//// Semester:         CS 354 Lecture 001? Fall 2023
//// Grade Group:      gg 14 (See canvas.wisc.edu/groups for your gg#)
//// Instructor:       deppeler
//// 
//// Author:           Amit Diggavi
//// Email:            diggavi
//// CS Login:         diggavi
////
/////////////////////////////  WORK LOG  //////////////////////////////
////  Document your work sessions on your copy http://tiny.cc/work-log
////  Download and submit a pdf of your work log for each project.
///////////////////////////// OTHER SOURCES OF HELP ////////////////////////////// 
//// Persons:          Identify persons by name, relationship to you, and email.
////                   Describe in detail the the ideas and help they provided.
////
//// Online sources:   avoid web searches to solve your problems, but if you do
////                   search, be sure to include Web URLs and description of 
////                   of any information you find.
//// 
//// AI chats:         save a transcript and submit with project.
////////////////////////////// 80 columns wide ///////////////////////////////////
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include "p3Heap.h"

/*
 * This structure serves as the header for each allocated and free block.
 * It also serves as the footer for each free block.
 */
typedef struct blockHeader {           

	/*
	 * 1) The size of each heap block must be a multiple of 8
	 * 2) heap blocks have blockHeaders that contain size and status bits
	 * 3) free heap block contain a footer, but we can use the blockHeader 
	 *.
	 * All heap blocks have a blockHeader with size and status
	 * Free heap blocks have a blockHeader as its footer with size only
	 *
	 * Status is stored using the two least significant bits.
	 *   Bit0 => least significant bit, last bit
	 *   Bit0 == 0 => free block
	 *   Bit0 == 1 => allocated block
	 *
	 *   Bit1 => second last bit 
	 *   Bit1 == 0 => previous block is free
	 *   Bit1 == 1 => previous block is allocated
	 * 
	 * Start Heap: 
	 *  The blockHeader for the first block of the heap is after skip 4 bytes.
	 *  This ensures alignment requirements can be met.
	 * 
	 * End Mark: 
	 *  The end of the available memory is indicated using a size_status of 1.
	 * 
	 * Examples:
	 * 
	 * 1. Allocated block of size 24 bytes:
	 *    Allocated Block Header:
	 *      If the previous block is free      p-bit=0 size_status would be 25
	 *      If the previous block is allocated p-bit=1 size_status would be 27
	 * 
	 * 2. Free block of size 24 bytes:
	 *    Free Block Header:
	 *      If the previous block is free      p-bit=0 size_status would be 24
	 *      If the previous block is allocated p-bit=1 size_status would be 26
	 *    Free Block Footer:
	 *      size_status should be 24
	 */
	int size_status;

} blockHeader;         

/* Global variable - DO NOT CHANGE NAME or TYPE. 
 * It must point to the first block in the heap and is set by init_heap()
 * i.e., the block at the lowest address.
 */
blockHeader *heap_start = NULL;     

/* Size of heap allocation padded to round to nearest page size.
*/
int alloc_size;

/*
 * Additional global variables may be added as needed below
 * TODO: add global variables needed by your function
 */
int int_max = 2147483647; // INT_MAX value



/* 
 * Function for allocating 'size' bytes of heap memory.
 * Argument size: requested size for the payload
 * Returns address of allocated block (payload) on success.
 * Returns NULL on failure.
 *
 * This function must:
 * - Check size - Return NULL if size < 1 
 * - Determine block size rounding up to a multiple of 8 
 *   and possibly adding padding as a result.
 *
 * - Use BEST-FIT PLACEMENT POLICY to chose a free block
 *
 * - If the BEST-FIT block that is found is exact size match
 *   - 1. Update all heap blocks as needed for any affected blocks
 *   - 2. Return the address of the allocated block payload
 *
 * - If the BEST-FIT block that is found is large enough to split 
 *   - 1. SPLIT the free block into two valid heap blocks:
 *         1. an allocated block
 *         2. a free block
 *         NOTE: both blocks must meet heap block requirements 
 *       - Update all heap block header(s) and footer(s) 
 *              as needed for any affected blocks.
 *   - 2. Return the address of the allocated block payload
 *
 *   Return if NULL unable to find and allocate block for required size
 *
 * Note: payload address that is returned is NOT the address of the
 *       block header.  It is the address of the start of the 
 *       available memory for the requesterr.
 *
 * Tips: Be careful with pointer arithmetic and scale factors.
 */
void* balloc(int size) {     
	//TODO: Your code goes in here.


	if(size < 1) // size can't be less than 1
	{
		return NULL;
	}

	int pad;
	int total_size = 0;

	size += 4; // 4 bytes for header. for example if size is 8, we do 8+4 = 12(not a multiple of 8 so + 4) 



	blockHeader *hp_start = heap_start; // blockHeader 

	//	int size_split = hp_start -> size_status;

	int size_status = hp_start -> size_status; // struct size set to a local var


	if(size % 8 != 0) // see if the size is a multiple of 8
	{
		pad = 8 - (size % 8); // doing this will always get us a multiple of 8 
		
		total_size = size + pad; // padding given to total_size. 
		
		size_status = total_size; //size_status is given total_size 


	}
	else
	{
		total_size = size; // just to make sure total_size will be the arguement size if it a multiple of 8 already	
		
		size_status = total_size; // size_status is given total_size

	}

	size_status += 1; // setting the a-bit


	//	blockHeader *endmark;
	//	endmark -> size_status = 1;


	//	blockHeader *footer;

	blockHeader *current = hp_start; // creating another one for the while loop
	blockHeader *best_fit = NULL; // keeping track
	blockHeader *next;	// for the next block 

	//int free_size; // used later 

//	int best_size = total_size + sizeof(blockHeader) + ((8  - total_size) % 8); // making space for the header, footer, and alignment 
	int best_fit_size = int_max;

	while(current -> size_status != 1) // until endmark, i could not use my endmark vairable. 
	{
	//	printf("current->size_status=%d\n",current->size_status);
	//	printf("End %d \n", (current+4088/sizeof(blockHeader))->size_status);

		int check_bits;

		if(current -> size_status % 2 != 0)
		{
			check_bits = 1; // means allocated
		}
		else
		{
			check_bits = 0; // means free
		}
		
		int curr_size = current -> size_status & ~7; // getting the actual size, by removing the status bit.  

		if(check_bits == 0 && curr_size >= total_size) //this is if free and if large enough to fit. 

		{

			if(best_fit_size >= curr_size) // if best_fit is large enough to fit
			{ 
				best_fit = current; // best fit is current
				best_fit_size = curr_size;
			}

			// free_size = current -> size_status; // getting the current size status before it is changed 
							    // so I can subtract it from the total size to get the remaining size later	

			//current -> size_status = total_size + 2 + 1; // setting the size_status for current to total size plus the p and a-bit



			/*next = (blockHeader*)((char*)current + total_size); //moving to next block after it is allocates. was curr_size

			if(next -> size_status != 1) // if next is not endmark
			{
				next -> size_status += 2; // since we moved forward, we make last block allocated
				
				next -> size_status = free_size - total_size; // this is the free block so it gets the remaining size
				//next -> size_status += 2; // setting p-bit of next block
			} */

			
			
		

			if(curr_size == total_size) // if exact match, return it
			{
				current -> size_status = total_size + 2 + 1;
				return(void*)current + 4;
			}

		}




			
	//	printf("curr_size=%d \n",curr_size);

		current = (blockHeader*)((char*)current + curr_size);
	}

	
	if(best_fit==NULL)
	{
		return NULL;
	}
	else
	{
		int b_size = (best_fit -> size_status) - (best_fit -> size_status % 8); // get actual size, without allocation bit. 


		int temp_size = b_size - total_size;  

		best_fit -> size_status = total_size + 2 + 1; // making sure the best fit block is allocated 

		blockHeader *extra = (blockHeader*)((void*) best_fit + total_size); // header for the block after the best fit allocated block

		extra -> size_status = temp_size + 2; // makes extra's p-bit show that it is allocated. 

		//blockHeader *footer = (blockHeader*)((void*)extra + temp_size - 4); // this gets to the location where the footer for extra should be placed. 

		//footer -> size_status = temp_size; // setting the footer

		return (void*)best_fit + 4; // return the best_fit block	
	}

	next = (blockHeader*)((void*)best_fit + total_size);
	
	if((void*)heap_start + alloc_size > (void*)next)
	{
		next -> size_status += 2; //setting next p-bit
	}
	
	


	return NULL; 
}	


/* 
 * Function for freeing up a previously allocated block.
 * Argument ptr: address of the block to be freed up.
 * Returns 0 on success.
 * Returns -1 on failure.
 * This function should:
 * - Return -1 if ptr is NULL.
 * - Return -1 if ptr is not a multiple of 8.
 * - Return -1 if ptr is outside of the heap space.
 * - Return -1 if ptr block is already freed.
 * - Update header(s) and footer as needed.
 *
 * If free results in two or more adjacent free blocks,
 * they will be immediately coalesced into one larger free block.
 * so free blocks require a footer (blockHeader works) to store the size
 *
 * TIP: work on getting immediate coalescing to work after your code 
 *      can pass the tests in partA and partB of tests/ directory.
 *      Submit code that passes partA and partB to Canvas before continuing.
 */                    
int bfree(void *ptr) {    
	//TODO: Your code goes in here.
	

	blockHeader *current = NULL;
//	int total_size = 0;
//	blockHeader *ftr = NULL;
	blockHeader *next;
	current = ptr - 4; // getting the current block

	if(ptr == NULL) // if NULL
	{
		return -1;
	}

	if((int)ptr % 8 != 0) // if multiple of 8( typecasted)
	{
		return -1;
	}

	if(ptr < (void*)heap_start || ptr > (void*)heap_start + alloc_size ) // if out of heap space
	{
		return -1;
	}

	if(current -> size_status % 2 == 0)
	{
		return -1; // a-bit is 0 so already freed. 
	}

	
	current -> size_status -= 1; // freeing current block
	next = (blockHeader*)((char*)current + ((current -> size_status) - (current-> size_status % 8)));
	
	blockHeader *next2;

	if(next -> size_status && 1)
	{
		next -> size_status -= 2; // unsetting p-bit
	}

	
	// implement coalescing 
	// -8 changed to ~7	
	if(current != heap_start && (current -> size_status & 2) == 0) // if prev free
	{
		blockHeader *footer_prev = current - 1; 
		
		//printf("%d\n", footer_prev -> size_status);

		blockHeader *prev = (void*)current - (footer_prev->size_status - (footer_prev -> size_status % 8)); // to get the previous block

		//total_size = (current->size_status - (current->size_status % 8)) + (prev->size_status - (prev->size_status % 8));

		prev -> size_status += current->size_status & ~7; // merge	

		current = prev;
	}

	next2 = (blockHeader*)((char*)current + (current-> size_status) - (current -> size_status % 8)); // get to next block
	
	if( (void*)next2 < (void*)heap_start + alloc_size)
	{	
		if(next2 -> size_status % 2 == 0) // if next free
		{
			//	total_size = (current->size_status - (current->size_status % 8)) + (next2->size_status - (next2->size_status % 8));

			current -> size_status += next2 -> size_status & ~7; //total_size + 2; // merge
				
			
			blockHeader *extra = (blockHeader*)((void*)current + (current -> size_status) - (current -> size_status % 8)); // the resulting block after merging 
			
			if(((void*)extra < (void*)heap_start + alloc_size ))
			{
				extra -> size_status += 2; // its p-bit needs to be set. 
			}
		}
	}

	blockHeader *ftr = (blockHeader*)((char*)current + (current->size_status & ~7) - 4); // updating footer. 
	ftr -> size_status = (current -> size_status & ~7);
	
	return 0;
} 


/* 
 * Initializes the memory allocator.
 * Called ONLY once by a program.
 * Argument sizeOfRegion: the size of the heap space to be allocated.
 * Returns 0 on success.
 * Returns -1 on failure.
 */                    
int init_heap(int sizeOfRegion) {    

	static int allocated_once = 0; //prevent multiple myInit calls

	int   pagesize; // page size
	int   padsize;  // size of padding when heap size not a multiple of page size
	void* mmap_ptr; // pointer to memory mapped area
	int   fd;

	blockHeader* end_mark;

	if (0 != allocated_once) {
		fprintf(stderr, 
				"Error:mem.c: InitHeap has allocated space during a previous call\n");
		return -1;
	}

	if (sizeOfRegion <= 0) {
		fprintf(stderr, "Error:mem.c: Requested block size is not positive\n");
		return -1;
	}

	// Get the pagesize from O.S. 
	pagesize = getpagesize();

	// Calculate padsize as the padding required to round up sizeOfRegion 
	// to a multiple of pagesize
	padsize = sizeOfRegion % pagesize;
	padsize = (pagesize - padsize) % pagesize;

	alloc_size = sizeOfRegion + padsize;

	// Using mmap to allocate memory
	fd = open("/dev/zero", O_RDWR);
	if (-1 == fd) {
		fprintf(stderr, "Error:mem.c: Cannot open /dev/zero\n");
		return -1;
	}
	mmap_ptr = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	if (MAP_FAILED == mmap_ptr) {
		fprintf(stderr, "Error:mem.c: mmap cannot allocate space\n");
		allocated_once = 0;
		return -1;
	}

	allocated_once = 1;

	// for double word alignment and end mark
	alloc_size -= 8;

	// Initially there is only one big free block in the heap.
	// Skip first 4 bytes for double word alignment requirement.
	heap_start = (blockHeader*) mmap_ptr + 1;

	// Set the end mark
	end_mark = (blockHeader*)((void*)heap_start + alloc_size);
	end_mark->size_status = 1;

	// Set size in header
	heap_start->size_status = alloc_size;

	// Set p-bit as allocated in header
	// note a-bit left at 0 for free
	heap_start->size_status += 2;

	// Set the footer
	blockHeader *footer = (blockHeader*) ((void*)heap_start + alloc_size - 4);
	footer->size_status = alloc_size;

	return 0;
} 

/* STUDENTS MAY EDIT THIS FUNCTION, but do not change function header.
 * TIP: review this implementation to see one way to traverse through
 *      the blocks in the heap.
 *
 * Can be used for DEBUGGING to help you visualize your heap structure.
 * It traverses heap blocks and prints info about each block found.
 * 
 * Prints out a list of all the blocks including this information:
 * No.      : serial number of the block 
 * Status   : free/used (allocated)
 * Prev     : status of previous block free/used (allocated)
 * t_Begin  : address of the first byte in the block (where the header starts) 
 * t_End    : address of the last byte in the block 
 * t_Size   : size of the block as stored in the block header
 */                     
void disp_heap() {     

	int    counter;
	char   status[6];
	char   p_status[6];
	char * t_begin = NULL;
	char * t_end   = NULL;
	int    t_size;

	blockHeader *current = heap_start;
	counter = 1;

	int used_size =  0;
	int free_size =  0;
	int is_used   = -1;

	fprintf(stdout, 
			"*********************************** HEAP: Block List ****************************\n");
	fprintf(stdout, "No.\tStatus\tPrev\tt_Begin\t\tt_End\t\tt_Size\n");
	fprintf(stdout, 
			"---------------------------------------------------------------------------------\n");

	while (current->size_status != 1) {
		t_begin = (char*)current;
		t_size = current->size_status;

		if (t_size & 1) {
			// LSB = 1 => used block
			strcpy(status, "alloc");
			is_used = 1;
			t_size = t_size - 1;
		} else {
			strcpy(status, "FREE ");
			is_used = 0;
		}

		if (t_size & 2) {
			strcpy(p_status, "alloc");
			t_size = t_size - 2;
		} else {
			strcpy(p_status, "FREE ");
		}

		if (is_used) 
			used_size += t_size;
		else 
			free_size += t_size;

		t_end = t_begin + t_size - 1;

		fprintf(stdout, "%d\t%s\t%s\t0x%08lx\t0x%08lx\t%4i\n", counter, status, 
				p_status, (unsigned long int)t_begin, (unsigned long int)t_end, t_size);

		current = (blockHeader*)((char*)current + t_size);
		counter = counter + 1;
	}

	fprintf(stdout, 
			"---------------------------------------------------------------------------------\n");
	fprintf(stdout, 
			"*********************************************************************************\n");
	fprintf(stdout, "Total used size = %4d\n", used_size);
	fprintf(stdout, "Total free size = %4d\n", free_size);
	fprintf(stdout, "Total size      = %4d\n", used_size + free_size);
	fprintf(stdout, 
			"*********************************************************************************\n");
	fflush(stdout);

	return;  
} 


// end p3Heap.c (Fall 2023)                                         

