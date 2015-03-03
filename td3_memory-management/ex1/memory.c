#include <stdio.h>
#include <stdlib.h>

//#define DEBUG
//#define DEBUG_CHECK
//#define DEBUG_ENTER

#define AUCUN_AFFICHAGE 0
#define AFFICHAGE 1
#define AFFICHAGE_DEFAUT 0


#define SIZE 0
#define NEXT 4
#define MYMEM(x,offset)  (*(int*)(&mem[x+offset]))
#define MYMEM_DATA(x,offset)  (*(char*)(&((char*)mem)[x+4+offset])) //+4 lecture des donnes dans une zone allouée

#define PTR_TO_ARRAY(ptr) ((char*)ptr) - ((char*)mem) - 4
#define ARRAY_TO_PTR(i) (char*)mem + i + 4

#define POOL_SIZE (1<<24)

static char mem[POOL_SIZE] = { '\0' }; /* Reservation de 16MO */

static int tete = 0; /* Tete de liste de zones libres */
static int total = 0; /* Total des zones allouees */
static int premiere_fois = 1;



int memory_check(char affichage);

#define MEMORY_ERROR(M)					\
  {							\
    if (affichage == AUCUN_AFFICHAGE)			\
      memory_check(AFFICHAGE);				\
    else						\
      {							\
	fprintf(stderr,"Erreur memoire : " M "\n");	\
	exit(EXIT_FAILURE);				\
      }							\
  }



void memory_check_allocated(int debut, int fin, int*taille_allouee, char affichage)
{
  int taille;
  int suivant = debut;
  do
    {
     
      taille = MYMEM(suivant, SIZE);

      if (affichage)
	fprintf (stderr, "[1;31m[41m      [0m \t%d\t%d\t%d\n", suivant,suivant+taille,taille);

      if (taille == 0)
	MEMORY_ERROR("Découverte d'une zone allouée de taille 0 ce qui est impossible.");
      
      suivant += taille;
      *taille_allouee  += taille;
    }
  while (suivant < fin);
  if (suivant != fin)
    MEMORY_ERROR("La fin de cette zone de mémoire allouée n'est pas contiguë à une zone de mémoire libre");
}

int memory_check(char affichage )
{
  //int taille_totale = 0;
  int taille_allouee = 0;
  int taille_libre = 0;
  int taille =0, courant, suivant;
  int debut, fin;

  /* Cas 1: la tete vaut -1 : il n'y a pas de mÃ©moire libre ! */
  if (tete == -1)
    memory_check_allocated(0,0+POOL_SIZE,&taille_allouee, affichage);
  /* Cas 2: il y a de la mémoire libre il faut donc parcourir la liste*/
  else 
    {
      /* Etape 2.1: la tete est differente de 0 (il y a de la mÃ©moire oqp en tete) */
      if (tete != 0)
	memory_check_allocated(0,tete,&taille_allouee, affichage);
     
      /* Etape 2.2: on parcourt la liste des zones libres */
      courant = -1;
      suivant = tete;
      while (suivant != -1) 
	{
	  courant = suivant;
	  suivant = MYMEM(suivant, NEXT);

	  taille = MYMEM(courant, SIZE);
	  taille_libre += taille;


	  if (affichage)
	    fprintf (stderr, "[1m[42m      [0m \t%d\t%d\t%d\n", courant,courant+taille,taille);

	  if ((suivant != -1) &&  (suivant <= courant))
	    MEMORY_ERROR("Découverte d'une zone libre pointant vers une zone d'adresse inférieur.");
	  
	  if (taille == 0)
	    MEMORY_ERROR("Découverte d'une zone libre de taille 0 ce qui n'est pas supportée par l'implémentation.");

	  /* Etape 2.2.1: Si il y a un suivant on regarde si on peut l'atteindre correctement */
	  if (suivant != -1)
	    {
	      debut = courant + taille;
	      fin = suivant;
	      memory_check_allocated(debut,fin,&taille_allouee, affichage);
	    }
	}
      /* Etape 2.3:: il n'y a pas de suivant, mais il peut rester de la mémoire oqp après */
      if (taille_libre + taille_allouee < POOL_SIZE)
	{
	  debut = courant + taille;
	  fin = POOL_SIZE;
	  memory_check_allocated(debut,fin,&taille_allouee, affichage);
	}
    }
  /* Final Las check*/
  if (taille_allouee != total)
    {
      fprintf(stderr,"%d !=  %d\n",taille_allouee,total);
      MEMORY_ERROR("La taille totale de la mémoire allouée lors de la vérification n'est pas égale à la mémoire allouée théorique.");
    }
  if (taille_allouee + taille_libre != POOL_SIZE)
    MEMORY_ERROR("La taille totale de la mémoire vérifiée (libre + allouée) n'est pas égale à la mémoire totale théorique.");

  return 0;
}


