#include <stdio.h>
#include <fcntl.h>
#include <pthread.h>

typedef struct thread_args
{
  int *fd;
  pthread_mutex_t *mutex;
} thread_args_t;

void run_thread(thread_args_t *args)
{
  pthread_mutex_lock(args->mutex);
  char c;
  int rc = 1;
  while (rc == 1)
  {
    if ((rc = read(args->fd,&c,1)) == 1)
      write(1,&c,1);
  }
  pthread_mutex_unlock(args->mutex);
}

int main()
{
  char c;    
  pthread_t thread;
  int fd1 = open("alphabet.txt",O_RDONLY);
  int fd2 = open("alphabet.txt",O_RDONLY);
  pthread_mutex_t mutex;
  pthread_mutex_init(&mutex, NULL);

  thread_args_t args;
  args.fd = fd2;
  args.mutex = &mutex;
  pthread_create(&thread, NULL, run_thread, &args);
  pthread_mutex_lock(&mutex);
  int rc = 1;
  while (rc == 1)
  {
    if ((rc = read(fd1,&c,1)) == 1)
      write(1,&c,1);
  }
  pthread_mutex_unlock(&mutex);
  pthread_join(thread, NULL);
  pthread_mutex_destroy(&mutex);
  return 0;
}