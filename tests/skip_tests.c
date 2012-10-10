#include <assert.h>
#include "minunit.h"
#include "../src/skipList/dbg.h"
#include "../src/skipList/skip.h"

#define EPSILON 0.0000001 // for float_check

int float_check(double a, double b);
/* Helpers */

int float_check(double a, double b)
{
	return (a - b) < EPSILON;
}

char * test_init()
{
    skip_t s = skip_init(NULL);
    mu_assert(s, "Failed to create skip list");
    return NULL;
}

char * test_insert()
{
    int rvalue;
    char hello [] =  "Hello";
    skip_t s = skip_init(NULL);
    skip_insert(s, 1, hello);
    skip_insert(s, 10, hello);
    skip_insert(s, 7, hello);
    skip_insert(s, 5, hello);
    skip_insert(s, 3, hello);
    rvalue = skip_insert(s, 3, hello);
    mu_assert(rvalue == 2, "Failed return Value");
    return NULL;
}


char *all_tests()
{
	mu_suite_start();
	mu_run_test(test_init);
	mu_run_test(test_insert);
	return NULL;
}

RUN_TESTS(all_tests);


