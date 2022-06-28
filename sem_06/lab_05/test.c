#include <stdio.h>
#include <fcntl.h>
#define FILE_NAME "out.txt"

int main()
{
    struct stat st1, st2;
    FILE *f1 = fopen(FILE_NAME, "w");
    stat(FILE_NAME, &st1);
    printf("F1 OPEN: inode = %d, size = %d\n", st1.st_ino, st1.st_size);
    FILE *f2 = fopen(FILE_NAME, "w");
    stat(FILE_NAME, &st2);
    printf("F2 OPEN: inode = %d, size = %d\n", st2.st_ino, st2.st_size);

    for (char c = 'a'; c <= 'z'; c++)
    {
        if (c % 2)
            fprintf(f1, "%c", c);
        else
            fprintf(f2, "%c", c);
    }
    fclose(f1);
    stat(FILE_NAME, &st1);
    printf("F1 CLOSE: inode = %d, size = %d\n", st1.st_ino, st1.st_size);
    fclose(f2);
    stat(FILE_NAME, &st2);
    printf("F2 CLOSE: inode = %d, size = %d\n", st2.st_ino, st2.st_size);
    return 0;
}