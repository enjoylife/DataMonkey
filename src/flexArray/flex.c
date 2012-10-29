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
DSTATUS flex_locate(flex_t  flex, data_p requested_data, unsigned long int requested_index, FLEXACTION);
DSTATUS flex_grow(flex_t flex);
DSTATUS flex_shrink(flex_t  flex);
void flex_string_dump(flex_t flex);
void flex_debug_out(flex_t flex);

/* Internal extra helper headers */
inline int default_msb(unsigned long int i);
inline static void default_free (void *ptr);
char * print_bool(bool x){ return x ? "True": "False"; }

/* Our type which holds the various sized data_blocks. */
typedef data_p *index_p;

/* Private per say,use the public functions to edit.*/
typedef struct flex_array{
    free_func_t free_func;
    index_p index_block; // array of data_blocks
    unsigned long int num_user_elements_inserted;
    unsigned long int usable_data_blocks;
    unsigned long int index_length;   
    unsigned long int num_super_blocks;   
    unsigned long last_index_occup;
    unsigned long int last_data_occup, last_data_size;  
    unsigned long int last_super_occup, last_super_size;
} flex_array;

extern flex_t flex_init(void){

    flex_array * new_flex = malloc(sizeof(*new_flex)); 
    index_p new_index = malloc(sizeof(index_p)); 
    data_p new_data = (data_p) malloc(sizeof(data_p));
    check(new_flex && new_index && new_data,"Failed to create inital structure");

    new_flex->free_func = default_free;
    new_flex->num_user_elements_inserted = 0;
    new_flex->index_block    = new_index;
    new_flex->index_block[0] = new_data;

    // SET ALL DA THE THINGS TO ONE!!!!
    new_flex->last_super_occup   = 1;
    new_flex->last_data_occup    = 1;
    new_flex->last_index_occup   = 1;
    new_flex->num_super_blocks   = 1;
    new_flex->last_data_size     = 1;
    new_flex->last_super_size    = 1;
    new_flex->index_length       = 1;
    new_flex->usable_data_blocks = 1;

    return new_flex;
error:
    return NULL;
}

