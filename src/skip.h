#ifndef _skip_H
#define _skip_H

#define SKIP_MAX 32

typedef  struct skip_list * skip_t;

typedef void (*free_func_t)(void * ptr);

inline skip_t skip_init(free_func_t free_func);
inline void skip_destroy(skip_t sl);

inline int skip_length(skip_t sl);

inline void * skip_search(skip_t sl, unsigned int key);
inline void * skip_finger_search(skip_t sl, unsigned int key);
inline int skip_insert(skip_t sl, unsigned int key, void * payload);
inline int skip_delete(skip_t sl, unsigned int key);
inline int skip_level(skip_t s);

#endif
