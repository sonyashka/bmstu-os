#include <stdio.h>

#include "constants.h"
#include "print.h"

char *stat_field_names[] = {
	"pid",	// %d Идентификатор процесса.
	"comm", // %s Имя исполняемого файла в круглых скобках.
	// Состояния:
	// R -- запущен,
	// S -- процесс "спит" в прерываемом ожидании,
	// D -- процесс ожидает в непрерываемом состоянии окончания дисковой операции,
	// Z -- процесс является зомби, или же
	// T -- процесс трассируется или остановлен (по сигналу) и
	// W -- процесс вызывается.
	"state",	   // %c Состояние процесса.
	"ppid",		   // %d Идентификатор (PID) родительского процесса.
	"pgrp",		   // %d Идентификатор группы процесса.
	"session",	   // %d Идентификатор сессии процесса.
	"tty_nr",	   // %d Терминал tty, который использует процесс.
	"tpgid",	   // %d Идентификатор группы процесса, который в настоящий момент владеет терминалом tty, к которому подключен данный процесс.
	"flags",	   // %lu Флаги процесса. Матеметический бит -- десятичное 4, бит трассировки -- десятичное 10.
	"minflt",	   // %lu Количество незначительных сбоев, которые возникли при работе процесса, и которые не требуют загрузки страницы памяти с диска.
	"cminflt",	   // %lu Количество незначительных сбоев, которые возникли при ожидании окончания работы процессов-потомков.
	"majflt",	   // %lu Количество значительных сбоев, которые возникли при работе процесса, и которые потребовали загрузки страницы памяти с диска.
	"cmajflt",	   // %lu Количество значительных сбоев, которые возникли при ожидании окончания работы процессов-потомков.
	"utime",	   // %lu Количество тиков (jiffies), которые получил процесс, выполняющийся в режиме пользователя.
	"stime",	   // %lu Количество тиков, которые данный процесс провел в режиме ядра.
	"cutime",	   // %ld Количество тиков, которые данный процесс и его потомки провели в режиме пользователя. (См. также times(2).)
	"cstime",	   // %ld Количество тиков, которые процесс, ожидающий завершения процессов-потомков, провёл в режиме ядра.
	"priority",	   // %ld Стандартная величина приоритета (для nice) плюс 15. Данное значение в ядре никогда не бывает отрицательным.
	"nice",		   // %ld Значение для nice в диапазоне от 19 (наиболее низкий приоритет) до -19 (наивысший приоритет).
	"num_threads", // Количество потоков
	"itrealvalue", // %ld Время (в тиках) перед следующей посылкой процессу SIGALRM из-за внутреннего таймера.
	"starttime",   // %lu Время (в тиках) запуска процесса после начальной загрузки системы.
	"vsize",	   // %lu Размер виртуальной памяти в байтах.
	// Резидентный Размер: количество страниц, которые занимает процесс в реальной памяти,
	//  минус 3 для административных целей. Это те страницы, которые заняты кодом, данными
	// и пространством стека. Сюда не включаются страницы, которые не были загружены
	// по требованию или которые находятся в своппинге.
	"rss",		   // %ld Resident Set Size: Количество страниц процесса в физической памяти.
	"rlim",		   // %lu Текущий лимит в байтах на резидентный размер процесса (обычно 4294967295 на i386).
	"startcode",   // %lu Адрес, выше которого может выполняться код программы.
	"endcode",	   // %lu Адрес, ниже которого может выполняться код программ.
	"startstack",  // %lu Адрес начала стека.
	"kstkesp",	   // %lu Текущее значение esp (указателя стека) для данного процесса, которое следует из страницы стека в ядре.
	"kstkeip",	   // %lu Текущее значение EIP (instruction pointer) (указатель инструкций).
	"signal",	   // %lu Карта ожидающих сигналов (обычно 0).
	"blocked",	   // %lu Карта блокированных сигналов (обычно 0, или 2 для командных интерпретаторов).
	"sigignore",   // %lu Карта игнорируемых сигналов.
	"sigcatch",	   // %lu Карта перехваченных сигналов.
	"wchan",	   // %lu Это "канал", в котором ожидает процесс.
	"nswap",	   // %lu Количество страниц на своппинге - не обслуживается.
	"cnswap",	   // %lu Суммарное nswap для процессов-потомков.
	"exit_signal", // %d Сигнал, который будет послан родителю, когда процесс умрёт.
	"processor"	   // %d Номер процессора, на котором последний раз выполнялся процесс.
};

// statm предоставляет информацию о состоянии памяти в страницах, как единицах измерения.
char *statm_field_names[] = {
	"size",		// общий размер программы
	"resident", // размер резидентной части
	"share",	// разделяемые страницы
	"trs",		// текст (код)
	"drs",		// данные/стек
	"lrs",		// библиотека
	"dt"		// "дикие" (dirty) страницы
};

void PrintStatm(char *str, FILE *f_out)
{
	char *istr = strtok(str, " ");
	int j = 0;

	while (istr != NULL && j < 7)
		fprintf(f_out, "%-16s", statm_field_names[j++]);

	fprintf(f_out, "\n");
	j = 0;

	while (istr != NULL && j < 7)
	{
		fprintf(f_out, "%-16s", istr);
		istr = strtok(NULL, " ");
	}
}

