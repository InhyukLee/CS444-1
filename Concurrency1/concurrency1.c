/*****************************
** Joesph Struth / Kevin Turkington
** CS444
** Concurrency 1
*****************************/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "mt19937ar.h"
//GLOBALS
#define BUFFERSIZE 32

//shared buffer for data
struct DataContainer DC;

//Toggle for system type.
int X86SYSTEM;
//END GLOBALS

//reference: https://linux.die.net/man/3/sleep
//reference: https://www.guyrutenberg.com/2014/05/03/c-mt19937-example/
//reference: http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/MT2002/emt19937ar.html
//reference: https://www.cs.nmsu.edu/~jcook/Tools/pthreads/pc.c
//reference: http://nirbhay.in/blog/2013/07/producer_consumer_pthreads/
//reference: http://stackoverflow.com/questions/1288189/elegant-and-safe-way-to-determine-if-architecture-is-32bit-or-64bit

typedef struct
{
    short number;
    short wait;
} DATA;

struct DataContainer
{
    //index for DataContainers
    short consumerIdx;
    short producerIdx;
    DATA items[BUFFERSIZE];
    pthread_mutex_t shareLock;
    //Conditions to signal that an item is ready to consume
    //and that an item has been consumed and needs another
    pthread_cond_t consumerCond;
    pthread_cond_t producerCond;
};

void checkSystemType()
{
    //check if largest data type is bigger than 32 bits.
    if ((size_t)-1 > 0xffffffffUL)
    {
        X86SYSTEM = 0;//is 64bit
    }
    else{
        X86SYSTEM = 1;//is 32bit
    }
    //printf("::IS X86 SYSTEM: %d\n\n",X86SYSTEM);
}

int genRandomNumber(int floor, int ceiling)
{
    int num = 0;

    if(X86SYSTEM == 0)
    {
        //if 64 bit system fill num using merelene twister.
        num =  (int)genrand_int32();
    }
    else
    {
        //if x86 fill num with random value.
        asm("rdrand %0":"=r"(num));
    }

    num = abs(num % (ceiling - floor));
    if(num < floor)
    {
        return floor;
    }

    //printf("generating: %d\n", num);
    return num;
}

void printItem(DATA *myItem)
{
    static int i = 0;
    printf("%d:",i);
    i++;

    printf("\tItem number: %d\n", myItem->number);
    printf("\tItem wait: %d\n\n", myItem->wait);
}

void *consumerFunc(void *foo)
{
    while(1)
    {
        //lock shared buffer
        pthread_mutex_lock(&DC.shareLock);
        //item to be consumed
        DATA consumeItem;
        //resize if at max buffer size
        if(DC.consumerIdx >= BUFFERSIZE)
        {
            DC.consumerIdx = 0;
        }
        //signal producer consumer is ready
        pthread_cond_signal(&(DC.producerCond));
        //wait for a number from producer
        pthread_cond_wait(&(DC.consumerCond), &DC.shareLock);
        //if a consumer thread arrives while the buffer is empty
        //it blocks until a producer adds a new item.
        if(DC.producerIdx == 0)
        {
            printf("AT MAX size\n");
            pthread_cond_wait(&(DC.consumerCond), &DC.shareLock);
        }
        //get item to consume from buffer
        consumeItem = DC.items[DC.consumerIdx];
        //random waiting period
        sleep(consumeItem.wait);
        //consume item from buffer
        printf("Consuming Item data: %d\n\n", consumeItem.number);
        //increase the count of consumed items
        DC.consumerIdx++;
        //unlock shared buffer
        pthread_mutex_unlock(&DC.shareLock);
    }
}

void *producerFunc(void *foo)
{
    while(1)
    {
        //lock shared buffer
        pthread_mutex_lock(&DC.shareLock);
        //item to be produced
        DATA newItem;
        //data value and wait time using Mersenne Twister
        newItem.number = genRandomNumber(1, 100);
        newItem.wait = genRandomNumber(2,9);
        
        printf("Producing Item:\n");
        printItem(&newItem);
        //block until consumer removes an item
        if(DC.producerIdx == BUFFERSIZE)
        {
            printf("AT MAX size\n");
            //Signal consumer to begin processing data.
            pthread_cond_signal(&(DC.consumerCond));
            pthread_cond_wait(&(DC.producerCond), &DC.shareLock);
        }
        //add item to buffer or overwrite old item.
        DC.items[DC.producerIdx] = newItem;
        DC.producerIdx++;
        //tell consumer a new item is ready
        pthread_cond_signal(&(DC.consumerCond));
        //wait for consumer to consume
        pthread_cond_wait(&(DC.producerCond), &DC.shareLock);
        //resize if at max buffer size
        if(DC.producerIdx >= BUFFERSIZE)
        {
            printf("AT MAX size\n");
            DC.producerIdx = 0;
        }
        //shared buffer unlock
        pthread_mutex_unlock(&DC.shareLock);
    }

}

int main()
{
    checkSystemType();

    DC.consumerIdx = 0;
    DC.producerIdx = 0;

    //producer and consumer threads
    pthread_t threads[2];
    //index var
    int i = 0;
    //create threads
    pthread_create(&threads[0], NULL, consumerFunc, NULL);
    pthread_create(&threads[1], NULL, producerFunc, NULL);
    //join threads

    for(i = 0; i < 2; i++)
    {
        pthread_join(threads[i], NULL);
    }

    return 0;
}