#ifndef MY_MEMORY_H
#define MY_MEMORY_H

extern void *my_pool_malloc(size_t taille);
extern void *my_pool_realloc(void *ptr, size_t new_taille);
extern void *my_pool_calloc(size_t nmemb, size_t size);
extern void  my_pool_free(void *ptr);
extern double my_pool_fragmentation();

#endif
