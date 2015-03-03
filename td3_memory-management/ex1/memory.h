#ifndef _MEMORY_H
#define _MEMORY_H

#include <sys/types.h>

extern void *my_pool_malloc(size_t);
extern void *my_pool_realloc(void*,size_t);
extern void *my_pool_calloc(size_t,size_t);
extern void  my_pool_free(void*);
extern double my_pool_fragmentation();

#endif

