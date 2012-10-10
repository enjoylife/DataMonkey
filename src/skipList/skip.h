#ifndef _skip_H
#define _skip_H

#define SKIP_MAX 32

typedef  struct skip_list * skip_t;
typedef void (*free_func_t)(void *);

extern skip_t skip_init(free_func_t free_func);
extern int skip_insert(skip_t sl, unsigned int key, void * payload);
extern int skip_current_level(skip_t s);

#endif
