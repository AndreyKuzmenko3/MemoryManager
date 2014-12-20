#include "stdafx.h"
#include "Memory_manager.h"
#include "Memory_stat.h"
#include <iostream>
#include <windows.h>
#include <time.h>

int SIZE_FOR_ALLOC[]={16,64,256,1024,4096,16384};
static struct t_s
{
	double av_time_al;//среднее врем€ выделени€ пам€ти
	double min_time_al;//минимальное
	double max_time_al;//максимальное
	double av_time_free;//среднее врем€ освобождени€ пам€ти
	double min_time_free;//минимальное
	double max_time_free;//макисмальное
}time_stat={0,100000000,0,0,100000000,0};
/*
ќписание:‘ункци€ дл€ получени€ количества тиков процессора со времени старта системы
ѕараметры: 
¬озвраща€емое значение:  оличество тиков
*/
long long get_time()
{
	long long t;
 
    __asm{
        CPUID 
        RDTSC 
        mov DWORD PTR[t], eax 
        mov DWORD PTR[t + 4], edx 
        }
	return t;
}
void main(int argc, char *argv[])
{
	LARGE_INTEGER f;//получние частоты работы CPU дл€ замеров времени выполнени€
	QueryPerformanceFrequency(&f);
	double Frequency=f.LowPart/1000;

	////—оздание большого массива массивов дл€ выделени€ под них блоков пам€ти случайного размера
	srand ( time(NULL) );
	const int max_var_mass_count=5000;
	char* mass[max_var_mass_count];
	int var_mass_count;
	for(var_mass_count=0;var_mass_count<max_var_mass_count && allocated_memory<(mem_size/100*80);var_mass_count++)
	{
		int size=(double)(rand())/RAND_MAX*(max_var_size-min_var_size)+min_var_size;

		if(argc!=1)size=SIZE_FOR_ALLOC[size%6];

		allocated_memory+=size+sizeof(mem_block);
		mass[var_mass_count]=(char*)alloc(size);
		if(!mass[var_mass_count])
			printf("%s","ERROR! Can't alloc 80%");
	} 
	print_mem_info();

	////—бор статистики по времени выполнени€ операций выделени€ и освобождени€
	int counter=0;
	int err_counter_f=0;
	int err_counter_a=0;
	bool init_time_stat=false;
	long start_time=clock();
	long double time_stamp;
	while (allocated_memory<mem_size)
	{
		int size=rand();

		time_stamp=get_time();
			freem(mass[size%(var_mass_count)]);
		time_stamp=(get_time()-time_stamp)/Frequency;

		if(init_time_stat && time_stat.av_time_free/counter*5<time_stamp)err_counter_f++;else
		{
		time_stat.av_time_free+=time_stamp;
		if(time_stat.max_time_free<time_stamp)time_stat.max_time_free=time_stamp;
		if(time_stamp<time_stat.min_time_free)time_stat.min_time_free=time_stamp;
		}

		time_stamp=get_time();

		if(argc!=1)		mass[size%(var_mass_count)]=(char*)alloc(SIZE_FOR_ALLOC[size%6]);
		else
			mass[size%(var_mass_count)]=(char*)alloc((double)(size)/RAND_MAX*(max_var_size-min_var_size)+min_var_size);
		time_stamp=(get_time()-time_stamp)/Frequency;

		if(init_time_stat && time_stat.av_time_al/counter*5<time_stamp)err_counter_a++;else
		{
		time_stat.av_time_al+=time_stamp;
		if(time_stat.max_time_al<time_stamp)time_stat.max_time_al=time_stamp;
		if(time_stamp<time_stat.min_time_al)time_stat.min_time_al=time_stamp;
		}

		if(!mass[size%(var_mass_count)])	break;

		counter++;
		if(counter==100000)
		{	
			time_stat.av_time_free/=(counter-err_counter_f);
			time_stat.av_time_al/=(counter-err_counter_a);
			printf("Avarage time to free: %.4f mcs\n",time_stat.av_time_free);
			printf("Max time to free: %.4f mcs\n",time_stat.max_time_free);
			printf("Min time to free: %.4f mcs\n",time_stat.min_time_free);
			printf("Avarage time to alloc: %.4f mcs\n",time_stat.av_time_al);
			printf("Max time to alloc: %.4f mcs\n",time_stat.max_time_al);
			printf("Min time to alloc: %.4f mcs\n",time_stat.min_time_al);
			printf("Total time: %.4f s\n\n",(double)(clock()-start_time)/1000);
			counter=1;
			err_counter_a=0;
			err_counter_f=0;
			print_mem_info();
			if(!init_time_stat)
			{
				init_time_stat=true;
				time_stat.max_time_free=-1;
				time_stat.min_time_free=1000;
				time_stat.max_time_al=-1;
				time_stat.min_time_al=1000;
			}
			start_time=clock();
		}
	}
	printf("\n%s%i\n","ERROR! Can't alloc memory!\n",counter);
	print_mem_info();
}