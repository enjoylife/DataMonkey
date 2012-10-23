#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include "./flex.h"
#include "../dbg.h"

#define CEILING(x,y) (((x) + (y) - 1) / (y))

inline int default_msb(unsigned long int i);
inline unsigned extract_continuous_bits(unsigned long value, int start, int end);

/*
// we alloc this with the 2^(k/2) amount of user_data
typedef struct data_block {
    char space;
    void * user_data[];
} data_block;
*/
typedef void ** data_block;

typedef struct flex_array{
    free_func_t free_func;
    long int num_elements;              // n
    long int num_super_blocks;          // s
    long int num_nonempty_data_blocks,/* d */ 
         num_empty_data_blocks;/* either 0 or 1*/ 
    // Lastof's
    long int last_data_occup, last_data_size;
    long int last_super_occup, last_super_size;
    long int last_index_occup, last_index_size;
    data_block ** index_block;
} flex_array;

inline static void default_free (void *ptr){
    ptr;
}

extern flex_t flex_init(unsigned long int size){
    flex_array *  new_flex;

    new_flex = malloc(sizeof(*new_flex));
    new_flex->free_func =  default_free;
    new_flex->index_block = calloc(1, sizeof(data_block *)); // single index 

    new_flex->num_elements = 0;
    new_flex->num_nonempty_data_blocks = 1;
    new_flex->num_empty_data_blocks = 1;
    new_flex->num_super_blocks = 1;

    new_flex->last_data_occup = 0;
    new_flex->last_data_size = 1;

    new_flex->last_super_occup = 0;
    new_flex->last_super_size = 1;

    new_flex->last_index_occup = 0;
    new_flex->last_index_size = 1;

    return new_flex;
}

extern DSTATUS flex_grow(flex_t  flex){
    //log_warn("data occup %ld, data size %ld", flex->last_data_occup, flex->last_data_size);
    // 1.
    // if the last nonempty data block is full
    if(flex->last_data_occup == flex->last_data_size){
        // (a)
        // if the lastsuperblock is full
        //log_warn("super occup %ld, super size %ld", flex->last_super_occup, flex->last_super_size);
        if(flex->last_super_occup == flex->last_super_size){
            flex->num_super_blocks++;
            if(flex->num_super_blocks % 2){
                // double the number of data blocks in a superblock
                flex->last_super_size = flex->last_super_size * 2;
                log_success("Doubled the size of a SUPERblock");
            } else {
                // double the number of elements in a data block
                flex->last_data_size = flex->last_data_size * 2;
                log_success("Doubled the size of a DATAblock");
            }
            // set occupancy of last superblock to zero
            flex->last_super_occup = 0;
            // why? if our logic is in occup == size we should just continually inc and dec this...
            //flex->last_super_occup++;
        }

        // (b)
        // if there are no empty data blocks
        if(flex->num_empty_data_blocks == 0){
            // if index block is full
            if(flex->last_index_occup == flex->last_index_size){
                // reallocate the index block to twice its current size
                flex->last_index_size = flex->last_index_size *2;
                //flex->index_block = realloc(flex->index_block, sizeof(data_block *) * flex->last_index_size);
                flex = realloc(flex, sizeof(flex_array) + sizeof(data_block *) * flex->last_index_size);
            }
            // allocate a new last data block and point to it from index block
            data_block * new_data_block = malloc(sizeof(data_block) + 
                    flex->last_data_size * sizeof(void *));
            flex->index_block[flex->last_index_size] = new_data_block;
            log_success("Doubled the size of the INDEXblock");
        }

        // (c)
        // increment d and  data blocks occupying last superblock
        flex->num_nonempty_data_blocks++;
        flex->last_super_occup++;

        // (d)
        //set occupancy of the last datablock to empty
        flex->last_data_occup = 0;
        // why? if our logic is in occup == size we should just leave this...
        //flex->last_data_occup++;
        
    }
    // 2.
    //increment n and  number of elements occupying last datablock
    flex->num_elements++;
    flex->last_data_occup++;
    //log_success("Simple INCR");

    return SUCCESS;
}

DSTATUS flex_locate(flex_t flex, void * requested_data, unsigned long int requested_index, FLEXFUNCTYPE type){

    long int  k, p, r; // Same  as in Paper's pseudocode
    int e, b,temp;
    
    r = requested_index +1 ;
    k = default_msb(r); // paper assumes 1 indexed while our extract assumes 0 indexed
    //k = __builtin_ctzl(r); // paper assumes 1 indexed while our extract assumes 0 indexed
    check_hard(k==7,"K INvalid acutally %ld", k);
    e = extract_continuous_bits(r, 0,  CEILING(k,2));
    //b = extract_continuous_bits(r, k -1 -((int)(k/2)),k-1);
    b = extract_continuous_bits(r, k-(int)(k/2),k-1);
    check_hard(e==14,"E INvalid acutally %ld", e);
    check_hard(b==6,"B INvalid it's actually %d", b);
    p =  (1 << (k)) -1; // account for paper assuming 1 indexed
    check_hard(p==127,"P INvalid %ld", p);

    // if this is correct, then wtf was p for?
    data_block * block = flex->index_block[b];
    //if(type == FLEXRETRIEVE) requested_data = block->user_data[e];
    if(type == FLEXRETRIEVE) requested_data = block[e];
    if(type == FLEXINSERT){
        log_warn("%ld",flex->last_data_size);
        //check_hard(block->user_data[e],"Invalid memory");
        check_hard(block[e],"Invalid memory");
        log_warn("madeit");
        check_hard(requested_data,"Invalid memory");
        //block->user_data[e] = requested_data;
        block[e] = requested_data;
    }
    return SUCCESS;
}

extern DSTATUS flex_insert(flex_t flex, void * user_data, unsigned long int requested_index){
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
