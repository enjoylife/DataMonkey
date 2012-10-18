#define debruijn32 0x077CB531UL
/* debruijn32 = 0000 0111 0111 1100 1011 0101 0011 0001 */
/* table to convert debruijn index to standard index */
int index32[32];
/* routine to initialize index32 */
void setup( void )
{
    int i;
    for (i = 0; i < 32; i++) {
        index32[ (debruijn32 << i) >> 27 ] = i;
    }
}
/* compute index of rightmost 1 */
int rightmost_index( unsigned long b )
{
    b &= -b;
    b *= debruijn32;
    b >>= 27;
    return index32[b];
}
