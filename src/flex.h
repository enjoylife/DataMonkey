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

typedef enum DSTATUS {
    SUCCESS,
    FAILURE,
    EQL,
    NEQL,
    LT,
    GT
}DSTATUS;

/* Our application datatype */
typedef int *data_p;

/* Internal datatypes */
typedef data_p *index_p;
typedef unsigned long int index_t;

/* Public facing typedef's for our array and it's runtime function's.*/
typedef struct flex_array * flex_t;
typedef DSTATUS (*free_func_t)(data_p);
typedef DSTATUS (*cmp_func_t)(data_p, data_p);

/* Private per say,use the public functions to edit.*/
typedef struct flex_array {
    free_func_t free_func;
    cmp_func_t cmp_func;
    index_p index_block; // array of data_blocks
    index_t num_user_elements_inserted;
    index_t usable_data_blocks;
    index_t index_length;   
    index_t num_super_blocks;   
    index_t last_index_occup;
    index_t last_data_size;  
    index_t last_super_size;
    index_t last_super_occup;
} flex_array;

/* Container altering functions.*/

/* Call to create our flexible array datastructure.
 * [params] none
 * [return] {flex_t} our newly initalized array, or NULL in the event of failure.
 */
extern flex_t flex_init(void);

/* Removes all internal memory associated with the data structure.
 * However it does not free the application elements contained within.  
 * ----------------------------------------------------------------------------------------
 * [params] {flex} a non null flex array
 * [return] {SUCCESS} on a good destory, FAILURE otherwise.
 */
extern DSTATUS flex_destroy(flex_t flex);

/* Removes all internal memory associated with the data structure and application data too.
 * ----------------------------------------------------------------------------------------
 * [params] {flex} a non null flex array
 * [return] {SUCCESS} on a good destory, FAILURE otherwise.
 */
extern DSTATUS flex_nuke(flex_t flex);

/* Change the array's called function when freeing application elements.
 * ---------------------------------------------------------------------
 * [params] {flex} nonvoid flex array to work with.
 *          {func} function with correct typedef to be called.
 * [return] {SUCCESS} if change was applied.
 *          {FAILURE} if otherwise.
 */
extern DSTATUS flex_change_free(flex_t flex, free_func_t func);

/* Inserts the application data into the specified index, making sure to grow the array till it fits.
 * --------------------------------------------------------------------------------------------------
 * [params] {flex} a nonvoid flex array to work with.
 *          {requested_index} where to put the data.
 *          {user_data} the data that will be stored.
 * [return] {SUCCESS} if inserting and any needed growing were completed.
 *          {FAILURE} if any needed growing was unable to complete.
 */
extern DSTATUS flex_insert(flex_t flex ,index_t requested_index, data_p user_data);

/* Deletes the application data into the specified index, making sure to shrink the array if nessacary.
 * --------------------------------------------------------------------------------------------------
 * [params] {flex} a nonvoid flex array to work with.
 *          {requested_index} where to put the data.
 *          {user_data} the data that will be stored.
 * [return] {SUCCESS} if inserting and any needed growing were completed.
 *          {FAILURE} if any needed growing was unable to complete.
 */
extern DSTATUS flex_delete(flex_t flex ,index_t requested_index);

/* Compares the data at the index with the element provided.
 * Uses the default compare function if not changed. */
extern DSTATUS flex_compare(flex_t flex, index_t requested_index, data_p user_data);
/* [params] {flex} a nonvoid flex array to work with.
 *          {requested_index} which data to compare.
 *          {user_data} the data to be compared against.
 * [return] {FAILURE} if index is out of bounds.
 *          {EQL} if data is the same.
 *          {LT} if contained data is less then user_data.
 *          {GT} if contained data is greater then user_data.
 */


/* Applies the action across all the contained elements.*/
extern DSTATUS flex_traverse(flex_t flex, DSTATUS (*action)(data_p));
/* [params] {flex} nonvoid flex array to work with.
 *          {action} a function with the correct typedef.
 * [return] {SUCCESS} if on every call to action a return of SUCCESS was found.
 *          {FAILURE} on the first occurance of a returned FAILURE by action .
 */

extern DSTATUS flex_sort(flex_t flex);

/* Private api */
DSTATUS flex_grow(flex_t flex);
DSTATUS flex_shrink(flex_t  flex);
void flex_string_dump(flex_t flex);
void flex_debug_out(flex_t flex);


#endif
