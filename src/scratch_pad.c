#include <stdlib.h>

int main(int argc, char *argv[]){

    int lgN, t, N;
    /* Number of bits required to represent N in binary */
    for (lgN = 0; N > 0; lgN++, N /=2){ 
    }

    for(lgN = 0, t = 1; t < N; lgN++, t += t){
        // emphasize the 2^n <= N < 2^(n+1)
    }


    return 0;
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
       

int **init_2d(int rows, int cols){
    int i;
    int **matrix = malloc(rows * sizeof(int*));
    for(i=0; i< rows; i++)
        matrix[i] = malloc(cols * sizeof(int));
    return matrix;
}



