#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include "file_manip.h"


#define BUFFER_SIZE 20

/* Gestion des erreurs dans les arguments */
#define ARGUMENTS_ERROR(MESSAGE) \
  { \
    fprintf(stderr,"Erreur " MESSAGE ".\n"); \
    fprintf(stderr,"Usage: mon_cp [-r] source destination\n"); \
    return -1; \
  }

/* Fonction utile à partir de l'exercice 4 pour déterminer le nom du
   fichier destination */
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
      perror("Malloc Failed in create_copy_name ");
      exit(-1);
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
  if (my_fclose(source_fd) !=0)
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

/* Copie de répertoires */
int copy_dir(char* source_dir, char* destination_dir)
{
  //On ouvre le repertoire source
  DIR * dir = opendir(source_dir);
  if (dir == NULL)
    {
      perror("Opening source directory Failed");
      return -1;
    }

  struct dirent * file_dirent;
  struct stat file_stat;
  char * source_file;
  char * destination_file;
  //On parcourt tous les fichiers du repertoire source
  while ( (file_dirent = readdir(dir))  )
    {
      //On construit le nom du fichier source
      source_file = create_copy_name(file_dirent -> d_name,source_dir);
      printf("Find file %s\n",source_file);

      //On verifie le type de fichier
      stat(source_file, &file_stat);
      //Si c est un fichier regulier
      if (S_ISREG(file_stat.st_mode))
        {
          //On construit le nom du fichier destination
          destination_file = create_copy_name(source_file, destination_dir);
          printf("-> will be copied in  %s\n",destination_file);
          
          //On appel notre fonction copie habituelle
          if (copy(source_file, destination_file) == -1)
            break;

          free(destination_file);
        }
      //Si c est un fichier repertoire et qu il ne s agit pas de . et ..
      else if (S_ISDIR(file_stat.st_mode) && strcmp(file_dirent -> d_name,".") && strcmp(file_dirent -> d_name,".."))
        {
          //On construit le nom du repertoire destination
          destination_file = create_copy_name(source_file, destination_dir);
          printf("-> creating directory %s\n",destination_file);
          //On cree le repertoire destination
          if (mkdir(destination_file,S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH) == -1)
            {
              perror("Mkdir Failed ");
              return -1;
            }
          //On copie alors tous ses fichiers en faisant un appel recursif a la fonction de copie des repertoires
          printf("-> will be copied in  %s\n",destination_file);
          if (copy_dir(source_file, destination_file) == -1)
            return -1;
          free(destination_file);
        }
      free(source_file);
    }
  if (errno)
    {
      perror("Reading dir Failed");
      closedir(dir);
      return -1;
    }
  closedir(dir);
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
      source = argv[1];
      destination = argv[2];
      /* cas des exercices 1 à 3 */
      /* des modifications sont requises pour l'exercice 4*/
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
        destination = create_copy_name(source, destination);


      error_code = copy(source, destination);
      break;
    case 4:
      /* cas de l'exercice 5*/
      if (strcmp(argv[1],"-r"))
        ARGUMENTS_ERROR("seul l'option -r est reconnue");
      
      //On verifie que la source est bien un repertoire
      source = argv[2];
      //On execute l appel system stat pour connaitre le type de la destination
      if (stat(source, &stat_file_source) == -1)
        {
          perror("Stat Failed ");
          return -1;
        }
      //Si ce n est pas un repertoire on affiche une erreur
      if (!S_ISDIR(stat_file_source.st_mode))
        ARGUMENTS_ERROR("Le fichier source doît être un fichier répertoire lorsque l'option -r est spécifiée");

      //On verifie que la destination est bien un repertoire
      destination = argv[3];
      //On execute l appel system stat pour connaitre le type de la destination
      if (stat(destination, &stat_file_destination) == -1)
        {
          perror("Stat Failed ");
          return -1;
        }
      //Si ce n est pas un repertoire on affiche une erreur
      if (!S_ISDIR(stat_file_destination.st_mode))
        ARGUMENTS_ERROR("Le fichier destination doît être un fichier répertoire lorsque l'option -r est spécifiée");
      
      //On a bien 2 repertoires : on effectue la copie
      error_code = copy_dir(source, destination);
      break;
    default:
      ARGUMENTS_ERROR("nombre d'arguments invalide");
    }
  return error_code;
}
