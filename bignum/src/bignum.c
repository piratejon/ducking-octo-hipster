#include <stdlib.h>

#include "bignum.h"

/*@null@*/ BigInt * init_bigint_empty ( void )
{
  BigInt * b = malloc(sizeof*b);

  if ( b )
  {
    b->count = 0;
    b->msb = b->lsb = NULL;
    b->positive = true;
  }

  return b;
}

bool bigint_pop_msb ( BigInt * bi )
{
  bool out = false;

  if ( bi->msb )
  {
    Bit * new_msb = bi->msb->prev;

    if ( new_msb )
    {
      new_msb->next = NULL;
    }

    out = bi->msb->bit;

    free ( bi->msb );
    bi->msb = new_msb;
  }

  return out;
}

bool bigint_pop_lsb ( BigInt * bi )
{
  bool out = false;

  if ( bi->lsb )
  {
    Bit * new_lsb = bi->lsb->next;

    if ( new_lsb )
    {
      new_lsb->prev = NULL;
    }

    out = bi->lsb->bit;

    free ( bi->lsb );
    bi->lsb = new_lsb;
  }

  return out;
}

void append_bit ( BigInt * bi, int b )
{
  Bit * bit = malloc(sizeof*bit);

  bit->bit = b == 1;
  bi->count ++;

  if ( bi->lsb )
  {
    bi->msb->next = bit;
    bit->prev = bi->msb;
    bit->next = NULL;

    bi->msb = bit;
  }
  else
  {
    bi->lsb = bi->msb = bit;
    bit->next = bit->prev = NULL;
  }
}

BigInt * bigint_copy ( BigInt * a )
{
  BigInt * b = init_bigint_empty ( );

  b->positive = a->positive;
  Bit * current = a->lsb;

  while ( current )
  {
    append_bit ( b, current->bit );
    current = current->next;
  }

  return b;
}

void prepend_bit ( BigInt * bi, int b )
{
  Bit * bit = malloc(sizeof*bit);

  bit->bit = b == 1;
  bi->count ++;

  bit->prev = NULL;
  bit->next = bi->lsb;

  if ( bi->lsb )
  {
    bi->lsb->prev = bit;
  }

  bi->lsb = bit;
}

BigInt * init_bigint ( int i )
{
  BigInt * bi = init_bigint_empty ( );

  if ( i < 0 )
  {
    i = 0-i;
    bi->positive = false;
  }

  while ( i > 0 )
  {
    append_bit ( bi, i & 1 );
    i >>= 1;
  }

  return bi;
}

void free_bigint_innards ( BigInt * bi )
{
  Bit * next;
  while ( bi->lsb )
  {
    next = bi->lsb->next;
    free ( bi->lsb );
    bi->lsb = next;
  }
  bi->msb = NULL;
}

void free_bigint ( BigInt * bi )
{
  if ( bi ) free_bigint_innards ( bi );
  free ( bi );
}

int bigint_slice_bits ( BigInt * bi, int start, int end, int * out )
{
  int i;

  Bit * current = bi->lsb;
  for (
      i = start;
      i < end && current && current->next;
      ++ i, current = current->next
      );

  for ( (*out) = 0; current; current = current->prev )
  {
    (*out) <<= 1;
    (*out) |= current->bit;
  }

  return i;
}

int bigint_low_dword ( BigInt * bi )
{
  int lodword;
  bigint_slice_bits ( bi, 0, sizeof(lodword)*8, &lodword );
  return lodword;
}

int bigint_compare_magnitude ( BigInt * A, BigInt * B )
{
  int i;
  Bit * a, * b;

  a = A->msb;
  b = B->msb;

  if ( B->count > A->count )
  {
    return -bigint_compare ( B, A );
  }
  else if ( A->count > B->count )
  {
    i = A->count;
    while ( i -- > B->count )
    {
      if ( a->bit )
      {
        if ( A->positive )
        {
          return 1;
        }
        else
        {
          return -1;
        }
      }
      else
      {
        a = a->prev;
      }
    }
  }
  else
  {
    i = A->count;
    while ( i -- > 0 )
    {
      // printf("comparing %d with %d (index %d)\n", a->bit, b->bit, i );
      if ( a->bit > b->bit )
      {
        if ( A->positive )
        {
          return 1;
        }
        else
        {
          return -1;
        }
      }
      else if ( b->bit > a->bit )
      {
        if ( A->positive )
        {
          return -1;
        }
        else
        {
          return 1;
        }
      }

      a = a->prev;
      b = b->prev;
    }
  }

  return 0;
}

int bigint_compare ( BigInt * A, BigInt * B )
{
  if ( A->positive && ! B->positive ) return 1;
  if ( ! A->positive && B->positive ) return -1;

  return bigint_compare_magnitude ( A, B );
}

void single_bit_add_in_place ( bool * a, bool B, bool * carry )
{
  bool A = *a;
  bool C = *carry;

  *a = A ^ B ^ C;
  *carry = (A&C)|(B&C)|(A&B&~C);
}

