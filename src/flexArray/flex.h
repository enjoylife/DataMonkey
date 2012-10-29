#ifndef _flex_h
#define _flex_h

#define CEILING(x,y) (((x) + (y) - 1) / (y))
#define LEADINGBIT(r) (sizeof(unsigned long int)*8 - __builtin_clzl(r)-1)
#define BITLAST(k,n) ((k) & ((1<<(n))-1))
#define BITSUBSET(k,m,n) BITLAST((k)>>(m),((n)-(m)))

typedef void (*free_func_t)(void * ptr);
typedef struct flex_array * flex_t;

typedef enum  {
    SUCCESS,
    FAILURE,
    FOUND,
    MISSING,
    GREW,
    SHRANK
}DSTATUS;

typedef int *data_p;

/* Container altering functions.*/
flex_t flex_init(void);
DSTATUS flex_destroy(flex_t flex);
DSTATUS flex_change_free(flex_t flex, free_func_t func);

/* Element manipulation functions.*/
DSTATUS flex_insert(flex_t flex, data_p user_data, unsigned long int requested_index);
DSTATUS flex_traverse(flex_t flex, void (*action)(void *));

#endif
