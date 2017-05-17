/**************************************
 * Alessandro Lim / Kevin Turkington
 * Group 13-07
 * CS444
 * Concurrency 3
 **************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
//GLOBALS

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

void *inserter(struct list_head *head, int d)
{
	//wait for noInserter and insertswitch
	struct list_node *iterator;
	struct list_node *temp;
	if (head->head == NULL) {
		head->head = (struct list_node*)malloc(sizeof(struct list
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
	return;
	//signal noInserter and insertswitch
}

void *searcher(struct list_head *head, int d)
{
	//wait for nosearcher
	struct list_node *iterator;
	if (head->head == NULL) {
		//print that the thread couldn't find d
	}
	else {
		iterator = head->head;
		while (iterator != NULL) {
			if (iterator->data == d) {
				//print that the thread has found d
				return;
			}
			else {
				iterator =  iterator->next;
			}
		}
		//print that the thread could not find d
		return;
	}
	//signal nosearcher
}

void *deleter(struct list_head *head, int d)
{
	//wait for nosearcher and noinserter
	struct list_node *iterator, *iteratorp;
	iteratorp = NULL;
	int pos = 0;
	if (head->head == NULL) {
		//print that the head is empty #thread num
		printf("Deleter %d: Head is empty.\n",pthread_self());
		return;
	}
	for (iterator = head->head; iterator != NULL; iteratorp = iterator, iterator = iterator->next) {
		if (iterator->dat == d) {
			if (iteratorp == NULL) {
				head->head = iterator->next;
				free(iterator);
				//print that the thread deleted:
				printf("Deleter %d: Deleted %d.\n",pthread_self(), d);
				return;
			}
			else {
				iteratorp->next = iterator->next;
				free(iterator);
				//prints that the thread deleted:
				printf("Deleter %d: Deleted %d.\n",pthread_self(), d);
				return;
			}
		}
	}
	//prints that the thread could not find x
	//signal noinsert and nosearcher
}

int main()
{	
	return 0;
}