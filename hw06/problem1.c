#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: problem1.out [count]\n");
        return -1;
    }
    
    int count = atoi(argv[1]);
    struct timeval tv[2];
    struct timezone tz;
    
    gettimeofday(&tv[0], &tz); // get time before doing stuffs
    for (int i = 0; i < count; ++i)
        read(STDOUT_FILENO, NULL, 0);
    gettimeofday(&tv[1], &tz); // get time after doing stuffs
    
    time_t diff_sec = tv[1].tv_sec - tv[0].tv_sec;
    suseconds_t diff_usec = tv[1].tv_usec - tv[0].tv_usec;
    printf("Time taken for doing zero byte read system call for %d : %ld.%06ld second\n", count, diff_sec, diff_usec);
    return 0;
}