/* -------------------------------------------------------------------------------*/
/* -------------------------------------------------------------------------------*/
/* -------------------------------------------------------------------------------*/
/* -------------------------------------------------------------------------------*/

void *my_pool_malloc(size_t taille) {
#ifdef DEBUG_ENTER
  fprintf(stderr,"-> enter malloc\n");
#endif 

  
  if (taille == 0)
    return NULL;

  if (premiere_fois) 
    {
      MYMEM(0,SIZE) = POOL_SIZE;
      MYMEM(0,NEXT) = -1;
      premiere_fois = 0;
    }


#ifdef DEBUG
  int original_taille = taille;
#endif
  taille = ((taille + 11) / 8) * 8;
  int ancien = -1, courant = tete;
  int trouve, libre;
  


  /* Recherche de la premiere zone libre assez grande */
  while (courant != -1) 
    {
      libre = MYMEM(courant, SIZE);
      if (libre >= taille)
	break;
      ancien = courant;
      courant = MYMEM(courant, NEXT);
    }
 
  /* Verifie la taille mémoire disponible */
  if (courant == -1) 
    {
      fprintf(stderr, "Pas assez de memoire (taille = %d)\n", (int)taille);
      exit(EXIT_FAILURE);
      return NULL;
    }
  
  trouve = courant;

  /* Fixe la liste des zones libres */
  /* Etape 1: besoin de creer un nouveau noeud courant */
  if (libre != taille)
    {
      //fprintf(stderr,"malloc 1.1\n");
      /* On calcul la position du noeud courant */
      courant += taille;
      /* On calcul la taille libre restante  */
      MYMEM(courant, SIZE) = libre - taille;
      /* On conserve la position du prochain noeud */
      MYMEM(courant, NEXT) = MYMEM(trouve, NEXT);
    }
  else
    {
      //fprintf(stderr,"malloc 1.2\n");
      /* si la taille libre est egale à la taille demandée
	 on n'a pas besoin d'un nouveau noeud */
    }
  
  /* Etape 2: on fixe la liste */
  /* Cas 2.1: pas de nouveau noeud (test 1) et on se trouve en tete de liste (test 2) */
  if ((libre == taille) && (ancien == -1))
    {
      //fprintf(stderr,"malloc 2.1\n");
      tete = MYMEM(trouve, NEXT);
    }
  /* Cas 2.2: pas de nouveau noeud (test 1) et on ne se trouve pas en tete de liste (test 2) */
  else if ((libre == taille) && (ancien != -1))
    {
      //fprintf(stderr,"malloc 2.2\n");
      MYMEM(ancien, NEXT) = MYMEM(trouve, NEXT);
    }
  /* Cas 2.3: un nouveau noeud (test 2) et on se trouve en tete de liste (test 2) */
  else if ((libre != taille) && (ancien == -1))
    {
      //fprintf(stderr,"malloc 2.3\n");
      tete = courant;
    }
  /* Cas 2.4: un nouveau noeud (test 2) et on se ne trouve pas  en tete de liste (test 2) */
  else if ((libre != taille) && (ancien != -1))
    {
      //fprintf(stderr,"malloc 2.4\n");
      MYMEM(ancien, NEXT) = courant;
    }
 
  /* Etape 3: Memorisation de la taille de la zone allouee */
  MYMEM(trouve, SIZE) =  taille;
  total += taille;

  /* Etape 4: Initialisation memoire */
  //for (int i=0; i<taille-4; i++)
  //MYMEM_DATA(trouve,i) = '\0';


  /* Calcul de l'adresse de la zone trouvee */
  void *ptr = ARRAY_TO_PTR(trouve);
  

#ifdef DEBUG
  fprintf(stderr, "malloc\t%d(%d)\t%d=%10p\t%d\n", (int)taille, original_taille ,trouve, ptr, total);
#endif

#ifdef DEBUG_CHECK
  memory_check(AFFICHAGE_DEFAUT);
#endif

  return ptr;
}

