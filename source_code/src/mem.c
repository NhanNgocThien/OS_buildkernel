
#include "mem.h"
#include "stdlib.h"
#include "string.h"
#include <pthread.h>
#include <stdio.h>

static BYTE _ram[RAM_SIZE];

static struct {
	uint32_t proc;	// ID of process currently uses this page
	int index;	// Index of the page in the list of pages allocated
			// to the process.
	int next;	// The next page in the list. -1 if it is the last
			// page.
} _mem_stat [NUM_PAGES];

static pthread_mutex_t mem_lock;

void init_mem(void) {
	memset(_mem_stat, 0, sizeof(*_mem_stat) * NUM_PAGES);
	memset(_ram, 0, sizeof(BYTE) * RAM_SIZE);
	pthread_mutex_init(&mem_lock, NULL);
}

/* get offset of the virtual address */
static addr_t get_offset(addr_t addr) {
	return addr & ~((~0U) << OFFSET_LEN);
}

/* get the first layer index */
static addr_t get_first_lv(addr_t addr) {
	return addr >> (OFFSET_LEN + PAGE_LEN);
}

/* get the second layer index */
static addr_t get_second_lv(addr_t addr) {
	return (addr >> OFFSET_LEN) - (get_first_lv(addr) << PAGE_LEN);
}

/* Search for page table table from the a segment table */
static struct page_table_t * get_page_table(
		addr_t index, 	// Segment level index
		struct seg_table_t * seg_table) { // first level table
	
	/*
	 * TODO: Given the Segment index [index], you must go through each
	 * row of the segment table [seg_table] and check if the v_index
	 * field of the row is equal to the index
	 *
	 * */

	int i;
	for (i = 0; i < seg_table->size; i++) {
		// Enter your code here
		if (seg_table->table[i].v_index == index){
			return seg_table->table[i].pages;
		}
	}
	return NULL;

}

/* Translate virtual address to physical address. If [virtual_addr] is valid,
 * return 1 and write its physical counterpart to [physical_addr].
 * Otherwise, return 0 */
static int translate(
		addr_t virtual_addr, 	// Given virtual address
		addr_t * physical_addr, // Physical address to be returned
		struct pcb_t * proc) {  // Process uses given virtual address

	/* Offset of the virtual address */
	addr_t offset = get_offset(virtual_addr);
	/* The first layer index */
	addr_t first_lv = get_first_lv(virtual_addr);
	/* The second layer index */
	addr_t second_lv = get_second_lv(virtual_addr);
	
	/* Search in the first level */
	struct page_table_t * page_table = NULL;
	page_table = get_page_table(first_lv, proc->seg_table);
	if (page_table == NULL) {
		return 0;
	}

	int i;
	for (i = 0; i < page_table->size; i++) {
		if (page_table->table[i].v_index == second_lv) {
			/* TODO: Concatenate the offset of the virtual addess
			 * to [p_index] field of page_table->table[i] to 
			 * produce the correct physical address and save it to
			 * [*physical_addr]  */
			*physical_addr = (page_table->table[i].p_index << OFFSET_LEN) + offset;
			return 1;
		}
	}
	return 0;	
}

addr_t alloc_mem(uint32_t size, struct pcb_t * proc) {
	pthread_mutex_lock(&mem_lock);
	addr_t ret_mem = 0;
	/* TODO: Allocate [size] byte in the memory for the
	 * process [proc] and save the address of the first
	 * byte in the allocated memory region to [ret_mem].
	 * */

	uint32_t num_pages = (size % PAGE_SIZE) ? size / PAGE_SIZE + 1 :
		size / PAGE_SIZE; // Number of pages we will use
	int mem_avail = 0; // We could allocate new memory region or not?

	/* First we must check if the amount of free memory in
	 * virtual address space and physical address space is
	 * large enough to represent the amount of required 
	 * memory. If so, set 1 to [mem_avail].
	 * Hint: check [proc] bit in each page of _mem_stat
	 * to know whether this page has been used by a process.
	 * For virtual memory space, check bp (break pointer).
	 * */
	int empty_page = 0;
	if ( NUM_PAGES - (proc->bp>>OFFSET_LEN) >= num_pages){
		for (int i = 0; i < NUM_PAGES; i++){
			if (_mem_stat[i].proc == 0) empty_page ++;
			if ( empty_page >= num_pages){
				mem_avail = 1;
				break;
			}
		}
	}
	if (mem_avail) {
		/* We could allocate new memory region to the process */
		ret_mem = proc->bp;
		proc->bp += num_pages * PAGE_SIZE;
		/* Update status of physical pages which will be allocated
		 * to [proc] in _mem_stat. Tasks to do:
		 * 	- Update [proc], [index], and [next] field
		 * 	- Add entries to segment table page tables of [proc]
		 * 	  to ensure accesses to allocated memory slot is
		 * 	  valid. */
		if (ret_mem == PAGE_SIZE){
			proc->seg_table->table[0].pages = (struct page_table_t*)malloc(sizeof(struct page_table_t));
			proc->seg_table->table[0].v_index = 0;
			proc->seg_table->table[0].pages->size = 1; 
			proc->seg_table->size = 1;
		}

		addr_t seg_after = get_first_lv(proc->bp);
		for (int i = proc->seg_table->size; i < seg_after + 1 ;i++){
			proc->seg_table->table[i].pages	= (struct page_table_t*)malloc(sizeof(struct page_table_t));
			proc->seg_table->table[i].v_index = i;
			proc->seg_table->table[i].pages->size = 0; 
			proc->seg_table->size ++;
		}

		empty_page = 0;
		int temp_index = 0;
		for (int i = 0; i < num_pages; i++){
			addr_t segment_index = get_first_lv(ret_mem + i*PAGE_SIZE);
			addr_t table_index = get_second_lv(ret_mem + i*PAGE_SIZE);
			proc->seg_table->table[segment_index].pages->size ++;
			struct page_table_t * page_table = NULL;
			page_table = get_page_table(segment_index, proc->seg_table);
			page_table->table[table_index].v_index = table_index;
			while(empty_page<NUM_PAGES) {
				if (_mem_stat[empty_page].proc == 0) {
					page_table->table[table_index].p_index = empty_page;
					_mem_stat[empty_page].proc = proc->pid;
					_mem_stat[empty_page].index = i;
					if (i > 0) _mem_stat[temp_index].next = empty_page;
					if (i == num_pages - 1) _mem_stat[empty_page].next = -1;
					temp_index = empty_page;
					break;
				}
				empty_page++;
			}
		}
	}
	pthread_mutex_unlock(&mem_lock);
	return ret_mem;
}

