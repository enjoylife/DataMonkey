#include <stdlib.h>
#include <limits.h>
#include "./dyna.h"
#include "../dbg.h"

#ifdef TRUE
#undef TRUE
#endif

#ifdef FALSE
#undef FALSE
#endif
#define MIN(a,b) (((a)<(b))?(a):(b))
#define CEILING(x,y) (((x) + (y) - 1) / (y))
#define LEADINGBIT(r) ((sizeof(index_t)*8) - __builtin_clzl(r))


typedef enum { FALSE, TRUE } bool;

/* Private api */
DSTATUS dyna_grow(dyna_t dyna);
DSTATUS dyna_shrink(dyna_t  dyna);
void dyna_string_dump(dyna_t dyna);
void dyna_debug_out(dyna_t dyna);

/* Internal extra helper headers */
inline static void default_free (data_t ptr);
inline static int default_cmp(data_t a, data_t b);
char * print_bool(bool x){ return x ? "True": "False"; }
const char* byte_to_binary( index_t x );

/* Our type which holds the array */
typedef data_t* index_p;

/* Private per say, use the public functions to manipulate.*/
typedef struct dyna_array{
    free_func_t free_func;
    cmp_func_t cmp_func;
    index_p array; 
    index_t index_length;   
    index_t last_index;
    double change_rate;
    double damping;
    int shrink_delay;
} dyna_array;

extern dyna_t dyna_init(index_t inital_size){

    dyna_array * new_dyna = malloc(sizeof(*new_dyna)); 
    index_p new_array = malloc(sizeof(data_t) * inital_size); 
    check(new_array && new_dyna,"Failed to create inital structure");

    new_dyna->array = new_array;
    new_dyna->free_func = default_free;
    new_dyna->cmp_func = default_cmp;
    new_dyna->index_length = inital_size -1;
    new_dyna->change_rate = 2;
    new_dyna->damping = 1.0;
    new_dyna->shrink_delay = 16;

    return new_dyna;
error:
    return NULL;
}

extern DSTATUS dyna_destroy(dyna_t dyna){
    index_t i;
    data_t stuff;
    check(dyna,"Was given uninitialized dyna");
    for(i=0; i<dyna->index_length; i++){
         stuff = dyna_get(dyna, i);

         if(stuff) dyna->free_func(stuff);
    }
    free(dyna->array);
    free(dyna);
    return SUCCESS;
error:
    return FAILURE;
}
extern DSTATUS dyna_change_free(dyna_t dyna, free_func_t func ){
    check(dyna && func,"Failed to change dyna's free function.");
    dyna->free_func = func;
    return SUCCESS;
error:
    return FAILURE;
}
extern DSTATUS dyna_change_cmp(dyna_t dyna, cmp_func_t func ){
    check(dyna && func,"Failed to change dyna's free function.");
    dyna->cmp_func = func;
    return SUCCESS;
error:
    return FAILURE;
}

extern data_t dyna_get(dyna_t dyna, index_t requested_index){
    if(requested_index > dyna->index_length){
        return (data_t)NULL;
    }
    return dyna->array[requested_index];
}

extern DSTATUS dyna_insert(dyna_t dyna, index_t requested_index,data_t user_data,  DSTATUS modifier){
    DSTATUS s;
    switch(modifier){
        default: //fall thru
        case DEF: 
            while(requested_index > dyna->index_length){
               s =  dyna_grow(dyna);
               check(s !=FAILURE,"Failed insert, Out of Memory");
            }
        break;

        case UNQ:
            while(requested_index > dyna->index_length){
               s =  dyna_grow(dyna);
               check(s != FAILURE,"Failed insert, Out of Memory");
            }
            if(dyna->cmp_func(user_data, dyna->array[requested_index])) 
                return FAILURE;
            break;
        case CHK:
            if(requested_index > dyna->index_length)
                return FAILURE;
        case SPD: //fall thru
            break;
    }
    if(dyna->last_index < requested_index) dyna->last_index = requested_index;
    dyna->array[requested_index] = user_data;
    return SUCCESS;
error:
    return FAILURE;
}

DSTATUS dyna_delete(dyna_t dyna, index_t requested_index){
    if(requested_index > dyna->last_index) return FAILURE;
    if(!dyna->array[requested_index]) return FAILURE;
    if(dyna->last_index > requested_index){
        if((dyna->last_index - requested_index) > dyna->index_length/dyna->change_rate){
            dyna_shrink(dyna);
        }
        dyna->last_index = requested_index;
    }
    dyna->free_func(dyna->array[requested_index]);
    dyna->array[requested_index] =(data_t) NULL;
    return SUCCESS;
}

DSTATUS dyna_grow_to(dyna_t dyna, index_t size){
     index_p new_array;
    dyna->index_length = size;
    new_array = realloc(dyna->array, sizeof(data_t)*dyna->index_length);
    check(new_array,"Failed to grow");
    dyna->array = new_array;
    return SUCCESS;
error:
    return FAILURE;
}

DSTATUS dyna_grow(dyna_t dyna){
    index_p new_array;

    index_t new_size = dyna->index_length + (dyna->index_length*dyna->change_rate);
    log_infob("diff:[%ld], new[%ld]",new_size - dyna->index_length, new_size);
    new_array = realloc(dyna->array, sizeof(data_t)*new_size);
    check(new_array,"Failed to grow");
    dyna->array = new_array;
    dyna->index_length = new_size;
    return SUCCESS;
error:
    return FAILURE;
}
DSTATUS dyna_shrink(dyna_t dyna){
    index_p new_array;
    index_t new_size = (index_t)dyna->index_length - dyna->index_length /dyna->change_rate ;
    log_infob("new size [%ld]",new_size);
    new_array = realloc(dyna->array, sizeof(data_t)*new_size);
    check(new_array,"Failed to shrink");
    dyna->array = new_array;
    dyna->index_length = new_size;
    return SUCCESS;
error:
    return FAILURE;
}

inline int default_cmp(data_t a, data_t b){
    return a==b;
}

inline void default_free(data_t ptr){
    ;
    //free(ptr);
}

inline void dyna_debug_out(dyna_t dyna){
    log_warn("-----BEGIN DEBUG-------");
    log_warn("Index length -----[%ld]",(long int) dyna->index_length);
    log_warn("Last index -------[%ld]",(long int) dyna->last_index);
    log_warn("Change rate ------[%f]", dyna->change_rate);
    log_warn("-----END DEBUG----------");
}
const char* byte_to_binary( index_t x ){
    static char b[sizeof(x)*8+1] = {0};
    int y;
    long long z;
    for (z=1LL<<sizeof(x)*8-1,y=0; z>0; z>>=1,y++)
    {
        b[y] = ( ((x & z) == z) ? '1' : '0');
    }

    b[y] = 0;

    return b;
}
