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

inline void flex_debug_out(flex_t flex);
inline int default_msb(unsigned long int i);
inline static void default_free (void *ptr);
inline double _flex_index_usage(flex_t f);

char * print_bool(bool x){
    return x ? "True": "False";
}

/*NOTES:
 * Index_block is a 2 dimensional array with each col being 2 ^(k/2) in size, 
 * where k is the leading bit on a query.
 * */
typedef struct flex_array{
    free_func_t free_func;
    index_p index_block;               // pointer our data_blocks

    unsigned long int num_user_elements_inserted;

    unsigned long int usable_data_blocks;
    unsigned long int num_super_blocks;   
    bool has_empty_data_blocks;

    unsigned long int index_length;   
    unsigned long last_index_occup; // Lastof's
    unsigned long int last_data_occup, last_data_size;  
    unsigned long int last_super_occup, last_super_size;
} flex_array;

/* Debug Helpers */
inline void flex_debug_out(flex_t flex){
    log_success("BEGIN DEBUG");
    log_warn("numuserstuff -[%ld]", flex->num_user_elements_inserted);
    log_warn("usable slots -[%ld]", flex->usable_data_blocks);
    log_warn("num supers ---[%ld]", flex->num_super_blocks);
    log_warn("has empty ----[%s]", print_bool(flex->has_empty_data_blocks));
    log_warn("index length -[%ld]", flex->index_length);
    log_warn("index --------[%ld, %ld]", flex->last_index_occup, flex->index_length);
    log_warn("data ---------[%ld, %ld]", flex->last_data_occup, flex->last_data_size);
    log_warn("super --------[%ld, %ld]", flex->last_super_occup, flex->last_super_size);
    log_success("END DEBUG");
}

/*Main API */
extern flex_t flex_init(void){
    flex_array * new_flex = malloc(sizeof(*new_flex)); 
    index_p new_index = malloc(sizeof(index_p)); // single index 
    data_p new_data = (data_p) malloc(sizeof(data_p));
    
    check_hard(new_flex && new_index && new_data,"Fail to create inital structure");
    new_flex->free_func = default_free;

    new_flex->num_user_elements_inserted = 0;
    new_flex->has_empty_data_blocks = FALSE;

    new_flex->index_block    = new_index;
    new_flex->index_block[0] = new_data;

    // SET ALL DA THINGS TO ONE!!!!
    // We use zero index's for any vars that repersent array indices
    new_flex->last_super_occup   = 1;
    new_flex->last_data_occup    = 1;
    new_flex->last_index_occup   = 1;
    new_flex->num_super_blocks   = 1;
    new_flex->last_data_size     = 1;
    new_flex->last_super_size    = 1;
    new_flex->index_length       = 1;
    new_flex->usable_data_blocks = 1;

    return new_flex;
}

