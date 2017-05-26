/**************************************
 * Alessandro Lim / Kevin Turkington
 * Group 13-07
 * CS444
 * Concurrency 3
 **************************************/
 //Reference: https://adrientetar.github.io/doc/other/Semaphore.pdf
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
//GLOBALS
sem_t no_inserter;
sem_t no_searcher;
sem_t no_deleter;

sem_t scounter;
sem_t icounter;
int searchcounter = 0;
int insertcounter = 0;
/////////

struct list_node
{
	int data;
	struct list_node *next;
};

struct list_head
{
	struct list_node *head;
};

void insert(struct list_head *head, int d)
{
	//only one inserter will wait for no inserter, then all other inserter will wait for that particular inserter
	if(sem_trywait(&icounter) < 0){
		printf("Inserter %lu: Is blocked.\n", pthread_self());
		sem_wait(&icounter);
	}

	insertcounter += 1;
	if (insertcounter == 1 && sem_trywait(&no_inserter) < 0) {
		printf("Inserter %lu: Is blocked.\n", pthread_self());
		sem_wait(&no_inserter);
	}
	sem_post(&icounter);
	
	struct list_node *iterator;
	struct list_node *temp;
	if (head->head == NULL) {
		head->head = (struct list_node*)malloc(sizeof(struct list_node));
		head->head->data = d;
		head->head->next = NULL;
	}
	else {
		iterator = head->head;
		while(iterator->next != NULL) {
			iterator = iterator->next;
		}
		temp = (struct list_node*)malloc(sizeof(struct list_node));
		temp->data = d;
		temp->next = NULL;
		iterator->next =  temp;
	}
	printf("Inserter %lu: Inserted %d.\n", pthread_self(), d);
	//signal noInserter and insertswitch
	sem_wait(&icounter);
	insertcounter -= 1;
	if (insertcounter == 0) {
		sem_post(&no_inserter);
	}
	sem_post(&icounter);
}


void search(struct list_head *head, int d)
{
	if(sem_trywait(&scounter) < 0){
		printf("Searcher %lu: Is blocked.\n", pthread_self());
		sem_wait(&scounter);
	}

	// sem_wait(&scounter);
	searchcounter += 1;
	if (searchcounter == 1 && sem_trywait(&no_searcher) < 0) {
		printf("Searcher %lu: Is blocked.\n", pthread_self());
		sem_wait(&no_searcher);
	}

	sem_post(&scounter);
	int searched = 0;
	struct list_node *iterator;
	if (head->head == NULL) {
		printf("Searcher %lu: Head is empty.\n", pthread_self());
	}
	else {
		iterator = head->head;
		while (iterator != NULL) {
			if (iterator->data == d) {
				printf("Searcher %lu: Found %d.\n", pthread_self(), d);
				searched = 1;
				break;
			}
			else {
				iterator =  iterator->next;
			}
		}
		if (searched == 0) {
		printf("Searcher %lu: Could not find %d.\n", pthread_self(), d);
		}
	}
	
	sem_wait(&scounter);
	searchcounter -= 1;
	if (searchcounter == 0) {
		sem_post(&no_searcher);
	}
	sem_post(&scounter);
}

void delete(struct list_head *head, int d)
{	

	if(sem_trywait(&no_searcher) < 0){
		printf("Deleter %lu: Is blocked.\n", pthread_self());
		sem_wait(&no_searcher);
	}
	if(sem_trywait(&no_inserter) < 0){
		printf("Deleter %lu: Is blocked.\n", pthread_self());
		sem_wait(&no_inserter);
	}
	if(sem_trywait(&no_deleter) < 0){
		printf("Deleter %lu: Is blocked.\n", pthread_self());
		sem_wait(&no_deleter);
	}
	struct list_node *iterator, *iteratorp;
	iteratorp = NULL;
	int pos = 0;
	int deleted = 0;
	if (head->head == NULL) {
		//print that the head is empty #thread num
		printf("Deleter %lu: Head is empty.\n",pthread_self());
		deleted = 1;
	}
	for (iterator = head->head; iterator != NULL; iteratorp = iterator, iterator = iterator->next) {
		if (iterator->data == d) {
			if (iteratorp == NULL) {
				head->head = iterator->next;
				free(iterator);
				//print that the thread deleted:
				printf("Deleter %lu: Deleted %d.\n",pthread_self(), d);
				deleted = 1;
				break;
			}
			else {
				iteratorp->next = iterator->next;
				free(iterator);
				//prints that the thread deleted:
				printf("Deleter %lu: Deleted %d.\n",pthread_self(), d);
				deleted = 1;
				break;
			}
		}
	}
	
	//prints that the thread could not find x
	if (deleted == 0) {
		printf("Deleter %lu: Could not find %d.\n",pthread_self(), d);
	}
	//signal noinsert and nosearcher
	sem_post(&no_searcher);
	sem_post(&no_inserter);
	sem_post(&no_deleter);
}

void *inserter(void *h) {
	int x;
	struct list_head *head = (struct list_head *)h;
	while(1) {
		x  = rand() % 10;
		insert(head, x);
		sleep(rand()%4);
	}
}

void *searcher(void *h) {
	int x;
	struct list_head *head = (struct list_head *)h;
	while(1) {
		x = rand() % 10;
		search(head, x);
		sleep(rand()%4);
	}
}

void *deleter(void *h) {
	int x;
	struct list_head *head = (struct list_head *)h;
	while(1) {
		x =  rand() % 10;
		delete(head, x);
		sleep(rand()%4);
	}
}


int main()
{	
	sem_init(&no_inserter, 0, 1);
	sem_init(&no_searcher, 0, 1);
	sem_init(&no_deleter, 0, 1);
	sem_init(&scounter, 0, 1);
	sem_init(&icounter, 0, 1);
	struct list_head *head;
	head = malloc(sizeof(struct list_head));
	int i;
	srand(time(NULL));
	pthread_t i_threads[3];
	pthread_t s_threads[3];
	pthread_t d_threads[3];
	for (i = 0; i < 3; i++) {
		pthread_create(&i_threads[i], NULL, inserter, head);
	}
	for (i = 0; i < 3; i++) {
		pthread_create(&s_threads[i], NULL, searcher, head);
	}
	for (i = 0; i < 3; i++) {
		pthread_create(&d_threads[i], NULL, deleter, head);
	}
	for (i = 0; i < 3; i++) {
		pthread_join(i_threads[i],NULL);
	}
	for (i = 0; i < 3; i++) {
		pthread_join(s_threads[i],NULL);
	}
	for (i = 0; i < 3; i++) {
		pthread_join(d_threads[i],NULL);
	}
	return 0;
}
