#include <unistd.h>
#include <math.h>
#include <limits.h>
#include "minunit.h"
#include "flex.h"
#include "debug.h"

void test_free(data_p  ptr){
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

    index_t x,y, a = 0;
    index_t data_size = 1, super_count = 0, super_last_count = 0, super_size=1, length = 30;
    index_t ** index = malloc(length * sizeof(index_t));;
    fflush(stdout);
    //printf("\n");
    for(x = 0 ; x < length; x++){
        index[x] = malloc(data_size * sizeof(index_t));
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
    //log_success("Finished element dump");

    unsigned long int get_index(index_t i){
        index_t r,k,b,e,p;
       // log_info("Trying to get %ld", i);
        r = i + 1;
        k = LEADINGBIT(r); // no need for minus 1. already zero indexed PERFECT
      //  log_info("k/2=%ld, Ceil(k,2)=%ld",k/2,CEILING(k,2));
        b = BITSUBSET(r,k-k/2,k);
        e = BITSUBSET(r,0, CEILING(k,2));
        //p =  (1 << (k-1)) ; //PEFECT
        p = (1 << (k/2 + 1)) - 2 + (k & 1) * (1 << (k/2));
        //p = k==0 ? 0 :  (1 << k-1) ; //PEFECT
        //p = (1 << k-1) ; //PEFECT
        //log_info("K: %ld",k);
        //log_info("B: %ld",b);
        //log_info("E: %ld",e);
        //log_info("P: %ld super blocks prior",p);
        log_info("trying [%ld,%ld]",(p+b),e);
        printf("p+b,e : [%ld,%ld] \n",p+b,e);
        return index[(p+b)][e];
    }
    for(x = 0 ; x < length; x++){
    mu_assert(get_index(x)==x,"Fail");
    }

    return NULL;
}

char * test_init()
{
    flex_t f = flex_init();
    mu_assert(f,"failed to create flex array");
    mu_assert(f->index_block,"failed to create");
    flex_destroy(f);

    return NULL;
}

char * test_destroy_after_growth()
{
    index_t x;
    flex_t f = flex_init();
    for(x=0;x<4;x++){
        flex_grow(f);
    }
    flex_destroy(f);
    return 0;
}

char * test_traverse()
{
    DSTATUS fake_printf(data_p x){
        *x = 10;
        //printf("%ld\n",(index_t)*x);
        return SUCCESS;
    }
    int x;
    flex_t f = flex_init();
    for(x=0;x<40;x++){
        flex_grow(f);
    }
    flex_traverse(f,&fake_printf);
    flex_destroy(f);

    return 0;
}

char * test_shrink()
{
    DSTATUS status;
    flex_t f = flex_init();

    status = flex_shrink(f);
    mu_assert(status == FAILURE, "FAIL");
    status = flex_shrink(f);
    mu_assert(status == FAILURE, "FAIL");

    index_t  x, max = 500, grow_step=0;
    while(grow_step < max){
    index_t prior_num_elems =  f->num_user_elements_inserted;
    index_t prior_usable =  f->usable_data_blocks;
    index_t prior_num_super =  f->num_super_blocks;
    index_t prior_index_length = f->index_length;
    index_t prior_index_occup =  f->last_index_occup;
    index_t prior_data_size = f->last_data_size;
    index_t prior_super_size = f->last_super_size;
        for(x=0;x<grow_step ;x++){
            flex_grow(f);
        }
        //flex_debug_out(f);
        for(x=0;x<grow_step;x++){
            //flex_debug_out(f);
            flex_shrink(f);
            //flex_debug_out(f);
        //log_info("k= %ld, supersize=%ld, datasize=%ld\n",f->last_index_occup,(1<<((f->last_index_occup)/2)),(1<<(CEILING((f->last_index_occup),2)))); 
            mu_assert(f->last_data_size ==(unsigned) 1<< ((f->num_super_blocks)/2),"Fail");
            //printf("\n");
        }
    mu_assert(prior_num_elems == f->num_user_elements_inserted,"FAIL");
    mu_assert( prior_usable == f->usable_data_blocks,"Fail");
    mu_assert( prior_num_super == f->num_super_blocks,"Fail");
    mu_assert(prior_index_length == f->index_length,"Fail");
    mu_assert(prior_index_occup == f->last_index_occup,"fail");
    mu_assert(prior_data_size == f->last_data_size,"fail");
    mu_assert(prior_super_size == f->last_super_size,"fail");

    //log_info("PASS %d", grow_step);
        grow_step++;
    }
    flex_destroy(f);
        return NULL;
}

char * test_grow_size()
{
    index_t x;
    flex_t f = flex_init();
    for(x=0;x<300;x++){
        flex_grow(f);
        //flex_debug_out(f);
        //log_info("k= %ld, supersize=%ld, datasize=%ld\n",f->num_super_blocks,(1<<((f->num_super_blocks)/2)),(1<<(CEILING((f->num_super_blocks),2)))); 
        mu_assert(f->last_data_size ==(unsigned) (1<< ((f->num_super_blocks)/2)),"Fail");
    }
    //flex_shrink(f);
    //flex_debug_out(f);
    flex_destroy(f);
    return 0;
}

char * test_insert()
{

    DSTATUS fake_printf(data_p x){
        printf("%d ",*x);
        return SUCCESS;
    }
    int x;
    DSTATUS status;
    data_p stuff = malloc(sizeof(data_p));
    *stuff = 4;
    flex_t f = flex_init();
    for(x=0;x<40;x++){
        status = flex_insert(f,x,stuff);
        mu_assert(status == SUCCESS, "FAIL");
    }
    //flex_traverse(f,&fake_printf);
    //flex_debug_out(f);
    flex_destroy(f);
    free(stuff);
    return 0;
}

char * test_compare()
{
    DSTATUS fake_printf(data_p x){
        printf("%d ",*x);
        return SUCCESS;
    }
    DSTATUS status;
    index_t x;
    data_p stuff = malloc(sizeof(data_p));
    flex_t f = flex_init();
    for(x=0;x<10;x++){
        *stuff = x;
        flex_insert(f,x,stuff);
    }
    for(x=0;x<10;x++){
        *stuff = x;
        status = flex_compare(f,x,stuff);
        mu_assert(status == EQL, "FAIL");
    }

    //flex_traverse(f, fake_printf);
    flex_destroy(f);
    free(stuff);
    return 0;
}

char *all_tests()
{
    mu_suite_start();
    //mu_run_test(sanity_check);
    mu_run_test(test_init);
    mu_run_test(test_grow_size);
    mu_run_test(test_destroy_after_growth);
    mu_run_test(test_traverse);
    mu_run_test(test_insert);
    mu_run_test(test_shrink);
    mu_run_test(test_compare);
	return NULL;
}

RUN_TESTS(all_tests);
