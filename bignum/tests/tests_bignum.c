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
  BigInt * a = init_bigint(0);

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

  ASSERT ( a != NULL, "failed to allocate zero" );
  ASSERT ( bigint_low_dword ( a ) == 0, "wrong value for zero" );
  ASSERT ( a->count == 0, "wrong number of bits for zero" );

  free_bigint ( a );
  free_bigint ( bi );
}

void test_initialize_negative ( void )
{
  BigInt * a = init_bigint ( -9345781 );

  ASSERT ( bigint_positive ( a ) == false, "BigInt had wrong sign." );
  ASSERT ( bigint_low_dword ( a ) == 9345781, "BigInt had wrong magnitude." );

  free_bigint ( a );
}

void test_bigint_compare ( void )
{
  BigInt * a = init_bigint ( 35 );
  BigInt * b = init_bigint ( 46 );
  BigInt * c = init_bigint ( 35 );
  BigInt * d = init_bigint ( 8 );
  BigInt * e = init_bigint ( 15 );
  BigInt * f = init_bigint ( 14 );
  BigInt * g = init_bigint ( -3 );
  BigInt * h = init_bigint ( -98765 );
  BigInt * i = init_bigint ( -12345 );
  BigInt * j = init_bigint ( 98765 );

  ASSERT ( bigint_compare ( e, f ) > 0, "cmp(15,14) failed" );
  ASSERT ( bigint_compare ( f, e ) < 0, "cmp(14,15) failed" );
  ASSERT ( bigint_compare ( a, b ) < 0, "cmp(35,46) failed" );
  ASSERT ( bigint_compare ( a, a ) == 0, "cmp(35,35) self failed" );
  ASSERT ( bigint_compare ( a, c ) == 0, "cmp(35,35) diff failed" );
  ASSERT ( bigint_compare ( a, d ) > 0, "cmp(35,8) failed" );
  ASSERT ( bigint_compare ( a, g ) > 0, "cmp(35,-3) failed" );
  ASSERT ( bigint_compare ( a, g ) > 0, "cmp(35,-98765) failed" );
  ASSERT ( bigint_compare ( g, h ) > 0, "cmp(-3,-98765) failed" );
  ASSERT ( bigint_compare ( h, i ) < 0, "cmp(-98765,-12345) failed" );
  ASSERT ( bigint_compare ( h, j ) < 0, "cmp(-98765,98765) failed" );

  free_bigint ( j );
  free_bigint ( i );
  free_bigint ( h );
  free_bigint ( g );
  free_bigint ( f );
  free_bigint ( e );
  free_bigint ( d );
  free_bigint ( c );
  free_bigint ( b );
  free_bigint ( a );
}

void test_bigint_add_in_place ( void )
{
  BigInt * zero = init_bigint ( 0 );
  BigInt * one = init_bigint ( 1 );
  BigInt * one_2 = init_bigint ( 1 );
  BigInt * two = init_bigint ( 2 );
  BigInt * three = init_bigint ( 3 );
  BigInt * a = init_bigint ( 99 );
  BigInt * b = init_bigint ( 365 );
  BigInt * c = init_bigint ( 464 );
  BigInt * d = init_bigint ( 928 );

  bigint_add_in_place ( zero, one );
  ASSERT ( bigint_compare ( zero, one ) == 0, "failed to add one to zero" );

  bigint_add_in_place ( zero, one_2 );
  ASSERT ( bigint_compare ( zero, two ) == 0, "failed to add one to one" );

  ASSERT ( one->count == 1, "one has the wrong number of bits prior to add" );
  ASSERT ( bigint_low_dword ( one ) == 1, "one has wrong value prior to add" );
  ASSERT ( bigint_low_dword ( two ) == 2, "two has wrong value prior to add" );
  bigint_add_in_place ( one, two );
  ASSERT ( bigint_low_dword ( one ) != 1, "one unchanged after add" );
  ASSERT ( bigint_low_dword ( two ) == 2, "two has wrong value after add" );
  ASSERT ( two->count == 2, "two has wrong number of bits after add" );
  ASSERT ( one->count == 2, "one has wrong number of bits after add" );
  ASSERT ( one->lsb->bit == true, "one lsb wrong after add" );
  ASSERT ( one->lsb->next->bit == true, "one has wrong 2nd bit after add" );
  ASSERT ( bigint_compare ( one, three ) == 0, "failed to add two to one" );

  ASSERT ( bigint_low_dword ( a ) == 99, "a wrong before added." );
  ASSERT ( bigint_low_dword ( b ) == 365, "b wrong before added." );
  bigint_add_in_place ( a, b );
  ASSERT ( bigint_low_dword ( a ) != 99, "a unchanged by add." );
  ASSERT ( bigint_low_dword ( b ) == 365, "b wrong after added." );
  ASSERT ( bigint_low_dword ( a ) == 464, "Add failed." );
  ASSERT ( bigint_compare ( a, c ) == 0, "Equality failed after add." );
  
  bigint_add_in_place ( c, c );
  ASSERT ( bigint_low_dword ( c ) == 928, "Self add failed." );
  ASSERT ( bigint_compare ( c, d ) == 0, "Equality failed after self add." );

  free_bigint ( zero );
  free_bigint ( one );
  free_bigint ( one_2 );
  free_bigint ( two );
  free_bigint ( three );
  free_bigint ( d );
  free_bigint ( c );
  free_bigint ( b );
  free_bigint ( a );
}