extern DSTATUS flex_grow(flex_t  flex){
    if(flex->last_data_occup == flex->last_data_size){
        // (a)
        if(flex->last_super_occup == flex->last_super_size){
            flex->has_empty_data_blocks = FALSE;
            flex->num_super_blocks++;  
            if(flex->num_super_blocks % 2){
                // double the number of data blocks in a superblock
              //  log_infob("(STEP A) Doubled SUPERblock");
                flex->last_super_size *= 2;
            } else {
                // double the number of elements in a data block
             //   log_infob("(STEP A) Doubled DATAblock");
                flex->last_data_size *= 2;
            }
            flex->last_super_occup = 0;
        }
        // (b)
        if(flex->has_empty_data_blocks==FALSE){ // we are full in the 2nd dimension too
            log_infob("(Step B) FIRST IF");
            log_warn("index --------[%ld, %ld]", flex->last_index_occup, flex->index_length);
            //if(flex->last_index_occup == flex->index_length){  
                
            //    log_infob("(Step B) SECOND IF");
                flex->index_length *= 2;
                index_p new_index_block = (index_p)realloc((flex->index_block), sizeof(index_p) * flex->index_length);
                check_hard(new_index_block,"FAIL of realloc");
                flex->index_block = new_index_block;
                //log_infob("(STEP B) Realloc of INDEXblock");

                // allocate a new last data block and point to it from index block
                data_p new_data_block = (data_p) malloc(sizeof(data_p) * flex->last_data_size);
                check_hard(new_data_block,"FAIL of alloc");
                unsigned long int i;
                for(i=0; i < flex->last_data_size; i++){
                    // for current debugging only
                    new_data_block[i] = 0;
                }
                flex->usable_data_blocks += flex->last_data_size;
                flex->index_block[(flex->last_index_occup)] = new_data_block;
               // log_infob("ADDED to index @ %ld, with array of %ld",flex->last_index_occup,flex->last_data_size);
                flex->has_empty_data_blocks = TRUE;
                flex->last_index_occup++;
            //}
        }
        //(c)
        flex->last_super_occup++;
        // (d)
        flex->last_data_occup = 1;
    } else {
        
        flex->last_data_occup++;
    }

    return SUCCESS;
}
extern DSTATUS flex_index_init(flex_t flex, unsigned long int requested_index){
    //log_info("Grow Check requested:[%ld], avalible: [%ld]",requested_index, flex->usable_data_blocks);
    log_info("Check %ld >= %ld",requested_index, (flex->usable_data_blocks - (flex->last_data_size)));
    while(requested_index >= (flex->usable_data_blocks - (flex->last_data_size ))){
    log_info("Check %ld >= %ld",requested_index, (flex->usable_data_blocks - (flex->last_data_size)));
        flex_grow(flex);
    }
    return SUCCESS;
}
extern DSTATUS flex_insert(flex_t flex, data_p user_data, unsigned long int requested_index){
    //log_info("Grow Check requested:[%ld], avalible: [%ld]",requested_index, flex->usable_data_blocks);
    log_info("Check %ld >= %ld",requested_index, (flex->usable_data_blocks - (flex->last_data_size)));
    while(requested_index >= (flex->usable_data_blocks - (flex->last_data_size ))){
    log_info("Check %ld >= %ld",requested_index, (flex->usable_data_blocks - (flex->last_data_size)));
        flex_grow(flex);
    }

    flex_locate(flex, user_data, requested_index, INSERT);
    return SUCCESS;
}

inline DSTATUS flex_locate(flex_t flex, data_p requested_data, unsigned long int requested_index, FLEXACTION type){
    unsigned long int r,k,b,e,p; // Same  as in Paper's pseudocode
    r = requested_index +1 ;
    //k = default_msb(r); 
    k = LEADINGBIT(r);           // -1 taken care in macro
    b = BITSUBSET(r,k-k/2,k);
    e = BITSUBSET(r,0, CEILING(k,2));
    //p = k==0 ? 0 : ((1 << k)-1) ; 
    p = k==0 ? 0 :  (1 << (k-1)) ; //PEFECT
    log_info("trying [%ld,%ld]\n",(p+b),e);

    index_p block = flex->index_block;
    switch(type){
        case RETRIEVE:
            *requested_data = block[p+b][e];
            break;
        case INSERT:
            block[(p+b)][e] = *requested_data;
            flex->num_user_elements_inserted++;
            break;
    }
    return SUCCESS;
}


inline double _flex_index_usage(flex_t f){
    return (double)f->num_user_elements_inserted / (double)f->index_length;
}

/* Various Helpers */
inline static void default_free (void *ptr){
    free(ptr);
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

extern void flex_string_dump(flex_t flex){

    unsigned int x,y;
    unsigned int data_size = 1, super_count = 0, super_last_count = 0, super_size=1;
    fflush(stdout);
    printf("\n");
    log_warn("Starting array element dump");
    for(x = 0 ; x < flex->index_length; x++){
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
inline extern DSTATUS flex_shrink(flex_t flex){
    // 1.
    flex->last_data_occup--;

    // 2.
    // B. we skip A //TODO: find out how the fuck to handle this, and even why?
    if(flex->last_data_occup <=0){
        if(_flex_index_usage(flex) < 0.4){
            flex->index_length /= 2;
            //TODO: We lose half of our 2d array, we need a way to free them before the realloc
            flex->index_block = realloc(flex->index_block, sizeof(index_p) * flex->index_length);
        }
        // C.
        flex->last_super_occup--;
        // D.
        if(flex->last_super_occup <= 0){
            // i.
            flex->num_super_blocks--;
            // iii.
            if(flex->num_super_blocks % 2){
                flex->last_data_size /= 2;
                log_success("(STEP iii) Halfed the size of a DATAblock\n");
            // ii.
            } else {
                flex->last_super_size /= 2;
                log_success("(STEP ii) Halfed the size of a SUPERblock\n");
            }
            // iv.
            flex->last_super_occup = flex->last_super_size;
        }
        // E.
        flex->last_data_occup = flex->last_super_size;
    }
    return SUCCESS;
}
