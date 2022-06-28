#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

#include "read.h"

void ReadFile(char fileName[MAX_LEN_CATALOG],  FILE * f_out, void (*myPrint)(char* , FILE *))
{
	char buf[BUF_SIZE];
	int len;

	FILE *f = fopen(fileName, FILE_READ);

	// Возвращает кол-во действительно прочитанных объектов.
	while ((len = fread(buf, 1, BUF_SIZE, f)) > 0)
	{	
		for (int i = 0; i < len; i++)
			if (buf[i] == 0)
				buf[i] = 10; 	// EOF
		buf[len] = 0; 			// '\0'
		myPrint(buf, f_out);
	}

	fclose(f);
}

void ReadSoftLink(char fileName[MAX_LEN_CATALOG],  FILE * f_out, void (*myPrint)(char* , FILE *))
{
	char buf[BUF_SIZE];
	int len;

	FILE *f = fopen(fileName, FILE_READ);
	len = readlink(fileName, buf, BUF_SIZE);
	buf[len] = '\0';
	myPrint(buf, f_out);

	fclose(f);
}

void ReadDir(char fileName[MAX_LEN_CATALOG],  FILE * f_out, void (*myPrint)(char* , FILE *))
{
	DIR *dir = opendir(fileName);
    struct dirent *readDir;

	char buf[BUF_SIZE];
	char string[PATH_MAX];
    char path[10000] = {'\0'};

    while ((readDir = readdir(dir)) != NULL)
    {
        if ((strcmp(readDir->d_name, ".") != 0) && (strcmp(readDir->d_name, "..") != 0))
        {
			if (strstr(fileName, "task") != NULL)
				snprintf(buf, BUF_SIZE, "%s->...\n", readDir->d_name);
			else
			{
				sprintf(path, "%s/%s", fileName, readDir->d_name);
            	int _read_len = readlink(path, string, PATH_MAX);

				snprintf(buf, BUF_SIZE, "%s->%s\n", readDir->d_name, string);
			}
            myPrint(buf, f_out);
        }
    }

    closedir(dir);
}