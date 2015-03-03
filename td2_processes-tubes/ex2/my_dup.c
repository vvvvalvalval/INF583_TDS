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
  char read_buffer[BUFFER_SIZE];
  int nb_data_read;
 
  /* No need to write in the fifo, I am just a reader ! */
  MY_CALL(close(pipe_fd[WRITE_FD]));
  
  /* I read in the fifo ! */
  MY_CALL(nb_data_read = read(pipe_fd[READ_FD],read_buffer,BUFFER_SIZE-1));

  printf("The writer sent %d data : %s\n",nb_data_read ,read_buffer);
 
  /* I have finished to read in the fifo. */
  MY_CALL(close(pipe_fd[READ_FD]));
}


void writer(int pipe_fd[])
{
  char *message = "I am your father Luke!";

  /* No need to read in the fifo, I am just a writer ! */
  MY_CALL(close(pipe_fd[READ_FD]));

  /* I replace the standard output by my own fifo */
  MY_CALL(dup2(pipe_fd[WRITE_FD],WRITE_FD));
  
  /* I don't need my own fifo anymore, now the standard output
     is equivalent to it */
  MY_CALL(close(pipe_fd[WRITE_FD]));

  /* I write the message on stdout (which is now my fifo) */
  printf ("%s", message); 
  putc (0, stdout);

  /* I have to flush, libC is bufferised  !!! */
  fflush (stdout); 
}


int main(int argc, char* argv[])
{
  int pipe_fd[2];
  int status;
  MY_CALL(pipe(pipe_fd));

  int pid = fork();
  switch(pid)
    {
    case -1:
      perror("Fork failed");
      exit(EXIT_FAILURE);
      break;
    case 0:
      //Fils
      reader(pipe_fd);
      break;
    default:
      //Pere
      writer(pipe_fd);
      MY_CALL (waitpid(pid, &status, 0));
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
