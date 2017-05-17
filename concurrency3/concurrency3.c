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
	if(head->head == NULL) {
		return;
	}
	else{
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
	struct list_node *temp;
	if(head->head == NULL) {
		//print that the thread couldn't find d
	}
	else{
		iterator = head->head;
		while(iterator != NULL) {
			if iterator
		}
	}
	//signal nosearcher
}

void *deleter(struct list_head *head, int d)
{
	//wait for nosearcher and noinserter
	
	//signal noinsert and nosearcher
}

int main()
{
	return 0;
}