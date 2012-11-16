/* File: dyna.h 
 * Version: 1.0
 * Purpose: API of simple dynamic array 
 * Author: Matthew Clemens
 * Copyright: MIT
*/
#ifndef _dyna_h
#define _dyna_h

typedef enum  {
    SUCCESS= 0,
    FAILURE,
    GROW,
    CHECK,
    DEF
}DSTATUS;

typedef int data_t;
typedef data_t* data_p;
typedef unsigned int index_t;

typedef void (*free_func_t)(data_p ptr);
typedef int (*cmp_func_t)(data_p a, data_p b);

/* Our type which holds the array */
typedef data_p* index_p;

/* Private per say, use the public functions to manipulate.*/
typedef struct dyna_array{
    index_p array; 
    index_t index_length;   
    index_t last_index;
} dyna_array;

typedef struct dyna_array * dyna_t;


/* Returns a dyna array.
 * ---------------------
 * [params] {inital_size} the size for the array on creation, has to be at least 1.
 * [return] {dyna_t} if malloc was succesful.
 *          {NULL} on inital_size error or malloc error.
 */
inline dyna_t dyna_init(index_t inital_size);

/* Destroys a dyna array in various ways
 * ------------------------------------
 *  If custom_free is NULL the application data will not be freed, otherwise it goes through each
 *  index and free's the element within by calling the custom_free.
 */
inline DSTATUS dyna_destroy(dyna_t dyna, free_func_t custom_free);

/* Element manipulation functions.*/
inline DSTATUS dyna_insert(dyna_t dyna,
        index_t requested_index, data_p user_data, DSTATUS modifier);

inline DSTATUS dyna_delete(dyna_t dyna, index_t requested_index, free_func_t custom_free);

inline data_p dyna_get(dyna_t dyna, 
        index_t requested_index);

/* Private api */
inline DSTATUS dyna_grow(dyna_t dyna);
inline DSTATUS dyna_grow_to(dyna_t dyna, index_t size);
inline DSTATUS dyna_shrink(dyna_t  dyna);
void dyna_string_dump(dyna_t dyna);
void dyna_debug_out(dyna_t dyna);

/* Internal extra helper headers 
inline static void default_free (data_p ptr);
inline static int default_cmp(data_p a, data_p b);
static inline int default_cmp(data_p a, data_p b){
    return a==b;
}

static inline void default_free(data_p ptr){
    free(ptr);
}*/
#endif