void *malloc(size_t taille) {
  return my_pool_malloc(taille);
}


/* -------------------------------------------------------------------------------*/
/* -------------------------------------------------------------------------------*/
/* -------------------------------------------------------------------------------*/
/* -------------------------------------------------------------------------------*/


void my_pool_free(void *ptr) 
{
#ifdef DEBUG_ENTER
  fprintf(stderr,"-> enter free %p\n",ptr);
#endif

  /* Verifie qu'on essaye pas de dÃ©sallouer le pointeur NULL */
  if (ptr == NULL)
    return;



  int zone = PTR_TO_ARRAY(ptr);
  if (zone > POOL_SIZE)
    {
      fprintf(stderr,"Invalid pointer\n");
      memory_check(1);
      exit(EXIT_FAILURE);
      return;
    }
  
  
  int taille = MYMEM(zone, SIZE);
  int precedent = -1, suivant = tete;

  /* Recherche de la premiere zone libre precedente */
  while (suivant != -1 && suivant < zone) 
    {
      precedent = suivant;
      suivant = MYMEM(suivant, NEXT);
    }
  /* Fixe la liste des zones libres */
  /* Cas 1 - liberation en tete de liste */
  if (precedent == -1) 
    {
      //fprintf(stderr,"free 1\n");
      /* la zone devient la nouvelle tete de liste */
      tete = zone;
      /* Cas 1.1: Unification avec la zone libre suivante */
      if (suivant != -1 && taille == suivant - zone)
	{
	  //fprintf(stderr,"free 1.1\n");
	  MYMEM(zone, SIZE) = MYMEM(suivant, SIZE) + taille;
	  MYMEM(zone, NEXT) = MYMEM(suivant, NEXT);
	}
      /* Cas 1.2: Pas d'unification avec la zone libre suivante */
      else
	{
	  //fprintf(stderr,"free 1.2\n");
	  MYMEM(zone, SIZE) = taille;
	  MYMEM(zone, NEXT) = suivant;
	}
    }
  /* Cas 2 - liberation au milieu de liste */
  else 
    {
      //fprintf(stderr,"free 2\n");
      int courant;
      /* Cas 2.1: Unification avec la zone libre precedente */
      if (precedent + MYMEM(precedent, SIZE) == zone) 
	{
	  //fprintf(stderr,"free 2.1\n");
	  MYMEM(precedent, SIZE) = MYMEM(precedent, SIZE) + taille;
	  MYMEM(precedent, NEXT) = suivant;
	  courant = precedent;
	} 
      /* Cas 2.2: Pas d'unification avec la zone libre precedente */
      else
	{
	  //fprintf(stderr,"free 2.2\n");
	  MYMEM(zone, SIZE) = taille;
	  MYMEM(zone, NEXT) = suivant;
	  MYMEM(precedent, NEXT) = zone;
	  courant = zone;
	}

      /* Cas 2.3: Unification avec la zone libre suivante */
      if (suivant != -1 && taille == suivant - zone)
	{
	  //fprintf(stderr,"free 2.3\n");
	  MYMEM(courant, SIZE) = MYMEM(courant, SIZE) + MYMEM(suivant, SIZE);
	  MYMEM(courant, NEXT) = MYMEM(suivant, NEXT); 
	} 
      /* Cas 2.4: Pas d'unification avec la zone libre suivante */
      else
	{
	  //fprintf(stderr,"free 2.4\n");
	  /* Nothing to do: it has been handled before */
	}
    }
  total -= taille;
#ifdef DEBUG
  fprintf(stderr, "free\t%d\t%d=%10p\t%d\n", (int)taille,zone ,ptr, taille);
#endif

#ifdef DEBUG_CHECK
  memory_check(AFFICHAGE_DEFAUT);
#endif
}

