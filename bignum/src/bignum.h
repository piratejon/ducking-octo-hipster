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

/**
  * These functions form the public interface of this library.
  **/
BigInt * bigint_init ( int );
void bigint_free ( BigInt * const );
void bigint_free_innards ( BigInt * const );
int bigint_low_dword ( BigInt const * const );
int bigint_compare ( BigInt const * const, BigInt const * const );
int bigint_compare_magnitude ( BigInt const * const, BigInt const * const );
void bigint_add_in_place ( BigInt * const, BigInt const * const );
BigInt * bigint_add ( BigInt const * const, BigInt const * const );
BigInt * bigint_multiply ( BigInt const * const, BigInt const * const );
BigInt * bigint_copy ( BigInt const * const );
void bigint_shift_right ( BigInt * const, int );
void bigint_shift_left ( BigInt * const, int );
bool bigint_pop_lsb ( BigInt * const );
bool bigint_pop_msb ( BigInt * const );
bool bigint_positive ( BigInt const * const );
void bigint_subtract_in_place ( BigInt * const, BigInt const * const );
void bigint_swap ( BigInt * const, BigInt * const );
BigInt * bigint_init_from_string ( char const * const );
BigInt * bigint_divide ( BigInt const * const, BigInt const * const, BigInt ** );
int bitlist_compare_magnitude_forward ( Bit const * const, Bit const * const, int );
BigInt * bigint_binary_slice ( BigInt const * const, int const, int const );
char * bigint_tostring_base2 ( BigInt const * const );
char * bigint_tostring_base10 ( BigInt const * const );
/*@out@*/ BigInt * bigint_init_empty ( void );
int bigint_slice_bits ( BigInt const * const, int const, int const, int * const );
BigInt * bigint_modulo ( BigInt const * const, BigInt const * const );

/**
  * These are considered private. Please don't use them!
  **/
void _bigint_reverse_bits ( BigInt * const );
void append_bit ( BigInt * const, bool );
void prepend_bit ( BigInt * const, bool );
void single_bit_subtract_in_place ( bool * const, bool const, bool * const );
void single_bit_add_in_place ( bool * const, bool const, bool * const );
void _real_bigint_add_in_place ( BigInt * const, BigInt const * const );
void _real_bigint_subtract_in_place ( BigInt * const, BigInt const * const );
Bit const * walk_toward_msb ( Bit const *, int );
Bit const * walk_toward_lsb ( Bit const *, int );
int _bigint_remove_high_zeroes ( BigInt * const );

#endif // _BIGNUM_H