void test_bigint_add ( void )
{
  BigInt * a = init_bigint ( 12345 );
  BigInt * _a_copy = bigint_copy ( a );
  BigInt * b = init_bigint ( 98765 );
  BigInt * a_plus_b = bigint_add ( a, b );
  BigInt * c = init_bigint ( 111110 );

  BigInt * e = init_bigint ( -98765 );
  BigInt * _e_copy = bigint_copy ( e );
  BigInt * f = init_bigint ( -72 );
  BigInt * e_plus_f = bigint_add ( e, f );
  BigInt * g = init_bigint ( -98837 );

  BigInt * a_plus_e = bigint_add ( a, e );
  BigInt * h = init_bigint ( -86420 );

  BigInt * g_plus_a = bigint_add ( g, a );
  BigInt * i = init_bigint ( -86492 );

  ASSERT ( bigint_compare ( a, _a_copy ) == 0, "a not equal a before add" );
  ASSERT ( bigint_compare ( a_plus_b, c ) == 0, "wrong answer for a+b" );
  ASSERT ( bigint_compare ( a, _a_copy ) == 0, "a not equal a after add" );
  ASSERT ( bigint_compare ( e, _e_copy ) == 0, "a not equal e before add" );
  ASSERT ( bigint_compare ( e_plus_f, g ) == 0, "wrong answer for e+f" );
  ASSERT ( bigint_compare ( e, _e_copy ) == 0, "a not equal e after add" );
  ASSERT ( bigint_compare ( a_plus_e, h ) == 0, "wrong answer for a+e" );
  ASSERT ( bigint_compare ( g_plus_a, i ) == 0, "wrong answer for g+a" );

  free_bigint ( _e_copy );
  free_bigint ( _a_copy );
  free_bigint ( e_plus_f );
  free_bigint ( a_plus_e );
  free_bigint ( g_plus_a );
  free_bigint ( i );
  free_bigint ( h );
  free_bigint ( g );
  free_bigint ( f );
  free_bigint ( e );
  free_bigint ( c );
  free_bigint ( a_plus_b );
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
  BigInt * a_x_b = bigint_multiply ( a, b );
  BigInt * c = init_bigint ( 60695 );
  BigInt * d = init_bigint ( -60695 );
  BigInt * e = init_bigint ( -305 );
  BigInt * f = init_bigint ( 199 );
  BigInt * e_x_f = bigint_multiply ( e, f );

  ASSERT ( bigint_low_dword ( a_x_b ) == 60695, "Multiply failed." );
  ASSERT ( bigint_compare ( a_x_b, c ) == 0, "Equality failed after multiply." );
  ASSERT ( bigint_compare ( e_x_f, d ) == 0, "Equality failed after negative multiply." );

  free_bigint ( e_x_f );
  free_bigint ( a_x_b );
  free_bigint ( f );
  free_bigint ( e );
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

void test_bigint_subtract_in_place ( void )
{
  BigInt * a = init_bigint ( 12345 );
  BigInt * b = init_bigint ( 750 );
  BigInt * c = init_bigint ( 11600 );
  BigInt * d = init_bigint ( 1 );
  BigInt * e = init_bigint ( 2 );

  BigInt * f = init_bigint ( 5 );
  BigInt * g = init_bigint ( 4 );

  BigInt * h = init_bigint ( 749 );
  BigInt * i = init_bigint ( 745 );

  bigint_subtract_in_place ( e, d );
  ASSERT ( bigint_compare ( e, d ) == 0, "Compare failed after 2-1." );

  bigint_subtract_in_place ( f, g );
  ASSERT ( bigint_compare ( f, d ) == 0, "Compare failed after 5-4." );

  bigint_subtract_in_place ( b, e );
  ASSERT ( bigint_compare ( b, h ) == 0, "Compare failed after 750-1." );

  bigint_subtract_in_place ( b, g );
  ASSERT ( bigint_compare ( b, i ) == 0, "Compare failed after 749-4." );

  bigint_subtract_in_place ( a, b );
  ASSERT ( bigint_compare ( a, c ) == 0, "Compare failed after 12345-750." );

  free_bigint ( i );
  free_bigint ( h );
  free_bigint ( g );
  free_bigint ( f );
  free_bigint ( e );
  free_bigint ( d );
  free_bigint ( c );
  free_bigint ( b );
  free_bigint ( a );
}

void test_bigint_subtract ( void )
{
  BigInt * a = init_bigint ( 9999 );
  BigInt * b = init_bigint ( 3456 );
  BigInt * c = init_bigint ( 1000000 );
  BigInt * d = init_bigint ( 6543 );
  BigInt * e = init_bigint ( -993457 );

  // bigint_subtract ( a, b );
  ASSERT ( bigint_compare ( a, d ) == 0, "Compare failed after subtract." );

  // bigint_subtract ( d, c );
  // ASSERT ( bigint_compare ( d, e

  free_bigint ( e );
  free_bigint ( d );
  free_bigint ( c );
  free_bigint ( b );
  free_bigint ( a );
}

void test_single_bit_subtract_in_place ( void )
{
  bool a, b, borrow;

  a = false; b = false; borrow = false;
  single_bit_subtract_in_place ( &a, b, &borrow );
  ASSERT ( a == false, "shit" );
  ASSERT ( borrow == false, "shit" );

  a = true; b = false; borrow = false;
  single_bit_subtract_in_place ( &a, b, &borrow );
  ASSERT ( a == true && borrow == false, "shit" );

  a = false; b = true; borrow = false;
  single_bit_subtract_in_place ( &a, b, &borrow );
  ASSERT ( a == true, "shit" );
  ASSERT ( borrow == true, "shit" );

  a = true; b = true; borrow = false;
  single_bit_subtract_in_place ( &a, b, &borrow );
  ASSERT ( a == false, "shit" );
  ASSERT ( borrow == false, "shit" );

  a = false; b = false; borrow = true;
  single_bit_subtract_in_place ( &a, b, &borrow );
  ASSERT ( a == true, "shit" );
  ASSERT ( borrow == true, "shit" );

  a = true; b = false; borrow = true;
  single_bit_subtract_in_place ( &a, b, &borrow );
  ASSERT ( a == false, "shit" );
  ASSERT ( borrow == false, "shit" );

  a = false; b = true; borrow = true;
  single_bit_subtract_in_place ( &a, b, &borrow );
  ASSERT ( a == false && borrow == true, "shit" );

  a = true; b = true; borrow = true;
  single_bit_subtract_in_place ( &a, b, &borrow );
  ASSERT ( a == true, "shit" );
  ASSERT ( borrow == true, "shit" );
}

void test_single_bit_add_in_place ( void )
{
  bool a, b, carry;

  a = false; b = false; carry = false;
  single_bit_add_in_place ( &a, b, &carry );
  ASSERT ( a == false, "shit" );
  ASSERT ( carry == false, "shit" );

  a = true; b = false; carry = false;
  single_bit_add_in_place ( &a, b, &carry );
  ASSERT ( a == true && carry == false, "shit" );

  a = false; b = true; carry = false;
  single_bit_add_in_place ( &a, b, &carry );
  ASSERT ( a == true, "shit" );
  ASSERT ( carry == false, "shit" );

  a = true; b = true; carry = false;
  single_bit_add_in_place ( &a, b, &carry );
  ASSERT ( a == false, "shit" );
  ASSERT ( carry == true, "shit" );

  a = false; b = false; carry = true;
  single_bit_add_in_place ( &a, b, &carry );
  ASSERT ( a == true, "shit" );
  ASSERT ( carry == false, "shit" );

  a = true; b = false; carry = true;
  single_bit_add_in_place ( &a, b, &carry );
  ASSERT ( a == false, "shit" );
  ASSERT ( carry == true, "shit" );

  a = false; b = true; carry = true;
  single_bit_add_in_place ( &a, b, &carry );
  ASSERT ( a == false && carry == true, "shit" );

  a = true; b = true; carry = true;
  single_bit_add_in_place ( &a, b, &carry );
  ASSERT ( a == true, "shit" );
  ASSERT ( carry == true, "shit" );
}

void do_tests ( void )
{
  TEST ( sanity_check_zero );
  TEST ( sanity_check_one );
  TEST ( test_initialize_bignum );
  TEST ( test_initialize_negative );
  TEST ( test_bigint_compare );
  TEST ( test_prepend );
  TEST ( test_bigint_add_in_place );
  // TEST ( test_bigint_add );
  TEST ( test_bigint_copy );
  TEST ( test_bigint_pop );
  TEST ( test_bigint_shift );
  TEST ( test_bigint_multiply );
  TEST ( test_single_bit_subtract_in_place );
  TEST ( test_single_bit_add_in_place );
  TEST ( test_bigint_subtract_in_place );
  // TEST ( test_bigint_subtract );
  // TEST ( test_bigint_div_10 );
}

