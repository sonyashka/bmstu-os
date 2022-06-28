#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>	  // syslog, openlog
#include <sys/stat.h> // umask
#include <unistd.h>
#include <pthread.h>
#include <time.h> // time, ctime

#include <sys/time.h>
#include <sys/resource.h>

// LOCKFILE и LOCKMODE - 562 стр.
#define LOCKFILE "/var/run/daemon.pid"
#define LOCKMODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
// S_IRUSR - user-read – дос­туп­но поль­зо­ва­те­лю для чте­ния
// S_IWUSR - user-write – дос­туп­но поль­зо­ва­те­лю для за­пи­си
// S_IRGRP - group-read – дос­туп­но груп­пе для чте­ния
// S_IROTH - other-read – дос­туп­но ос­таль­ным для чте­ния

sigset_t mask;


int lockfile(int fd)
{

    struct flock fl;

    fl.l_type = F_WRLCK;	// Блокируем разделение записи
    fl.l_start = 0;			// Смещение относительно l_whence равно 0 (Т.е. начало файла)
    fl.l_whence = SEEK_SET; /*с начала файла*/
    fl.l_len = 0;			// 0 - Значит до конца файла.

    return (fcntl(fd, F_SETLK, &fl));
}


int already_running(void)
{
    int fd;
    char buf[16];

    // O_CREAT | O_RDWR - от­крыть или создать для чте­ния и для за­пи­си
    fd = open(LOCKFILE, O_RDWR | O_CREAT, LOCKMODE);
    if (fd < 0)
    {
        syslog(LOG_ERR, "невозможно открыть %s: %s", LOCKFILE, strerror(errno));
        exit(1);
    }
    if (lockfile(fd) < 0)
    {
        if (errno == EACCES || errno == EAGAIN)
        {
            close(fd);
            return (1);
        }
        syslog(LOG_ERR, "невозможно установить блокировку на %s: %s", LOCKFILE, strerror(errno));
        exit(1);
    }

    ftruncate(fd, 0);

    sprintf(buf, "%ld", (long)getpid());
    write(fd, buf, strlen(buf) + 1);
    return (0);
}


void quit(const char *errMessage, const char *command) {
    syslog(LOG_ERR, "%s %s", errMessage, command);
    exit(1);
}


void daemonize(const char *cmd)
{
    int i, fd0, fd1, fd2;
    pid_t pid;
    struct rlimit rl;
    struct sigaction sa;

    umask(0); // user mask

    if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
        quit("%s: невозможно получить максимальный номер дескриптора ", cmd);


    if ((pid = fork()) < 0)
        quit("%s: ошибка вызова функции fork", cmd);
    else if (pid != 0)
        exit(0);
    if (setsid() < 0)
        quit("%s: ошибка вызова функции setsid", cmd);

    sa.sa_handler = SIG_IGN;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) < 0)
        quit("%s: невозможно игнорировать сигнал SIGHUP", cmd);


    if (chdir("/") < 0)
        quit("%s: невозможно сделать текущим рабочим каталогом /", cmd);


    if (rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 1024;
    for (int i = 0; i < rl.rlim_max; i++)
        close(i);


    fd0 = open("/dev/null", O_RDWR);

    fd1 = dup(0);
    fd2 = dup(0);


    openlog(cmd, LOG_CONS, LOG_DAEMON);
    if (fd0 != 0 || fd1 != 1 || fd2 != 2)
    {
        syslog(LOG_ERR, "ошибочные файловые дескрипторы %d %d %d",
               fd0, fd1, fd2);
        exit(1);
    }
}

void *thr_fn(void *arg)
{
    int err, signo;
    for (;;)
    {

        err = sigwait(&mask, &signo);
        if (err != 0)
        {
            syslog(LOG_ERR, "ошиб­ка вы­зо­ва функ­ции sigwait");
            exit(1);
        }
        switch (signo)
        {
        case SIGHUP:
            syslog(LOG_INFO, "Получен сигнал SIGHUP getlogin = %s", getlogin());
            // reread();
            break;
        case SIGTERM:
        {
            // SIGTERM (от англ. signal и terminate — завершить) — сигнал, применяемый в POSIX-системах для запроса завершения процесса
            syslog(LOG_INFO, "Получен сигнал SIGTERM; вы­ход");
            remove("/var/run/daemon.pid");
            exit(0);
            }
        default:
            syslog(LOG_INFO, "по­лу­чен не­пред­ви­ден­ный сиг­нал %d\n", signo);
        }
    }
    return (0);
}

int main(int argc, char *argv[])
{
    int err;
    pthread_t tid;
    char *cmd;

    struct sigaction sa;

    daemonize("my_daemon");

    if (already_running() != 0)
    {
        // syslog - от­прав­ля­ет со­об­ще­ния че­рез со­кет до­ме­на UNIX – /dev/log.
        syslog(LOG_ERR, "Демон уже запущен!\n");
        exit(1);
    }

    sa.sa_handler = SIG_DFL;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGHUP, &sa, NULL) < 0)
        quit("%s: не­воз­мож­но вос­ста­но­вить дей­ст­вие SIG_DFL для SIGHUP", "");

    sigfillset(&mask);

    if ((err = pthread_sigmask(SIG_BLOCK, &mask, NULL)) != 0)
        quit("ошиб­ка вы­пол­не­ния опе­ра­ции SIG_BLOCK", "");


    err = pthread_create(&tid, NULL, thr_fn, 0);
    if (err != 0)
        quit("не­воз­мож­но соз­дать по­ток", "");

    syslog(LOG_WARNING, "Проверка пройдена!");

    long int ttime;

    ttime = time(NULL);

    while (1)
    {
        syslog(LOG_INFO, "Демон! Время: %s", ctime(&ttime));
        sleep(3);
    }
}
