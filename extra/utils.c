#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/time.h>

/* Returns the number of microseconds since Unix epoch. */
long long microseconds(void) {
    struct timeval tv;
    long long mst;
    gettimeofday(&tv, NULL);
    mst = ((long long)tv.tv_sec)*1000000;
    mst += tv.tv_usec;
    return mst;
}

/* Convert number of bytes into a human readable string of the form:
 * 100B, 2G, 100M, 4K, and so forth. */
void bytesToHuman(char *s, long long n) {
    double d;

    if (n < 0) {
        *s = '-';
        s++;
        n = -n;
    }
    if (n < 1024) {
        /* Bytes */
        sprintf(s,"%lluB",n);
        return;
    } else if (n < (1024*1024)) {
        d = (double)n/(1024);
        sprintf(s,"%.2fK",d);
    } else if (n < (1024LL*1024*1024)) {
        d = (double)n/(1024*1024);
        sprintf(s,"%.2fM",d);
    } else if (n < (1024LL*1024*1024*1024)) {
        d = (double)n/(1024LL*1024*1024);
        sprintf(s,"%.2fG",d);
    }
}
inline int default_msb(unsigned long int i){
    /* Just to double check myself when using gcc's  __builtin_ctzl */
    int r = 0;
    while (i >>= 1)
    {
      r++;
    }
    return r;
}

/* Sequntial search 
 *
 * Restriction: Sorted array. */
int seq_search(int a[], int reqV, int start, int end){
    int i;

    for(i=start; i<=end; i++)
        if (reqV == a[i]) return i;

    return -1;
}


/* Binary search
 *
 * Restriction: Sorted array. */
int bin_search(int a[], int reqV, int start, int end){
    int middle;

    while(end >= start){
        middle = (start+end)/2;

        if(reqV == a[middle]) return middle;

        if(reqV <a[middle]) end = middle-1; else start = middle +1;
    }

    return -1;
}

/* Three parts to a recommended enineering practic.
 *
 *      Interface, Implementation, client driver.
 */

inline unsigned extract_continuous_bits(unsigned long value, int start, int end) {
/* Assumes zero indexed */
    unsigned mask = (~0u) >> (CHAR_BIT*sizeof(value) - end - 1);
    return (value & mask) >> start;
}      

int **init_2d(int rows, int cols){
    int i;
    int **matrix = malloc(rows * sizeof(int*));
    for(i=0; i< rows; i++)
        matrix[i] = malloc(cols * sizeof(int));
    return matrix;
}

const char* byte_to_binary( unsigned long int x ){
    static char b[sizeof(x)*8+1] = {0};
    int y;
    long long z;
    for (z=1LL<<sizeof(x)*8-1,y=0; z>0; z>>=1,y++)
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
