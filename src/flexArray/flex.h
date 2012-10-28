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
    MISSING
}DSTATUS;

typedef enum {
    INSERT,
    CHECKINSERT,
    EXPAND,
    SHRINK,
    UPDATE,
    RETRIEVE,
} FLEXACTION;

typedef int *data_p, **index_p;

flex_t flex_init(void);
extern DSTATUS flex_index_init(flex_t flex, unsigned long int requested_index);
DSTATUS flex_locate(flex_t  flex, data_p requested_data, unsigned long int requested_index, FLEXACTION);
DSTATUS flex_insert(flex_t flex, data_p user_data, unsigned long int requested_index);

extern DSTATUS flex_grow(flex_t flex);
DSTATUS flex_shrink(flex_t  flex);

void flex_string_dump(flex_t flex);
void flex_debug_out(flex_t flex);

#endif