static int i = 0;
void PrintStat(char *str, FILE *f_out)
{
	char *istr = strtok(str, " ");

	while (istr != NULL && i < 37)
	{
		// fprintf (f_out, "%-2d %-16s %-16s\n", i, stat_field_names[i++], istr);
		fprintf(f_out, "%-16s %-16s\n", stat_field_names[i++], istr);
		istr = strtok(NULL, " ");
	}
}

void SimplePrint(char *str, FILE *f_out)
{
	fprintf(f_out, "%s", str);
}

int pagemap_get_entry(PagemapEntry *entry, int pagemap_fd, uintptr_t vaddr)
{
    size_t nread;
    ssize_t ret;
    uint64_t data;

    nread = 0;
    while (nread < sizeof(data)) {
        ret = pread(pagemap_fd, ((uint8_t*)&data) + nread, sizeof(data) - nread,
                (vaddr / sysconf(_SC_PAGE_SIZE)) * sizeof(data) + nread);
        nread += ret;
        if (ret <= 0) {
            return 1;
        }
    }
    entry->pfn = data & (((uint64_t)1 << 55) - 1);
    entry->soft_dirty = (data >> 55) & 1;
    entry->file_page = (data >> 61) & 1;
    entry->swapped = (data >> 62) & 1;
    entry->present = (data >> 63) & 1;
    return 0;
}

void printPAGEMAP(char *curr_catalog, FILE *f_out)
{
    char buffer[BUF_SIZE];
    char maps_file[BUF_SIZE];
    char pagemap_file[BUF_SIZE];
    int maps_fd;
    int offset = 0;
    int pagemap_fd;

    // fprintf(f, "addr = Если страница присутствует в ОЗУ (бит 63), то эти биты обеспечивают номер страничного фрейма, Если страница присутствует в свопе (бит 62), затем биты 4–0 задают тип подкачки, а биты 54–5 кодируют смещение подкачки.\npfn \nsoft-dirty \nfile/shared = Страница представляет собой страницу с отображением файлов или общую анонимную страницу \nswapped = Если установлено, страница находится в пространстве подкачк \npresent = Если установлено, страница находится в оперативной памяти.\n\n");

    snprintf(maps_file, sizeof(maps_file), "%s/maps", curr_catalog);
    snprintf(pagemap_file, sizeof(pagemap_file), "%s/pagemap", curr_catalog);
    maps_fd = open(maps_file, O_RDONLY);
    if (maps_fd < 0) {
        perror("open maps");
        // return EXIT_FAILURE;
    }
    pagemap_fd = open(pagemap_file, O_RDONLY);
    if (pagemap_fd < 0) {
        perror("open pagemap");
        // return EXIT_FAILURE;
    }
    fprintf(f_out, "addr pfn soft-dirty file/shared swapped present library\n");
    for (;;) {
        ssize_t length = read(maps_fd, buffer + offset, sizeof buffer - offset);
        if (length <= 0) break;
        length += offset;
        for (size_t i = offset; i < (size_t)length; i++) {
            uintptr_t low = 0, high = 0;
            if (buffer[i] == '\n' && i) {
                const char *lib_name;
                size_t y;
                {
                    size_t x = i - 1;
                    while (x && buffer[x] != '\n') x--;
                    if (buffer[x] == '\n') x++;
                    while (buffer[x] != '-' && x < sizeof buffer) {
                        char c = buffer[x++];
                        low *= 16;
                        if (c >= '0' && c <= '9') {
                            low += c - '0';
                        } else if (c >= 'a' && c <= 'f') {
                            low += c - 'a' + 10;
                        } else {
                            break;
                        }
                    }
                    while (buffer[x] != '-' && x < sizeof buffer) x++;
                    if (buffer[x] == '-') x++;
                    while (buffer[x] != ' ' && x < sizeof buffer) {
                        char c = buffer[x++];
                        high *= 16;
                        if (c >= '0' && c <= '9') {
                            high += c - '0';
                        } else if (c >= 'a' && c <= 'f') {
                            high += c - 'a' + 10;
                        } else {
                            break;
                        }
                    }
                    lib_name = 0;
                    for (int field = 0; field < 4; field++) {
                        x++;
                        while(buffer[x] != ' ' && x < sizeof buffer) x++;
                    }
                    while (buffer[x] == ' ' && x < sizeof buffer) x++;
                    y = x;
                    while (buffer[y] != '\n' && y < sizeof buffer) y++;
                    buffer[y] = 0;
                    lib_name = buffer + x;
                }
                /* Get info about all pages in this page range with pagemap. */
                {
                    PagemapEntry entry;
                    for (uintptr_t addr = low; addr < high; addr += sysconf(_SC_PAGE_SIZE)) {
                        /* TODO always fails for the last page (vsyscall), why? pread returns 0. */
                        if (!pagemap_get_entry(&entry, pagemap_fd, addr)) {
                            fprintf(f_out, "%jx %jx %u %u %u %u %s\n",
                                (uintmax_t)addr,
                                (uintmax_t)entry.pfn,
                                entry.soft_dirty,
                                entry.file_page,
                                entry.swapped,
                                entry.present,
                                lib_name
                            );
                        }
                    }
                }
                buffer[y] = '\n';
            }
        }
    }
    close(maps_fd);
    close(pagemap_fd);
    // return EXIT_SUCCESS;
}