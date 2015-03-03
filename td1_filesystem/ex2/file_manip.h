#ifndef FILE_MANIP_H
#define FILE_MANIP_H

/* Inclusion du fichier unistd.h pour la definition de size_t */
#include <unistd.h>

/* Définition de la structure gérant le flux associé à un fichier */
typedef struct 
{
  int fd;
} MY_FILE;

/* Definition des prototypes de fonctions  */
extern MY_FILE *my_fopen(const char *, const char *);
extern size_t my_fread(void *, size_t, size_t, MY_FILE *);
extern size_t my_fwrite(const void *, size_t, size_t, MY_FILE *);
extern int my_fclose(MY_FILE *);

#endif

