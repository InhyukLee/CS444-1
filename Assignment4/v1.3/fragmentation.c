#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>


int main(void){
    float ft = ((float)syscall(353)/(float)syscall(354));


    printf("free space: %lu\n",syscall(353));
    printf("total space: %lu\n\n",syscall(354));

    printf("free/total: %f\n",ft);

    return 0;
}