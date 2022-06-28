#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "stack.h"

#define RESET "\033[0m"
#define RED "\033[0;31m"
#define CYAN "\033[0;36m"
#define PURPLE "\033[0;35m"
#define YELLOW "\033[0;33m"

#define FTW_F 1 // Файл, не являющийся каталогом
#define FTW_D 2 // Каталог

#define PATH_MAX 1024

#define ERROR 1
#define SUCCESS 0

typedef int myfunc(const char *, int, int, ino_t);

static struct stack stk;

// Обход дерева каталогов
int doPath(myfunc *func, char *fullpath, int depth)
{
    if (depth < 0) // Возврат из просмотренного каталога
    {
        chdir(fullpath);
        return SUCCESS;
    }

    struct stat statbuf;
    struct dirent *dirp;
    DIR *dp;

    if (lstat(fullpath, &statbuf) == -1)
        return ERROR;

    if (!S_ISDIR(statbuf.st_mode))
    {
        // Это не каталог
        func(fullpath, FTW_F, depth, statbuf.st_ino);
        return SUCCESS;
    }

    // Это каталог
    func(fullpath, FTW_D, depth, statbuf.st_ino);

    if ((dp = opendir(fullpath)) == NULL) /* Каталог не доступен */
        return ERROR;

    if (chdir(fullpath) == -1)
    {
        closedir(dp);
        return ERROR;
    }

    depth++;

    // Элемент возврата
    struct stackItem item = {.fileName = "..", .depth = -1};
    push(&stk, &item);

    while ((dirp = readdir(dp)) != NULL)
    {
        // Пропуск . и ..
        if (strcmp(dirp->d_name, ".") != 0 && strcmp(dirp->d_name, "..") != 0)
        {
            strcpy(item.fileName, dirp->d_name);
            item.depth = depth;
            push(&stk, &item);
        }
    }

    if (closedir(dp) == -1)
        return ERROR;

    return SUCCESS;
}

// Первичный вызов для переданного программе каталога
static int myFtw(char *pathname, myfunc *func)
{
    // Меняем текущую директорию на переданную
    if (chdir(pathname) == -1)
    {
        printf("%sПереданного каталога не существует, либо он не доступен%s", RED, RESET);
        return ERROR;
    }

    init(&stk);

    struct stackItem item = {.depth = 0};
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == 0)
    {
        printf("%s Ошибка: невозможно получить путь рабочего каталога %s", RED, RESET);
        return ERROR;
    }

    strcpy(item.fileName, cwd);
    push(&stk, &item);

    while (!empty(&stk))
    {
        doPath(func, item.fileName, item.depth);
        item = pop(&stk);
    }

    return SUCCESS;
}

static int output(const char *pathname, int type, int depth, ino_t inode)
{
    printf(PURPLE);
    for (int i = 0; i < depth; i++)
        printf("    │");

    if (type == FTW_F)
        printf(CYAN);

    printf("    ├── %s%s", pathname, RESET);
    if (type == FTW_F)
        printf(" inode: %d\n", inode);
    else
        printf("\n");

    return SUCCESS;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("%sИспользование: %s <начальный_каталог>%s\n", RED, argv[0], RESET);
        return 1;
    }

    return myFtw(argv[1], output);
}
