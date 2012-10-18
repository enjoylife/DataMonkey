#ifndef _flex_h
#define _flex_h


typedef struct flex_array * flex_t;


typedef enum  {
    SUCCES = 0,
    FAILURE,
    FOUND,
    MISSING
}data_struct_error;

extern flex_t flex_init(free_func_t free_func);


 flex_grow(flex_t * flex, unsigned int growth);

