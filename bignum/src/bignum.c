#include <stdlib.h>
#include <stdio.h>

#include "bignum.h"

///
/// Initializes a BigInt with no bits (equivalent to zero).
///
/// @return A pointer to a newly-initialized empty BigInt. Must be freed with
/// free_bigint().
///
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

///
/// Removes and returns a BigInt's MSB.
///
/// @param bi The BigInt being MSB-popped
///
/// @return The value of the BigInt's MSB prior to removal
///
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

///
/// Removes and returns a BigInt's LSB, shifting right in the process
///
/// @param bi The BigInt being LSB-popped
///
/// @return The value of the BigInt's LSB prior to shifting/popping.
///
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

///
/// Grows a BigInt by one bit by adding a new MSB
///
/// @param bi The BigInt being extended
/// @param b The bit extending bi
///
void append_bit ( BigInt * bi, bool b )
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

///
/// Creates a new BigInt with the same value as an existing BigInt.
///
/// @param a The BigInt to copy.
///
/// @return A pointer to a new BigInt with the same value as the parameter.
/// This is a separate copy and must be freed with free_bigint().
/// 
BigInt * bigint_copy ( BigInt * a )
{
  BigInt * b = init_bigint_empty ( );

  b->positive = a->positive;
  Bit * current = a->lsb;

  while ( current )
  {
    append_bit ( b, current->bit );
    current = walk_toward_msb ( current, 1 );
  }

  return b;
}

///
/// Shifts a new LSB into a BigInt
///
/// @param bi The BigInt being shifted and receiving a new LSB
/// @param b The value of the BigInt's new LSB
///
void prepend_bit ( BigInt * bi, bool b )
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

///
/// Creates a new BigInt object with specified integer value.
///
/// @param i The value of the new BigInt object.
///
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

///
/// Frees a BigInt's internal linked list and resets values to zero/NULL as if
/// it had just been returned by init_bigint_empty()
///
/// @param bi The BigInt being reset.
///
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

///
/// Releases all memory associated with a BigInt
///
/// @param bi The BigInt being freed.
///
void free_bigint ( BigInt * bi )
{
  if ( bi ) free_bigint_innards ( bi );
  free ( bi );
}

///
/// Copies a range of bits from a BigInt into a C integer.
///
/// @param bi The BigInt to slice from
/// @param start The index of the first bit to copy.
/// @param end The index of the last bit to copy.
///
/// @return The number of bits actually copied, in case [start:end] was outside
/// bi's actual range.
///
int bigint_slice_bits ( BigInt * bi, int start, int end, int * out )
{
  int i;

  Bit * current = bi->lsb;
  for (
      i = start;
      i < end && current && current->next;
      ++ i, current = walk_toward_msb ( current, 1 )
      );

  for ( (*out) = 0; current; current = walk_toward_lsb ( current, 1 ) )
  {
    (*out) <<= 1;
    (*out) |= current->bit;
  }

  return i;
}

///
/// Returns a BigInt's low sizeof(int) bits.
///
/// @param bi The BigInt to retrieve the low sizeof(int) bits from.
///
/// @return The low sizeof(int) bits from bi
///
int bigint_low_dword ( BigInt * bi )
{
  int lodword;
  bigint_slice_bits ( bi, 0, sizeof(lodword)*8, &lodword );
  return lodword;
}

///
/// Compares the magnitude of two BigInts. See bigint_compare for signed
/// comparison.
///
/// @param A The first BigInt to compare.
/// @param B The second BigInt to compare.
///
/// @return 0 if |A| == |B|, -1 if |A| < |B|, 1 if |A| > |B|
///
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
      a = walk_toward_lsb ( a, 1 );
    }
  }
  else if ( B->count > A->count )
  {
    i = B->count;
    while ( i -- > A->count )
    {
      if ( b->bit ) return -1;
      b = walk_toward_lsb ( b, 1 );
    }
  }

  while ( a && b )
  {
    if ( a->bit != b->bit ) return a->bit ? 1 : -1;
    a = walk_toward_lsb ( a, 1 );
    b = walk_toward_lsb ( b, 1 );
  }

  return 0;
}

///
/// Compares two BigInts. See bigint_compare_magnitude for unsigned comparison.
///
/// @param A The first BigInt to compare.
/// @param B The second BigInt to compare.
///
/// @return 0 if A == B, -1 if A < B, 1 if A > B
///
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

///
/// Performs an in-place single-bit addition, updating the augend and carry
/// flag.
///
/// @param a The address of the augend
/// @param B The value of the addend
/// @param carry The address of the carry bit. If NULL, carry is assumed to be
/// unset and not updated.
///
void single_bit_add_in_place ( bool * a, bool B, bool * carry )
{
  bool A = *a;
  bool C = carry ? *carry : false;

  *a = A ^ B ^ C;
  if ( carry ) *carry = (A&C)|(B&C)|(A&B&~C);
}

