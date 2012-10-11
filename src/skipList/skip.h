#ifndef _skip_H
#define _skip_H

#define SKIP_MAX 32

typedef  struct skip_list * skip_t;
typedef void (*free_func_t)(void * ptr);

extern skip_t skip_init(free_func_t free_func);
extern void skip_destroy(skip_t sl);

extern void * skip_search(skip_t sl, unsigned int key);
extern int skip_insert(skip_t sl, unsigned int key, void * payload);
extern int skip_delete(skip_t sl, unsigned int key);
extern int skip_current_level(skip_t s);

#endif
