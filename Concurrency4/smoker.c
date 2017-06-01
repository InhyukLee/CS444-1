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

pthread_mutex pusher_mutex; //only one pusher can go at a time

//used for pusher
int is_tobacco = 0;
int is_paper = 0;
int is_match = 0;

//tobacco and paper
void *agent_a() 
{
	sem_wait(&agent_sem);
	sem_post(tobacco);
	sem_post(paper);
}

//paper and match
void *agent_b() 
{
	sem_wait(&agent_sem);
	sem_post(paper);
	sem_post(match);
}

//tobacco and match
void *agent_c() 
{
	sem_wait(&agent_sem);
	sem_post(tobacco);
	sem_post(match);
}

//pusher for tobacco
void *pusher_t() 
{
}

//pusher for paper
void *pusher_p() 
{
}

//pusher for match
void *pusher_m() 
{
}

//smoker with tobacco
void *smoker_t() 
{
}

//smoker with paper
void *smoker_p() 
{
}

//smoker with match
void *smoker_m() 
{
}

int main()
{
	return 0;
}