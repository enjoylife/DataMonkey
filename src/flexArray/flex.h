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
    FLEXINSERT = 0,
    FLEXEXPAND,
    FLEXUPDATE,
    FLEXRETRIEVE,
} FLEXACTION;

typedef int *data_p, **index_p;

flex_t flex_init(unsigned long int size);
DSTATUS flex_locate(flex_t  flex, data_p requested_data, unsigned long int requested_index, FLEXACTION);
DSTATUS flex_insert(flex_t flex, data_p user_data, unsigned long int requested_index);

DSTATUS flex_grow(flex_t  flex);
DSTATUS flex_shrink(flex_t  flex);

void flex_string_dump(flex_t flex);
void flex_debug_out(flex_t flex);

#endif

