#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
  pid_t pid;
  int status, options = 0;

  pid = fork();
  if(pid==-1)
  {
    perror("fork() error!");
  }
  if(pid==0)
  {
    sleep(10);
  }
  else
  {
    waitpid(pid, &status, options);
    printf("My child's process Id is : %d\n",pid);
    if (!WIFEXITED(status))
      printf("Parent: child has not terminated normally.\n");
  }
  return 0;
}
