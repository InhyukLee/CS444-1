#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>


int main(void){
    long free_sp = syscall(353);
    long total_sp = syscall(354); 

    float ft = ((float)free_sp/(float)total_sp);


    printf("free space: %lu\n",free_sp);
    printf("total space: %lu\n\n",total_sp);

    printf("free/total: %f\n",ft);

    return 0;
}