extern DSTATUS flex_destroy(flex_t flex){
    check(flex,"Was given uninitialized flex");
    flex_traverse(flex, flex->free_func);
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

extern DSTATUS flex_insert(flex_t flex, data_p user_data, unsigned long int requested_index){
    //log_info("Grow Check requested:[%ld], avalible: [%ld]",requested_index, flex->usable_data_blocks);
    //log_info("Check %ld >= %ld",requested_index, (flex->usable_data_blocks - (flex->last_data_size)));
    while(requested_index >= (flex->usable_data_blocks - (flex->last_data_size ))){
        flex_grow(flex);
    }
    flex_locate(flex, user_data, requested_index, INSERT);
    return SUCCESS;
}

DSTATUS flex_shrink(flex_t flex){

    if(flex->last_data_occup ==1){
        if(flex->last_index_occup > 1){  //catching too many repeated  calls
            flex->last_index_occup--;
            //log_infob("REMOVED from index @ %ld, with array of %ld",flex->last_index_occup,flex->last_data_size);
            free(flex->index_block[flex->last_index_occup]);
            flex->usable_data_blocks -= flex->last_data_size;
        if((flex->last_index_occup/flex->index_length)<= 0.25){
            flex->index_length /= 2;
            index_p new_index = realloc(flex->index_block, sizeof(index_p) * flex->index_length);
            check(new_index,"Failed in Shrinking flex");
            flex->index_block = new_index;
        }
        flex->last_super_occup= flex->last_super_occup-1 <= 0 ? 1 : flex->last_super_occup-1;;
        if(flex->last_super_occup == 1 ){
            flex->num_super_blocks--;
            // iii.
            if(flex->num_super_blocks % 2){
                flex->last_data_size /= 2;
                //log_success("(STEP iii) Halfed the size of a DATAblock\n");
            // ii.
            } else {
                flex->last_super_size /= 2;
                //log_success("(STEP ii) Halfed the size of a SUPERblock\n");
            }
            // iv.
            flex->last_super_occup = flex->last_super_size;
            flex->last_data_occup = flex->last_data_size;
        }
        } else {
            return FAILURE; // too many calls to shrink
        }
    } else{ 
        flex->last_data_occup--;
    }
    return SUCCESS;
error:
    return FAILURE;
}

DSTATUS flex_grow(flex_t  flex){
    if(flex->last_data_occup == flex->last_data_size){
        if(flex->last_super_occup == flex->last_super_size){
            flex->num_super_blocks++;  
            if(flex->num_super_blocks % 2){
                //log_infob("(STEP A) Doubled SUPERblock");
                flex->last_super_size *= 2;
            } else {
                //log_infob("(STEP A) Doubled DATAblock");
                flex->last_data_size *= 2;
            }
            flex->last_super_occup = 0;
        }
                //log_warn("index --------[%ld, %ld]", flex->last_index_occup, flex->index_length);
                flex->index_length *= 2;
                index_p new_index_block = (index_p)realloc((flex->index_block), sizeof(index_p) * flex->index_length);
                check(new_index_block,"Could not increase the size of a new_block");
                flex->index_block = new_index_block;
                data_p new_data_block = (data_p) malloc(sizeof(data_p) * flex->last_data_size);
                check(new_data_block,"Could not create a new data_block");
                unsigned long int i;
                for(i=0; i < flex->last_data_size; i++){
                    // for current debugging only
                    new_data_block[i] = 0;
                }
                flex->usable_data_blocks += flex->last_data_size;
                flex->index_block[(flex->last_index_occup)] = new_data_block;
                //log_infob("ADDED to index @ %ld, with array of %ld",flex->last_index_occup,flex->last_data_size);
                flex->last_index_occup++;
        flex->last_super_occup++;
        flex->last_data_occup = 1;
    } else {
        flex->last_data_occup++;
    }
    return SUCCESS;
error:
    return FAILURE;
}

inline DSTATUS flex_locate(flex_t flex, data_p requested_data, unsigned long int requested_index, FLEXACTION type){
    unsigned long int r,k,b,e,p; // Same  as in Paper's pseudocode
    r = requested_index +1 ;
    //k = default_msb(r); 
    k = LEADINGBIT(r);           // -1 taken care in macro
    b = BITSUBSET(r,k-k/2,k);
    e = BITSUBSET(r,0, CEILING(k,2));
    p = k==0 ? 0 :  (1 << (k-1)) ; //TODO:  avoid conditional?
    //log_info("trying [%ld,%ld]\n",(p+b),e);

    index_p block = flex->index_block;
    switch(type){
        case RETRIEVE:
            *requested_data = block[p+b][e];
            break;
        case INSERT:
            block[(p+b)][e] = *requested_data;
            flex->num_user_elements_inserted++;
            break;
       default:
       break;
    }
    return SUCCESS;
}

/* Various Helpers */
DSTATUS flex_traverse(flex_t flex, void (*action)(void *)){
    unsigned int x,y;
    unsigned int data_size = 1, super_count = 0, super_last_count = 0, super_size=1;
    for(x = 0 ; x < flex->last_index_occup; x++){
        for(y = 0; y < data_size; y++){
            action(&flex->index_block[x][y]);
        }
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
}
inline static void default_free (void *ptr){
    ptr;
    //free(ptr);
}

inline int default_msb(unsigned long int i){
    /* Just to double check myself when using gcc's  __builtin_ctzl */
    int r = 0;
    while (i >>= 1)
    {
      r++;
    }
    return r;
}

/* Debug Helpers */
extern void flex_string_dump(flex_t flex){

    unsigned int x,y;
    unsigned int data_size = 1, super_count = 0, super_last_count = 0, super_size=1;
    fflush(stdout);
    printf("\n");
    log_warn("Starting array element dump");
    for(x = 0 ; x < flex->last_index_occup; x++){
        /*
        printf("\nrow: %d,data_size: %d, super_size: %d," 
        "super_last_count: %d, super_count: %d\n",
        x, data_size,super_size,super_last_count, super_count);
        */
        for(y = 0; y < data_size; y++){
            printf("%d ", flex->index_block[x][y]);
        }
        super_last_count++;
        printf("\n");
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
    log_success("Finished element dump");
}
inline void flex_debug_out(flex_t flex){
    log_success("BEGIN DEBUG");
    log_warn("numuserstuff -[%ld]", flex->num_user_elements_inserted);
    log_warn("usable slots -[%ld]", flex->usable_data_blocks);
    log_warn("num supers ---[%ld]", flex->num_super_blocks);
    log_warn("index length -[%ld]", flex->index_length);
    log_warn("index --------[%ld, %ld]", flex->last_index_occup, flex->index_length);
    log_warn("data ---------[%ld, %ld]", flex->last_data_occup, flex->last_data_size);
    log_warn("super --------[%ld, %ld]", flex->last_super_occup, flex->last_super_size);
    log_success("END DEBUG");
}
