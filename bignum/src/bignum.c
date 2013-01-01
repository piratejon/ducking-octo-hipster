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

void bigint_multiply ( BigInt * a, BigInt * b )
{
  if ( a == b )
  {
    BigInt * c = bigint_copy ( a );
    bigint_multiply ( a, c );
    free_bigint ( c );
    return;
  }
}

