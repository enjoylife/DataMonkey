#ifndef _flex_h
#define _flex_h


typedef void (*free_func_t)(void * ptr);
typedef struct flex_array * flex_t;


typedef enum  {
    SUCCES = 0,
    FAILURE,
    FOUND,
    MISSING
}struct_error;


extern flex_t flex_init(free_func_t free_func);
struct_error flex_locate(flex_t * flex, void * user_data, unsigned int index);

#endif

