#ifndef _PRINT_H_

#define _PRINT_H_

#include <stdint.h>
 
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef struct {
    uint64_t pfn : 55;
    unsigned int soft_dirty : 1;
    unsigned int file_page : 1;
    unsigned int swapped : 1;
    unsigned int present : 1;
} PagemapEntry;

void PrintStat(char *str, FILE * f_out);

void PrintStatm(char *str, FILE * f_out);

int pagemap_get_entry(PagemapEntry *entry, int pagemap_fd, uintptr_t vaddr);
void printPAGEMAP(char *curr_catalog, FILE *f_out);

void SimplePrint(char *str, FILE * f_out);

#endif // _PRINT_H_