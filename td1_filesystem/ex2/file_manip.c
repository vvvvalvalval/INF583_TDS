#include "file_manip.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

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
      errno = EINVAL;
      return NULL;
    }


  //Ouverture du fichier et récupération du descripteur
  int res = open(path,flags,permission);
  if (res == -1)
    return NULL;

  //Allocation de la mémoire pour notre structure
  MY_FILE* file_info = malloc(sizeof(MY_FILE));
  if (file_info != NULL)
    file_info->fd = res;
  return file_info;
}

//This version of my_fread checks for read interruption
//This algorithm was presented in the teaching
size_t my_fread(void *ptr, size_t size, size_t nmemb, MY_FILE *stream)
{
  int count = size*nmemb;
  int nb_data_read = 0;
  int nb_total_data_read = 0;
  errno = 0;
  while ( (nb_data_read = read(stream->fd, ptr, count)) != 0 )
    {
      if (nb_data_read == -1)
	{
	  if (errno == EINTR) {
            errno = 0;
	    continue;
          }
	  else
	    return 0;
	}
      nb_total_data_read += nb_data_read;
      ptr = (char*)ptr + nb_data_read;
      count -= nb_data_read;
    }
  return nb_total_data_read / size;
}

//This version of my_fread checks for read interruption
//This algorithm was presented in the teaching
size_t my_fwrite(const void *ptr, size_t size, size_t nmemb, MY_FILE *stream)
{
  int count = size*nmemb;
  int nb_data_write = 0;
  int nb_total_data_write = 0;
  errno = 0;
  while ( (nb_data_write = write(stream->fd, ptr, count)) != 0 )
    {
      if (nb_data_write == -1)
        {
          if (errno == EINTR) {
            errno = 0;
            continue;
          }
          else
            return 0;
        }
      nb_total_data_write += nb_data_write;
      ptr = (char*)ptr + nb_data_write;
      count -= nb_data_write;
    }
  return nb_total_data_write / size;
}

int my_fclose(MY_FILE *fp)
{
  //On ferme le fichier
  int res = close (fp->fd);
  //On libere la mémoire
  free(fp);
  return  res; 
}

