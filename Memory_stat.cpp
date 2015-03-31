#include "stdafx.h"
#include "Memory_manager.h"
#include "Memory_stat.h"
#include <iostream>

extern char mem[mem_size];
extern bool init;
void get_memory_info() {
	allocated_memory=0;
	size_meta_info=0;

	if (init) {
		mem_block* mb = (mem_block*)mem;
		while ((char*)mb < mem+mem_size) {
			if (!mb->is_available)
				allocated_memory += mb->size;

			size_meta_info++;
			mb = (mem_block*)((char*)mb+(mb->size+sizeof(mem_block)));
		}
		size_meta_info *= sizeof(mem_block);
	}
}

void print_mem_info() {
	get_memory_info();
	printf("Avalible space: %u \n",mem_size-allocated_memory-size_meta_info);
	printf("Busy space: %u \n",allocated_memory+size_meta_info);
}