#include "file_manip.h"
#include <stdio.h>

MY_FILE *my_fopen(const char *path, const char *mode)
{
  //Ne gerez que les mode d'ouverture 'r' et 'w', les autres ne sont
  //pas utiles pour votre programme mon_cp
  switch (*mode)
    {
    case 'r':
      break;
    case 'w':
      break;
    default: 
      fprintf(stderr,"Désolé ce mode d'accès n'est pas supporté.\n");
      return NULL;
    }
  return NULL;
}

size_t my_fread(void *ptr, size_t size, size_t nmemb, MY_FILE *stream)
{
  return 0;
}

size_t my_fwrite(const void *ptr, size_t size, size_t nmemb, MY_FILE *stream)
{
  return 0;
}

int my_fclose(MY_FILE *fp)
{
  return 0;
}

