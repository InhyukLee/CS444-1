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

//index for itemBuffer
int consumerNum = 0;
int producerNum = 0;

//Conditions to signal that an item is ready to consume
//and that an item has been consumed and needs another
pthread_cond_t consumerCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t producerCond = PTHREAD_COND_INITIALIZER;

//shared buffer for data
struct buffer itemBuffer;

//Toggle for system type.
int X86SYSTEM;

//END GLOBALS

//reference: https://linux.die.net/man/3/sleep
//reference: https://www.guyrutenberg.com/2014/05/03/c-mt19937-example/
//reference: http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/MT2002/emt19937ar.html
//reference: https://www.cs.nmsu.edu/~jcook/Tools/pthreads/pc.c
//reference: http://nirbhay.in/blog/2013/07/producer_consumer_pthreads/
//reference: http://stackoverflow.com/questions/1288189/elegant-and-safe-way-to-determine-if-architecture-is-32bit-or-64bit

struct item
{
    int number;
    int wait;
};

struct buffer
{
    struct item items[BUFFERSIZE];
    pthread_mutex_t shareLock;
};

void checkSystemType()
{
    //check if largest data type is bigger than 32 bits.
    if ((size_t)-1 > 0xffffffffUL
    ){
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

    //printf("generating: %d\n", num);
    num = abs(num % (ceiling - floor));
    if(num < floor)
    {
        return floor;
    }

    return num;
}

void printItem(struct item *myItem)
{
    printf("Item number: %d\n", myItem->number);
    printf("Item wait: %d\n", myItem->wait);
}

void *consumer(void *foo)
{
    while(1)
    {
        //lock shared buffer
        pthread_mutex_lock(&itemBuffer.shareLock);
        //item to be consumed
        struct item consumeItem;
        //signal producer consumer is ready
        pthread_cond_signal(&producerCond);
        //wait for a number from producer
        pthread_cond_wait(&consumerCond, &itemBuffer.shareLock);
        //if a consumer thread arrives while the buffer is empty
        //it blocks until a producer adds a new item.
        if(producerNum == 0)
        {
            printf("AT MAX size\n");
            pthread_cond_wait(&consumerCond, &itemBuffer.shareLock);
        }
        //get item to consume from buffer
        consumeItem = itemBuffer.items[consumerNum];
        //increase the count of consumed items
        consumerNum++;
        //random waiting period
        sleep(consumeItem.wait);
        //consume item from buffer
        printf("Consuming Item data: %d\n", consumeItem.number);
        //resize if at max buffer size
        if(consumerNum >= BUFFERSIZE)
        {
            consumerNum = 0;
        }
        //ready to consume again
        //pthread_cond_signal(&producerCond);
        //unlock shared buffer
        pthread_mutex_unlock(&itemBuffer.shareLock);
    }
}

void *producer(void *foo)
{
    while(1)
    {
        //lock shared buffer
        pthread_mutex_lock(&itemBuffer.shareLock);
        //item to be produced
        struct item newItem;
        //data value and wait time using Mersenne Twister
        newItem.number = genRandomNumber(1, 100);
        newItem.wait = genRandomNumber(2,9);
        printf("Producing Item:\n");
        printItem(&newItem);
        //block until consumer removes an item
        if(producerNum == 31)
        {
            printf("AT MAX size\n");
            //Signal consumer to begin processing data.
            pthread_cond_signal(&consumerCond);
            pthread_cond_wait(&producerCond, &itemBuffer.shareLock);
        }
        //add item to buffer
        itemBuffer.items[producerNum] = newItem;
        producerNum++;
        //tell consumer a new item is ready
        pthread_cond_signal(&consumerCond);
        //wait for consumer to consume
        pthread_cond_wait(&producerCond, &itemBuffer.shareLock);
        //resize if at max buffer size
        if(producerNum >= BUFFERSIZE)
        {
            printf("AT MAX size\n");
            producerNum = 0;
        }
        //ready to consume
        //pthread_cond_signal(&consumerCond);
        //shared buffer unlock
        pthread_mutex_unlock(&itemBuffer.shareLock);
    }

}

int main()
{
    checkSystemType();

    //producer and consumer threads
    pthread_t threads[2];
    //index var
    int i = 0;
    //create threads
    pthread_create(&threads[0], NULL, consumer, NULL);
    pthread_create(&threads[1], NULL, producer, NULL);
    //join threads

    for(i = 0; i < 2; i++)
    {
        pthread_join(threads[i], NULL);
    }

    return 0;
}