///
/// Adds one BigInt to another, storing the result in the augend. Calls are
/// made to _real_bigint_add_in_place or _real_big_int_subtract_in_place
/// depending on the signs and relative magnitudes of the arguments.
///
/// @param A The address of the augend
/// @param B The address of the addend
///
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

///
/// Performs addition of addend to augend without regard for sign.
///
/// @param augend The augend, receiving the result.
/// @param addend The addend, added to the augend.
///
void _real_bigint_add_in_place ( BigInt * augend, BigInt * addend )
{
  bool carry = false;
  Bit * a, * b;
  a = augend->lsb;
  b = addend->lsb;

  while ( a && b )
  {
    single_bit_add_in_place ( &(a->bit), b->bit, &carry );

    a = walk_toward_msb ( a, 1 );
    b = walk_toward_msb ( b, 1 );
  }

  while ( a )
  {
    single_bit_add_in_place ( &(a->bit), false, &carry );
    a = walk_toward_msb ( a, 1 );
  }

  while ( b )
  {
    bool A = false;
    single_bit_add_in_place ( &A, b->bit, &carry );
    append_bit ( augend, A );
    b = walk_toward_msb ( b, 1 );
  }

  if ( carry )
  {
    append_bit ( augend, true );
  }
}

///
/// Shifts a BigInt to the right by a given count. The bits shifted off are not
/// retained.
///
/// @param a The BigInt being right-shifted
/// @param count The number of bits to right-shift by
///
void bigint_shift_right ( BigInt * a, int count )
{
  for ( ; count > 0; count -- )
  {
    bigint_pop_lsb ( a );
  }
}

///
/// Shift a BigInt to the left by a given count of bits, filling the new
/// positions with zeroes.
///
/// @param a The BigInt being left-shifted
/// @param count The number of bits to left-shift by
///
void bigint_shift_left ( BigInt * a, int count )
{
  for ( ; count > 0; count -- )
  {
    prepend_bit ( a, false );
  }
}

///
/// Shallow copy the fields of one BigInt to another. This is used by
/// bigint_swap and may or may not be useful elsewhere.
///
/// @param a The destination BigInt
/// @param b The source BigInt
///
void bigint_shallow_copy ( BigInt * a, BigInt * b )
{
  a->count = b->count;
  a->lsb = b->lsb;
  a->msb = b->msb;
  a->positive = b->positive;
}

///
/// Swap the values of two BigInts by scalar member value exchange.
///
/// @param a One BigInt being swapped.
/// @param b Another BigInt being swapped.
///
void bigint_swap ( BigInt * a, BigInt * b )
{
  BigInt tmp;
  bigint_shallow_copy ( &tmp, a );
  bigint_shallow_copy ( a, b );
  bigint_shallow_copy ( b, &tmp );
}

///
/// Multiply two BigInts
///
/// @param a One multiplicand
/// @param b Another multiplicand
///
/// @return A new BigInt containing the product of a and b. Must be freed with
/// free_bigint()
///
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

///
/// Determine a BigInt's sign
///
/// @param a The BigInt whose sign to check
///
/// @return true if a is positive; false if a is negative; undefined if a is
/// zero
///
bool bigint_positive ( BigInt * a )
{
  return a->positive;
}

///
/// Compute the sum of two BigInts
///
/// @param a First addend
/// @param b Second addend
///
/// @return New BigInt whose value is the sum of first and second addend; must
/// be freed with free_bigint()
///
BigInt * bigint_add ( BigInt * a, BigInt * b )
{
  BigInt * sum = bigint_copy ( a );
  bigint_add_in_place ( sum, b );
  return sum;
}

///
/// Performs an in-place single-bit subtraction, updating the minuend and
/// borrow flag.
///
/// @param a The address of the minuend
/// @param B The value of the subtrahend
/// @param borrow The address of the borrow flag. If NULL, borrow is assumed to
/// be unset and not updated.
///
void single_bit_subtract_in_place ( bool * a, bool B, bool * borrow )
{
  bool A, Borrow;

  A = *a;
  Borrow = borrow ? *borrow : false;

  *a = A ^ B ^ Borrow;
  if ( borrow ) *borrow = (Borrow & ~A) | (Borrow & B) | (~Borrow & ~A & B);
}

///
/// Subtracts one BigInt from another, storing the result in the minuend. Calls
/// are made to _real_bigint_add_in_place or _real_big_int_subtract_in_place
/// depending on the signs and relative magnitudes of the arguments.
///
/// @param A The address of the minuend
/// @param B The address of the subtrahend
///
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

///
/// Performs subtraction of subtrahend from minuend without regard for sign
///
/// @param A The minuend, receiving the result.
/// @param B The subtrahend, subtracted from the minuend.
///
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

