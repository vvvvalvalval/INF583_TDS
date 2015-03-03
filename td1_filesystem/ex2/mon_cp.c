#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include "file_manip.h"

#define BUFFER_SIZE 20

/* Gestion des erreurs dans les arguments */
#define ARGUMENTS_ERROR(MESSAGE) \
  { \
    fprintf(stderr,"Erreur " MESSAGE ".\n"); \
    fprintf(stderr,"Usage: mon_cp [-r] source destination\n"); \
    return -1; \
  }


/* Fonction utile à partir de l'exercice 4 pour déterminer le nom
   du fichier destination */
char * create_copy_name(char* source, char* destination)
{
  char * basename = strrchr(source,'/');
  if (basename == NULL)
    basename = source;
  char * dirname  = destination;
  char * filename = malloc(sizeof(char) * (strlen(basename) + 1 +
                                           strlen(dirname)  + 1  ));
  if (filename == NULL)
    {
      perror("Malloc Failed ");
      return NULL;
    }
  *filename='\0';
  strcat(filename,dirname);
  strcat(filename,"/");
  strcat(filename,basename);
  return filename;
}


    
/* Copie de fichier */
int copy(char* source, char* destination)
{
  int res = 0;
  int nb_data_read = 0;

  printf("Copie du fichier %s sur le fichier %s\n",source,destination);
  
  //Allocation de la mémoire tampon pour transférer des données du
  //fichier source sur le fichier destination  
  char * buf = malloc(sizeof(char)*BUFFER_SIZE);
  if (buf == NULL)
    {
      perror("Malloc Failed ");
      return -1;
    }

  //Ouverture des fichiers source et destination respectivement en
  //lecture et en écriture
  MY_FILE * source_fd = my_fopen(source,"r");
  if (source_fd == NULL)
    {
      perror("Opening source Failed");
      return -1;
    }
  MY_FILE * destination_fd = my_fopen(destination,"w");
  if (destination_fd == NULL)
    {
      perror("Opening destination Failed");
      return -1;
    }

  //On boucle tant qu'il y a quelque chose à lire
  //On copie ensuite ce qui vient d'être lu dans le fichier destination
  //On prend soin de ne copier que ce qui vient d'être lu
  //(nb_data_read != BUFFER_SIZE) !
  errno=0;
  while ((nb_data_read = my_fread(buf,1,BUFFER_SIZE,source_fd)))
    {
      if (errno)
        {
          perror("my_fread failed");
      return -1;
        }
      res = my_fwrite(buf,1,nb_data_read,destination_fd);
      //on verifie les erreurs eventuelles sur errno pour my_fwrite
      if (nb_data_read != res)
        {
          perror("my_frite failed");
          return -1;
        }
    }
  //on verifie les erreurs eventuelles sur errno pour my_fread
  if (errno)
    {
      perror("my_fread failed");
      return -1;
    }
  
  //On ferme les fichiers source et destination
  if (my_fclose(source_fd) != 0)
    {
      perror("Closing source Failed");
      return -1;
    }
  if (my_fclose(destination_fd) != 0)
    {
      perror("Closing destination Failed");
      return -1;
    }

  //On libere la mémoire occupée par le buffer
  free(buf);
  
  return 0;
}


/* Programme principal*/
int main(int argc, char* argv[])
{
  int error_code = 0;
  /* Gestion des paramètres */
  char * source;
  char * destination;
  struct stat stat_file_source;
  struct stat stat_file_destination;
  switch(argc)
    {
    case 3:
      /* cas des exercices 1 à 3 */
      /* des modifications sont requises pour l'exercice 4*/
      source = argv[1];
      destination = argv[2];
      
      if (stat(source, &stat_file_source) == -1)
        {
          perror("Stat on source Failed");
          return -1;
        }
      if (!S_ISREG(stat_file_source.st_mode))
        ARGUMENTS_ERROR("Le fichier source doît être un fichier régulier");
      
      if (stat(destination, &stat_file_destination) == -1)
        {
          if (errno != ENOENT)
            {
              perror("Stat on destination Failed");
              return -1;
            }
        }
      else if ((stat_file_destination.st_dev == stat_file_source.st_dev) &&
               (stat_file_destination.st_ino == stat_file_source.st_ino))
        {
          ARGUMENTS_ERROR("Les fichiers source et destination sont identiques");
        }
      else if (S_ISDIR(stat_file_destination.st_mode))
        {
          destination = create_copy_name(source, destination);
          if (destination == NULL)
            {
              perror("Malloc failed ");
              return -1;
            }
        }

      error_code = copy(source, destination);
      break;
    case 4:
      /* cas de l'exercice 5*/
      if (strcmp(argv[1],"-r"))
        ARGUMENTS_ERROR("seul l'option -r est reconnue");
      source = argv[2];
      destination = argv[3];
      break;
    default:
      ARGUMENTS_ERROR("nombre d'arguments invalide");
    }
  return error_code;
}
