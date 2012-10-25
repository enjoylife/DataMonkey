#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include "./flex.h"
#include "../dbg.h"

#define CEILING(x,y) (x - (x / y))
#ifdef TRUE
#undef TRUE
#endif

#ifdef FALSE
#undef FALSE
#endif

typedef enum { FALSE, TRUE } bool;

inline void flex_debug_out(flex_t flex);
inline int default_msb(unsigned long int i);
inline unsigned extract_continuous_bits(unsigned long value, int start, int end);

/*NOTES:
 * (num_super_blocks < index_block_length < num_elements).
 * Index_block is a 2 dimensional array with each col being 2 ^(k/2) in size, 
 * where k is the leading bit on a query.
 * */
typedef struct flex_array{
    free_func_t free_func;
    int index_block_length;
    int data_block_length;
    long int num_elements;              // n
    long int num_super_blocks;          // s
    long int num_nonempty_data_blocks; // d
    long int num_empty_data_blocks;    // either 0 or 1 
    long int last_data_occup, last_data_block_length;  // Lastof's
    long int last_super_occup, last_super_size;
    long int last_index_occup, last_index_block_length;
    index_p index_block; 
} flex_array;

inline void flex_debug_out(flex_t flex){
    log_info("data  [occup %ld, size %ld]", flex->last_data_occup, flex->last_data_block_length);
    log_info("super [occup %ld, size %ld]", flex->last_super_occup, flex->last_super_size);
    log_info("index [occup %ld, size %ld]\n", flex->last_index_occup, flex->last_index_block_length);
}

inline static void default_free (void *ptr){
    free(ptr);
}

extern flex_t flex_init(unsigned long int size){
    flex_array *  new_flex;
    index_p  new_index;

    new_flex = malloc(sizeof(flex_array)); 
    new_flex->free_func =  default_free;

    new_index = malloc(sizeof(index_p)); // single index 
    new_index[0] = malloc(sizeof(data_p));
    new_flex->index_block = new_index;

    new_flex->data_block_length = 1;
    new_flex->index_block_length = 1;

    new_flex->num_elements = 0;
    new_flex->num_nonempty_data_blocks = 0;
    new_flex->num_empty_data_blocks = 1;
    new_flex->num_super_blocks = 1;

    new_flex->last_data_occup = 0;
    new_flex->last_data_block_length = 1;
    new_flex->last_super_occup = 1;
    new_flex->last_super_size = 1;
    new_flex->last_index_occup = 1;
    new_flex->last_index_block_length = 1;

    return new_flex;
}

inline bool _flex_check_data(flex_t f){
    if(f->last_data_occup == f->last_data_block_length){
        return TRUE;
    }
    return FALSE;
}
inline bool _flex_check_super(flex_t f){
    if(f->last_super_occup == f->last_super_size){
       return TRUE;
    }
    return FALSE;
}
inline bool _flex_check_index(flex_t f){
    if(f->last_index_block_length == f->last_index_occup){
        return TRUE;
    }
    return FALSE;
}
inline double _flex_index_usage(flex_t f){
    return (double)f->data_block_length / (double)f->index_block_length;
}

