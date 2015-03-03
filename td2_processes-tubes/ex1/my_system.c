#define _POSIX_C_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int my_system(char *command)
{
  int r = 0;
  //On gere le cas special ou command est NULL
  if (command == NULL)
    {
      return !access("/bin/sh", X_OK);
    }

  //Prepare les arguments pour execv
  char *arguments[] = 
    {
      "/bin/sh",
      "-c",
      command,
      (char *) NULL
    };
  
  //Ignore SIGINT et SIGQUIT
  struct sigaction act_ignore,sigint_oact,sigquit_oact;
  act_ignore.sa_handler = SIG_IGN;
  sigemptyset(&act_ignore.sa_mask);
  act_ignore.sa_flags = 0;
  if (sigaction(SIGINT,&act_ignore,&sigint_oact) == -1)
    {
      perror("Sigaction for SIGINT failed");
      return -1;
    }
  if (sigaction(SIGQUIT,&act_ignore,&sigint_oact) )
    {
      perror("Sigaction for SIGQUIT failed");
      r = -1;
      goto restore_sigint;
    }
  
  //Bloque SIGCHLD
  sigset_t mask,omask;
  sigemptyset(&mask);
  sigaddset(&mask,SIGCHLD);
  if (sigprocmask(SIG_BLOCK,&mask,&omask))
    {
      perror("Sigprocmask for SIGCHLD failed");
      r = -1;
      goto restore_sigquit;
    }

  //Si command n'est pas NULL on se prepare à l'utilisation de execv
  int pid = fork();
  int status;
  switch(pid)
    {
    case -1: //Erreur de fork ici
      perror("Fork failed");
      return -1; 
      break;
    case 0: //On est dans le fils ici
      //Cas du fils
      //On restore les signaux dans le fils
      sigprocmask(SIG_SETMASK,&omask,NULL);
      sigaction(SIGQUIT,&sigquit_oact,NULL);
      sigaction(SIGINT,&sigint_oact,NULL);
 
      break;
    default: //On est dans le pere ici
      break;
    }

restore_signal_stuff:
  if (sigprocmask(SIG_SETMASK,&omask,NULL))
    {
      perror("Cannot restore stuff for SIGCHLD");
      return -1;
    }
restore_sigquit:
  if (sigaction(SIGQUIT,&sigquit_oact,NULL) == -1)
    {
      perror("Cannot restore stuff for SIGQUIT");
      return -1;
    }
restore_sigint:
  if (sigaction(SIGINT,&sigint_oact,NULL) == -1)
    {
      perror("Cannot restore stuff for SIGINT");
      return -1;
    }

  return r;
}

int main(int argc, char *argv[])
{
  int res;
  if (argc != 2)
    {
      fprintf(stderr,"Error: wrong number of arguments.\n");
      fprintf(stderr,"Usage: %s command.\n",argv[0]);
      fprintf(stderr,"Usage: %s \"echo Long life to Inf583.\".\n",argv[0]);
      return -1;
    }
  res = my_system(argv[1]);
  return res;
}
