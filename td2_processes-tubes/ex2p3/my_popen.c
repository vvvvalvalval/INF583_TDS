#define _POSIX_C_SOURCE 1

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>

#define READ_FD 0
#define WRITE_FD 1

#define BUFFER_SIZE 256

typedef struct
{
  int fd;
  pid_t son_pid;
} MY_FILE;
  


MY_FILE *my_popen (const char *command, const char *type)
{
  int pipe_fd[2];
  int father_used_id, father_unused_id, son_used_id, son_unused_id;
  MY_FILE * my_pipe;

  switch(*type)
    {
    case 'r':
      father_used_id = READ_FD;
      father_unused_id = WRITE_FD;
      son_used_id = WRITE_FD;
      son_unused_id = READ_FD;
      break;
    case 'w':
      father_used_id = WRITE_FD;
      father_unused_id = READ_FD;
      son_used_id = READ_FD;
      son_unused_id = WRITE_FD;
      break;
    default:
      errno = EINVAL;
      return NULL;
      break;
    }

  if (pipe(pipe_fd) == -1)
    return NULL;

  int pid = fork();
  switch(pid)
    {
    case -1:
      return NULL;
      break;
    case 0:
      //Fils
      
      break;
    default:
      //Pere
      
      break;
    }
  return my_pipe;
}

int my_pclose (MY_FILE *stream)
{
  int status;
  close(stream->fd);
  waitpid(stream->son_pid,&status,0);
  free(stream);
  return 0;
}


int main(int argc, char* argv[])
{
  char read_buffer[BUFFER_SIZE];
  int nb_data_read;

  MY_FILE *my_pipe_1 = my_popen("echo Vive inf 422","r");
  MY_FILE *my_pipe_2 = my_popen("sed s/422/583/","w");

  nb_data_read = read(my_pipe_1->fd,read_buffer,BUFFER_SIZE); 
  write(my_pipe_2->fd,read_buffer,nb_data_read); 

  my_pclose(my_pipe_1);
  my_pclose(my_pipe_2);

  return 0;
}