void free(void *ptr) 
{
  my_pool_free(ptr);
}


/* -------------------------------------------------------------------------------*/
/* -------------------------------------------------------------------------------*/
/* -------------------------------------------------------------------------------*/
/* -------------------------------------------------------------------------------*/


void *my_pool_realloc(void *ptr, size_t nouvelle_taille) 
{
#ifdef DEBUG_ENTER
  fprintf(stderr,"-> enter realloc %p %d\n", ptr, nouvelle_taille);
#endif

  /* Court-circuit pour faire les appels directs Ã  malloc et Ã  free */
  if (ptr == NULL)
    {
      return my_pool_malloc(nouvelle_taille);
    }
  else if (nouvelle_taille == 0)
    {
      my_pool_free(ptr);
      return NULL;
    }

  if (premiere_fois) 
    {
      MYMEM(0,SIZE) = POOL_SIZE;
      MYMEM(0,NEXT) = -1;
      premiere_fois = 0;
    }
  
  void *ptr_resultat = ptr;
  int zone = PTR_TO_ARRAY(ptr);
  int ancienne_taille = MYMEM(zone, SIZE);  
#ifdef DEBUG
  fprintf(stderr, "av realloc\t%d\t%d=%10p\t%d\n", ancienne_taille, zone,ptr,total);
#endif


  /* On verifie si on a besoin de moins de mémoire ou non */
  int original_nouvelle_taille = nouvelle_taille;
  nouvelle_taille = ((nouvelle_taille + 11) / 8) * 8;

  /* Cas 1: si on demande la même quantitée de mémoire on ne fait rien */
  if (ancienne_taille == nouvelle_taille)
    {
      /* rien à faire on vous dit */
    }
  /* Cas 2: Si on a besoin de moins : on libere la zone en trop */
  else if (ancienne_taille > nouvelle_taille)
    {
      int precedent = 0, suivant = tete;
      /* Recherche de la premiere zone libre predente */
      while (suivant != -1 && suivant < zone) {
	precedent = suivant;
	suivant = MYMEM(suivant, NEXT);
      }
      /* On insere dans la liste la zone memoire liberee */
      int zone_liberee = zone +  nouvelle_taille;
      int taille_liberee = ancienne_taille - nouvelle_taille;
      /* On verifie si la nouvelle zone ne peut pas etre fusionnee avec la suivante */
      if (suivant == zone_liberee + taille_liberee) 
	{
	  MYMEM(zone_liberee, SIZE) = taille_liberee + MYMEM(suivant, SIZE);
	  MYMEM(zone_liberee, NEXT) = MYMEM(suivant, NEXT);
	}
      /* Si ce n est pas le cas: on fixe la nouvelle zone libre correctement */
      else
	{
	  MYMEM(zone_liberee, SIZE) = taille_liberee;
	  MYMEM(zone_liberee, NEXT) = suivant;
	}
      /* On insere la nouvelle zone comme suivant de la zone precedente */
      if (precedent != 0)
	MYMEM(precedent,NEXT) = zone_liberee ;
      /* Si il n y a pas de zone précédente on faire une insertion en tête */
      else
	tete = zone_liberee;
      MYMEM(zone,SIZE) = nouvelle_taille;
      total = total - ancienne_taille + nouvelle_taille;
    }
  /* Si on a besoin de plus : */
  else
    {
      /* On regarde si il y a suffisament de place apres */ 
      int precedent = 0, courant = tete;
      /* On recherche la zone libre suivante */
      while (courant != -1 && courant < zone) {
	precedent = courant;
	courant = MYMEM(courant, NEXT);
      }
      int taille_libre = 0;
      /*Plus de de place libre*/
      if (courant == -1)
	{
	  fprintf(stderr, "Pas assez de memoire (taille = %d)\n", (int)nouvelle_taille);
	  return NULL;
	}
      /* Si cette zone existe */
      else if (zone + ancienne_taille == courant)
	{
	  taille_libre = MYMEM(courant, SIZE);
	}
      /* Il y a suffisament de memoire libre pour étendre la zone courante*/
      if (ancienne_taille + taille_libre == nouvelle_taille)
	{
	  MYMEM(zone,SIZE) = nouvelle_taille;
	  /* Il faut modifier la liste des zones libres */
	  if (precedent != 0)
	    MYMEM(precedent,NEXT) = MYMEM(courant, NEXT);
	  else
	    tete = MYMEM(courant, NEXT);
	  total = total - ancienne_taille + nouvelle_taille;
 	}
      else if (ancienne_taille + taille_libre > nouvelle_taille)
	{
	  MYMEM(zone + nouvelle_taille,SIZE) = ancienne_taille + taille_libre - nouvelle_taille;
	  MYMEM(zone + nouvelle_taille,NEXT) = MYMEM(courant, NEXT);
	  MYMEM(zone,SIZE) = nouvelle_taille;
	  /* Il faut modifier la liste des zones libres */
	  if (precedent != 0)
	    MYMEM(precedent,NEXT) = zone + nouvelle_taille;
	  else
	    tete = zone + nouvelle_taille;
	  total = total - ancienne_taille + nouvelle_taille;
	}
      /* Si ce n est pas le cas, il faut faire une nouvelle allocation */
      else
	{
#ifdef DEBUG
	  fprintf(stderr," ");
#endif
	  if ((ptr_resultat = my_pool_malloc(original_nouvelle_taille)) != NULL)
	    {
	      int nouvelle_zone = PTR_TO_ARRAY(ptr_resultat);
	      for (int i=0; i<(ancienne_taille-4); i++)
		{
		  MYMEM_DATA(nouvelle_zone,i) = MYMEM_DATA(zone,i);
		}
#ifdef DEBUG
	      fprintf(stderr," ");
#endif
  	      my_pool_free(ptr);
	    }
	}
    }
  //total = total - ancienne_taille + nouvelle_taille;
  /* - Si il n'y a pas suffisament de place apres, on cherche ailleurs et on dÃ©place le tout */ 
#ifdef DEBUG
  fprintf(stderr, "ap realloc\t%d\t%d=%10p\t%d\n", nouvelle_taille, PTR_TO_ARRAY(ptr_resultat),ptr_resultat, total);
#endif

#ifdef DEBUG_CHECK
  memory_check(AFFICHAGE_DEFAUT);
#endif
	      
  return ptr_resultat ;
}


