/* File: debug.h
 * Version: 1.1
 * Purpose: Definitions for debugging and error checking 
 * Author: Matthew Clemens
 * Copyright: MIT
*/

#ifndef _debug_h
#define _debug_h

#include <stdio.h>
#include <errno.h>
#include <string.h>
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define BOLDBLACK   "\033[1m\033[30m"
#define BOLDRED     "\033[1m\033[31m"
#define BOLDGREEN   "\033[1m\033[32m"
#define BOLDYELLOW  "\033[1m\033[33m"
#define BOLDBLUE    "\033[1m\033[34m"
#define BOLDMAGENTA "\033[1m\033[35m"
#define BOLDCYAN    "\033[1m\033[36m"
#define BOLDWHITE   "\033[1m\033[37m"

#define MSGDEBUG  "[" BOLDCYAN "DEBUG" RESET "]"
#define MSGWARN  "[" BOLDYELLOW "WARN" RESET "]"
#define MSGSUCCESS  "[" BOLDGREEN "OK" RESET "]"
#define MSGINFO  "[" BOLDMAGENTA "INFO" RESET "]"
#define MSGERR  "[" BOLDRED "ERROR" RESET "]"

#ifndef NDEBUG
    #define DEBUG 1
#else
    #define DEBUG 0
#endif

#define log_success(M, ...) \
    do {if(DEBUG) fprintf(stderr,"%-20s[%s:%d] "M" \n",MSGSUCCESS, __FILE__, __LINE__,##__VA_ARGS__); } while(0)

#define log_warn(M, ...) \
    do {if(DEBUG) fprintf(stderr,"%-20s[%s:%d] [E: %s] "M" \n",MSGWARN ,__FILE__,__LINE__, (errno == 0 ? "n/a": strerror(errno)),##__VA_ARGS__); } while(0)

#define log_info(M, ...) \
    do {if(DEBUG) fprintf(stderr,"%-20s[%s:%d] "M" \n",MSGINFO, __FILE__, __LINE__,##__VA_ARGS__); } while(0)

#define log_err(M, ...) \
    do {if(DEBUG) fprintf(stderr,"%-20s[%s:%d] "M" \n",MSGERR, __FILE__, __LINE__,##__VA_ARGS__); } while(0)

#define debug(M, ...) \
    do {if(DEBUG) fprintf(stderr,"%-20s[%s:%d] "M" \n",MSGDEBUG, __FILE__, __LINE__,##__VA_ARGS__); } while(0)


#define check(A, M, ...) if(!(A)) { log_err(M, ##__VA_ARGS__); errno=0; goto error; }
#define check_alt(A) if(!(A)) {goto error;}
#define check_hard(A, M, ...) if(!(A)) { log_err(M, ##__VA_ARGS__); errno=0; exit(1); }
#define check_debug(A, M, ...) if(!(A)) { debug(M, ##__VA_ARGS__); errno=0; goto error; }

#endif
