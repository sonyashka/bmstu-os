#include <fcntl.h>

int main()
{
  char c;
  int fd1 = open("alphabet.txt",O_RDONLY);
  int fd2 = open("alphabet.txt",O_RDONLY);
  int rc1 = 1, rc2 = 1;
  while (rc1 == 1 && rc2 == 1)
  {
    if ((rc1 = read(fd1,&c,1)) == 1)
      write(1,&c,1);
    if ((rc2 = read(fd2,&c,1)) == 1)
      write(1,&c,1);
  }
  return 0;
}