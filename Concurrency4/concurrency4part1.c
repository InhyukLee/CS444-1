/**************************************
 * Alessandro Lim / Kevin Turkington
 * Group 13-07
 * CS444
 * Concurrency 4 part 1
 **************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
sem_t available_resource;
sem_t check_resource;

void *do_something(int id)
{
	while(1) {
		use_resource(id);
	}
}

void use_resource(int id)
{
	printf("thread %s: using resource.", id);
	int i = rand()%5+1;
	sleep(i);
}

int main()
{
	sem_init(&available_resource, 0, 3);
	sem_init(&check_resource, 0, 1);
	srand(time(NULL));
	return 0;
}