///
/// Find the end of a C-string
///
/// @param str The C-string whose last character to find
///
/// @return A pointer to the last (non-NULL) character in the NULL-terminated
/// C-string
///
char * find_string_end ( char * str )
{
  while ( *str ) str ++;
  return str-1;
}

///
/// Create a BigInt from a C-string describing a decimal integer value. This
/// is meant to overcome limits on the argument to init_bigint(). In fact there
/// is otherwise intended to be no difference between init_bigint_from_string
/// and init_bigint
///
/// @param str C-string describing a decimal integer value
///
/// @return A new BigInt whose value is equal to the argument's
///
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

///
/// Compare two equal-length lists of bits
///
/// @param msb_a Pointer to the MSB of the first list
/// @param msb_b Pointer to the MSB of the second list
/// @param count Number of bits to compare
///
/// @return 0 if equal; 1 if first list > second list; -1 if first list <
/// second list
///
int bitlist_compare_magnitude_forward ( Bit * msb_a, Bit * msb_b, int count )
{
  while ( msb_a && msb_b && count -- )
  {
    if ( msb_a->bit && !msb_b->bit ) return 1;
    if ( !msb_a->bit && msb_b->bit ) return -1;

    msb_a = walk_toward_lsb ( msb_a, 1 );
    msb_b = walk_toward_lsb ( msb_b, 1 );
  }

  if ( !msb_a && msb_b ) return -1;
  if ( msb_a && !msb_b ) return 1;

  return 0;
}

///
/// Skip to the nth item after a given bit in a bit list
///
/// @param b The bit list to advance through
/// @param count The number of bits to advance through
///
/// @return A pointer to the nth bit after the argument
///
Bit * walk_toward_msb ( Bit * b, int count )
{
  while ( b && count -- ) b = b->next;
  return b;
}

///
/// Skip to the nth item toward the LSB
///
/// @param b the bit list to retreat through
/// @param count The number of bits to retreat through
///
/// @return A pointer to the nth behind the argument
///
Bit * walk_toward_lsb ( Bit * b, int count )
{
  while ( b && count -- ) b = b->prev;
  return b;
}

///
/// Divides a BigInt by another BigInt, storing the quotient in a new BigInt
/// and optionally preserving the remainder
///
/// @param dividend The number being divided
/// @param divisor The number dividing
/// @param premainder The address of the remainder; if NULL the remainder is
/// not stored
///
/// @return The quotient of dividend/divisor in a new BigInt that must be freed
/// with free_bigint
///
BigInt * bigint_divide ( BigInt * dividend, BigInt * divisor, BigInt ** premainder )
{
  BigInt * quotient, * subby;
  Bit * dividend_pointer;

  quotient = init_bigint_empty ( );
  subby = init_bigint ( 0 );
  dividend_pointer = dividend->msb;

  while ( dividend_pointer )
  {
    if ( bigint_compare ( subby, divisor ) < 0 )
    {
      fprintf(stderr, "0");
      prepend_bit ( quotient, false );
      prepend_bit ( subby, dividend_pointer->bit );
      dividend_pointer = walk_toward_lsb ( dividend_pointer, 1 );
    }
    else
    {
      fprintf(stderr, "1");
      prepend_bit ( quotient, true );
      bigint_subtract_in_place ( subby, divisor );
    }
  }

  if ( premainder )
  {
    *premainder = subby;
  }
  else
  {
    free_bigint ( subby );
  }

  return quotient;
}

///
/// Copies a forward-oriented non-wrapping consecutive range of bits from one
/// BigInt into a new BigInt.
///
/// @param a The BigInt to slice from
/// @param lsb The index of the least significant bit in the slice
/// @param msb The index of the first bit to stop copying at
///
/// @return A new BigInt whose bits consist of [lsb,msb) from the argument. If
/// lsb is greater than the index of the last bit in the argument, no bits are
/// copied. If msb implies a longer sequence than the argument contains,
/// copying will stop at the last bit in the argument. The new BigInt's sign is
/// the same as the source BigInt's sign.
///
BigInt * bigint_binary_slice ( BigInt * a, int lsb, int msb )
{
  BigInt * out = init_bigint_empty ( );
  Bit * c = walk_toward_msb ( a->lsb, lsb );
  out->positive = a->positive;
  while ( c && lsb ++ < msb )
  {
    append_bit ( out, c->bit );
    c = walk_toward_msb ( c, 1 );
  }
  return out;
}

///
/// Reverses the bits in a BigInt. No zeroes are hacked off.
///
/// @param bi The BigInt to reverse.
///
void bigint_reverse_bits ( BigInt * bi )
{
  Bit * next, * a;

  for ( a = bi->lsb; a; a = a->prev )
  {
    next = a->next;
    a->next = a->prev;
    a->prev = next;
  }

  next = bi->lsb;
  bi->lsb = bi->msb;
  bi->msb = next;
}

