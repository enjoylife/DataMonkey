/* File: dyna_test.c 
 * Version: 1.0
 * Purpose: testing of simple dynamic array 
 * Author: Matthew Clemens
 * Copyright: MIT
*/
#include "minunit.h"
#include "debug.h"
#include "dyna.h"

char * test_init(){
    dyna_t d = dyna_init(10);
    mu_check(d);
    mu_check(d->index_length == 10);
    mu_check(d->last_index == 0);
    dyna_destroy(d, NULL);
    return 0;
}
char * test_grow(){
    dyna_t d = dyna_init(0);
    mu_check(dyna_grow(d) == SUCCESS);
    mu_check(dyna_grow(d) == SUCCESS);
    mu_check(dyna_grow(d) == SUCCESS);
    mu_check(dyna_grow(d) == SUCCESS);
    mu_check(dyna_grow(d) == SUCCESS);
    mu_check(dyna_grow(d) == SUCCESS);
    dyna_destroy(d,NULL);
    return 0;
}
char * test_shrink(){
    dyna_t d = dyna_init(0);
    mu_check(dyna_shrink(d) == FAILURE);
    mu_check(dyna_shrink(d) == FAILURE);
    mu_check(dyna_grow(d) == SUCCESS);
    mu_check(dyna_grow(d) == SUCCESS);
    mu_check(dyna_grow(d) == SUCCESS);
    mu_check(dyna_grow(d) == SUCCESS);
    mu_check(dyna_grow(d) == SUCCESS);
    mu_check(dyna_grow(d) == SUCCESS);
    mu_check(dyna_shrink(d) == SUCCESS);
    mu_check(dyna_shrink(d) == SUCCESS);
    mu_check(dyna_shrink(d) == SUCCESS);
    mu_check(dyna_shrink(d) == SUCCESS);
    mu_check(dyna_shrink(d) == SUCCESS);
    mu_check(dyna_shrink(d) == FAILURE);
    dyna_destroy(d,NULL);
    return 0;
}

char * test_insert(){
    dyna_t d = dyna_init(10);

    dyna_destroy(d,NULL);
    return 0;
}


char * all_tests()
{
    mu_suite_start();
    mu_run_test(test_init);
    mu_run_test(test_grow);
    mu_run_test(test_shrink);
    mu_run_test(test_insert);
    return 0;
}

RUN_TESTS(all_tests);
