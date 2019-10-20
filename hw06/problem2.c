#define _GNU_SOURCE
#include <stdio.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sched.h>

// Check time delay when "context switching" is occurred.
#define PERR_EXIT(msg) do { perror(msg), exit(EXIT_FAILURE);\
                            } while(0);

// in my environment (pretier cloud9 instance -> cpu per core 1, core per socket 1, socket 1)
// with and without "sched_setaffinity call", results are equal.

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: problem2.out [count]\n");
        return -1;
    }
    
    int count = atoi(argv[1]);
    struct timeval start, end;
    struct timezone tz;
    
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(0, &set);
    
    int pipefd[2][2];
    if (pipe(pipefd[0]) == -1) PERR_EXIT("pipe");
    if (pipe(pipefd[1]) == -1) PERR_EXIT("pipe");
    
    switch(fork())
    {
    case -1:
        PERR_EXIT("fork");
        break;
    case 0: // child
        if (sched_setaffinity(getpid(), sizeof(cpu_set_t), &set) == -1)
            PERR_EXIT("sched_setaffinity");
        
        for (int i = 0; i < count; ++i)
        {
            read(pipefd[0][0], NULL, 0);
            write(pipefd[1][1], NULL, 0);
        }
        break;
    default: // parent
        if (sched_setaffinity(getpid(), sizeof(cpu_set_t), &set) == -1)
            PERR_EXIT("sched_setaffinity");
        
        gettimeofday(&start, &tz);
        for (int i = 0; i < count; ++i)
        {
            write(pipefd[0][1], NULL, 0);
            read(pipefd[1][0], NULL, 0);
        }
        gettimeofday(&end, &tz);
        
        printf("Context switch %f microsecond\n", (float)(end.tv_sec * 1000000 + end.tv_usec - start.tv_sec * 1000000 - start.tv_usec) / (float)count);
        wait(NULL);
        break;
    }
    return 0;
}