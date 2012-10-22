#include <stdlib.h>
#include <limits.h>
#include "./flex.h"


inline int default_msb(unsigned long int i);
inline unsigned extract_continuous_bits(unsigned long value, int start, int end);

typedef struct data_block {
    int size;
    void * user_data[];
} data_block;

typedef struct flex_array{
    int num_elements;
    int num_super_blocks;
    int num_nonempty_data_blocks;
    int num_empty_data_blocks;
    struct last {
        int data_block_location;
        int data_block_size;
        int super_block_location;
        int supe_block_size;
        int index_block_location;
        int index_block_size;
    } last;
    data_block * index_block[];
} flex_array;


DSTATUS flex_grow(flex_t flex){
    DSTATUS error;

//    if(flex->last.

}

DSTATUS flex_locate(flex_t flex, void * requested_data, unsigned long int requested_index){

    long int  k, b, e, p, r; // Same  as in Paper's pseudocode
    
    r = requested_index +1 ;
    k = default_msb(r) -1;     
    b = extract_continuous_bits(r, r, r - (k/2));
    e = extract_continuous_bits(r, r -(k/2), 0);
    p =  (1 << k) -1 ;
    data_block * block = flex->index_block[k];
    requested_data = block->user_data[p+b];


    return  SUCCESS;
}

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
