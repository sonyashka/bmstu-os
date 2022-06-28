#ifndef _READ_H_

#define _READ_H_

#include <stdio.h>
#include <stdint.h>

#include "constants.h"

void ReadFile(char fileName[MAX_LEN_CATALOG],  FILE * f_out, void (*myPrint)(char* , FILE *));
void ReadSoftLink(char fileName[MAX_LEN_CATALOG],  FILE * f_out, void (*myPrint)(char* , FILE *));
void ReadDir(char fileName[MAX_LEN_CATALOG],  FILE * f_out, void (*myPrint)(char* , FILE *));
void ReadPagemap(char fileName[MAX_LEN_CATALOG],  FILE * f_out, void (*myPrint)(char* , FILE *));

#endif // _READ_H_