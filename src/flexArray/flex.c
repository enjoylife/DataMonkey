#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include "./flex.h"
#include "../dbg.h"

#define CEILING(x,y) (((x) + (y) - 1) / (y))
#ifdef TRUE
#undef TRUE
#endif

#ifdef FALSE
#undef FALSE
#endif

typedef enum { FALSE, TRUE } bool;


inline int default_msb(unsigned long int i);
inline unsigned extract_continuous_bits(unsigned long value, int start, int end);



typedef struct flex_array{
    free_func_t free_func;
    long int num_elements;              // n

    long int num_super_blocks;          // s

    long int num_nonempty_data_blocks; // d
    long int num_empty_data_blocks;    // either 0 or 1 

    long int index_length;

    // Lastof's
    long int last_data_occup, last_data_size;
    long int last_super_occup, last_super_size;
    long int last_index_occup, last_index_size;

    bool is_data_full;
    bool is_super_full;
    bool is_index_full;

    index_p index_block; // our 2 dimensional array with each col being 2 ^(k/2) in size
} flex_array;

inline static void default_free (void *ptr){
    free(ptr);
}

extern flex_t flex_init(unsigned long int size){
    flex_array *  new_flex;

    new_flex = malloc(sizeof(flex_array)); 
    new_flex->free_func =  default_free;

    new_flex->index_block = malloc(sizeof(index_p)); // single index 
    new_flex->index_block[0] = malloc(sizeof(data_p));
    (new_flex->index_block[0])= NULL; 
    new_flex->index_length = 1;

    new_flex->num_elements = 0;
    new_flex->num_nonempty_data_blocks = 0;
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
    // if the last data block is full
    // TODO: this is wrong, right now were comparing it's current incr with its size,
    // needs to be 
    if(flex->is_data_full){
        // (a)
        // if the lastsuperblock is full
        //log_warn("super occup %ld, super size %ld", flex->last_super_occup, flex->last_super_size);
        if(flex->is_super_full){
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
        //if(flex->num_empty_data_blocks == 0){
            // if index block is full
            log_warn("index: [occup %ld,  size %ld]", flex->last_index_occup, flex->last_index_size);
            if(flex->last_index_occup == flex->last_index_size){
                log_success("WE MADE IT");
                // reallocate the index block to twice its current size
                flex->last_index_size = flex->last_index_size *2;
                flex->index_block = realloc(flex->index_block, sizeof(index_p) * flex->last_index_size);
                flex->index_length *=2;
            }
            // allocate a new last data block and point to it from index block
            data_p  new_data_block = malloc(sizeof(data_p) * flex->last_data_size);
            flex->index_block[flex->last_index_size] = new_data_block;
            log_success("Doubled the size of the INDEXblock");
        //}

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

DSTATUS flex_locate(flex_t flex, data_p requested_data, unsigned long int requested_index, FLEXACTION type){

    long int  k, p, r; // Same  as in Paper's pseudocode
    int e, b,temp;
    
    r = requested_index +1 ;
    k = default_msb(r); // paper assumes 1 indexed while our extract assumes 0 indexed
    //k = __builtin_ctzl(r); // paper assumes 1 indexed while our extract assumes 0 indexed
    //check_hard(k==7,"K INvalid acutally %ld", k);
    e = extract_continuous_bits(r, 0,  CEILING(k,2));
    //b = extract_continuous_bits(r, k -1 -((int)(k/2)),k-1);
    b = extract_continuous_bits(r, k-(int)(k/2),k-1);
    //check_hard(e==14,"E INvalid acutally %ld", e);
    //check_hard(b==6,"B INvalid it's actually %d", b);
    p =  (1 << (k)) -1; // account for paper assuming 1 indexed
   // check_hard(p==127,"P INvalid %ld", p);

    // if this is correct, then wtf was p for?
    data_p block = flex->index_block[b];
    //if(type == FLEXRETRIEVE) requested_data = block->user_data[e];
    if(type == FLEXRETRIEVE) requested_data =(data_p) block[e];
    if(type == FLEXINSERT){
        log_warn("%ld",flex->last_data_size);
        //check_hard(block->user_data[e],"Invalid memory");
        log_warn("madeit");
        log_warn("index length: %ld", flex->index_length);
        check_hard(block,"Invalid memory");
        check_hard(*requested_data,"Invalid memory");
        //block->user_data[e] = requested_data;
        block = (data_p)requested_data;
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
