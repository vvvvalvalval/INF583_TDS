#define _POSIX_C_SOURCE 1

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>

int my_system(char * command)
{
  //On gere le cas special ou command est NULL
  if (command == NULL)
    {
      return !access("/bin/sh", X_OK);
    }


  //Prepare les arguments pour execv
  char * arguments[] = 
    {
      "/bin/sh",
      "-c",
      command,
      (char *) NULL
    };

  // Valeur de retour de la fonction my_system()
  int r = 0;
  
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
  if (sigaction(SIGQUIT,&act_ignore,&sigquit_oact) )
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
    case -1:
      perror("Fork failed");
      r = -1; 
      break;
    case 0:
      //Cas du fils
      //On restore les signaux dans le fils
      sigprocmask(SIG_SETMASK,&omask,NULL);
      sigaction(SIGQUIT,&sigquit_oact,NULL);
      sigaction(SIGINT,&sigint_oact,NULL);
      //Solution with execl
      //execl("/bin/sh","/bin/sh","-c",command,(char *) NULL);
      //Solution with execve
      execv("/bin/sh",arguments);
      
      //This call will never be called unless execl failed
      exit(127); 
      break;
    default:
      //Cas du père
      if (waitpid(pid, &status, 0) == -1)
	      {
      	  perror("Wait failed");
      	  r = -1;
          goto restore_signal_stuff;
      	}
      if (!WIFEXITED(status))
	      {
	        fprintf(stderr,"Child terminated abnormally\n");
      	  r = -1;
          goto restore_signal_stuff;
      	}      
      r = WEXITSTATUS(status);
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

int main(int argc, char* argv[])
{
  int res;
  if (argc != 2)
    {
      fprintf(stderr,"Error: wrong number of arguments.\n");
      fprintf(stderr,"Usage: %s command.\n",argv[0]);
      fprintf(stderr,"Usage: %s \"echo Long life to INF583.\".\n",argv[0]);
      return -1;
    }
  res = my_system(argv[1]);
  return res;
}
