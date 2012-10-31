#include <math.h>
#include <limits.h>
#include "minunit.h"
#include "../src/dbg.h"
#include "../src/flexArray/flex.h"
#include "../src/flexArray/flex.c"

#define EPSILON 0.0000001

int float_check(double a, double b);

/* Helpers */
int float_check(double a, double b)
{
	return (a - b) < EPSILON;
}

void test_free(void * ptr){
    free(ptr);
}

char * sanity_check()
{
    
    return NULL;
}

char * test_init()
{
    flex_t f = flex_init(10);
    mu_assert(f,"failed to create flex array");
    mu_assert(f->array,"FAiled to create array");
    flex_destroy(f);
    return NULL;
}

char * test_stuff()
{
    DSTATUS s;
    flex_t f = flex_init(9);
    mu_assert(f->index_length == 8,"Wrong index length");
    s = flex_insert(f,0,434,DEF);
    mu_assert(s==SUCCESS,"fail return value");
    mu_assert(f->array[0] == 434,"failure in data value");
    s = flex_insert(f,8,1337,DEF);
    mu_assert(s==SUCCESS,"fail return value");
    mu_assert(f->array[8] == 1337,"failure in data value");
    mu_assert(f->index_length == 8,"Wrong index length");
    s = flex_insert(f,13,1337,DEF);
    mu_assert(s==SUCCESS,"fail return value");
    mu_assert(f->array[13] == 1337,"failure in data value");
    s = flex_insert(f,10,1337,DEF);
    s = flex_insert(f,10000,1337,DEF);


    mu_assert(f,"failed to create flex array");
    flex_destroy(f);
    return NULL;
}

char * test_locate()
{
    flex_t f = flex_init(51);
    DSTATUS s = flex_
    return NULL;
}


char * test_shrink(){
   return NULL;
}

char *all_tests()
{
	mu_suite_start();
    mu_run_test(sanity_check);
	mu_run_test(test_init);
    mu_run_test(test_stuff);
	mu_run_test(test_locate);
    mu_run_test(test_shrink);
	return NULL;
}

RUN_TESTS(all_tests);
