#include <stdio.h>
#include <fcntl.h>
#include <pthread.h>

typedef struct thread_args
{
  FILE *f;
  pthread_mutex_t *mutex;
} thread_args_t;

void run_thread(thread_args_t *args)
{
  pthread_mutex_lock(args->mutex);
  printf("\nThread 2: ");
  int flag = 1;
  while (flag == 1)
  {
    char c;
    flag = fscanf(args->f,"%c",&c);
    if (flag == 1) 
    {
        fprintf(stdout,"%c",c);
    }
  }
  pthread_mutex_unlock(args->mutex);
}

int main()
{
  setbuf(stdout, NULL);
  pthread_t thread;
  int fd = open("alphabet.txt",O_RDONLY);

  FILE *fs1 = fdopen(fd,"r");
  char buff1[20];
  setvbuf(fs1,buff1,_IOFBF,20);

  FILE *fs2 = fdopen(fd,"r");
  char buff2[20];
  setvbuf(fs2,buff2,_IOFBF,20);

  pthread_mutex_t mutex;
  pthread_mutex_init(&mutex, NULL);

  thread_args_t args;
  args.f = fs2;
  args.mutex = &mutex;
  pthread_create(&thread, NULL, run_thread, &args);
  
  pthread_mutex_lock(&mutex);
  printf("Thread 1: ");
  int flag = 1;
  while(flag == 1)
  {
    char c;
    flag = fscanf(fs1,"%c",&c);
    if (flag == 1) 
    {
        fprintf(stdout,"%c",c);
    }
  }
  pthread_mutex_unlock(&mutex);
  pthread_join(thread, NULL);
  pthread_mutex_destroy(&mutex);
  return 0;
}