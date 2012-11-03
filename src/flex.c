#include <stdlib.h>
#include <limits.h>
#include "flex.h"
#include "dbg.h"

/* Internal extra helper headers */
inline int default_msb(index_t i);
inline static void default_free (data_p ptr);
char * print_bool(bool x){ return x ? "True": "False"; }

flex_t flex_init(void){

    flex_array * new_flex = malloc(sizeof(*new_flex)); 
    index_p new_index = malloc(sizeof(index_p)); 
    data_p new_data = (data_p) malloc(sizeof(data_p));
    check(new_flex && new_index && new_data,"Failed to create inital structure");

    new_flex->free_func = default_free;
    new_flex->index_block    = new_index;
    new_flex->index_block[0] = new_data;

    new_flex->last_index_occup   = 0;
    new_flex->index_length       = 1;
    new_flex->last_data_size     = 1;
    new_flex->num_super_blocks   = 1;
    new_flex->last_super_size    = 1;
    new_flex->usable_data_blocks = 1;
    new_flex->num_user_elements_inserted = 0;

    return new_flex;
error:
    return NULL;
}

DSTATUS flex_traverse(flex_t flex, void (*action)(data_p)){
    index_t x,y;
    for(x = 0 ; x <= flex->last_index_occup; x++){
        //log_infob("k= %ld, supersize=%ld, datasize=%ld\n",x,(1<<((x)/2)),(1<<(CEILING((x),2)))); 
        for(y = 0; y < (1<<(CEILING((x),2))); y++){
            action(&flex->index_block[x][y]);
            //log_info("[%ld,%ld]",x,y);
        }
    }
    return SUCCESS;
}
DSTATUS flex_destroy(flex_t flex){
    check(flex,"Was given uninitialized flex");
    index_t x,y;
    for(x = 0 ; x <= flex->last_index_occup; x++){
        free(flex->index_block[x]);
    }
    free(flex->index_block);
    free(flex);
    return SUCCESS;
error:
    return FAILURE;
}

DSTATUS flex_change_free(flex_t flex, free_func_t func ){
    check(flex && func,"Failed to change flex's free function.");
    flex->free_func = func;
    return SUCCESS;
error:
    return FAILURE;
}

DSTATUS flex_grow(flex_t  flex)
{
    if(flex->num_user_elements_inserted == flex->usable_data_blocks){
            flex->num_super_blocks++;  // correct
            if(flex->num_super_blocks % 2){
                //log_infob("(STEP A) Doubled SUPERblock");
                flex->last_super_size *= 2;
            } else {
                //log_infob("(STEP A) Doubled DATAblock");
                flex->last_data_size *= 2;
            }
            flex->last_index_occup++;
            if(flex->index_length == flex->last_index_occup){
                flex->index_length *= 2;
                index_p new_index_block = realloc((flex->index_block), sizeof(index_p) * flex->index_length);
                check(new_index_block,"Could not increase the size of a new_block");
                flex->index_block = new_index_block;
            }
            //log_infob("added new data size: %ld",flex->last_data_size);
            data_p new_data_block = (data_p) malloc(sizeof(data_p) * flex->last_data_size);
            check(new_data_block,"Could not create a new data_block");

            index_t i;
            for(i=0; i < flex->last_data_size; i++){
                // for current debugging only
                new_data_block[i] = 0;
            }

            flex->usable_data_blocks += flex->last_data_size;
            flex->index_block[(flex->last_index_occup)] = new_data_block;
            //log_infob("ADDED to index @ %ld, with array of %ld",flex->last_index_occup,flex->last_data_size);
        flex->num_user_elements_inserted++;
    } else {
        flex->num_user_elements_inserted++;
        return SUCCESS;
    }
error:
    return FAILURE;
}
DSTATUS flex_shrink(flex_t  flex)
{
    // bail early
    if(flex->num_user_elements_inserted == 0  )return FAILURE; //correct

    //log_infob("test %ld == %ld",flex->usable_data_blocks - flex->last_data_size, flex->num_user_elements_inserted-1);
    if(flex->num_user_elements_inserted != 1 && 
            flex->usable_data_blocks - flex->last_data_size == flex->num_user_elements_inserted-1){
        flex->num_super_blocks--;
        //log_infob("REMOVED  @ %ld",flex->last_index_occup);
        free(flex->index_block[flex->last_index_occup]);
        flex->usable_data_blocks -= flex->last_data_size;

        //log_infob("realloc %ld == %ld",flex->last_index_occup*2, flex->index_length);
        if((flex->last_index_occup) *2 ==  flex->index_length ){ //TODO! see grow x=4 and x=7
            flex->index_length =CEILING(flex->index_length,2);
            index_p new_index = realloc(flex->index_block,
                        sizeof(index_p) * flex->index_length);
            check(new_index,"Failed in Shrinking flex");
            flex->index_block = new_index;
        }
        flex->last_index_occup--;
        
        // if odd, cut data
        if(flex->num_super_blocks % 2){
            //log_infob("cut data size");
            //flex->last_data_size /= 2;
            flex->last_data_size = CEILING(flex->last_data_size,2);
        } else {
            //log_infob("cut super size");
            //flex->last_super_size /= 2;
            flex->last_super_size = CEILING(flex->last_super_size,2);
        }
        flex->num_user_elements_inserted--;
    }  else {
    flex->num_user_elements_inserted--;
        return SUCCESS;
    }
error:
    return FAILURE;
}

