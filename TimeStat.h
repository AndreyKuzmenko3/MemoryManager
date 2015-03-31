#pragma once
struct t_s {
	double avg_time;//среднее время
	double min_time;//минимальное
	double max_time;//максимальное
	long long prev_time;
	int time_count;
	t_s() {avg_time=0; min_time=1000000; max_time=-1; time_count=0; prev_time=0;}
};

long long get_time();

void addtime(t_s*,long long);

void printtime(t_s*);