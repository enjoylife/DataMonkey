#include <math.h>
#include "minunit.h"
#include "../src/dbg.h"
#include "../src/flexArray/flex.h"

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
    flex_t f = flex_init(20);
    mu_assert(f,"failed to create flex array");
    return NULL;
}

char * test_locate()
{
    flex_t f = flex_init(1);
    data_p hello = malloc(sizeof(data_p));
    flex_insert(f,hello, 7);

    return NULL;
}
    

char *all_tests()
{
	mu_suite_start();
	mu_run_test(test_init);
	mu_run_test(test_locate);
	return NULL;
}

RUN_TESTS(all_tests);


