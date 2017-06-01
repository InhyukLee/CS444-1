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

int num_user = 6;
sem_t available_r;
sem_t check_r;
int stop_r = 0;

void use_resource(void *id)
{
	int *u_id = (int *)id;
	printf("thread %d: using resource.\n", *u_id);
	fflush(stdout);
	sleep(2);
	printf("thread %d: is finished.\n", *u_id);
	fflush(stdout);
}

void *use(void *id)
{
	int *u_id = (int *)id;
	int free_r;
	while(1) {
		if(stop_r == 1) {
			printf("thread %d: waiting.\n", *u_id);
			fflush(stdout);
			sem_wait(&check_r);
			sem_post(&check_r);
		}
		sem_getvalue(&available_r, &free_r);
		if (free_r == 1) {
			sem_wait(&check_r);
			stop_r = 1;
		}
		sem_wait(&available_r);
		use_resource(id);
		
		sem_getvalue(&available_r, &free_r);
		if(free_r == 2) {
			sem_post(&check_r);
			stop_r = 0;
		}
		
		sem_post(&available_r);
		
	}
}

int main()
{	
	pthread_t user[num_user];
	int i;
	int id[num_user];
	for(i = 0; i<num_user;i++) {
		id[i]= i;
	}
	sem_init(&available_r, 0, 3);
	sem_init(&check_r, 0, 1);
	srand(time(NULL));
	for(i = 0; i < num_user; i++) {
		pthread_create(&user[i], NULL, use, &id[i]);
	}
	for(i = 0; i < num_user; i++) {
		pthread_join(user[i], NULL);
	}
	return 0;
}