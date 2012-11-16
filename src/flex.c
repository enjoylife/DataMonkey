#include <stdlib.h>
#include <limits.h>
#include "flex.h"
#include "debug.h"

/* Internal extra helper headers */
inline int default_msb(index_t i);
inline static DSTATUS default_free (data_p ptr);
inline static DSTATUS default_cmp(data_p a, data_p b);


extern inline flex_t flex_init(void)
{

    flex_array * new_flex = malloc(sizeof(*new_flex)); 
    index_p new_index = malloc(sizeof(index_p)); 
    data_p new_data = (data_p) malloc(sizeof(data_p));
    check(new_flex && new_index && new_data,"Failed to create inital structure");

    new_flex->free_func = default_free;
    new_flex->cmp_func = default_cmp;
    new_flex->index_block    = new_index;
    new_flex->index_block[0] = new_data;

    new_flex->last_index_occup   = 0;
    new_flex->index_length       = 1;
    new_flex->last_data_size     = 1;
    new_flex->num_super_blocks   = 1;
    new_flex->last_super_size    = 1;
    new_flex->last_super_occup  = 0;
    new_flex->usable_data_blocks = 1;
    new_flex->num_user_elements_inserted = 0;

    return new_flex;
error:
    return NULL;
}

extern inline DSTATUS flex_change_free(flex_t flex, free_func_t func )
{
    check(flex && func,"Failed to change flex's free function.");
    flex->free_func = func;
    return SUCCESS;
error:
    return FAILURE;
}

extern inline DSTATUS flex_change_cmp(flex_t flex, cmp_func_t func )
{
    check(flex && func,"Failed to change flex's compare function.");
    flex->cmp_func = func;
    return SUCCESS;
error:
    return FAILURE;
}


extern inline DSTATUS flex_destroy(flex_t flex)
{
    check(flex,"Was given uninitialized flex");
    index_t x;

    for(x = 0 ; x <= flex->last_index_occup; x++){
        free(flex->index_block[x]);
    }
    free(flex->index_block);
    free(flex);
    return SUCCESS;
error:
    return FAILURE;
}

extern inline DSTATUS flex_nuke(flex_t flex)
{
    check(flex,"Was given uninitialized flex");
    index_t x;
    DSTATUS status;

    status = flex_traverse(flex,flex->free_func);
    check_alt(status == SUCCESS);
    for(x = 0 ; x <= flex->last_index_occup; x++){
        free(flex->index_block[x]);
    }
    free(flex->index_block);
    free(flex);
    return SUCCESS;
error:
    return FAILURE;
}

extern inline DSTATUS flex_traverse(flex_t flex, DSTATUS (*action)(data_p))
{
    DSTATUS status;
    index_t x,y;
    index_t data_size = 1, super_count = 0, super_last_count = 0, super_size=1;
    for(x = 0 ; x <= flex->last_index_occup; x++){
        for(y = 0; y < data_size; y++){
            status = action(&flex->index_block[x][y]); //log_info("MAIDIT");
            check(status == SUCCESS, "Failure at index [%ld,%ld]",x,y); 
        } //printf("\n");
        super_last_count++;
        if(super_last_count == super_size){
            super_last_count = 0;
            if(super_count%2){
                super_size *=2;
                super_count++;
            } else {
                    data_size *= 2;
                super_count++;
            }
        }
    }
    return SUCCESS;
error:
    return FAILURE;
}

extern inline data_p flex_get(flex_t flex, index_t requested_index)
{
    index_t r,k,b,e,p;
    r = requested_index + 1;
    k = LEADINGBIT(r); // no need for minus 1. already zero indexed 
    b = BITSUBSET(r,k-k/2,k);
    e = BITSUBSET(r,0, CEILING(k,2));
    p = (1 << (k/2 + 1)) - 2 + (k & 1) * (1 << (k/2));
    if(p+b > flex->last_index_occup){ // we have an index which would seg fault
        return NULL;
    } 
    return &flex->index_block[(p+b)][e];
}

