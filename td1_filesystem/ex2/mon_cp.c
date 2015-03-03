#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 20

/* Gestion des erreurs dans les arguments */
#define ARGUMENTS_ERROR(MESSAGE) \
  { \
    fprintf(stderr,"Erreur " MESSAGE ".\n"); \
    fprintf(stderr,"Usage: mon_cp [-r] source destination\n"); \
    return -1; \
  }

/* Fonction utile à partir de l'exercice 4 pour déterminer
   le nom du fichier destination */
char * create_copy_name(char* source, char* destination)
{
  char * basename = strrchr(source,'/');
  if (basename == NULL)
    basename = source;
  char * dirname  = destination;
  char * filename = malloc(sizeof(char) * (strlen(basename) + 1 +
                                           strlen(dirname)  + 1  ));
  *filename = '\0';
  strcat(filename,dirname);
  strcat(filename,"/");
  strcat(filename,basename);
  return filename;
}

/* Copie de fichier */
/* Version de l'exercice 1 sans detection des erreurs */
int copy(char* source, char* destination)
{
  int res = 0;
  int nb_data_read = 0;

  printf("Copie du fichier %s sur le fichier %s\n",source,destination);
  
  //Allocation de la mémoire tampon pour transférer des données du
  //fichier source sur le fichier destination  
  char * buf = malloc(sizeof(char)*BUFFER_SIZE);

  //Ouverture des fichiers source et destination respectivement en
  //lecture et en écriture
  FILE * source_fd = fopen(source,"r");
  FILE * destination_fd = fopen(destination,"w");

  //On boucle tant qu'il y a quelque chose à lire
  //On copie ensuite ce qui vient d'être lu dans le fichier destination
  //On prend soin de ne copier que ce qui vient d'être lu
  //(nb_data_read != BUFFER_SIZE) !
  while ((nb_data_read = fread(buf,1,BUFFER_SIZE,source_fd)))
    res = fwrite(buf,1,nb_data_read,destination_fd);
  
  //On ferme les fichiers source et destination
  fclose(source_fd);
  fclose(destination_fd);

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
  switch(argc)
    {
    case 3:
      /* cas des exercices 1 à 3 */
      /* des modifications sont requises pour l'exercice 4*/
      source = argv[1];
      destination = argv[2];
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

