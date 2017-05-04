/**************************************
 * Alessandro Lim / Kevin Turkington
 * Group 13-07
 * CS444
 * Concurrency 2
 **************************************/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
//GLOBALS
pthread_mutex_t print_lock;
pthread_mutex_t fork[5];
////////

struct philosopher
{
	char name[20];
	int seat_num;
};

void think(struct philosopher *ph)
{
	pthread_mutex_lock(&print_lock);
	printf("%s is thinking.\n", ph->name);
	pthread_mutex_unlock(&print_lock);
	//thinks for 1-20 seconds.
	sleep(rand()%20+1);
}

void get_forks(struct philosopher *ph)
{
	int first_fork;
	int second_fork;
	if (ph->seat_num+1 == 5) {
		first_fork = 0;
		second_fork = ph->seat_num;
	}
	else {
		first_fork = ph->seat_num;
		second_fork = ph->seat_num+1;
	}
	pthread_mutex_lock(&print_lock);
	printf("%s tries to get fork number %d\n", ph-> name,first_fork);
	pthread_mutex_unlock(&print_lock);
	pthread_mutex_lock(&fork[first_fork]);
	//second fork
	pthread_mutex_lock(&print_lock);
	printf("%s tries to get fork number %d\n", ph-> name,second_fork);
	pthread_mutex_unlock(&print_lock);
	pthread_mutex_lock(&fork[second_fork]);
}

void eat(struct philosopher *ph)
{
	pthread_mutex_lock(&print_lock);
	printf("%s is eating.\n", ph->name);
	pthread_mutex_unlock(&print_lock);
	//eats for 2 - 9 seconds.
	sleep(rand()%8+2);
	
	pthread_mutex_lock(&print_lock);
	printf("%s finishes eating.\n", ph->name);
	pthread_mutex_unlock(&print_lock);
}

void put_forks(struct philosopher *ph)
{
	int first_fork;
	int second_fork;
	if (ph->seat_num+1 == 5) {
		first_fork = 0;
		second_fork = ph->seat_num;
	}
	else {
		first_fork = ph->seat_num;
		second_fork = ph->seat_num+1;
	}
	pthread_mutex_lock(&print_lock);
	printf("%s puts down the forks.\n", ph-> name);
	pthread_mutex_unlock(&print_lock);
	pthread_mutex_unlock(&fork[first_fork]);
	pthread_mutex_unlock(&fork[second_fork]);
}

void *philosopher_do(struct philosopher *ph)
{
	while(true) {
		think(ph);
		get_forks(ph);
		eat(ph);
		put_forks(ph);
	}
}

int main()
{
	//declaring philosophers
	struct philosopher philosophers[5];
	//naming
	strcpy(philosophers[0].name, "Plato");
	strcpy(philosophers[1].name, "Aristotle");
	strcpy(philosophers[2].name, "Socrates");
	strcpy(philosophers[3].name, "Descartes");
	strcpy(philosophers[4].name, "Chomksy");
	//seating
	for (i = 0; i < 5; i++) {
		philosophers[i].seat_num = i;
	}
	
	//initializing mutex
	pthread_mutex_init(&print_lock);
	for (i = 0; i < 5; i++) {
		pthread_mutex_init(&fork[i]);
	}
	
	pthread_t threads[5];
	for (i = 0; i < 5; i++) {
		pthread_create(&threads[i], NULL, philosopher_do, philosophers[i]);
	}
	
	
	return 0;
}
