#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>
#define FILE_NAME "outThread.txt"

void run_thread(FILE *f)
{
    for (char c = 'b'; c <= 'z'; c += 2)
    {
        fprintf(f, "%c", c);
    }
}

int main()
{
    pthread_t thread;
    struct stat st1, st2;
    FILE *f1 = fopen(FILE_NAME, "w");
    stat(FILE_NAME, &st1);
    printf("F1 OPEN: inode = %d, size = %d\n", st1.st_ino, st1.st_size);
    FILE *f2 = fopen(FILE_NAME, "w");
    stat(FILE_NAME, &st2);
    printf("F2 OPEN: inode = %d, size = %d\n", st2.st_ino, st2.st_size);

    pthread_create(&thread, NULL, run_thread, f2);
    for (char c = 'a'; c <= 'z'; c += 2)
    {
        fprintf(f1, "%c", c);
    }
    
    pthread_join(thread, NULL);
    fclose(f1);
    stat(FILE_NAME, &st1);
    printf("F1 CLOSE: inode = %d, size = %d\n", st1.st_ino, st1.st_size);
    fclose(f2);
    stat(FILE_NAME, &st2);
    printf("F2 CLOSE: inode = %d, size = %d\n", st2.st_ino, st2.st_size);
    return 0;
}