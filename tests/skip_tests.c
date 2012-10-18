#include "minunit.h"
#include "../src/skipList/dbg.h"
#include "../src/skipList/skip.h"

#define EPSILON 0.0000001 // for float_check

/* We have to be careful in this test func's with a while loop because the while loop may run indefinitely 
 * in the event thatwe exhaust the key space that the rand function emits before the skip_list's rand func
 * makes us jump up to the next level. (rand % num) is the culprit if the test case hangs.
 * TODO: Find a better solution to those tests. */
int float_check(double a, double b);
/* Helpers */

int float_check(double a, double b)
{
	return (a - b) < EPSILON;
}

void test_free(void * ptr){
    free(ptr);
}
char * test_init()
{
    skip_t s = skip_init(NULL);
    mu_assert(s, "Failed to create skip list");
    skip_destroy(s);
    return NULL;
}

char * test_insert()
{
    int rvalue;
    char hello [] =  "Hello";
    skip_t s = skip_init(NULL);
    skip_insert(s, 1, hello);
    skip_insert(s, 10, hello);
    mu_assert(skip_length(s) == 2, "Failed length");
    skip_insert(s, 7, hello);
    skip_insert(s, 5, hello);
    skip_insert(s, 3, hello);
    rvalue = skip_insert(s, 3, hello);
    mu_assert(rvalue == 2, "Failed return Value");
    skip_destroy(s);
    return NULL;
}

char * test_search()
{
    char hello [] = "Hello";
    char world [] = "World";
    //char  * fake_data  = malloc(sizeof(*fake_data)*30);
    
    skip_t s = skip_init(NULL); // default free
    skip_insert(s, 9, hello);
    skip_insert(s, 11, world);
    skip_insert(s, 1, hello);
    skip_insert(s, 7, hello);
    skip_insert(s, 7, hello);

    char * r = NULL;
    r = skip_search(s, 7);
    mu_assert(r != NULL, "Search func failure");
    mu_assert(skip_length(s) == 4, "Failed length");

    r = NULL;
    r = skip_search(s, 9);
    mu_assert(r != NULL, "Search func failure");

    r = NULL;
    r = skip_search(s, 1);
    mu_assert(r != NULL, "Search func failure");

    r = NULL;
    r = skip_search(s, 2);
    mu_assert(r == NULL, "Search func failure");

    char * z = NULL;
    z = skip_search(s, 70);
    mu_assert(z == NULL, "Failed search gave back an noninserted key");
    skip_destroy(s);
    return NULL;
}
char * test_delete_node()
{
    unsigned int key;
    int level;
    int length;
    int * hello;
    skip_t s = skip_init(free);
    // wait till we jump to third level
    while(skip_level(s) < 2){
    hello = malloc(sizeof( int) * 10);
        // give it a large space to avoid hangs
        key = (rand() % 1000) + 1;
        skip_insert(s, key, hello);
    }
    length = skip_length(s);
    // immediately delete that node to make us go back down
    level = skip_level(s);
    skip_delete(s,key);
    mu_assert(skip_length(s) == length -1,"failed to decrease length" )
    mu_assert(skip_level(s) <= level-1, "Failed to decrease level");
    skip_destroy(s);

    return NULL;
}

char * test_delete_skip_list(){

    unsigned int key;
    int * hello;
    skip_t s = skip_init(test_free);
    // wait till we jump to third level
    while(skip_level(s) < 3){
        hello  = malloc(sizeof( int) * 10);
        // give it a large space to avoid hangs
        key = (rand() % 1000) + 1;
        skip_insert(s, key, hello);
    }
    skip_destroy(s);
    return NULL;
}

char * test_everything()
{
    char hello [] = "Hello";
    char world [] = "World";
    char  * fake_data  = malloc(sizeof(*fake_data)*30);
    
    skip_t s = skip_init(NULL); // default free
    skip_insert(s, 9, hello);
    skip_delete(s,1);

    char * z = NULL;
    z = skip_search(s, 70);
    mu_assert(z == NULL, "Failed search gave back an noninserted key");
    free(fake_data);
    skip_destroy(s);
    return NULL;
}
char * test_search_finger()
{
    unsigned int key;
    int num_found;
    int level;
    int * hello;

    skip_t s = skip_init(free);
    while(skip_level(s) < 2){
    hello = malloc(sizeof( int) * 10);
        key = (rand() % 200) + 1;
        skip_insert(s, key, hello);
    }
    debug("NUM is %d",skip_length(s));
    debug("MADIT");
    num_found = 0;
    void * response = NULL;
    while(num_found< skip_length(s)){
        key = (rand() % 200) + 1;
        response =  skip_finger_search(s, key);
        if(response){
            num_found++;
        }
    }
    skip_destroy(s);

    return NULL;
}

    

char *all_tests()
{
	mu_suite_start();
	mu_run_test(test_init);
	mu_run_test(test_insert);
    mu_run_test(test_search);
    mu_run_test(test_delete_node);
    mu_run_test(test_delete_skip_list);
    mu_run_test(test_everything);
    mu_run_test(test_search_finger);
	return NULL;
}

RUN_TESTS(all_tests);


