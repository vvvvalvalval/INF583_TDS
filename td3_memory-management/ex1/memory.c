#define _POSIX_C_SOURCE 1

#include <stdio.h>
#include <stdlib.h>

//#define DEBUG_CHECK
//#define DEBUG_ENTER

#define AUCUN_AFFICHAGE 0
#define AFFICHAGE 1
#define AFFICHAGE_DEFAUT 0


#define SIZE 0
#define NEXT 4
#define MYMEM(x,offset) (*(int*)(&mem[x + offset]))
/* +4 lecture des donnes dans une zone allouée */
#define MYMEM_DATA(x,offset) (*(char*)(&((char*)mem)[x + 4 + offset]))

#define PTR_TO_ARRAY(ptr) ((char*)ptr) - ((char*)mem) - 4
#define ARRAY_TO_PTR(i) (char*)mem + i + 4

#define POOL_SIZE (1 << 24)

static char mem[POOL_SIZE] = { '\0' }; /* Reservation de 16MO */

static int tete = 0; /* Tete de liste de zones libres */
static int total = 0; /* Total des zones allouees */
static int premiere_fois = 1;

int memory_check(char);

#define MEMORY_ERROR(M) \
  { \
    if (affichage == AUCUN_AFFICHAGE) { \
      memory_check(AFFICHAGE); \
    } \
    else { \
      fprintf(stderr,"Erreur memoire : " M "\n"); \
      exit(EXIT_FAILURE); \
    } \
  }

void memory_check_allocated(int debut,int fin,int *taille_allouee,
                            char affichage)
{
  int taille;
  int suivant = debut;
  do {
      taille = MYMEM(suivant, SIZE);
      if ( affichage ) {
        fprintf(stderr,"[1;31m[41m      [0m \t%d\t%d\t%d\n",
                       suivant,suivant + taille,taille);
      }
      if ( taille == 0 ) {
        MEMORY_ERROR("Découverte d'une zone allouée de"
                     "taille 0 ce qui est impossible.");
      }
      suivant += taille;
      *taille_allouee += taille;
  } while (suivant < fin);
  if ( suivant != fin ) {
    MEMORY_ERROR("La fin de cette zone de mémoire allouée"
                 "n'est pas contiguë à une zone de mémoire libre");
  }
}

int memory_check(char affichage )
{
  //int taille_totale = 0;
  int taille_allouee = 0;
  int taille_libre = 0;
  int taille =0, courant, suivant;
  int debut, fin;

  /* Cas 1: la tete vaut -1 : il n'y a pas de memoire libre ! */
  if (tete == -1 ) {
    memory_check_allocated(0,0+POOL_SIZE,&taille_allouee, affichage);
  }
  /* Cas 2: il y a de la mémoire libre il faut donc parcourir la liste*/
  else {
    /* La tete est differente de 0 (il y a de la memoire oqp en tete) */
    if (tete != 0) {
      memory_check_allocated(0,tete,&taille_allouee, affichage);
    }
    /* On parcourt la liste des zones libres */
    courant = -1;
    suivant = tete;
    while( suivant != -1 ) {
      courant = suivant;
      suivant = MYMEM(suivant,NEXT);

      taille = MYMEM(courant,SIZE);
      taille_libre += taille;

      if (affichage) {
        fprintf (stderr,"[1m[42m      [0m \t%d\t%d\t%d\n",
                        courant,courant+taille,taille);
      }

      if ( (suivant != -1) && (suivant <= courant) ) {
        MEMORY_ERROR("Découverte d'une zone libre pointant"
                     "vers une zone d'adresse inférieur.");
      }
    
      if (taille == 0) {
        MEMORY_ERROR("Découverte d'une zone libre de taille 0 ce"
                     "qui n'est pas supportée par l'implémentation.");
      }
    /* Si il y a un suivant on regarde si on peut l'atteindre correctement */
      if (suivant != -1) {
        debut = courant + taille;
        fin = suivant;
        memory_check_allocated(debut,fin,&taille_allouee, affichage);
      }
    }
    /* Il n'y a pas de suivant, mais il peut rester de la mémoire oqp après */
    if ( taille_libre + taille_allouee < POOL_SIZE ) {
      debut = courant + taille;
      fin = POOL_SIZE;
      memory_check_allocated(debut,fin,&taille_allouee,affichage);
    }
  }  
  /* Final Last check*/
  if ( taille_allouee != total ) {
    fprintf(stderr,"%d !=  %d\n",taille_allouee,total);
    MEMORY_ERROR("La taille totale de la mémoire allouée lors"
                 "de la vérification n'est pas égale à la mémoire"
                 "allouée théorique.");
  }
  if ( taille_allouee + taille_libre != POOL_SIZE ) {
    MEMORY_ERROR("La taille totale de la mémoire vérifiée"
                 "(libre + allouée) n'est pas égale à la mémoire"
                 "totale théorique.");
  }
  return 0;
}

