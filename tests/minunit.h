#ifndef _minunit_h
#define _minunit_h
#include <stdio.h>
#include <stdlib.h>

#define RESET   "\033[0m"
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */

#define mu_suite_start() char *message = NULL

#define mu_assert(test, message) if (!(test)) { log_err(message); return message; }
#define mu_run_test(test) printf("\n-----%s", " " #test); message = test(); tests_run++; if (message) return message;

#define RUN_TESTS(name) int main(int argc, char *argv[]) {\
    argc = 1; \
    printf("------\n" "[" BOLDCYAN "RUNNING:" RESET "]"" %s \n", argv[0]);\
        char *result = name();\
        if (result != 0) {\
            printf("\n\n[" BOLDRED "FAILED: %s" RESET "]" "\n", result);\
        }\
        else {\
            printf("\n\n[" BOLDGREEN "ALL TESTS PASSED" RESET "]" "\n");\
        }\
    printf("Tests run: %d\n", tests_run);\
        exit(result != 0);\
}

int tests_run;

#endif
