#ifndef _flex_h
#define _flex_h


typedef void (*free_func_t)(void * ptr);
typedef struct flex_array * flex_t;

typedef enum  {
    SUCCESS = 0,
    FAILURE,
    FOUND,
    MISSING
}DSTATUS;

typedef enum {
    FLEXINSERT = 1,
    FLEXUPDATE = 2,
    FLEXRETRIEVE = 3
} FLEXFUNCTYPE;

flex_t flex_init(unsigned long int size);
DSTATUS flex_locate(flex_t  flex, void * requested_data, unsigned long int requested_index, FLEXFUNCTYPE);
DSTATUS flex_insert(flex_t flex, void * user_data, unsigned long int requested_index);

DSTATUS flex_grow(flex_t  flex);
DSTATUS flex_shrink(flex_t  flex);

#endif

