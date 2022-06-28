#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define TIME_SLEEP 1

void analyse_status(int stat_loc);

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
        printf("          %d | %d | %d\n", getpid(), getppid(), getpgrp());
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
        printf("          %d | %d | %d\n\n", getpid(), getppid(), getpgrp());
        exit(EXIT_SUCCESS);
    }
    if (childpid1 && childpid2)
    {
        int stat_loc;
        pid_t childpid;

        childpid = wait(&stat_loc);
        printf("chiilpid: %d stat_loc: %d\n", childpid, stat_loc);
        analyse_status(stat_loc);

        childpid = wait(&stat_loc);
        printf("chiilpid: %d stat_loc: %d\n", childpid, stat_loc);
        analyse_status(stat_loc);

        printf("Parent:    id   |  pgrp | child1 | child2\n");
        printf("          %d | %d |  %d |  %d\n", getpid(), getpgrp(), childpid1, childpid2);
        printf("Parent-process finished.\n");
    }
    return EXIT_SUCCESS;
}

void analyse_status(int stat_loc)
{
    if (WIFEXITED(stat_loc))
    {
        printf("Child-process finished normally.\n");
        printf("Child-process termination code %d.\n\n", WEXITSTATUS(stat_loc));
        return;
    }
    if (WIFSIGNALED(stat_loc))
    {
        printf("Child-process terminates with un-intercepted signal.\n");
        printf("Signal number %d.\n\n", WTERMSIG(stat_loc));
        return;
    }
    if (WIFSTOPPED(stat_loc))
    {
        printf("Child-process stoped.\n");
        printf("Signal number %d.\n\n", WSTOPSIG(stat_loc));
    }
}
