#include "bignum.h"
#include "tests.h"

void sanity_check_zero ( void )
{
  ASSERT ( 0 == 0, "Zero failed to be equal to zero." );
}

void sanity_check_one ( void )
{
  ASSERT ( 1 == 1, "One failed to be equal to one." );
}

void test_initialize_bignum ( void )
{
  BigInt * bi = init_bigint(29);

  ASSERT ( bi != NULL, "Failed to allocate BigInt." );
  ASSERT ( bigint_low_dword ( bi ) == 29, "Wrong value for BigInt." );
  ASSERT ( bi->count == 5, "Wrong number of bits in BigInt." );
  ASSERT (
                              bi->lsb->bit == 1 &&
                        bi->lsb->next->bit == 0 &&
                  bi->lsb->next->next->bit == 1 &&
            bi->lsb->next->next->next->bit == 1 &&
      bi->lsb->next->next->next->next->bit == 1 &&

      bi->msb->prev->prev->prev->prev->bit == 1 &&
            bi->msb->prev->prev->prev->bit == 0 &&
                  bi->msb->prev->prev->bit == 1 &&
                        bi->msb->prev->bit == 1 &&
                              bi->msb->bit == 1 &&

      bi->msb->prev->prev == bi->lsb->next->next &&

      bi->lsb->next->next->next->next->next == NULL,
      "Wrong bits for BigInt 29"
      );

  free_bigint ( bi );
}

void test_bigint_compare ( void )
{
  BigInt * a = init_bigint ( 35 );
  BigInt * b = init_bigint ( 46 );
  BigInt * c = init_bigint ( 35 );
  BigInt * d = init_bigint ( 8 );
  BigInt * e = init_bigint ( 15 );
  BigInt * f = init_bigint ( 14 );

  ASSERT ( bigint_compare ( e, f ) > 0, "cmp(15,14) failed" );
  ASSERT ( bigint_compare ( f, e ) < 0, "cmp(14,15) failed" );
  ASSERT ( bigint_compare ( a, b ) < 0, "cmp(35,46) failed" );
  ASSERT ( bigint_compare ( a, a ) == 0, "cmp(35,35) self failed" );
  ASSERT ( bigint_compare ( a, c ) == 0, "cmp(35,35) diff failed" );
  ASSERT ( bigint_compare ( a, d ) > 0, "cmp(35,8) failed" );

  free_bigint ( f );
  free_bigint ( e );
  free_bigint ( d );
  free_bigint ( c );
  free_bigint ( b );
  free_bigint ( a );
}

void test_bigint_add ( void )
{
  BigInt * a = init_bigint ( 99 );
  BigInt * b = init_bigint ( 365 );
  BigInt * c = init_bigint ( 464 );
  BigInt * d = init_bigint ( 928 );

  bigint_add ( a, b );
  ASSERT ( bigint_low_dword ( a ) == 464, "Add failed." );
  ASSERT ( bigint_compare ( a, c ) == 0, "Equality failed after add." );
  
  bigint_add ( c, c );
  ASSERT ( bigint_low_dword ( c ) == 928, "Self add failed." );
  ASSERT ( bigint_compare ( c, d ) == 0, "Equality failed after self add." );

  free_bigint ( d );
  free_bigint ( c );
  free_bigint ( b );
  free_bigint ( a );
}

void test_prepend ( void )
{
  BigInt * a = init_bigint ( 55 );
  BigInt * b = init_bigint ( 110 );
  BigInt * c = init_bigint ( 221 );

  prepend_bit ( a, 0 );
  ASSERT ( bigint_compare ( a, b ) == 0, "Failed to prepend 0." );

  prepend_bit ( b, 1 );
  ASSERT ( bigint_compare ( b, c ) == 0, "Failed to prepend 1." );

  free_bigint ( c );
  free_bigint ( b );
  free_bigint ( a );
}

void test_bigint_copy ( void )
{
  BigInt * a = init_bigint ( 99999 );
  
  BigInt * c = bigint_copy ( a );

  ASSERT ( bigint_low_dword ( c ) == 99999, "Copy failed." );
  ASSERT ( bigint_compare ( a, c ) == 0, "Equality failed after copy." );

  free_bigint ( c );
  free_bigint ( a );
}

void test_bigint_multiply ( void )
{
  BigInt * a = init_bigint ( 199 );
  BigInt * b = init_bigint ( 305 );
  BigInt * c = init_bigint ( 60695 );
  BigInt * d = init_bigint ( 93025 );

  bigint_multiply ( a, b );
  ASSERT ( bigint_low_dword ( a ) == 60695, "Multiply failed." );
  ASSERT ( bigint_compare ( a, c ) == 0, "Equality failed after multiply." );

  bigint_multiply ( b, b );
  ASSERT ( bigint_low_dword ( b ) == 93025, "Self multiply failed." );
  ASSERT ( bigint_compare ( b, d ) == 0, "Equality failed after self multiply." );

  free_bigint ( d );
  free_bigint ( c );
  free_bigint ( b );
  free_bigint ( a );
}

void test_bigint_shift ( void )
{
  BigInt * a = init_bigint ( 100 );

  bigint_shift_right ( a, 2 );
  ASSERT ( bigint_low_dword ( a ) == 25, "Failed to shift 100 right by two." );

  bigint_shift_left ( a, 4 );
  ASSERT ( bigint_low_dword ( a ) == 400, "Failed to shift 25 left by four." );

  free_bigint ( a );
}

void test_bigint_pop ( void )
{
  BigInt * a = init_bigint ( 100 );

  ASSERT ( false == bigint_pop_lsb ( a ), "Popped wrong value from 100." );
  ASSERT ( bigint_low_dword ( a ) == 50, "Failed to pop LSB from 100." );

  ASSERT ( true == bigint_pop_msb ( a ), "Popped wrong value from 50." );
  ASSERT ( bigint_low_dword ( a ) == 18, "Failed to pop MSB from 50." );

  free_bigint ( a );
}

void test_bigint_div_10 ( void )
{
  BigInt * a = init_bigint ( 987653 );
  BigInt * b = init_bigint ( 98765 );
  BigInt * c = init_bigint ( 3 );
  BigInt * r = bigint_div_10 ( a );

  ASSERT ( bigint_compare ( a, b ) == 0, "Incorrect quotient after div10." );
  ASSERT ( bigint_compare ( r, c ) == 0, "Incorrect remainder after div10." );

  free_bigint ( r );
  free_bigint ( c );
  free_bigint ( b );
  free_bigint ( a );
}

void do_tests ( void )
{
  TEST ( sanity_check_zero );
  TEST ( sanity_check_one );
  TEST ( test_initialize_bignum );
  TEST ( test_bigint_compare );
  TEST ( test_prepend );
  TEST ( test_bigint_add );
  TEST ( test_bigint_copy );
  TEST ( test_bigint_pop );
  TEST ( test_bigint_shift );
  TEST ( test_bigint_multiply );
  // TEST ( test_bigint_div_10 );
}