void *realloc(void *ptr, size_t size) {
  return my_pool_realloc(ptr, size);
}


/* -------------------------------------------------------------------------------*/
/* -------------------------------------------------------------------------------*/
/* -------------------------------------------------------------------------------*/
/* -------------------------------------------------------------------------------*/


void *my_pool_calloc(size_t nmemb, size_t size) 
{
#ifdef DEBUG_ENTER
  fprintf(stderr,"-> enter calloc\n");
#endif 

  void * ptr = my_pool_malloc(nmemb * size);
  int zone = PTR_TO_ARRAY(ptr);
  if (ptr != NULL)
    for (int i=0; i<nmemb * size; i++)
      MYMEM_DATA(zone,i) = '\0';
#ifdef DEBUG_CHECK
  memory_check(AFFICHAGE_DEFAUT);
#endif

  return ptr;
}


void *calloc(size_t nmemb, size_t size) {
  return my_pool_calloc(nmemb, size);
}

/* -------------------------------------------------------------------------------*/
/* -------------------------------------------------------------------------------*/
/* -------------------------------------------------------------------------------*/
/* -------------------------------------------------------------------------------*/


double my_pool_fragmentation() {
  int courant = tete;
  int max = 0, taille = 0;

  while (courant != -1) {
    taille = MYMEM(courant,SIZE);
    if (taille > max)
      max = taille;
    courant = MYMEM(courant, NEXT);
  }

  return total == 0 ? 0 : 1 - (double)taille/(POOL_SIZE-total);
}

/* -------------------------------------------------------------------------------*/
/* -------------------------------------------------------------------------------*/
/* -------------------------------------------------------------------------------*/
/* -------------------------------------------------------------------------------*/

