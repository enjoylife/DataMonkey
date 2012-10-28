#include <math.h>
#include <limits.h>
#include "minunit.h"
#include "../src/dbg.h"
#include "../src/flexArray/flex.h"

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
    
void test_free(void * ptr){
    free(ptr);
}

char * sanity_check()
{
    // proof of correct ceilling macro
    mu_assert(CEILING(7,4) == 2, "ceiling fail");
    // proof of implicit floor
    mu_assert(7/4  == 1, "floor fail");
    mu_assert(1/2  == 0, "floor fail");
    // proof of zero index
    mu_assert(LEADINGBIT(4) ==  2  ,"FAIL");
    mu_assert(LEADINGBIT(1) ==  0  ,"FAIL");
    mu_assert(LEADINGBIT(8) ==  3  ,"FAIL");

    unsigned int x,y;
    long int a = 0;
    unsigned int data_size = 1, super_count = 0, super_last_count = 0, super_size=1, length = 6;
    long int ** index = malloc(length * sizeof(long int));;
    for(x = 0 ; x < length; x++){
        /*
        printf("\nrow: %d,data_size: %d, super_size: %d," 
        "super_last_count: %d, super_count: %d\n",
        x, data_size,super_size,super_last_count, super_count);
        */
        index[x] = malloc(data_size * sizeof(long int));
        for(y = 0; y < data_size; y++){
            index[x][y]=a;
            a++;
            printf("%ld ", index[x][y]);
        }
        super_last_count++;
        printf("\n");
        if(super_last_count == super_size){
            super_last_count = 0;
            if(super_count%2){
                super_size *=2;
                super_count++;
            } else {
                    data_size *= 2;
                super_count++;
            }
        }
    }
    log_success("Finished element dump");

    long int get_index(unsigned long int i){
        unsigned long int r,k,b,e,p;
       // log_info("Trying to get %ld", i);
        r = i + 1;
     //   log_info("R: %s",byte_to_binary(r));
        k = LEADINGBIT(r); // no need for minus 1. already zero indexed PERFECT
      //  log_info("k/2=%ld, Ceil(k,2)=%ld",k/2,CEILING(k,2));
        b = BITSUBSET(r,k-k/2,k);
        e = BITSUBSET(r,0, CEILING(k,2));
        p = k==0 ? 0 :  (1 << k-1) ; //PEFECT
        //p = (1 << k-1) ; //PEFECT
        //log_info("K: %ld",k);
        //log_info("B: %ld",b);
        //log_info("E: %ld",e);
        //log_info("P: %ld super blocks prior",p);
        //log_info("trying [%ld,%ld]\n",(p+b),e);
        return index[(p+b)][e];
    }
    mu_assert(get_index(0)==0,"Fail");
    mu_assert(get_index(1)==1,"Fail");
    mu_assert(get_index(2)==2,"Fail");
    mu_assert(get_index(3)==3,"Fail");
    mu_assert(get_index(4)==4,"Fail");
    mu_assert(get_index(5)==5,"Fail");
    mu_assert(get_index(6)==6,"Fail");
    mu_assert(get_index(7)==7,"Fail");
    mu_assert(get_index(8)==8,"Fail");
    mu_assert(get_index(9)==9,"Fail");
    mu_assert(get_index(11)==11,"Fail");
    mu_assert(get_index(12)==12,"Fail");
    mu_assert(get_index(13)==13,"Fail");
    mu_assert(get_index(14)==14,"Fail");

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
    //fflush(stdout);
    //printf("\n");
    data_p x = malloc(sizeof(data_p));
    data_p y = malloc(sizeof(data_p));
    *x = 5;
    *y = 7;
    flex_t f = flex_init();
    //flex_index_init(f, 10);
    flex_grow(f);
    flex_grow(f);
    flex_grow(f);
    flex_grow(f);
    flex_grow(f);
    flex_grow(f);
    flex_grow(f);
    flex_grow(f);
    flex_grow(f);
    flex_grow(f);
    flex_grow(f);
    flex_grow(f);
    flex_grow(f);
    flex_grow(f);
    flex_grow(f);
    //flex_insert(f,x, 0);
    //flex_insert(f,y, 1);
    //flex_insert(f,x, 2);
    //flex_insert(f,y, 3);
    //flex_insert(f,x, 4);
    flex_debug_out(f);
//    flex_string_dump(f);


    return NULL;
}
    

char *all_tests()
{
	mu_suite_start();
//    mu_run_test(sanity_check);
	mu_run_test(test_init);
	mu_run_test(test_locate);
	return NULL;
}

RUN_TESTS(all_tests);
