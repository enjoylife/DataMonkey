#ifndef _dyna_h
#define _dyna_h


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
typedef struct dyna_array * dyna_t;

/* Container altering functions.*/
dyna_t dyna_init(
        index_t inital_size);

DSTATUS dyna_destroy(
        dyna_t dyna);

DSTATUS dyna_change_free(dyna_t dyna,
        free_func_t func);

DSTATUS dyna_change_cmp(dyna_t dyna,
        cmp_func_t func);

/* Element manipulation functions.*/
DSTATUS dyna_insert(dyna_t dyna,
        index_t requested_index, data_t user_data, DSTATUS modifier);

DSTATUS dyna_delete(dyna_t dyna,
        index_t requested_index);

data_t dyna_get(dyna_t dyna, 
        index_t requested_index);

#endif
