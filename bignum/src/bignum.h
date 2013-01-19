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
  bool positive;
  Bit * lsb, * msb;
} BigInt;

BigInt * init_bigint ( int );
void free_bigint ( BigInt * );
void free_bigint_innards ( BigInt * );
int bigint_low_dword ( BigInt * );
int bigint_compare ( BigInt *, BigInt * );
int bigint_compare_magnitude ( BigInt *, BigInt * );
void bigint_add_in_place ( BigInt *, BigInt * );
BigInt * bigint_add ( BigInt *, BigInt * );
BigInt * bigint_multiply ( BigInt *, BigInt * );
BigInt * bigint_copy ( BigInt * );
void bigint_shift_right ( BigInt *, int );
void bigint_shift_left ( BigInt *, int );
bool bigint_pop_lsb ( BigInt * );
bool bigint_pop_msb ( BigInt * );
void append_bit ( BigInt *, bool );
void prepend_bit ( BigInt *, bool );
bool bigint_positive ( BigInt * );
void bigint_subtract_in_place ( BigInt *, BigInt * );
void single_bit_subtract_in_place ( bool *, bool, bool * );
void single_bit_add_in_place ( bool *, bool, bool * );
void _real_bigint_add_in_place ( BigInt *, BigInt * );
void _real_bigint_subtract_in_place ( BigInt *, BigInt * );
void bigint_swap ( BigInt *, BigInt * );
BigInt * init_bigint_from_string ( char * );
BigInt * bigint_divide ( BigInt *, BigInt *, BigInt ** );
int bitlist_compare_magnitude_forward ( Bit * , Bit * , int );
BigInt * bigint_binary_slice ( BigInt *, int, int );
Bit * walk_toward_msb ( Bit * , int );
Bit * walk_toward_lsb ( Bit * , int );
void bigint_reverse_bits ( BigInt * );
char * bigint_tostring ( BigInt * );
BigInt * init_bigint_empty ( );

#endif // _BIGNUM_H

