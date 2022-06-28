#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define MAX_LEN 1000
#define MESSAGE_1 "dfghjkl\n"
#define MESSAGE_2 "dc m,kloiuhg;kjhbv\n"

void analyse_status(int stat_loc);

int main(void)
{
    pid_t childpid1, childpid2;
    int fd[2];
    if (pipe(fd) == -1)
    {
        perror("Cant pipe.\n");
        return EXIT_FAILURE;
    }
    if ((childpid1 = fork()) == -1)
    {
        perror("Cant fork\n");
        exit(EXIT_FAILURE);
    }
    else if (childpid1 == 0)
    {
        close(fd[0]);
        write(fd[1], MESSAGE_1, strlen(MESSAGE_1) + 1);
        printf("First message sent to parent.\n");
        exit(EXIT_SUCCESS);
    }
    if ((childpid2 = fork()) == -1)
    {
        perror("Cant fork\n");
        exit(EXIT_FAILURE);
    }
    else if (childpid2 == 0)
    {
        close(fd[0]);
        write(fd[1], MESSAGE_2, strlen(MESSAGE_2) + 1);
        printf("Second message sent to parent.\n\n");
        exit(EXIT_SUCCESS);
    }
    if (childpid1 && childpid2)
    {
        int stat_loc;
        pid_t childpid;
        char mes1[MAX_LEN], mes2[MAX_LEN];

        childpid = wait(&stat_loc);
        printf("chiilpid: %d stat_loc: %d\n", childpid, stat_loc);
        analyse_status(stat_loc);

        childpid = wait(&stat_loc);
        printf("chiilpid: %d stat_loc: %d\n", childpid, stat_loc);
        analyse_status(stat_loc);

        close(fd[1]);
        read(fd[0], mes1, strlen(MESSAGE_1) + 1);
        read(fd[0], mes2, strlen(MESSAGE_2) + 1);
        printf("First message: %s\n", mes1);
        printf("Second message: %s\n", mes2);

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
