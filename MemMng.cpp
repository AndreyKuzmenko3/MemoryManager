#include "stdafx.h"
#include "Memory_manager.h"
#include "Memory_stat.h"
#include <iostream>
#include <windows.h>
#include <time.h>
using namespace std;
static struct t_s
{
	double av_time_al;//������� ����� ��������� ������
	double min_time_al;//�����������
	double max_time_al;//������������
	double av_time_free;//������� ����� ������������ ������
	double min_time_free;//�����������
	double max_time_free;//������������
}time_stat={0,100000000,0,0,100000000,0};
/*
��������:������� ��� ��������� ���������� ����� ���������� �� ������� ������ �������
���������: 
������������� ��������: ���������� �����
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
void main()
{
	/*
	TODO:�������� ������� ������� �������� time_stamp'��
	*/
	LARGE_INTEGER f;//�������� ������� ������ CPU ��� ������� ������� ����������
	QueryPerformanceFrequency(&f);
	double Frequency=f.LowPart/1000;
	/*
	�������� �������� ������� �������� ��� ��������� ��� ��� ������ ������ ���������� �������
	*/
	srand ( time(NULL) );
	const int var_mass_size=5000;
	char* mass[var_mass_size];
	int i;
	for(i=0;i<var_mass_size&&allocated_memory<(mem_size/100*80);i++)
	{
		int size=(double)(rand())/RAND_MAX*(max_var_size-min_var_size)+min_var_size;
		allocated_memory+=size+sizeof(mem_block);
		mass[i]=(char*)alloc(size);
		if(!mass[i])
			printf("%s","ERROR! Can't alloc 80%");
	} 
	print_mem_info();
	/*
	���� ���������� �� ������� ���������� �������� ��������� � ������������
	*/
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
			freem(mass[size%(i)]);
		time_stamp=(get_time()-time_stamp)/Frequency;

		if(init_time_stat && time_stat.av_time_free/counter*5<time_stamp)err_counter_f++;else
		{
		time_stat.av_time_free+=time_stamp;
		if(time_stat.max_time_free<time_stamp)time_stat.max_time_free=time_stamp;
		if(time_stamp<time_stat.min_time_free)time_stat.min_time_free=time_stamp;
		}

		time_stamp=get_time();
			mass[size%(i)]=(char*)alloc((double)(size)/RAND_MAX*(max_var_size-min_var_size)+min_var_size);
		time_stamp=(get_time()-time_stamp)/Frequency;

		if(init_time_stat && time_stat.av_time_al/counter*5<time_stamp)err_counter_a++;else
		{
		time_stat.av_time_al+=time_stamp;
		if(time_stat.max_time_al<time_stamp)time_stat.max_time_al=time_stamp;
		if(time_stamp<time_stat.min_time_al)time_stat.min_time_al=time_stamp;
		}

		if(!mass[size%(i)])	break;

		counter++;
		if(counter==100000)
		{	
			time_stat.av_time_free/=(counter-err_counter_f);
			time_stat.av_time_al/=(counter-err_counter_a);
			printf("Avarage time to free: %.8f mcs\n",time_stat.av_time_free);
			printf("Max time to free: %.8f mcs\n",time_stat.max_time_free);
			printf("Min time to free: %.8f mcs\n",time_stat.min_time_free);
			printf("Avarage time to alloc: %.8f mcs\n",time_stat.av_time_al);
			printf("Max time to alloc: %.8f mcs\n",time_stat.max_time_al);
			printf("Min time to alloc: %.8f mcs\n",time_stat.min_time_al);
			printf("Total time: %.4f s\n\n",(double)(clock()-start_time)/1000);
			counter=1;
			err_counter_a=0;
			err_counter_f=0;
			start_time=clock();
			print_mem_info();
			if(!init_time_stat)
			{
				init_time_stat=true;
				time_stat.max_time_free=-1;
				time_stat.min_time_free=1000;
				time_stat.max_time_al=-1;
				time_stat.min_time_al=1000;
			}
		}
	}
	printf("\n%s%i\n","ERROR! Can't alloc memory!\n",counter);
	print_mem_info();
}