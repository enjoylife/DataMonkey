#ifndef _flex_h
#define _flex_h

#define CEILING(x,y) (((x) + (y) - 1) / (y))
#define LEADINGBIT(r) (sizeof(index_t)*8 - __builtin_clzl(r)-1)
#define BITLAST(k,n) ((k) & ((1<<(n))-1))
#define BITSUBSET(k,m,n) BITLAST((k)>>(m),((n)-(m)))

#ifdef TRUE
#undef TRUE
#endif

#ifdef FALSE
#undef FALSE
#endif

typedef enum bool { FALSE, TRUE } bool;
typedef enum FLEXACTION {
    INSERT,
    EXPAND,
    SHRINK,
    UPDATE,
    RETRIEVE,
} FLEXACTION;


typedef enum  {
    SUCCESS,
    FAILURE,
    FOUND,
    MISSING,
    GREW,
    SHRANK
}DSTATUS;

/* Our type which holds the various sized data_blocks. */
typedef int *data_p;
typedef data_p *index_p;
typedef unsigned long int index_t;

typedef void (*free_func_t)(data_p);
typedef struct flex_array * flex_t;


/* Private per say,use the public functions to edit.*/
typedef struct flex_array
{
    free_func_t free_func;
    index_p index_block; // array of data_blocks
    index_t num_user_elements_inserted;
    index_t usable_data_blocks;
    index_t index_length;   
    index_t num_super_blocks;   
    index_t last_index_occup;
    index_t last_data_size;  
    index_t last_super_size;
} flex_array;

/* Private api */
DSTATUS flex_locate(flex_t  flex, data_p requested_data, index_t requested_index, FLEXACTION);
DSTATUS flex_grow(flex_t flex);
DSTATUS flex_shrink(flex_t  flex);
void flex_string_dump(flex_t flex);
void flex_debug_out(flex_t flex);



/* Container altering functions.*/
extern flex_t flex_init(void);
extern DSTATUS flex_destroy(flex_t flex);
extern DSTATUS flex_change_free(flex_t flex, free_func_t func);

/* Element manipulation functions.*/
extern DSTATUS flex_insert(flex_t flex, data_p user_data, index_t requested_index);
extern DSTATUS flex_traverse(flex_t flex, void (*action)(data_p));

#endif
