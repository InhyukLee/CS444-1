/**************************************
 * Alessandro Lim / Kevin Turkington
 * CS444
 * Concurrency 1
 **************************************/

/**
 * reference: https://www.guyrutenberg.com/2014/05/03/c-mt19937-example/
 * reference: https://linux.die.net/man/3/pthread_cond_signal
 * reference: http://www.cs.cmu.edu/afs/cs/academic/class/15492-f07/www/pthreads.html
 * reference: http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/MT2002/emt19937ar.html
 * reference: https://codereview.stackexchange.com/questions/147656/checking-if-cpu-supports-rdrand
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "mt19937ar.h"
#define BUFFERSIZE 32
//GLOBALS
//shared buffer for data
struct DataContainer DC;
//Toggle for system type.
int X86SYSTEM;
//END GLOBALS

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

int checkSystemType()
{
    unsigned int eax = 0x01;
	unsigned int ebx;
	unsigned int ecx;
	unsigned int edx;

    __asm__ __volatile__(
	                     "cpuid;"
	                     : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
	                     : "a"(eax)
	                     );

    //check if largest data type is bigger than 32 bits.
    if (ecx & 0x40000000)
    {
        X86SYSTEM = 1;//is 32bit
    }
    else{
        X86SYSTEM = 0;//is 64bit
    }
    //printf("::IS X86 SYSTEM: %d\n\n",X86SYSTEM);

    return X86SYSTEM;
}

int genRandomNumber(int floor, int ceiling)
{
    int num = 0;

    if(X86SYSTEM == 0)
    {
        //if 64 bit system fill num using merelene twister.
        num =  (int)genrand_int32();//floor
    }
    else
    {
        //if x86 fill num with random value.
        __asm__ __volatile__("rdrand %0":"=r"(num));
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
        //lock thread exculsively to consumer thread.
        pthread_mutex_lock(&DC.shareLock);
        DATA consumeItem;
        //resize if at max buffer size
        if(DC.consumerIdx >= BUFFERSIZE)
        {
            DC.consumerIdx = 0;
        }

        //signal producer thread, that consumer is ready.
        pthread_cond_signal(&(DC.producerCond));
        //wait for producer to create a thread.
        pthread_cond_wait(&(DC.consumerCond), &DC.shareLock);
        
        //if the data buffer is empty, the consumer reaches 32
        // in data buffer, wait for producers to create an item.
        if(DC.producerIdx == 0)
        {
            printf("AT MAX size\n");
            pthread_cond_wait(&(DC.consumerCond), &DC.shareLock);
        }
        //Begin consuming item.
        consumeItem = DC.items[DC.consumerIdx];
        printf("Consuming Item data: %d\n\n", consumeItem.number);
        sleep(consumeItem.wait);
        //printf("Consumed Item data: %d\n\n", consumeItem.number);
        
        //increase the consumers position in the buffer.
        DC.consumerIdx++;
        // unlock data.
        pthread_mutex_unlock(&DC.shareLock);
    }
}

void *producerFunc(void *foo)
{
    while(1)
    {
        //lock thread exculsively to consumer thread.
        pthread_mutex_lock(&DC.shareLock);
        
        DATA newItem;
        //items number and wait time is randomly generated.
        newItem.number = genRandomNumber(1, 100);
        newItem.wait = genRandomNumber(2,9);
        
        //print item details.
        printf("Producing Item:\n");
        printItem(&newItem);

        //if producer position in data buffer is at max
        // signal to consumer to consume. and reset producer position.
        if(DC.producerIdx == BUFFERSIZE)
        {
            printf("AT MAX size\n");
            //Signal consumer to begin processing data.
            pthread_cond_signal(&(DC.consumerCond));
            pthread_cond_wait(&(DC.producerCond), &DC.shareLock);
        }

        //add item to buffer
        // or overwrite old item, if wrapping around in data buffer.
        DC.items[DC.producerIdx] = newItem;
        DC.producerIdx++;

        //signal to consumer, to consume.
        pthread_cond_signal(&(DC.consumerCond));
        pthread_cond_wait(&(DC.producerCond), &DC.shareLock);
        //resize if at max buffer size
        if(DC.producerIdx >= BUFFERSIZE)
        {
            printf("AT MAX size\n");
            DC.producerIdx = 0;
        }
        //unlock data.
        pthread_mutex_unlock(&DC.shareLock);
    }

}

int main(int argc,char* argv[])
{
    int thread_pair_count,i;

    if(argc <= 1){
        printf("Note: for multiple threads pairs run './concurrency1 5'\n\n");
        thread_pair_count = 1;
    }
    else{
        thread_pair_count = atoi(argv[1]);
    }

    checkSystemType();

    DC.consumerIdx = 0;
    DC.producerIdx = 0;

    //producer and consumer threads
    pthread_t threads[2 * thread_pair_count];
    
    for(i = 0; i < thread_pair_count; i++){
        //create threads
        pthread_create(&threads[i], NULL, consumerFunc, NULL);
        pthread_create(&threads[i+1], NULL, producerFunc, NULL);
    }

    //join threads
    for(i = 0; i < (2 * thread_pair_count); i++)
    {
        pthread_join(threads[i], NULL);
    }

    return 0;
}