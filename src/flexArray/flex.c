#include <stdlib.h>
#include "./flex.h"

struct_error flex_grow(flex_t * flex);
struct_error flex_shrink(flex_t * flex);


typedef struct {
    int num_elements;
    int num_super_blocks;
    int num_open_data_blocks;
    int num_empty_data_blocks;
    struct last_block {
        int data_block;
        int super_block;
        int index_block;
    } last;
    void * index_block [];
} flex_array;





