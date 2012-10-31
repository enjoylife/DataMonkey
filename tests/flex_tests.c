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
const char* byte_to_binary( unsigned long int x )
{
    static char b[sizeof(x)*8+1] = {0};
    int y;
    long long z;
    for (z=(1LL<<sizeof(x)*8)-1,y=0; z>0; z>>=1,y++)
    {
        b[y] = ( ((x & z) == z) ? '1' : '0');
    }
    b[y] = 0;

    return b;
}
char *binrep (unsigned int val, char *buff, int sz) {
    char *pbuff = buff;

    /* Must be able to store one character at least. */
    if (sz < 1) return NULL;

    /* Special case for zero to ensure some output. */
    if (val == 0) {
            *pbuff++ = '0';
                *pbuff = '\0';
                    return buff;
                    }

    /* Work from the end of the buffer back. */
    pbuff += sz;
    *pbuff-- = '\0';

    /* For each bit (going backwards) store character. */
    while (val != 0) {
            if (sz-- == 0) return NULL;
                *pbuff-- = ((val & 1) == 1) ? '1' : '0';

                    /* Get next bit. */
                    val >>= 1;
                    }
    return pbuff+1;
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
    flex_t f = flex_init();
    mu_assert(f,"failed to create flex array");
    return NULL;
}

char * test_locate()
{
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
    mu_run_test(test_data_dum);
	mu_run_test(test_locate);
    mu_run_test(test_shrink);
	return NULL;
}

RUN_TESTS(all_tests);
