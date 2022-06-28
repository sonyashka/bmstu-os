#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#define TIME_SLEEP 10
int main(void)
{
    pid_t childpid1, childpid2;
    if ((childpid1 = fork()) == -1)
    {
        perror("Cant fork\n");
        exit(EXIT_FAILURE);
    }
    else if (childpid1 == 0)
    {
        sleep(TIME_SLEEP);
        printf("\nChild1:    id   |  pgrp | ppid\n");
        printf("          %d |  %d | %d\n", getpid(), getppid(), getpgrp());
        exit(EXIT_SUCCESS);
    }
    if ((childpid2 = fork()) == -1)
    {
        perror("Cant fork\n");
        exit(EXIT_FAILURE);
    }
    else if (childpid2 == 0)
    {
        sleep(TIME_SLEEP);
        printf("\nChild2:    id   |  pgrp | ppid\n");
        printf("          %d |  %d | %d\n", getpid(), getppid(), getpgrp());
        exit(EXIT_SUCCESS);
    }
    printf("Parent:    id   |  pgrp | child1 | child2\n");
    printf("          %d | %d |  %d |  %d\n", getpid(), getpgrp(), childpid1, childpid2);
    printf("Parent-process finished.\n");
    return EXIT_SUCCESS;
}
