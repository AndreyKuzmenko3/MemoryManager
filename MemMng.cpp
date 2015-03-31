#include "stdafx.h"
#include "Memory_manager.h"
#include "Memory_stat.h"
#include "TimeStat.h"
#include <iostream>
#include <windows.h>
#include <time.h>

int SIZE_FOR_ALLOC[]={16,64,256,1024,4096,16384};

void main(int argc, char *argv[])
{
	////Создание большого массива массивов для выделения под них блоков памяти случайного размера
	srand ( time(NULL) );
	const int max_var_mass_count=5000;
	char* mass[max_var_mass_count];
	int var_mass_count=0;
	for (; var_mass_count < max_var_mass_count && allocated_memory < (mem_size/100*80); var_mass_count++) {
		int size=(double)(rand())/RAND_MAX*(max_var_size-min_var_size)+min_var_size;

		if (argc != 1) size=SIZE_FOR_ALLOC[size%6];

		allocated_memory += size+sizeof(mem_block);
		mass[var_mass_count] = (char*)alloc(size);
		if (!mass[var_mass_count])
			printf("%s","ERROR! Can't alloc 80%");
	} 
	print_mem_info();

	////Сбор статистики по времени выполнения операций выделения и освобождения
	int counter=0;
	long start_time=clock();
	t_s time_to_free;
	t_s time_to_alloc;
	while (allocated_memory<mem_size) {
		int size=rand();

		addtime(&time_to_free,get_time());
			freem(mass[size%(var_mass_count)]);
		addtime(&time_to_free,get_time());

		addtime(&time_to_alloc,get_time());
		if (argc != 1)
			mass[size%(var_mass_count)]=(char*)alloc(SIZE_FOR_ALLOC[size%6]);
		else
			mass[size%(var_mass_count)]=(char*)alloc((double)(size)/RAND_MAX*(max_var_size-min_var_size)+min_var_size);
		addtime(&time_to_alloc,get_time());

		if (!mass[size%(var_mass_count)])	break;

		counter++;
		if (counter == 100000) {	
			printf("Free time:\n");
			printtime(&time_to_free);
			printf("Alloc time:\n");
			printtime(&time_to_alloc);
			printf("Total time: %.4f s\n\n",(double)(clock()-start_time)/1000);
			counter=0;
			print_mem_info();
			start_time=clock();
		}
	}
	printf("\n%s%i\n","ERROR! Can't alloc memory!\n",counter);
	print_mem_info();
}