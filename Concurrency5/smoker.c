/**************************************
 * Alessandro Lim / Kevin Turkington
 * Group 13-07
 * CS444
 * Cigarette Smoker Problem
 **************************************/
 //Reference: http://greenteapress.com/semaphores/LittleBookOfSemaphores.pdf
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
//semaphores
sem_t agent_sem;
sem_t tobacco;
sem_t paper;
sem_t match;

sem_t tobacco_sem; //for smoker with tobacco
sem_t paper_sem; //for smoker with paper
sem_t match_sem; //for smoker with match

pthread_mutex_t pusher_mutex; //only one pusher can go at a time

//used for pusher
int is_tobacco = 0;
int is_paper = 0;
int is_match = 0;

//tobacco and paper
void *agent_a() 
{
	while(1) {
		sem_wait(&agent_sem);
		printf("agentA: paper and tobacco is released.\n");
		fflush(stdout);
		sem_post(&tobacco);
		sem_post(&paper);
	}
}

//paper and match
void *agent_b() 
{
	while(1) {
		sem_wait(&agent_sem);
		printf("agentB: paper and match is released.\n");
		fflush(stdout);
		sem_post(&paper);
		sem_post(&match);
	}
}

//tobacco and match
void *agent_c() 
{
	while(1) {
		sem_wait(&agent_sem);
		printf("agentC: tobacco and match is released.\n");
		fflush(stdout);
		sem_post(&tobacco);
		sem_post(&match);
	}
}

//pusher for tobacco
void *pusher_t() 
{
	while(1) {
		sem_wait(&tobacco);
		pthread_mutex_lock(&pusher_mutex);
		if(is_paper) {
			is_paper -= 1;
			sem_post(&match_sem);
		}
		else if(is_match) {
			is_match -= 1;
			sem_post(&paper_sem);
		}
		else {
			is_tobacco += 1;
		}
		pthread_mutex_unlock(&pusher_mutex);
	}
}

//pusher for paper
void *pusher_p() 
{
	while(1) {
		sem_wait(&paper);
		pthread_mutex_lock(&pusher_mutex);
		if(is_match) {
			is_match -= 1;
			sem_post(&tobacco_sem);
		}
		else if(is_tobacco) {
			is_tobacco -= 1;
			sem_post(&match_sem);
		}
		else {
			is_paper += 1;
		}
		pthread_mutex_unlock(&pusher_mutex);
	}
}

//pusher for match
void *pusher_m() 
{
	while(1) {
		sem_wait(&match);
		pthread_mutex_lock(&pusher_mutex);
		if(is_paper) {
			is_paper -= 1;
			sem_post(&tobacco_sem);
		}
		else if(is_tobacco) {
			is_tobacco -= 1;
			sem_post(&paper_sem);
		}
		else {
			is_match += 1;
		}
		pthread_mutex_unlock(&pusher_mutex);
	}
}

//smoker with tobacco
void *smoker_t() 
{	
	while(1) {
		sem_wait(&tobacco_sem);
		sem_post(&agent_sem);
		printf("Smoker T: smokes.\n");
		fflush(stdout);
		sleep(2);
	}
}

//smoker with paper
void *smoker_p() 
{
	while(1) {
		sem_wait(&paper_sem);
		sem_post(&agent_sem);
		printf("Smoker P: smokes.\n");
		fflush(stdout);
		sleep(2);
	}
}

//smoker with match
void *smoker_m() 
{
	while(1) {
		sem_wait(&match_sem);
		sem_post(&agent_sem);
		printf("Smoker M: smokes.\n");
		fflush(stdout);
		sleep(2);
	}
}

int main()
{	
	int i;
	sem_init(&agent_sem, 0, 1);
	sem_init(&tobacco, 0, 0);
	sem_init(&paper, 0, 0);
	sem_init(&match, 0, 0);
	sem_init(&tobacco_sem, 0, 0);
	sem_init(&paper_sem, 0, 0);
	sem_init(&match_sem, 0, 0);
	pthread_mutex_init(&pusher_mutex, NULL);
	pthread_t threads[9];
	pthread_create(&threads[0], NULL, smoker_t, NULL);
	pthread_create(&threads[1], NULL, smoker_p, NULL);
	pthread_create(&threads[2], NULL, smoker_m, NULL);
	pthread_create(&threads[3], NULL, pusher_t, NULL);
	pthread_create(&threads[4], NULL, pusher_p, NULL);
	pthread_create(&threads[5], NULL, pusher_m, NULL);
	pthread_create(&threads[6], NULL, agent_a, NULL);
	pthread_create(&threads[7], NULL, agent_b, NULL);
	pthread_create(&threads[8], NULL, agent_c, NULL);
	
	for(i = 0; i < 9; i++) {
		pthread_join(threads[i], NULL);
	}
	return 0;
}