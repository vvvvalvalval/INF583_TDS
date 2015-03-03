#include "file_manip.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

MY_FILE *my_fopen(const char *path, const char *mode)
{
  int flags;
  mode_t permission;

  //Ne gerez que les mode d'ouverture 'r' et 'w', les autres ne sont
  //pas utiles pour votre programme mon_cp
  switch (*mode)
    {
    case 'r':
      flags = O_RDONLY;
      break;
    case 'w':
      flags =  O_CREAT|O_WRONLY|O_TRUNC;
      permission = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
      break;
    default: 
      fprintf(stderr,"Désolé ce mode d'accès n'est pas supporté.\n");
      return NULL;
    }

  //Allocation de la mémoire pour notre structure
  MY_FILE* file_info = malloc(sizeof(MY_FILE));

  //Ouverture du fichier et récupération du descripteur
  file_info->fd = open(path,flags,permission);
  return file_info;
}

size_t my_fread(void *ptr, size_t size, size_t nmemb, MY_FILE *stream)
{
  return read(stream->fd,ptr,size*nmemb) / size;
}

size_t my_fwrite(const void *ptr, size_t size, size_t nmemb, MY_FILE *stream)
{
  return write(stream->fd,ptr,size*nmemb)  / size;
}

int my_fclose(MY_FILE *fp)
{
  //On ferme le fichier
  int res = close (fp->fd);
  //On libere la mémoire
  free(fp);
  return  res; 
}

