#include <stdlib.h>
#include <stdio.h>

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
    bi->count --;
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
  bi->count = 0;
  bi->positive = true;
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

  if ( A->count > B->count )
  {
    i = A->count;
    while ( i -- > B->count )
    {
      if ( a->bit ) return 1;
      a = a->prev;
    }
  }
  else if ( B->count > A->count )
  {
    i = B->count;
    while ( i -- > A->count )
    {
      if ( b->bit ) return -1;
      b = b->prev;
    }
  }

  while ( a && b )
  {
    if ( a->bit != b->bit ) return a->bit ? 1 : -1;
    a = a->prev;
    b = b->prev;
  }

  return 0;
}

int bigint_compare ( BigInt * A, BigInt * B )
{
  if ( A->positive )
  {
    if ( B->positive )
    {
      return bigint_compare_magnitude ( A, B );
    }
    else
    {
      return 1;
    }
  }
  else
  {
    if ( B->positive )
    {
      return -1;
    }
    else
    {
      return bigint_compare_magnitude ( B, A );
    }
  }
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
    bigint_shift_left ( A, 1 );
  }
  else if ( A->positive )
  {
    if ( B->positive )
    {
      _real_bigint_add_in_place ( A, B );
    }
    else
    {
      // A + (-B)
      if ( bigint_compare_magnitude ( A, B ) > 0 )
      {
        _real_bigint_subtract_in_place ( A, B );
      }
      else
      {
        BigInt * b_tmp = bigint_copy ( B );
        _real_bigint_subtract_in_place ( b_tmp, A );
        bigint_swap ( b_tmp, A );
        A->positive = false;
        free_bigint ( b_tmp );
      }
    }
  }
  else
  {
    if ( B->positive )
    {
      // (-A) + B
      // negative if |A| > |B|
      // positive if |A| < |B|
      if ( bigint_compare_magnitude ( A, B ) > 0 )
      {
        _real_bigint_subtract_in_place ( A, B );
      }
      else
      {
        BigInt * b_tmp = bigint_copy ( B );
        _real_bigint_subtract_in_place ( b_tmp, A );
        bigint_swap ( b_tmp, A );
        free_bigint ( b_tmp );
      }
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
  for ( ; count > 0; count -- )
  {
    bigint_pop_lsb ( a );
  }
}

void bigint_shift_left ( BigInt * a, int count )
{
  for ( ; count > 0; count -- )
  {
    prepend_bit ( a, false );
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

  product = init_bigint_empty ( );

  // if a is equal to zero
  if ( a->count == 0 ) return product;

  tmp = bigint_copy ( a );
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

  return product;
}

BigInt * bigint_div_10 ( BigInt * a )
{
  BigInt * r = init_bigint ( 3 );
  BigInt * b = init_bigint ( 98765 );
  bigint_swap ( a, b );
  free_bigint ( b );
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

void bigint_subtract_in_place ( BigInt * A, BigInt * B )
{
  if ( A == B || bigint_compare ( A, B ) == 0 )
  {
    free_bigint_innards ( A );
  }
  else
  {
    if ( A->positive )
    {
      if ( B->positive )
      {
        if ( bigint_compare_magnitude ( A, B ) > 0 )
        {
          _real_bigint_subtract_in_place ( A, B );
        }
        else
        {
          BigInt * b = bigint_copy ( B );
          _real_bigint_subtract_in_place ( b, A );
          bigint_swap ( b, A );
          A->positive = false;
          free_bigint ( b );
        }
      }
      else
      {
        _real_bigint_add_in_place ( A, B );
      }
    }
    else
    {
      if ( B->positive )
      {
        _real_bigint_add_in_place ( A, B );
      }
      else
      {
        // both are negative
        if ( bigint_compare_magnitude ( A, B ) > 0 )
        { // i.e. -100 - -72 = -28
          _real_bigint_subtract_in_place ( A, B );
        }
        else
        { // i.e. -100 - -1000 = 900
          BigInt * b = bigint_copy ( B );
          _real_bigint_subtract_in_place ( b, A );
          bigint_swap ( b, A );
          A->positive = true;
          free_bigint ( b );
        }
      }
    }
  }
}

void _real_bigint_subtract_in_place ( BigInt * A, BigInt * B )
{
  Bit * a, *b;
  bool borrow;

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

char * find_string_end ( char * str )
{
  while ( *str ) str ++;
  return str-1;
}

BigInt * init_bigint_from_string ( char * str )
{
  char * end;
  BigInt * a = init_bigint_empty ( );
  BigInt * place = init_bigint ( 1 );
  BigInt * ten = init_bigint ( 10 );

  for ( end = find_string_end ( str ); end >= str; -- end )
  {
    if ( *end != '-' )
    {
      BigInt * current_digit = init_bigint ( (*end) - '0' );
      BigInt * new_place_value = bigint_multiply ( current_digit, place );
      free_bigint ( current_digit );
      bigint_add_in_place ( a, new_place_value );
      free_bigint ( new_place_value );

      BigInt * new_place = bigint_multiply ( place, ten );
      bigint_swap ( new_place, place );
      free_bigint ( new_place );
    }
    else
    {
      a->positive = false;
      break;
    }
  }

  free_bigint ( place );
  free_bigint ( ten );

  return a;
}

