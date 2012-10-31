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
#define MIN(a,b) (((a)<(b))?(a):(b))
#define CEILING(x,y) (((x) + (y) - 1) / (y))
#define LEADINGBIT(r) ((sizeof(index_t)*8) - __builtin_clzl(r))

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
typedef enum { FALSE, TRUE } bool;

/* Private api */
DSTATUS flex_grow(flex_t flex);
DSTATUS flex_shrink(flex_t  flex);
void flex_string_dump(flex_t flex);
void flex_debug_out(flex_t flex);

/* Internal extra helper headers */
inline static void default_free (data_t ptr);
inline static int default_cmp(data_t a, data_t b);
char * print_bool(bool x){ return x ? "True": "False"; }

/* Our type which holds the array */
typedef data_t* index_p;

/* Private per say, use the public functions to manipulate.*/
typedef struct flex_array{
    free_func_t free_func;
    cmp_func_t cmp_func;
    index_p array; 
    index_t index_length;   
    index_t last_index;
    double change_rate;
    double damping;
    int shrink_delay;
} flex_array;

extern flex_t flex_init(index_t inital_size){

    flex_array * new_flex = malloc(sizeof(*new_flex)); 
    index_p new_array = malloc(sizeof(data_t) * inital_size); 
    check(new_array && new_flex,"Failed to create inital structure");

    new_flex->array = new_array;
    new_flex->free_func = default_free;
    new_flex->cmp_func = default_cmp;
    new_flex->index_length = inital_size -1;
    new_flex->change_rate = 2;
    new_flex->damping = 1.0;
    new_flex->shrink_delay = 16;

    return new_flex;
error:
    return NULL;
}

extern DSTATUS flex_destroy(flex_t flex){
    index_t i;
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
extern DSTATUS flex_change_cmp(flex_t flex, cmp_func_t func ){
    check(flex && func,"Failed to change flex's free function.");
    flex->cmp_func = func;
    return SUCCESS;
error:
    return FAILURE;
}

extern data_t flex_get(flex_t flex, index_t requested_index){
    if(requested_index > flex->index_length){
        return (data_t)NULL;
    }
    return flex->array[requested_index];
}

extern DSTATUS flex_insert(flex_t flex, index_t requested_index,data_t user_data,  DSTATUS modifier){
    DSTATUS s;
    switch(modifier){
        default: //fall thru
        case DEF: 
            while(requested_index > flex->index_length){
               s =  flex_grow(flex);
               check(s !=FAILURE,"Failed insert, Out of Memory");
            }
        break;

        case UNQ:
            while(requested_index > flex->index_length){
               s =  flex_grow(flex);
               check(s != FAILURE,"Failed insert, Out of Memory");
            }
            if(flex->cmp_func(user_data, flex->array[requested_index])) 
                return FAILURE;
            break;
        case CHK:
            if(requested_index > flex->index_length)
                return FAILURE;
        case SPD: //fall thru
            break;
    }
    if(flex->last_index < requested_index) flex->last_index = requested_index;
    flex->array[requested_index] = user_data;
    return SUCCESS;
error:
    return FAILURE;
}

DSTATUS flex_delete(flex_t flex, index_t requested_index){
    if(requested_index > flex->last_index) return FAILURE;
    if(!flex->array[requested_index]) return FAILURE;
    if(flex->last_index > requested_index){
        if((flex->last_index - requested_index) > flex->index_length/flex->change_rate){
            flex_shrink(flex);
        }
        flex->last_index = requested_index;
    }
    flex->free_func(flex->array[requested_index]);
    flex->array[requested_index] =(data_t) NULL;
    return SUCCESS;
}

DSTATUS flex_grow_to(flex_t flex, index_t size){
     index_p new_array;
    flex->index_length = size;
    new_array = realloc(flex->array, sizeof(data_t)*flex->index_length);
    check(new_array,"Failed to grow");
    flex->array = new_array;
    return SUCCESS;
error:
    return FAILURE;
}

DSTATUS flex_grow(flex_t flex){
    index_p new_array;

    index_t new_size = (index_t)LEADINGBIT(flex->index_length + (flex->index_length*flex->change_rate));
    log_infob("diff:[%ld], new[%ld]",new_size - flex->index_length, new_size);
    new_array = realloc(flex->array, sizeof(data_t)*new_size);
    check(new_array,"Failed to grow");
    flex->array = new_array;
    flex->index_length = new_size;
    return SUCCESS;
error:
    return FAILURE;
}
DSTATUS flex_shrink(flex_t flex){
    index_p new_array;
    index_t new_size = (index_t)flex->index_length - flex->index_length /flex->change_rate ;
    log_infob("new size [%ld]",new_size);
    new_array = realloc(flex->array, sizeof(data_t)*new_size);
    check(new_array,"Failed to shrink");
    flex->array = new_array;
    flex->index_length = new_size;
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

inline void flex_debug_out(flex_t flex){
    log_warn("-----BEGIN DEBUG-------");
    log_warn("Index length -----[%ld]",(long int) flex->index_length);
    log_warn("Last index -------[%ld]",(long int) flex->last_index);
    log_warn("Change rate ------[%f]", flex->change_rate);
    log_warn("-----END DEBUG----------");
}
