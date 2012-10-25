#include <stdlib.h>
#include <limits.h>
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
    unsigned int index_block_length;
    unsigned int data_block_length;
    unsigned long int num_elements;              // n
    long int num_super_blocks;          // s
    long int num_nonempty_data_blocks; // d
    long int num_empty_data_blocks;    // either 0 or 1 
    long int last_data_occup, last_data_block_length;  // Lastof's
    long int last_super_occup, last_super_size;
    long int last_index_occup, last_index_block_length;
    index_p index_block; 
} flex_array;

inline void flex_debug_out(flex_t flex){
    log_success("Begin flex debug");
    log_warn("data  [occup %ld, size %ld]", flex->last_data_occup, flex->last_data_block_length);
    log_warn("super [occup %ld, size %ld]", flex->last_super_occup, flex->last_super_size);
    log_warn("index [occup %ld, size %ld]\n", flex->last_index_occup, flex->last_index_block_length);
    log_warn("index length [%d]", flex->index_block_length);
    log_warn("data length  [%d]", flex->data_block_length);
    log_warn("super length [%ld]", flex->num_super_blocks);
    log_success("End flex debug\n");
}

extern void flex_string_dump(flex_t flex){
    int i,y;
    int super_size = 1;
    int length = 1;
    log_warn("Starting array element dump");
    for(i = 0 ; i < flex->index_block_length ;i++){
        printf("row: %d,length: %d\n", i, length);
        for(y = 0; y < length;  y++){
            printf("%d ", flex->index_block[i][y]);
        }
        printf("\n");
        if(super_size % 2){
        } else {
                // double the number of elements in a data block
                length *= 2;
            }
        super_size++;
    }
    log_success("Finished element dump");
}

inline static void default_free (void *ptr){
    free(ptr);
}

extern flex_t flex_init(unsigned long int size){
    size= 0;// temp, just to remove gcc warn
    flex_array *  new_flex;
    index_p  new_index;

    new_flex = malloc(sizeof(flex_array)); 
    new_flex->free_func =  default_free;

    new_index = malloc(sizeof(index_p)); // single index 
    new_index[0] = malloc(sizeof(data_p));
    new_index[0][0] =0;
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
                flex->last_data_block_length /= 2;
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

inline extern DSTATUS flex_grow(flex_t  flex){
    //flex_debug_out(flex);
    // 1.
    if(flex->last_data_occup == flex->last_data_block_length){
        // (a)
        if(flex->last_super_occup == flex->last_super_size){
            flex->num_super_blocks++;
            if(flex->num_super_blocks % 2){
                // double the number of data blocks in a superblock
                flex->last_super_size *= 2;
              //  log_success("(STEP A) Doubled the size of a SUPERblock\n");
            } else {
                // double the number of elements in a data block
                flex->last_data_block_length *= 2;
             //   log_success("(STEP A) Doubled the size of a DATAblock\n");
            }
            flex->last_super_occup = 0;
            // (b) we skip it
            flex->last_index_block_length *= 2;
            flex->index_block = realloc(flex->index_block, sizeof(index_p) * flex->last_index_block_length);
            //log_success("(STEP B) Realloc of INDEXblock\n");
            // allocate a new last data block and point to it from index block
            data_p new_data_block = (data_p) malloc(sizeof(data_p) * flex->last_data_block_length);
            int i;
            log_err("MADIT");
            for(i=0; i < flex->last_data_block_length; i++){
                new_data_block[i] = 0;
            }
            flex->index_block_length++;
            flex->index_block[flex->index_block_length-1] = new_data_block;
            flex->last_index_occup++;
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
    int e, b;
    
    r = requested_index +1 ;
    //k = default_msb(r); 
    k = __builtin_ctzl(r); 
    // paper assumes 1 indexed while our extract assumes 0 indexed
    b = extract_continuous_bits(r, (k/2)+1 ,k-1);
    e = extract_continuous_bits(r, 0,CEILING(k,2) ); //ceiling
    p =  (1 << (k)) -1; // account for paper assuming 1 indexed
    
    /*
    check_hard(k==7,"K INvalid acutally %ld", k);
    check_hard(k/2 == 3,"Fail");
    check_hard(CEILING(k,2) == ,"Fail");
    check_hard(e==,"E INvalid acutally %ld", e);
    check_hard(b==,"B INvalid it's actually %d", b);
    check_hard(p==,"P INvalid %ld", p);
    */

    log_err("index length is %d", flex->index_block_length);
    log_err("K is %ld", k);
    log_err("E is %d", e);
    log_err("B is %d", b);
    log_err("P is %ld\n", p);

    // if this is correct, then wtf is p and b for?
    data_p block = (flex->index_block)[k];
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

// Just to double check myself when using gcc's  __builtin_ctzl
inline int default_msb(unsigned long int i){
    int r = 0;
    while (i >>= 1)
    {
      r++;
    }
    return r;
}
