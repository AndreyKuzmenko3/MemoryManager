#include "stdafx.h"
#include "TimeStat.h"
#include <iostream>
#include <Windows.h>

LARGE_INTEGER f;//получние частоты работы CPU для замеров времени выполнения
bool init_frq=QueryPerformanceFrequency(&f);
double Frequency=f.LowPart/1000;

long long get_time() {
	long long t;

	__asm{
		CPUID 
			RDTSC 
			mov DWORD PTR[t], eax 
			mov DWORD PTR[t + 4], edx 
	}
	return t;
}

void addtime(t_s* tm_s,long long cur_time) {
	if (0 == tm_s->prev_time) {
		tm_s->prev_time=cur_time;
		return;
	}
	long double time_stamp = (cur_time-tm_s->prev_time)/Frequency;
	if (time_stamp < tm_s->min_time) tm_s->min_time = time_stamp;
	if (tm_s->max_time < time_stamp) {
		if (tm_s->avg_time*5/tm_s->time_count > time_stamp || tm_s->avg_time == 0) {
			tm_s->max_time = time_stamp;
		}
		else
			tm_s->time_count--;
	}
	tm_s->time_count++;
	tm_s->avg_time += time_stamp;
	tm_s->prev_time=0;
	if (tm_s->max_time > tm_s->avg_time*5/tm_s->time_count) tm_s->max_time=tm_s->avg_time*5/tm_s->time_count;
}

void printtime(t_s* time_stat) {
	printf("Average time : %.2f mcs\n",time_stat->avg_time/time_stat->time_count);
	printf("Max time     : %.2f mcs\n",time_stat->max_time);
	printf("Min time     : %.2f mcs\n\n",time_stat->min_time);
}