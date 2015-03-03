#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
int copy(char* source, char* destination)
{
  printf("Copie du fichier %s sur le fichier %s\n",source,destination);
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
