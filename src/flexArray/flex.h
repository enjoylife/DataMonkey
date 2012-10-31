#ifndef _flex_h
#define _flex_h


typedef enum  {
    SUCCESS= 0,
    FAILURE,
    DEF,
    SPD,
    UNQ,
    CHK,
}DSTATUS;

typedef int data_t;
typedef unsigned int index_t;

typedef void (*free_func_t)(data_t ptr);
typedef int (*cmp_func_t)(data_t a, data_t b);
typedef struct flex_array * flex_t;

/* Container altering functions.*/
flex_t flex_init(
        index_t inital_size);

DSTATUS flex_destroy(
        flex_t flex);

DSTATUS flex_change_free(flex_t flex,
        free_func_t func);

DSTATUS flex_change_cmp(flex_t flex,
        cmp_func_t func);

/* Element manipulation functions.*/
DSTATUS flex_insert(flex_t flex,
        index_t requested_index, data_t user_data, DSTATUS modifier);

DSTATUS flex_delete(flex_t flex,
        index_t requested_index);

data_t flex_get(flex_t flex, 
        index_t requested_index);

#endif
