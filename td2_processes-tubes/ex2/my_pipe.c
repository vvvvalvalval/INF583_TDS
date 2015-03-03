#define _POSIX_C_SOURCE 1

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

#define READ_FD 0
#define WRITE_FD 1

#define BUFFER_SIZE 256

#define MY_CALL(F) \
  { \
    if ((F) == -1) \
      { \
        fprintf(stderr,"Error File: '%s'\n" \
                       "Function:'%s'\n" \
                       "Line:'%d'\n",__FILE__, __FUNCTION__, __LINE__); \
        perror( #F " failed"); \
        exit(EXIT_FAILURE); \
      } \
  }

void reader(int pipe_fd[])
{

}


void writer(int pipe_fd[])
{

}


int main(int argc, char *argv[])
{
  int status;
  int pipe_fd[2];
  /* Creation du pipe */
  
  int pid = fork();
  switch(pid)
    {
    case -1:
      perror("Fork failed");
      return -1;
      break;
    case 0:
      /* Fils */
      reader(pipe_fd);
      break;
    default:
      /* Pere */
      writer(pipe_fd);
      MY_CALL(waitpid(pid, &status, 0))
      if (!WIFEXITED(status))
        {
          fprintf(stderr,"Child terminated abnormally\n");
          return -1;
        }      
      return WEXITSTATUS(status);
      break;
    }
  return 0;
}

