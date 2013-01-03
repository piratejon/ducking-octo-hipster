#ifndef _BIGNUM_H
#define _BIGNUM_H

#include <stdbool.h>

#define MAX2(x,y) (((x)>=(y))?x:y)

typedef struct _tag_bit
{
  bool bit;
  struct _tag_bit * next;
  struct _tag_bit * prev;
} Bit;

typedef struct _tag_bigint
{
  int count;
   // bool positive;
  Bit * lsb, * msb;
} BigInt;

BigInt * init_bigint ( int );
void free_bigint ( BigInt * );
void free_bigint_innards ( BigInt * );
int bigint_low_dword ( BigInt * );
int bigint_compare ( BigInt *, BigInt * );
void bigint_add ( BigInt *, BigInt * );
void bigint_multiply ( BigInt *, BigInt * );
BigInt * bigint_copy ( BigInt * );
void bigint_shift_right ( BigInt *, int );
void bigint_shift_left ( BigInt *, int );
bool bigint_pop_lsb ( BigInt * );
bool bigint_pop_msb ( BigInt * );
void append_bit ( BigInt *, int );
void prepend_bit ( BigInt *, int );
BigInt * bigint_div_10 ( BigInt * );

#endif // _BIGNUM_H