DSTATUS flex_insert(flex_t flex, data_p user_data, index_t requested_index){
    index_t r,k,b,e,p;
    r = requested_index + 1;
    k = LEADINGBIT(r); // no need for minus 1. already zero indexed PERFECT
    b = BITSUBSET(r,k-k/2,k);
    e = BITSUBSET(r,0, CEILING(k,2));
    p = (1 << (k/2 + 1)) - 2 + (k & 1) * (1 << (k/2));
    log_info("Grow Check P+B:[%ld], index: [%ld]",p+b, flex->index_length);
    printf("k/2=[%ld], Ceil(k,2)=[%ld]\n",k/2,CEILING(k,2));
    printf("K: [%ld] is the leading 1 bit\n",k);
    printf("B: [%ld]\n",b);
    printf("E: [%ld]\n",e);
    printf("P: [%ld] data blocks prior to our superblock\n",p);
    printf("p+b,e : [%ld,%ld] \n",p+b,e);


    // we have an index which would seg fault
    while(p+b >= flex->index_length){
        flex_grow(flex);
        //flex_debug_out(flex);
    }
    //flex_locate(flex, user_data, requested_index, INSERT);
    flex->index_block[(p+b)][e] = *user_data;
    return SUCCESS;
}
 
inline DSTATUS flex_locate(flex_t flex, data_p requested_data, index_t requested_index, FLEXACTION type){
    index_t r,k,b,e,p; // Same  as in Paper's pseudocode
    r = requested_index +1 ;
    //k = default_msb(r); 
    k = LEADINGBIT(r);           // -1 taken care in macro
    b = BITSUBSET(r,k-k/2,k);
    e = BITSUBSET(r,0, CEILING(k,2));
    p = k==0 ? 0 :  (1 << (k-1)) ; //TODO:  avoid conditional?
    p = (1 << (k/2 + 1)) - 2 + (k & 1) * (1 << (k/2));
    //log_info("trying [%ld,%ld]\n",(p+b),e);

    index_p block = flex->index_block;
    switch(type){
        case RETRIEVE:
            *requested_data = block[p+b][e];
            break;
        case INSERT:
            block[(p+b)][e] = *requested_data;
            //flex->num_user_elements_inserted++;
            break;
       default:
       break;
    }
    return SUCCESS;
}

/* Various Helpers */
inline static void default_free (data_p ptr){
    log_infob("freeing");
    free(ptr);
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
    log_warn("numuserstuff -[%ld]", flex->num_user_elements_inserted);
    log_warn("usable slots -[%ld]", flex->usable_data_blocks);
    log_warn("index length -[%ld]", flex->index_length);
    log_warn("data length --[%ld]", flex->last_data_size);
    log_warn("super length -[%ld]", flex->last_super_size);
    log_warn("num super - --[%ld]", flex->num_super_blocks);
    log_warn("index --------[%ld, %ld]", flex->last_index_occup, flex->index_length);
    log_success("END DEBUG");
}