extern inline DSTATUS flex_insert(flex_t flex, index_t requested_index, data_p user_data)
{
    DSTATUS status;
    index_t r,k,b,e,p;
    r = requested_index + 1;
    k = LEADINGBIT(r); // no need for minus 1. already zero indexed 
    b = BITSUBSET(r,k-k/2,k);
    e = BITSUBSET(r,0, CEILING(k,2));
    p = (1 << (k/2 + 1)) - 2 + (k & 1) * (1 << (k/2));
    //log_info("Grow Check P+B:[%ld], index: [%ld]",p+b, flex->index_length);
    //printf("k/2=[%ld], Ceil(k,2)=[%ld]\n",k/2,CEILING(k,2));
    //printf("K: [%ld] is the leading 1 bit\n",k); // printf("B: [%ld]\n",b);
    while(p+b > flex->last_index_occup){ // we have an index which would seg fault
        status = flex_grow(flex);  //flex_debug_out(flex);
        check_alt(status == SUCCESS);    
    }
    //log_info("trying [%ld,%ld]",(p+b),e);
    (flex->index_block[(p+b)][e]) = *user_data;
    return SUCCESS;
error:
    return FAILURE;
}

extern inline DSTATUS flex_compare(flex_t flex, index_t requested_index, data_p user_data)
{
    index_t r,k,b,e,p;
    r = requested_index + 1;
    k = LEADINGBIT(r); // no need for minus 1. already zero indexed 
    b = BITSUBSET(r,k-k/2,k);
    e = BITSUBSET(r,0, CEILING(k,2));
    p = (1 << (k/2 + 1)) - 2 + (k & 1) * (1 << (k/2));
    if(p+b > flex->last_index_occup){ // we have an index which would seg fault
        return FAILURE;
    } 
    return (flex->cmp_func(&(flex->index_block[(p+b)][e]), user_data));
}

extern inline DSTATUS flex_delete(flex_t flex, index_t requested_index)
{
    index_t r,k,b,e,p;
    r = requested_index + 1;
    k = LEADINGBIT(r); // no need for minus 1. already zero indexed 
    b = BITSUBSET(r,k-k/2,k);
    e = BITSUBSET(r,0, CEILING(k,2));
    p = (1 << (k/2 + 1)) - 2 + (k & 1) * (1 << (k/2));
    if(p+b > flex->last_index_occup){ // we have an index which would seg fault
        return FAILURE;
    } 
    flex->index_block[(p+b)][e] = 0;
    return SUCCESS;
}

/* Increases the array's internal counters, and records a plus one to the application element count.
 * ------------------------------------------------------------------------------------------------
 * [params] {flex} nonvoid array to work with.
 * [return] {SUCCESS} if grow was completed.
 *          {FAILURE} if memory or other error was encountered.
 */
extern inline DSTATUS flex_grow(flex_t  flex)
{
    if(flex->num_user_elements_inserted == flex->usable_data_blocks){
        flex->last_super_occup++;
        if(flex->last_super_occup == flex->last_super_size){
            flex->num_super_blocks++;  
            if(flex->num_super_blocks % 2){
                flex->last_super_size *= 2; //log_info("(STEP A) Doubled SUPERblock");
            } else {
                flex->last_data_size *= 2; //log_info("(STEP A) Doubled DATAblock");
            }
        flex->last_super_occup = 0;
        }
        flex->last_index_occup++;
        if(flex->index_length == flex->last_index_occup){
            flex->index_length *= 2;
            index_p new_index_block = realloc((flex->index_block), sizeof(index_p) * flex->index_length);
            check(new_index_block,"Could not increase the size of index block from[%ld] to[%ld]",
                    flex->index_length/2, flex->index_length);
            flex->index_block = new_index_block;
        }
        //log_info("added new data @ %ld size: %ld",flex->last_index_occup, flex->last_data_size);
        data_p new_data_block = (data_p) malloc(sizeof(data_p) * flex->last_data_size);
        check(new_data_block,"Could not create a new data_block of size[%ld], @ index[%ld]",
                flex->last_data_size, flex->last_index_occup);
        index_t i;
        for(i=0; i < flex->last_data_size; i++){
            new_data_block[i] = 0; // for current debugging only
        }
        flex->usable_data_blocks += flex->last_data_size;
        flex->index_block[(flex->last_index_occup)] = new_data_block;
        //log_info("ADDED to index @ %ld, with array of %ld",flex->last_index_occup,flex->last_data_size);
        flex->num_user_elements_inserted++;
    } else {
        flex->num_user_elements_inserted++;
        return SUCCESS;
    }
    return SUCCESS;
error:
    return FAILURE;
}

