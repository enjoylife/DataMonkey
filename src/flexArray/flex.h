#ifndef _flex_h
#define _flex_h

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

typedef int data_t;
typedef int flex_index_t;

/* Container altering functions.*/
flex_t flex_init(flex_index_t inital_size);
DSTATUS flex_destroy(flex_t flex);

/* Element manipulation functions.*/
DSTATUS flex_insert(flex_t flex, data_t user_data, flex_index_t  requested_index);
data_t flex_get(flex_t flex, flex_index_t requested_index);
DSTATUS flex_remove(flex_t flex, 

#endif
