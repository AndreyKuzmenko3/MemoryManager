#include "stdafx.h"
#include <iostream>
#include <windows.h>
#include <time.h>
#include <stdlib.h>

//using namespace System;
using namespace std;

#define mem_size 1024*1024*10 //размер(в байтах) памяти
#define min_var_size 16 //минимальный размер переменной

static char mem[mem_size];
static bool init=false;
#pragma pack(1)
struct mem_block
{
	char is_available;
	unsigned int size;
};
#pragma pack()

static unsigned long allocated_memory=0;
static unsigned long size_meta_info=0;

void free(void* m)
{
	if(m){
	struct mem_block *mb;
	mb=(mem_block*)((char*)m-sizeof(mem_block));
	mb->is_available=1;}
	return;
}
void*alloc(int size)
{
	if(!init)
	{
		mem_block *qq=(mem_block*)mem;
		qq->is_available=1;
		qq->size=mem_size-sizeof(mem_block);
		init=true;
	}
	if(size<min_var_size||16*1024<size)return NULL;//из задания минимальный блок 16 байт
	

	void* cur_loc=mem;//текущая позиция в массиве,ее возвращаем
	void* buf_loc=0;//указатель для объединения блоков
	int buf_size=0;//сумма блоков для объединения
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
					if(!buf_size) buf_size=((mem_block*)cur_loc)->size;else
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

			if(buf_size>=size+sizeof(mem_block))
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
			buf_size=0;
			}

		cur_loc=(char*)cur_loc+mb->size+sizeof(mem_block);
	}
	if(cur_loc>=mem+mem_size) return NULL;
	return (char*)cur_loc+sizeof(mem_block);
}
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
void print_mem_info()
{
	get_memory_info();
	cout<<"Avalible space:"<<mem_size-allocated_memory-size_meta_info<<'\n';
	cout<<"Busy space:"<<allocated_memory+size_meta_info<<'\n';
}

void main()
{
	mem_block *qq=(mem_block*)mem;
		qq->is_available=1;
		qq->size=mem_size-sizeof(mem_block);
		cout<<qq->size;
		return;
	srand ( time(NULL) );
	char* mass[1500];
	int i;

	printf("\n\n%d\n\n", sizeof(struct mem_block));

	for(i=0;i<1500&&allocated_memory<(mem_size/100*80);i++)
	{
		int size=rand()/2;
		allocated_memory+=size;
		mass[i]=(char*)alloc(size);
		if(!mass[i])
			cout<<"ERROR!";
	} 
	print_mem_info();

	//return;
	int counter=0;
	long start_time=clock();
	while (allocated_memory<mem_size)
	{
		int size=rand();
		cout<<&mass[size%(i+1)]<<" "<<size/2<<" ";
		free(mass[size%(i+1)]);
		mass[size%(i+1)]=(char*)alloc(size/2);
		if(!mass[size%(i+1)])break;
		counter++;
		get_memory_info();
		if(allocated_memory>mem_size) 
			get_memory_info();
		if(counter==100000)
		{
			cout<<start_time-clock()<<'\n';
			start_time=clock();
			counter=0;
			print_mem_info();
		}
	}
	get_memory_info();
	cout<<"ERROR!"<<'\n';
	print_mem_info();
}