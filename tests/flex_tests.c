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
    int x = 5;
    flex_t f = flex_init(1);
    data_p hello2 = malloc(sizeof(data_p));
    int y = 1;
    data_p hello = malloc(sizeof(data_p));
    hello2 = &y;
    hello = &x;
    flex_insert(f,hello, 700);
    flex_locate(f,hello2, 700,FLEXRETRIEVE);
    log_info("h is %p and %d",hello,*hello);
    log_info("h2 is %p and %d",hello2,*hello2);
    mu_assert(*hello2 == *hello, "Failed");
    data_p a = malloc(sizeof(data_p));
    data_p b = malloc(sizeof(data_p));
    a = &y;
    b = &x;
    flex_insert(f,a, 40);
    flex_locate(f,b,40,FLEXRETRIEVE);
    mu_assert(*a == *b, "FAILD");

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


