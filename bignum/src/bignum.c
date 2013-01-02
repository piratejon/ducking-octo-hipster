#include <stdlib.h>

#include "bignum.h"

/*@null@*/ BigInt * init_bigint_empty ( void )
{
  BigInt * b = malloc(sizeof*b);

  if ( b )
  {
    b->count = 0;
    b->msb = b->lsb = NULL;
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
  if ( bi )
  {
    do
    {
      append_bit ( bi, i & 1 );
      i >>= 1;
    } while ( i > 0 );
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

int bigint_compare ( BigInt * A, BigInt * B )
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
        return 1;
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
        return 1;
      }
      else if ( b->bit > a->bit )
      {
        return -1;
      }

      a = a->prev;
      b = b->prev;
    }
  }

  return 0;
}

void bigint_add ( BigInt * augend, BigInt * addend )
{
  int carry = 0;
  Bit * a, * b;

  if ( augend == addend )
  {
    prepend_bit ( augend, 0 );
    return;
  }

  a = augend->lsb;
  b = addend->lsb;

  while ( a && b )
  {
    switch ( carry + a->bit + b->bit )
    {
      case 3:
        a->bit = 1;
        carry = 1;
        break;

      case 2:
        a->bit = 0;
        carry = 1;
        break;

      case 1:
        a->bit = 1;
        carry = 0;
        break;

      case 0:
        a->bit = 0;
        carry = 0;
        break;
    }

    a = a->next;
    b = b->next;
  }

  if ( a )
  {
    while ( a )
    {
      switch ( carry + a->bit )
      {
        case 2:
          a->bit = 0;
          carry = 1;
          break;

        case 1:
          a->bit = 1;
          carry = 0;
          break;

        case 0:
          a->bit = 0;
          carry = 0;
          break;
      }

      a = a->next;
    }

    if ( carry )
    {
      append_bit ( augend, 1 );
    }
  }
  else if ( b )
  {
    while ( b )
    {
      switch ( carry + b->bit )
      {
        case 2:
          append_bit ( augend, 0 );
          carry = 1;
          break;

        case 1:
          append_bit ( augend, 1 );
          carry = 0;
          break;

        case 0:
          append_bit ( augend, 0 );
          carry = 0;
          break;
      }

      b = b->next;
    }

    if ( carry )
    {
      append_bit ( augend, 1 );
    }
  }
  else
  {
    return;
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

void bigint_swap ( BigInt * a, BigInt * b )
{
  BigInt tmp;
  tmp.count = a->count;
  tmp.lsb = a->lsb;
  tmp.msb = a->msb;

  a->count = b->count;
  a->lsb = b->lsb;
  a->msb = b->msb;

  b->count = tmp.count;
  b->lsb = tmp.lsb;
  b->msb = tmp.msb;
}

void bigint_multiply ( BigInt * a, BigInt * b )
{
  // shift-add method
  BigInt * r, *tmp;
  Bit * current;
  int i;

  r = init_bigint_empty ( );

  for ( current = b->lsb, i = 0; current; ++i, current = current->next )
  {
    if ( current->bit )
    {
      tmp = bigint_copy ( a );
      bigint_shift_left ( tmp, i );
      bigint_add ( r, tmp );
      free_bigint ( tmp );
    }
  }

  bigint_swap ( a, r );

  free_bigint ( r );
}

BigInt * bigint_div_10 ( BigInt * a )
{
  BigInt * r = NULL;

  return r;
}

