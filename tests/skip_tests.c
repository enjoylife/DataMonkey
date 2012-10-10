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

char * test_search()
{
    char * r = NULL;
    char hello [] = "Hello";
    skip_t s = skip_init(NULL);
    skip_insert(s, 9, hello);
    skip_insert(s, 1111111111, hello);
    skip_insert(s, 1, hello);
    skip_insert(s, 7, hello);
    skip_insert(s, 7, hello);
    r = skip_search(s, 7);
    mu_assert(r != NULL, "Failed to find key");
    char * z = NULL;
    z = skip_search(s, 70);
    mu_assert(z == NULL, "Failed search gave back an noninserted key");
    return NULL;
}



char *all_tests()
{
	mu_suite_start();
	mu_run_test(test_init);
	mu_run_test(test_insert);
    mu_run_test(test_search);
	return NULL;
}

RUN_TESTS(all_tests);


