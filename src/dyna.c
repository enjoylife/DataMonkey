/* File: dyna.c 
 * Version: 1.0
 * Purpose: Implementation of simple dynamic array 
 * Known issues: 
 * Author: Matthew Clemens
 * Copyright: MIT
*/
#include <stdlib.h>
#include <limits.h>
#include "dyna.h"
#include "debug.h"

extern inline dyna_t dyna_init(index_t inital_size){
    dyna_array * new_dyna = malloc(sizeof(*new_dyna)); 
    index_p new_array = malloc(sizeof(data_p) * (inital_size)); 
    new_dyna->array = new_array;
    new_dyna->index_length = inital_size;
    new_dyna->last_index = 0;
    return new_dyna;
}

extern inline DSTATUS dyna_destroy(dyna_t dyna, free_func_t custom_free){
    index_t i;
    data_p stuff;
    if(custom_free != NULL){
        for(i=0; i<dyna->index_length; i++){
             stuff = dyna_get(dyna, i);
             if(stuff) custom_free(stuff);
        }
    }
    if(dyna->array)free(dyna->array);
    free(dyna);
    return SUCCESS;
}

extern inline data_p dyna_get(dyna_t dyna, index_t requested_index){
    return dyna->array[requested_index];
}

extern inline DSTATUS dyna_insert(dyna_t dyna, index_t requested_index, data_p user_data,  DSTATUS modifier){
    DSTATUS s;
    switch(modifier){
        case GROW: 
            while(requested_index > dyna->index_length){
               s =  dyna_grow(dyna);
               if(s ==FAILURE) return FAILURE;
            }
        break;
        case CHECK:
            if(requested_index > dyna->index_length) return FAILURE;
        case DEF:
        default: //fall thru
            break;
    }
    if(dyna->last_index < requested_index) dyna->last_index = requested_index;
    dyna->array[requested_index] = user_data;
    return SUCCESS;
}

extern inline DSTATUS dyna_delete(dyna_t dyna, index_t requested_index, free_func_t custom_free){

    if(requested_index > dyna->last_index) return FAILURE;
    if(!dyna->array[requested_index]) return FAILURE;
    if(dyna->last_index == requested_index){
        while( requested_index > 0 && dyna->array[--requested_index] != NULL);
        dyna->last_index = requested_index;
    }
    custom_free(dyna->array[requested_index]);
    dyna->array[requested_index] = NULL;
    return SUCCESS;
}

extern inline DSTATUS dyna_grow_to(dyna_t dyna, index_t size){
    index_p new_array;

    if(size <= dyna->index_length) return FAILURE;
    new_array = realloc(dyna->array, sizeof(data_t)*size);
    if(new_array == NULL){
        dyna->array = NULL;
        return FAILURE;
    }
    dyna->array = new_array;
    dyna->index_length = size;
    return SUCCESS;
}

extern inline DSTATUS dyna_grow(dyna_t dyna){
    index_p new_array;
    index_t new_size;
    
     //The growth pattern is:  0, 4, 8, 16, 25, 35, 46, 58, 72, 88, ...
    new_size =  (dyna->index_length >> 3) + (dyna->index_length < 9 ? 3 : 6);
    log_info("diff:[%ld], new[%ld]",(long int)new_size, (long int)new_size + dyna->index_length);
    new_size += dyna->index_length;
    new_array = realloc(dyna->array, sizeof(data_t)*new_size);
    check(new_array,"Failed to grow");
    dyna->array = new_array;
    dyna->index_length = new_size;
    return SUCCESS;
error:
    return FAILURE;
}

extern inline DSTATUS dyna_shrink(dyna_t dyna){
    index_p new_array;
    index_t new_size = dyna->index_length / 2 ;
    log_info("new size [%ld]",(long int)new_size);
    new_array = realloc(dyna->array, sizeof(data_t)*new_size);
    check(new_array,"Failed to shrink");
    dyna->array = new_array;
    dyna->index_length = new_size;
    return SUCCESS;
error:
    dyna->array = NULL;
    return FAILURE;
}

inline void dyna_debug_out(dyna_t dyna){
    log_warn("-----BEGIN DEBUG-------");
    log_warn("Index length -----[%ld]",(long int) dyna->index_length);
    log_warn("Last index -------[%ld]",(long int) dyna->last_index);
    log_warn("-----END DEBUG----------");
}
