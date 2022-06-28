// proc - предоставляет различную системную информацию.
// https://www.opennet.ru/man.shtml?topic=proc&category=5&russian=0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"
#include "print.h"
#include "read.h"

void WrapperForOutput(char catalog[MAX_LEN_CATALOG], char *curr_name, void (*myPrint)(char *, FILE *), FILE *f_out)
{
	int str_end_index = strlen(catalog);
	char buf[BUF_SIZE];
	strcpy(buf, catalog);

	strcat(catalog, curr_name);
	fprintf(f_out, "File: " "%s" "\nContent:\n", catalog);
	if (strcmp(curr_name, "pagemap") == 0)
		printPAGEMAP(buf, f_out);
    else if ((strcmp(curr_name, "cwd") == 0) || (strcmp(curr_name, "exe") == 0) || (strcmp(curr_name, "root") == 0))
        ReadSoftLink(catalog, f_out, myPrint);
    else if ((strcmp(curr_name, "fd") == 0) || (strcmp(curr_name, "task") == 0))
        ReadDir(catalog, f_out, myPrint);
    else
	    ReadFile(catalog, f_out, myPrint);

	catalog[str_end_index] = 0;
	fprintf(f_out, "\n\n");
}

int main(int argc, char *argv[])
{
	char catalog[MAX_LEN_CATALOG] = "/proc/";
	char curr_catalog[MAX_LEN_CATALOG];

	if (argc != COUNT_ARGS)
		strcat(catalog, "self/");
	else
		strcat((strcat(catalog, argv[PID])), "/");

	strcpy(curr_catalog, catalog);

	FILE *f_out = fopen(FILE_RESULT, FILE_WRITE);

	WrapperForOutput(curr_catalog, "stat", PrintStat, f_out);

	WrapperForOutput(curr_catalog, "environ", SimplePrint, f_out);

	WrapperForOutput(curr_catalog, "cmdline", SimplePrint, f_out);

    WrapperForOutput(curr_catalog, "cwd", SimplePrint, f_out);

    WrapperForOutput(curr_catalog, "exe", SimplePrint, f_out);

    WrapperForOutput(curr_catalog, "root", SimplePrint, f_out);

    WrapperForOutput(curr_catalog, "fd", SimplePrint, f_out);

	WrapperForOutput(curr_catalog, "maps", SimplePrint, f_out);

    WrapperForOutput(curr_catalog, "io", SimplePrint, f_out);

    // WrapperForOutput(curr_catalog, "pagemap", SimplePrint, f_out);

    WrapperForOutput(curr_catalog, "comm", SimplePrint, f_out);

    WrapperForOutput(curr_catalog, "task", SimplePrint, f_out);

	// STATM предоставляет информацию о состоянии памяти в страницах, как единицах измерения.
	// WrapperForOutput(curr_catalog, "statm", PrintStatm, f_out);

	fclose(f_out);
	return OK;
}