/* --------------------------------------------------------------*/

void *my_pool_malloc(size_t taille) {
#ifdef DEBUG_ENTER
  fprintf(stderr,"-> enter malloc\n");
#endif

  if (taille == 0)
    return NULL;

  void *ptr_resultat = NULL;
  /* allouez la mémoire ici */




  /* total += taille; */
#ifdef DEBUG_CHECK
  memory_check(AFFICHAGE_DEFAUT);
#endif

  return ptr_resultat; 
}

void *malloc(size_t taille) 
{
  return my_pool_malloc(taille);
}

/* --------------------------------------------------------------*/

void my_pool_free(void *ptr) 
{
#ifdef DEBUG_ENTER
  fprintf(stderr,"-> enter free %p\n",ptr);
#endif

  if (ptr == NULL)
    return;
  
  /* libèrez la mémoire ici */

  /* total -= taille_ptr; */

#ifdef DEBUG_CHECK
  memory_check(AFFICHAGE_DEFAUT);
#endif
}

void free(void *ptr) 
{
  my_pool_free(ptr);
}


/* --------------------------------------------------------------*/

void *my_pool_realloc(void *ptr,size_t nouvelle_taille) 
{
#ifdef DEBUG_ENTER
  fprintf(stderr,"-> enter realloc %p %d\n", ptr, nouvelle_taille);
#endif

  /* Court-circuit pour faire les appels directs à malloc et à free */
  if ( ptr == NULL ) {
    return my_pool_malloc(nouvelle_taille);
  }
  else if ( nouvelle_taille == 0 ) {
    my_pool_free(ptr);
    return NULL;
  }

  void *ptr_resultat = NULL;
  /* réallouer la mémoire ici */

  /* total -= taille_ptr; */
  /* total += nouvelle_taille; */

#ifdef DEBUG_CHECK
  memory_check(AFFICHAGE_DEFAUT);
#endif

  return ptr_resultat;
}


void *realloc(void *ptr,size_t size) 
{
  return my_pool_realloc(ptr, size);
}

/* --------------------------------------------------------------*/

void *my_pool_calloc(size_t nmemb,size_t size) 
{
#ifdef DEBUG_ENTER
  fprintf(stderr,"-> enter calloc\n");
#endif 

  void *ptr_resultat = NULL;
  /* allouez la mémoire et initialisez la ici */

  /* total += nmemb * size; */

#ifdef DEBUG_CHECK
  memory_check(AFFICHAGE_DEFAUT);
#endif

  return ptr_resultat;
}


void *calloc(size_t nmemb,size_t size) {
  return my_pool_calloc(nmemb, size);
}

/* --------------------------------------------------------------*/

double my_pool_fragmentation() {
  int courant = tete;
  int max = 0,taille = 0;
  while (courant != -1) {
    taille = MYMEM(courant,SIZE);
    if (taille > max) max = taille;
    courant = MYMEM(courant,NEXT);
  }
  return total == 0 ? 0 : 1 - (double)taille / (POOL_SIZE - total);
}

