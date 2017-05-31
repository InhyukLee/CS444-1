#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

int main(void){

    printf("free space: %lu\n",syscall(353));
    printf("total space: %lu\n\n",syscall(354));

    printf("free/total: %lu\n",syscall(353)/syscall(354));

    return 0;
}