void bigint_add_in_place ( BigInt * A, BigInt * B )
{
  if ( A == B || bigint_compare ( A, B ) == 0 )
  {
    prepend_bit ( A, false );
    return;
  }

  if ( A->positive )
  {
    if ( B->positive )
    {
      _real_bigint_add_in_place ( A, B );
    }
    else
    {
      // A + (-B)
      bool result_positive;
      BigInt * bneg = bigint_copy ( B );
      bneg->positive = true;
      result_positive = bigint_compare ( A, bneg ) > 0;
      _real_bigint_subtract_in_place ( bneg, A );
      bigint_swap ( bneg, A );
      free_bigint ( bneg );
      A->positive = result_positive;
    }
  }
  else
  {
    if ( B->positive )
    {
      // (-A) + B
      // positive if |A| < |B|
      // negative if |A| > |B|
      bool result_positive;
      BigInt * a, * b;
      a = bigint_copy ( A );
      b = bigint_copy ( B );

      if ( bigint_compare_magnitude ( a, b ) > 0 )
      {
        a->positive = true;
        _real_bigint_subtract_in_place ( b, a );
        bigint_swap ( A, b );
      }
      else
      {
        a->positive = true;
        _real_bigint_subtract_in_place ( a, b );
        a->positive = false;
        bigint_swap ( A, a );
      }

      free_bigint ( a );
      free_bigint ( b );
    }
    else
    {
      _real_bigint_add_in_place ( A, B );
    }
  }
}

void _real_bigint_add_in_place ( BigInt * augend, BigInt * addend )
{
  bool carry = false;
  Bit * a, * b;
  a = augend->lsb;
  b = addend->lsb;

  while ( a && b )
  {
    single_bit_add_in_place ( &(a->bit), b->bit, &carry );

    a = a->next;
    b = b->next;
  }

  while ( a )
  {
    single_bit_add_in_place ( &(a->bit), false, &carry );
    a = a->next;
  }

  while ( b )
  {
    bool A = false;
    single_bit_add_in_place ( &A, b->bit, &carry );
    append_bit ( augend, A );
    b = b->next;
  }

  if ( carry )
  {
    append_bit ( augend, true );
  }
}

void bigint_shift_right ( BigInt * a, int count )
{
  for ( ; count > 0 && a->count > 0; count --, a->count -- )
  {
    bigint_pop_lsb ( a );
  }
}

void bigint_shift_left ( BigInt * a, int count )
{
  for ( ; count > 0 && a->count > 0; count --, a->count -- )
  {
    prepend_bit ( a, 0 );
  }
}

void bigint_shallow_copy ( BigInt * a, BigInt * b )
{
  a->count = b->count;
  a->lsb = b->lsb;
  a->msb = b->msb;
  a->positive = b->positive;
}

void bigint_swap ( BigInt * a, BigInt * b )
{
  BigInt tmp;
  bigint_shallow_copy ( &tmp, a );
  bigint_shallow_copy ( a, b );
  bigint_shallow_copy ( b, &tmp );
}

BigInt * bigint_multiply ( BigInt * a, BigInt * b )
{
  // shift-add method
  BigInt * product, *tmp;
  Bit * current;

  tmp = bigint_copy ( a );
  product = init_bigint_empty ( );

  for ( current = b->lsb; current; current = current->next )
  {
    if ( current->bit )
    {
      bigint_add_in_place ( product, tmp );
    }
    bigint_shift_left ( tmp, 1 );
  }

  free_bigint ( tmp );

  product->positive = ( a->positive == b->positive );

  return ( product );
}

BigInt * bigint_div_10 ( BigInt * a )
{
  BigInt * r = NULL;

  return r;
}

bool bigint_positive ( BigInt * a )
{
  return a->positive;
}

BigInt * bigint_add ( BigInt * a, BigInt * b )
{
  BigInt * sum = bigint_copy ( a );
  bigint_add_in_place ( sum, b );
  return sum;
}

void single_bit_subtract_in_place ( bool * a, bool B, bool * borrow )
  /*
     this does a -= b, respecting the borrow flag
   */
{
  bool A, Borrow;

  A = *a;
  Borrow = *borrow;

  *a = A ^ B ^ Borrow;
  *borrow = (Borrow & ~A) | (Borrow & B) | (~Borrow & ~A & B);
}

void _real_bigint_subtract_in_place ( BigInt * A, BigInt * B )
{
  Bit * a, *b;
  bool borrow;

  /*
  if ( A->positive && B->positive )
  {
    if ( bigint_compare ( A, B ) < 0 )
    {
      bigint_subtract_in_place ( B, A );
      bigint_swap ( A, B );
      return;
    }
    else
    {
      // subtraction proceeds as usual
    }
  }
  else if ( A->positive && !B->positive )
  {
    BigInt * b_pos = bigint_copy ( B );
    b_pos->positive = true;
    bigint_add_in_place ( A, b_pos );
    free_bigint ( b_pos );
    return;
  }
  else if ( !A->positive && B->positive )
  {
    // A stays negative; subtraction proceeds as usual
  }
  else if ( !A->positive && !B->positive )
  {
    if ( bigint_compare ( A, B ) < 0 )
    {
      bigint_add_in_place ( A, B );
      return;
    }
    else
    {
      // proceed as normal
    }
  }
  */

  a = A->lsb;
  b = B->lsb;
  borrow = false;

  while ( a && b )
  {
    single_bit_subtract_in_place ( &(a->bit), b->bit, &borrow );

    a = a->next;
    b = b->next;
  }

  while ( a )
  {
    single_bit_subtract_in_place ( &(a->bit), false, &borrow );
    a = a->next;
  }
}

