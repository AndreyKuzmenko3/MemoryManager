#include "stdafx.h"
#include <iostream>
#include <windows.h>
#include <time.h>
#include <stdlib.h>
//using namespace System;
using namespace std;

#define mem_size 10*1024*1024 //������(� ������) ������
#define min_var_size 16 //����������� ������ ����������
#define max_var_size 16*1024//������������ ������ ����������

static char mem[mem_size];//������-������ �� ������� ���������� ����������
static bool init=false;//���� �������������
/*��������� ����������*/
#pragma pack(1)
struct mem_block
{
	char is_available;
	unsigned int size;
};
#pragma pack()
/*���������� ��� �������� ���������� � �������� ������� ������*/
static unsigned long allocated_memory=0;
static unsigned long size_meta_info=0;
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
��������:������� ��� ������������ ���������� ������
���������: void* m- ��������� �� ����������,������ �� ������� ���������� ����������
������������� ��������:-
*/
void free(void* m)
{
	if(m){
	struct mem_block *mb;
	mb=(mem_block*)((char*)m-sizeof(mem_block));
	mb->is_available=1;}
	return;
}
/*
��������:������� ��� ��������� ������
���������: int size- ������ ���������� ������,��������� �������� �� 16 �� 16384
������������� ��������:Null � ������ ������,��������� �� ������ � ������ ������
*/
void*alloc(int size)
{
	if(!init)
	{
		mem_block *qq=(mem_block*)mem;
		qq->is_available=1;
		qq->size=mem_size-sizeof(mem_block);
		init=true;
	}
	if(size<min_var_size||max_var_size<size)return NULL;//�� ������� ����������� ���� 16 ����
	

	void* cur_loc=mem;//������� ������� � �������,�� ����������
	void* buf_loc=0;//��������� ��� ����������� ������
	int buf_size=-1;//����� ������ ��� �����������
	while (cur_loc<mem+mem_size)
	{
		mem_block *mb=(mem_block*)cur_loc;
		if(mb->is_available)
		{
			if(!buf_loc)  buf_loc=cur_loc;

			if(mb->size==size)
			{
				mb->is_available=0;
				break;
			}
			if(mb->size>=size+sizeof(mem_block))
			{
				int odd_size=mb->size-size;
				mb->is_available=0;
				mb->size=size;
				mb=(mem_block*)((char*)mb+(size+sizeof(mem_block)));
				mb->is_available=1;
				mb->size=odd_size-sizeof(mem_block);//!!!!!
				break;
			}
			else
				{
					if(-1==buf_size) buf_size=((mem_block*)cur_loc)->size;else
					buf_size+=((mem_block*)cur_loc)->size+sizeof(mem_block);
				}
		}
			if(buf_size==size)
			{
				mb=(mem_block*)buf_loc;
				mb->is_available=0;
				mb->size=size;
				cur_loc=buf_loc;
				break;
			}

			if((int)(size+sizeof(mem_block))<=buf_size)
				{
					int odd_size=buf_size-size;
					mb=(mem_block*)buf_loc;
					mb->is_available=0;
					mb->size=size;
					mb=(mem_block*)((char*)mb+(size+sizeof(mem_block)));
					mb->is_available=1;
					mb->size=odd_size-sizeof(mem_block);//!!!!
					cur_loc=buf_loc;
					break;
				}
		
			if(!mb->is_available)
				{
			buf_loc=0;
			buf_size=-1;
			}

		cur_loc=(char*)cur_loc+mb->size+sizeof(mem_block);
	}

	if(cur_loc>=mem+mem_size) return NULL;
	return (char*)cur_loc+sizeof(mem_block);
}
/*
��������:��������� ���������� � ���������� ������
���������: -
������������� ��������:-
*/
void get_memory_info()
{
	allocated_memory=0;
	size_meta_info=0;

	if(init){
	mem_block* mb=(mem_block*)mem;
	while ((char*)mb<mem+mem_size)
	{
		if(!mb->is_available)
			allocated_memory+=mb->size;
		
		size_meta_info++;
		mb=(mem_block*)((char*)mb+(mb->size+sizeof(mem_block)));
	}
	size_meta_info*=sizeof(mem_block);
	}
}
/*
��������:��������� � ����� ���������� � ���������� ������
���������: -
������������� ��������:-
*/
void print_mem_info()
{
	get_memory_info();
	printf("Avalible space: %u \n",mem_size-allocated_memory-size_meta_info);
	printf("Busy space: %u \n",allocated_memory+size_meta_info);
}
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
	LARGE_INTEGER f;//�������� ������� ������ CPU ��� ������� ������� ����������
	QueryPerformanceFrequency(&f);
	double Frequency=f.LowPart;
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
	long start_time=clock();
	while (allocated_memory<mem_size)
	{
		int size=rand();

		long double time_stamp=get_time();
			free(mass[size%(i)]);
		time_stamp=(get_time()-time_stamp)/Frequency;
		time_stat.av_time_free+=time_stamp;
		if(time_stat.max_time_free<time_stamp)time_stat.max_time_free=time_stamp;
		if(time_stamp<time_stat.min_time_free)time_stat.min_time_free=time_stamp;

		time_stamp=get_time();
			mass[size%(i)]=(char*)alloc((double)(size)/RAND_MAX*(max_var_size-min_var_size)+min_var_size);
		time_stamp=(get_time()-time_stamp)/Frequency;
		time_stat.av_time_al+=time_stamp;
		if(time_stat.max_time_al<time_stamp)time_stat.max_time_al=time_stamp;
		if(time_stamp<time_stat.min_time_al)time_stat.min_time_al=time_stamp;

		if(!mass[size%(i)])
		{
			get_memory_info();
			break;
		}
		counter++;
		get_memory_info();
		if(counter==100000)
		{	
			time_stat.av_time_free/=counter;
			time_stat.av_time_al/=counter;
			printf("Avarage time to free: %.8f\n",time_stat.av_time_free);
			printf("Max time to free: %.8f\n",time_stat.max_time_free);
			printf("Min time to free: %.8f\n",time_stat.min_time_free);
			printf("Avarage time to alloc: %.8f\n",time_stat.av_time_al);
			printf("Max time to alloc: %.8f\n",time_stat.max_time_al);
			printf("Min time to alloc: %.8f\n",time_stat.min_time_al);
			printf("Total time: %.4f\n\n",(double)(clock()-start_time)/1000);
			counter=0;
			start_time=clock();
			print_mem_info();
		}
	}
	get_memory_info();
	printf("\n%s%i\n","ERROR! Can't alloc memory!",counter);
	print_mem_info();
}