int free_mem(addr_t address, struct pcb_t * proc) {
	/*TODO: Release memory region allocated by [proc]. The first byte of
	 * this region is indicated by [address]. Task to do:
	 * 	- Set flag [proc] of physical page use by the memory block
	 * 	  back to zero to indicate that it is free.
	 * 	- Remove unused entries in segment table and page tables of
	 * 	  the process [proc].
	 * 	- Remember to use lock to protect the memory from other
	 * 	  processes.  */
	pthread_mutex_lock(&mem_lock);
	addr_t physical_addr;
	int num_free_pages = 0; 
	if (translate(address, &physical_addr, proc)) {
	 	addr_t _mem_stat_index = physical_addr >> 10;
	 	while(1){
	 		addr_t temp_index = _mem_stat_index;
	 		_mem_stat[_mem_stat_index].proc = 0;
	 		_mem_stat[_mem_stat_index].index = 0;
	 		_mem_stat_index = _mem_stat[_mem_stat_index].next;
			_mem_stat[temp_index].next = 0;
			num_free_pages++;
			if(_mem_stat_index == -1) break;
	 	}
	}
	struct page_table_t * page_table = NULL;
	int temp_index = -1;
	int decrease_size = 0;
	for (int i = 0; i < num_free_pages; i++){
		addr_t first_lv = get_first_lv(address + i*PAGE_SIZE);
		addr_t second_lv = get_second_lv(address + i*PAGE_SIZE);
		page_table = get_page_table(first_lv, proc->seg_table);
		if (first_lv == (proc->bp>>15)){
			if (temp_index == -1) temp_index = i; 
			else temp_index --;
			if (address + i*PAGE_SIZE + (num_free_pages - temp_index)*PAGE_SIZE < proc->bp){
				page_table->table[second_lv].p_index = page_table->table[second_lv + num_free_pages - temp_index].p_index;
				for (int reg = 0; reg < 10; reg ++){
					if (proc->regs[reg] ==  address + i*PAGE_SIZE + (num_free_pages - temp_index)*PAGE_SIZE ){
						proc->regs[reg] = address + i*PAGE_SIZE;
						break;
					}
				}
			}
			decrease_size++;
		}
		else {
			for (int j = 0; j < page_table->size; j++) {
				if (page_table->table[j].v_index == second_lv) {
			 		page_table->table[j].v_index = page_table->table[page_table->size - 1].v_index;
					page_table->table[j].p_index = page_table->table[page_table->size - 1].p_index;
					page_table->table[page_table->size - 1].v_index = 0;
					page_table->table[page_table->size - 1].p_index = 0;
					page_table->size --;
					if (page_table->size == 0)free(page_table);
					break;
				}
			}
		}
	}
	if (decrease_size > 0){
		page_table->size =  page_table->size - decrease_size;
		proc->bp -= PAGE_SIZE*decrease_size;
		printf("\nsize: %d \n", page_table->size );
	}
	pthread_mutex_unlock(&mem_lock);
	return 0;
}

int read_mem(addr_t address, struct pcb_t * proc, BYTE * data) {
	addr_t physical_addr;
	if (translate(address, &physical_addr, proc)) {
		*data = _ram[physical_addr];
		return 0;
	}else{
		return 1;
	}
}

int write_mem(addr_t address, struct pcb_t * proc, BYTE data) {
	addr_t physical_addr;
	if (translate(address, &physical_addr, proc)) {
		_ram[physical_addr] = data;
		return 0;
	}else{
		return 1;
	}
}

void dump(void) {
	int i;
	for (i = 0; i < NUM_PAGES; i++) {
		if (_mem_stat[i].proc != 0) {
			printf("%03d: ", i);
			printf("%05x-%05x - PID: %02d (idx %03d, nxt: %03d)\n",
				i << OFFSET_LEN,
				((i + 1) << OFFSET_LEN) - 1,
				_mem_stat[i].proc,
				_mem_stat[i].index,
				_mem_stat[i].next
			);
			int j;
			for (	j = i << OFFSET_LEN;
				j < ((i+1) << OFFSET_LEN) - 1;
				j++) {
				
				if (_ram[j] != 0) {
					printf("\t%05x: %03x\n", j, _ram[j]);
				}
					
			}
		}
	}
}