/* Reduces the internal counters to our array, and subtracts one from the count of contained application elements.
 * ---------------------------------------------------------------------------------------------------------------
 * [params] {flex} a nonvoid flex array to work with.
 * [return] {SUCCESS} if shrinking was completed.
 *          {FAILURE} if array is shrunk to 0 elements, or if  memory error was encountered.
 */
extern inline DSTATUS flex_shrink(flex_t  flex)
{
    if(flex->num_user_elements_inserted == 0  )return FAILURE; // bail early

    //log_info("test %ld == %ld",flex->usable_data_blocks - flex->last_data_size, flex->num_user_elements_inserted-1);
    if(flex->num_user_elements_inserted != 1 && 
            flex->usable_data_blocks - flex->last_data_size == flex->num_user_elements_inserted-1){
        //log_info("REMOVED  @ %ld",flex->last_index_occup);
        free(flex->index_block[flex->last_index_occup]);
        flex->usable_data_blocks -= flex->last_data_size;

        //log_info("realloc %ld == %ld",flex->last_index_occup*2, flex->index_length);
        if((flex->last_index_occup) *2 ==  flex->index_length ){ //TODO! see grow x=4 and x=7
            flex->index_length =CEILING(flex->index_length,2);
            index_p new_index = realloc(flex->index_block,
                        sizeof(index_p) * flex->index_length);
            check(new_index,"Failed in Shrinking flex");
            flex->index_block = new_index;
        }
        flex->last_index_occup--;
        
        //log_info("size change 0 == %ld",flex->last_super_occup);
        if( 0== flex->last_super_occup){
            flex->num_super_blocks--;
            // if odd, cut data
            if(flex->num_super_blocks % 2){
                //log_info("cut data size");
                flex->last_data_size /= 2;
                //flex->last_data_size = CEILING(flex->last_data_size,2);
            } else {
                //log_info("cut super size");
                flex->last_super_size /= 2;
                //flex->last_super_size = CEILING(flex->last_super_size,2);
            }
            flex->last_super_occup = flex->last_super_size;
            flex->last_super_occup--;
        } else {
            flex->last_super_occup--;
        }
        flex->num_user_elements_inserted--;
    }  else {
    flex->num_user_elements_inserted--;
        return SUCCESS;
    }
error:
    return FAILURE;
}

/* Various Helpers */
inline static DSTATUS default_free (data_p ptr){
    log_info("freeing");
    free(ptr);
    return SUCCESS;
}

inline static DSTATUS default_cmp(data_p a, data_p b){
    if(*a ==*b) return EQL;
    if(*a < *b) return LT;
    return GT;
}

inline int default_msb(index_t i){
    /* Just to double check myself when using gcc's  __builtin_ctzl */
    int r = 0;
    while (i >>= 1)
    {
      r++;
    }
    return r;
}

/* Debug Helpers */
inline void flex_debug_out(flex_t flex){
    log_success("BEGIN DEBUG");
    log_warn("index length -[%ld]", flex->index_length);
    log_warn("data length --[%ld]", flex->last_data_size);
    log_warn("num super ----[%ld]", flex->num_super_blocks);
    log_warn("userstuff ----[%ld, %ld]", flex->num_user_elements_inserted, flex->usable_data_blocks);
    log_warn("index --------[%ld, %ld]", flex->last_index_occup, flex->index_length);
    log_warn("super --------[%ld, %ld]", flex->last_super_occup, flex->last_super_size);
    log_success("END DEBUG");
}
