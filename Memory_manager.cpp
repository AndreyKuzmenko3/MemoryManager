#include "stdafx.h"
#include "Memory_manager.h"

char mem[mem_size];//массив-память из которой выделяются переменные
bool init=false;//флаг инициализации
/*
Описание:Функция для освобождения выделенной памяти
Параметры: void* m- указатель на переменную,память от которой необходимо освободить
Возвращаяемое значение:-
*/
void freem(void* m)
{
	if(m){
	struct mem_block *mb;
	mb=(mem_block*)((char*)m-sizeof(mem_block));
	mb->is_available=1;}
	return;
}
/*
Описание:Функция для выделения памяти
Параметры: int size- размер выделяемой памяти,возможные значения от 16 до 16384
Возвращаяемое значение:Null в случае ошибки,указатель на память в случае успеха
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
	if(size<min_var_size||max_var_size<size)return NULL;//из задания минимальный блок 16 байт
	

	void* cur_loc=mem;//текущая позиция в массиве,ее возвращаем
	void* buf_loc=0;//указатель для объединения блоков
	int buf_size=-1;//сумма блоков для объединения
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
};