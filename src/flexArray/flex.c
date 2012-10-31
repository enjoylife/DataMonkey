#include <stdlib.h>
#include <limits.h>
#include "./flex.h"
#include "../dbg.h"

#ifdef TRUE
#undef TRUE
#endif

#ifdef FALSE
#undef FALSE
#endif

typedef enum { FALSE, TRUE } bool;
typedef enum {
    INSERT,
    EXPAND,
    SHRINK,
    UPDATE,
    RETRIEVE,
} FLEXACTION;

/* Private api */
DSTATUS flex_grow(flex_t flex);
DSTATUS flex_shrink(flex_t  flex);
void flex_string_dump(flex_t flex);
void flex_debug_out(flex_t flex);

/* Internal extra helper headers */
inline static void default_free (void *ptr);
char * print_bool(bool x){ return x ? "True": "False"; }

/* Our type which holds the array */
typedef data_t *index_p;

/* Private per say, use the public functions to manipulate.*/
typedef struct flex_array{
    free_func_t free_func;
    index_p array; 
    flex_index_t index_length;   
    flex_index_t last_index;
} flex_array;

inline void flex_debug_out(flex_t flex){
    log_success("BEGIN DEBUG");
    log_warn("index length -[%ld]", flex->index_length);
    log_warn("Last index ---[%ld]", flex->last_index);
    log_success("END DEBUG");
}
extern flex_t flex_init(flex_index_t inital_size){

    flex_array * new_flex = malloc(sizeof(*new_flex)); 
    index_p new_array = malloc(sizeof(data_t) * inital_size); 
    check(new_array && new_flex,"Failed to create inital structure");

    new_flex->free_func = default_free;
    new_flex->index_length = inital_size -1;
    new_flex->last_index = 0;

    return new_flex;
error:
    return NULL;
}

extern DSTATUS flex_destroy(flex_t flex){
    flex_index_t i;
    data_t stuff;
    check(flex,"Was given uninitialized flex");
    for(i=0; i<flex->index_length; i++){
         stuff = flex_get(flex, i);
         if(stuff) flex->free_func(stuff);
    }
    free(flex->array);
    free(flex);
    return SUCCESS;
error:
    return FAILURE;
}
extern DSTATUS flex_change_free(flex_t flex, free_func_t func ){
    check(flex && func,"Failed to change flex's free function.");
    flex->free_func = func;
    return SUCCESS;
error:
    return FAILURE;
}

extern DSTATUS flex_insert(flex_t flex, data_t user_data, unsigned long int requested_index){
    if(requested_index > flex->index_length){
        flex_grow(flex);
    }
    flex->array[requested_index] = user_data;
    //TODO: handle overwrite checking
    return SUCCESS;
}