inline extern DSTATUS flex_shrink(flex_t flex){
    // 1.
    flex->num_elements--;
    flex->last_data_occup--;

    // 2.
    // B. we skip A //TODO: find out how the fuck to handle this, and even why?
    if(flex->last_data_occup <=0){
        if(_flex_index_usage(flex) < 0.4){
            flex->last_index_block_length /= 2;
            //TODO: We lose half of our 2d array, we need a way to free them before the realloc
            flex->index_block = realloc(flex->index_block, sizeof(index_p) * flex->last_index_block_length);
        }
        // C.
        flex->num_nonempty_data_blocks--;
        flex->last_super_occup--;
        // D.
        if(flex->last_super_occup <= 0){
            // i.
            flex->num_super_blocks--;
            // iii.
            if(flex->num_super_blocks % 2){
                // double the number of elements in a data block
                flex->last_data_block_length /= 2;
                log_success("(STEP iii) Halfed the size of a DATAblock\n");
            // ii.
            } else {
                // double the number of data blocks in a superblock
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

inline extern DSTATUS flex_grow(flex_t  flex){
    //flex_debug_out(flex);
    // 1.
    if(_flex_check_data(flex)){
        // (a)
        if(_flex_check_super(flex)){
            flex->num_super_blocks++;
            if(flex->num_super_blocks % 2){
                // double the number of data blocks in a superblock
                flex->last_super_size *= 2;
                log_success("(STEP A) Doubled the size of a SUPERblock\n");
            } else {
                // double the number of elements in a data block
                flex->last_data_block_length *= 2;
                log_success("(STEP A) Doubled the size of a DATAblock\n");
            }
            flex->last_super_occup = 0;
        //}

        // (b) if there are no empty data blocks
        //if(flex->num_empty_data_blocks>0){
        //if(_flex_check_data(flex) && flex->num_empty_data_blocks > 0 ){
            // if index block is full
            //if(_flex_check_index(flex)){
                // reallocate the index block to twice its current size
                flex->last_index_block_length *= 2;
                flex->index_block = realloc(flex->index_block, sizeof(index_p) * flex->last_index_block_length);
                log_success("(STEP B) Realloc of INDEXblock\n");
            // allocate a new last data block and point to it from index block
            data_p  new_data_block = malloc(sizeof(data_p) * flex->last_data_block_length);
            flex->index_block_length++;
            flex->index_block[flex->index_block_length] = new_data_block;
            flex->last_index_occup++;
            //}
        }

        // (c)
        // increment d and  data blocks occupying last superblock
        flex->num_nonempty_data_blocks++;
        flex->last_super_occup++;

        // (d)
        //set occupancy of the last datablock to empty
        flex->last_data_occup = 0;
    }
    // 2.
    flex->num_elements++;
    flex->last_data_occup++;

    return SUCCESS;
}

inline DSTATUS flex_locate(flex_t flex, data_p requested_data, unsigned long int requested_index, FLEXACTION type){

    long int  k, p, r; // Same  as in Paper's pseudocode
    int e, b,temp;
    
    r = requested_index +1 ;
    //k = default_msb(r); 
    k = __builtin_ctzl(r); 
    // paper assumes 1 indexed while our extract assumes 0 indexed
    b = extract_continuous_bits(r, (k/2)+1 ,k-1);
    e = extract_continuous_bits(r, 0,CEILING(k,2) ); //ceiling
    p =  (1 << (k)) -1; // account for paper assuming 1 indexed
    //check_hard(k==7,"K INvalid acutally %ld", k);
    //check_hard(k/2 == 3,"Fail");
    //check_hard(CEILING(k,2) == ,"Fail");
    //check_hard(e==,"E INvalid acutally %ld", e);
    //check_hard(b==,"B INvalid it's actually %d", b);
    //check_hard(p==,"P INvalid %ld", p);

    // if this is correct, then wtf was p for?
    data_p block = (flex->index_block)[k];
    log_err("index length is %d", flex->index_block_length);
    log_err("K is %ld", k);
    log_err("E is %d", e);
    log_err("B is %d", b);
    log_err("P is %ld", p);
    if(type == FLEXRETRIEVE){ 
        *requested_data = block[e];
    }
    if(type == FLEXINSERT){
        block[e] = *requested_data;
    }
    return SUCCESS;
}

extern DSTATUS flex_insert(flex_t flex, data_p user_data, unsigned long int requested_index){
    DSTATUS status;
    while(flex->num_elements <= requested_index){
        status = flex_grow(flex);
    }
        status  = flex_locate(flex, user_data, requested_index, FLEXINSERT);
    return SUCCESS;
}

// Assumes zero indexed 
inline unsigned extract_continuous_bits(unsigned long value, int start, int end) {
    unsigned mask = (~0u) >> (CHAR_BIT*sizeof(value) - end - 1);
    return (value & mask) >> start;
}

inline int default_msb(unsigned long int i){
    int r = 0;
    while (i >>= 1)
    {
      r++;
    }
    return r;
}
