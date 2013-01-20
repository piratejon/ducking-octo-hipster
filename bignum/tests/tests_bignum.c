#include <string.h>
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
  BigInt * bi = bigint_init(29); // 0b11101
  BigInt * a = bigint_init(0);

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

  bigint_free ( a );
  bigint_free ( bi );
}

void test_initialize_negative ( void )
{
  BigInt * a = bigint_init ( -9345781 );

  ASSERT ( bigint_positive ( a ) == false, "BigInt had wrong sign." );
  ASSERT ( bigint_low_dword ( a ) == 9345781, "BigInt had wrong magnitude." );

  bigint_free ( a );
}

void test_bigint_compare ( void )
{
  BigInt * a = bigint_init ( 35 );
  BigInt * b = bigint_init ( 46 );
  BigInt * c = bigint_init ( 35 );
  BigInt * d = bigint_init ( 8 );
  BigInt * e = bigint_init ( 15 );
  BigInt * f = bigint_init ( 14 );
  BigInt * g = bigint_init ( -3 );
  BigInt * h = bigint_init ( -98765 );
  BigInt * i = bigint_init ( -12345 );
  BigInt * j = bigint_init ( 98765 );
  BigInt * k = bigint_init ( 987653 );
  BigInt * l = bigint_init ( 10 );

  ASSERT ( bigint_compare ( k, l ) > 0, "cmp(987653,10) failed" );

  ASSERT ( bigint_compare ( e, f ) > 0, "cmp(15,14) failed" );
  ASSERT ( bigint_compare ( f, e ) < 0, "cmp(14,15) failed" );
  ASSERT ( bigint_compare ( a, b ) < 0, "cmp(35,46) failed" );
  ASSERT ( bigint_compare ( a, a ) == 0, "cmp(35,35) self failed" );
  ASSERT ( bigint_compare ( a, c ) == 0, "cmp(35,35) diff failed" );
  ASSERT ( bigint_compare ( a, d ) > 0, "cmp(35,8) failed" );
  ASSERT ( bigint_compare ( a, g ) > 0, "cmp(35,-3) failed" );
  ASSERT ( bigint_compare ( a, h ) > 0, "cmp(35,-98765) failed" );
  ASSERT ( g->positive == false && h->positive == false, "g and h have wrong signs." );
  ASSERT ( bigint_compare ( g, h ) > 0, "cmp(-3,-98765) failed" );
  ASSERT ( bigint_compare ( h, i ) < 0, "cmp(-98765,-12345) failed" );
  ASSERT ( bigint_compare ( h, j ) < 0, "cmp(-98765,98765) failed" );

  ASSERT ( bigint_compare_magnitude ( e, f ) > 0, "mag_cmp(15,14) failed" );
  ASSERT ( bigint_compare_magnitude ( f, e ) < 0, "mag_cmp(14,15) failed" );
  ASSERT ( bigint_compare_magnitude ( a, b ) < 0, "mag_cmp(35,46) failed" );
  ASSERT ( bigint_compare_magnitude ( a, a ) == 0, "mag_cmp(35,35) self failed" );
  ASSERT ( bigint_compare_magnitude ( a, c ) == 0, "mag_cmp(35,35) diff failed" );
  ASSERT ( bigint_compare_magnitude ( a, d ) > 0, "mag_cmp(35,8) failed" );
  ASSERT ( bigint_compare_magnitude ( a, g ) > 0, "mag_cmp(35,-3) failed" );
  ASSERT ( bigint_compare_magnitude ( a, h ) < 0, "mag_cmp(35,-98765) failed" );
  ASSERT ( bigint_compare_magnitude ( g, h ) < 0, "mag_cmp(-3,-98765) failed" );
  ASSERT ( bigint_compare_magnitude ( h, i ) > 0, "mag_cmp(-98765,-12345) failed" );
  ASSERT ( bigint_compare_magnitude ( h, j ) == 0, "mag_cmp(-98765,98765) failed" );

  bigint_free ( l );
  bigint_free ( k );
  bigint_free ( j );
  bigint_free ( i );
  bigint_free ( h );
  bigint_free ( g );
  bigint_free ( f );
  bigint_free ( e );
  bigint_free ( d );
  bigint_free ( c );
  bigint_free ( b );
  bigint_free ( a );
}

void test_bigint_add_in_place ( void )
{
  BigInt * x, * y;

  x = bigint_init ( 12345 );
  y = bigint_init ( -98765 );
  bigint_add_in_place ( x, y );
  bigint_free ( y );
  y = bigint_init ( -86420 );
  ASSERT ( bigint_compare ( x, y  ) == 0, "Equality failed after self add." );

  bigint_free ( x );
  bigint_free ( y );
}

void test_bigint_add ( void )
{
  BigInt * a = bigint_init ( 12345 );
  BigInt * _a_copy = bigint_copy ( a );
  BigInt * b = bigint_init ( 98765 );
  BigInt * a_plus_b = bigint_add ( a, b );
  BigInt * c = bigint_init ( 111110 );

  BigInt * e = bigint_init ( -98765 );
  BigInt * _e_copy = bigint_copy ( e );
  BigInt * f = bigint_init ( -72 );
  BigInt * e_plus_f = bigint_add ( e, f );
  BigInt * g = bigint_init ( -98837 );

  ASSERT ( bigint_low_dword ( a ) == 12345, "wrong value for a, pre-add" );
  ASSERT ( bigint_positive ( a ), "a should be positive pre-add" );
  ASSERT ( bigint_low_dword ( e ) == 98765, "wrong value for e, pre-add" );
  ASSERT ( !bigint_positive ( e ), "e should be negative, pre-add" );
  BigInt * a_plus_e = bigint_add ( a, e );
  BigInt * h = bigint_init ( -86420 );

  BigInt * g_plus_a = bigint_add ( g, a );
  BigInt * i = bigint_init ( -86492 );

  ASSERT ( bigint_compare ( a, _a_copy ) == 0, "a not equal a before add" );
  ASSERT ( bigint_compare ( a_plus_b, c ) == 0, "wrong answer for a+b" );
  ASSERT ( bigint_compare ( a, _a_copy ) == 0, "a not equal a after add" );

  ASSERT ( bigint_compare ( e, _e_copy ) == 0, "e not equal e before add" );
  ASSERT ( bigint_compare ( e_plus_f, g ) == 0, "wrong answer for e+f" );
  ASSERT ( bigint_low_dword ( e_plus_f ) == 98837, "wrong low bits for e+f" );
  ASSERT ( bigint_compare ( e, _e_copy ) == 0, "e not equal e after add" );

  ASSERT ( bigint_compare ( a_plus_e, h ) == 0, "wrong answer for a+e" );

  ASSERT ( bigint_low_dword ( g ) == 98837 && !bigint_positive ( g ), "g wrong" );
  ASSERT ( bigint_compare ( g_plus_a, i ) == 0, "wrong answer for g+a" );

  bigint_free ( _e_copy );
  bigint_free ( _a_copy );
  bigint_free ( e_plus_f );
  bigint_free ( a_plus_e );
  bigint_free ( g_plus_a );
  bigint_free ( i );
  bigint_free ( h );
  bigint_free ( g );
  bigint_free ( f );
  bigint_free ( e );
  bigint_free ( c );
  bigint_free ( a_plus_b );
  bigint_free ( b );
  bigint_free ( a );
}

void test_prepend ( void )
{
  BigInt * a = bigint_init ( 55 );
  BigInt * b = bigint_init ( 110 );
  BigInt * c = bigint_init ( 221 );
  BigInt * d = bigint_init_empty ( );

  prepend_bit ( a, 0 );
  ASSERT ( bigint_compare ( a, b ) == 0, "Failed to prepend 0." );

  prepend_bit ( b, 1 );
  ASSERT ( bigint_compare ( b, c ) == 0, "Failed to prepend 1." );

  prepend_bit ( d, 1 );
  ASSERT ( bigint_low_dword ( d ) == 1, "failed to prepend 1" );

  prepend_bit ( d, 0 );
  ASSERT ( bigint_low_dword ( d ) == 2, "failed to prepend 0" );

  bigint_free ( d );
  bigint_free ( c );
  bigint_free ( b );
  bigint_free ( a );
}

void test_append ( void )
{
  BigInt * a = bigint_init ( 55 ); // 0b110111
  BigInt * b = bigint_init ( 119 ); // 0b1110111
  BigInt * c = bigint_init_empty ( );

  append_bit ( a, 1 );
  ASSERT ( bigint_compare ( a, b ) == 0, "Failed to append 1." );
  append_bit ( a, 0 );
  ASSERT ( bigint_compare ( a, b ) == 0, "Failed to append 0." );
  append_bit ( a, 1 );
  ASSERT ( bigint_low_dword ( a ) == 375, "Failed to append 1." );

  append_bit ( c, 1 );
  ASSERT ( bigint_low_dword ( c ) == 1, "Failed to append 1 to empty bigint" );

  bigint_free ( c );
  bigint_free ( b );
  bigint_free ( a );
}

void test_bigint_copy ( void )
{
  BigInt * a = bigint_init ( 99999 );
  
  BigInt * c = bigint_copy ( a );

  ASSERT ( bigint_low_dword ( c ) == 99999, "Copy failed." );
  ASSERT ( bigint_compare ( a, c ) == 0, "Equality failed after copy." );

  bigint_free ( c );
  bigint_free ( a );
}

void test_bigint_multiply ( void )
{
  BigInt * a, * b, * c, * a_x_b;

  a = bigint_init ( 199 );
  b = bigint_init ( 305 );
  c = bigint_init ( 60695 );
  a_x_b = bigint_multiply ( a, b );
  ASSERT ( bigint_compare ( a_x_b, c ) == 0, "Equality failed after multiply." );
  bigint_free ( a_x_b );
  bigint_free ( c );
  bigint_free ( b );
  bigint_free ( a );

  a = bigint_init ( -305 );
  b = bigint_init ( 199 );
  c = bigint_init ( -60695 );
  a_x_b = bigint_multiply ( a, b );
  ASSERT ( bigint_compare ( a_x_b, c ) == 0, "Equality failed after negative multiply." );
  bigint_free ( a_x_b );
  bigint_free ( c );
  bigint_free ( b );
  bigint_free ( a );

  a = bigint_init ( 0 );
  b = bigint_init ( 350 );
  c = bigint_init ( 0 );
  a_x_b = bigint_multiply ( a, b );
  ASSERT ( bigint_compare ( a_x_b, c ) == 0, "failed to multiply by zero" );
  bigint_free ( a_x_b );
  bigint_free ( c );
  bigint_free ( b );
  bigint_free ( a );
}

void test_bigint_shift ( void )
{
  BigInt * a = bigint_init ( 100 );

  bigint_shift_right ( a, 2 );
  ASSERT ( bigint_low_dword ( a ) == 25, "Failed to shift 100 right by two." );

  bigint_shift_left ( a, 4 );
  ASSERT ( bigint_low_dword ( a ) == 400, "Failed to shift 25 left by four." );

  bigint_free ( a );
}

void test_bigint_pop ( void )
{
  BigInt * a = bigint_init ( 100 );

  ASSERT ( false == bigint_pop_lsb ( a ), "Popped wrong value from 100." );
  ASSERT ( bigint_low_dword ( a ) == 50, "Failed to pop LSB from 100." );

  ASSERT ( true == bigint_pop_msb ( a ), "Popped wrong value from 50." );
  ASSERT ( bigint_low_dword ( a ) == 18, "Failed to pop MSB from 50." );

  bigint_free ( a );
}

void test_bigint_divide ( void )
{
  BigInt * dividend = bigint_init ( 1 );
  BigInt * divisor = bigint_init ( 1 );
  BigInt * zero = bigint_init ( 0 );
  BigInt * q, * r;

  q = bigint_divide ( dividend, divisor, &r );
  ASSERT ( bigint_compare ( q, divisor ) == 0, "quotient not one" );
  ASSERT ( bigint_compare ( r, zero ) == 0, "remainder not zero" );

  bigint_free ( q );
  bigint_free ( r );
  bigint_free ( zero );
  bigint_free ( divisor );
  bigint_free ( dividend );

  BigInt * a = bigint_init ( 987653 );
  BigInt * b = bigint_init ( 98765 );
  BigInt * c = bigint_init ( 3 );
  BigInt * ten = bigint_init ( 10 );
  q = bigint_divide ( a, ten, &r );
  ASSERT ( bigint_compare ( q, b ) == 0, "wrong quotient" );
  ASSERT ( bigint_compare ( r, c )  == 0, "wrong remainder" );

  bigint_free ( a );
  bigint_free ( b );
  bigint_free ( c );
  bigint_free ( r );
  bigint_free ( q );

  a = bigint_init ( 21 );
  b = bigint_init ( 2 );
  c = bigint_init ( 1 );
    
  q = bigint_divide ( a, ten, &r );
  ASSERT ( bigint_compare ( r, c ) == 0, "Incorrect remainder after division." );
  ASSERT ( bigint_compare ( q, b ) == 0, "Incorrect quotient after division." );

  bigint_free ( ten );
  bigint_free ( q );
  bigint_free ( r );
  bigint_free ( c );
  bigint_free ( b );
  bigint_free ( a );

  a = bigint_init ( 0 );
  b = bigint_init ( 99 );
  c = bigint_init ( 99 );
  q = bigint_divide ( a, b, &r );
  ASSERT ( bigint_compare ( r, c ) == 0, "Incorrect remainder after division of zero." );
  ASSERT ( bigint_compare ( a, q ) == 0, "Incorrect quotient after division of zero." );

  bigint_free ( a );
  bigint_free ( b );
  bigint_free ( c );
  bigint_free ( q );
  bigint_free ( r );
}

void test_real_bigint_subtract_in_place ( void )
{
  BigInt * a = bigint_init ( 12345 );
  BigInt * b = bigint_init ( 750 );
  BigInt * c = bigint_init ( 11600 );
  BigInt * d = bigint_init ( 1 );
  BigInt * e = bigint_init ( 2 );

  BigInt * f = bigint_init ( 5 );
  BigInt * g = bigint_init ( 4 );

  BigInt * h = bigint_init ( 749 );
  BigInt * i = bigint_init ( 745 );

  BigInt * j = bigint_init ( 98765 );
  BigInt * k = bigint_init ( 12345 );
  BigInt * l = bigint_init ( 86420 );
  _real_bigint_subtract_in_place ( j, k );
  ASSERT ( bigint_compare ( j, l ) == 0, "Compare failed after 98765-12345." );

  _real_bigint_subtract_in_place ( e, d );
  ASSERT ( bigint_compare ( e, d ) == 0, "Compare failed after 2-1." );

  _real_bigint_subtract_in_place ( f, g );
  ASSERT ( bigint_compare ( f, d ) == 0, "Compare failed after 5-4." );

  _real_bigint_subtract_in_place ( b, e );
  ASSERT ( bigint_compare ( b, h ) == 0, "Compare failed after 750-1." );

  _real_bigint_subtract_in_place ( b, g );
  ASSERT ( bigint_compare ( b, i ) == 0, "Compare failed after 749-4." );

  _real_bigint_subtract_in_place ( a, b );
  ASSERT ( bigint_compare ( a, c ) == 0, "Compare failed after 12345-750." );

  bigint_free ( l );
  bigint_free ( k );
  bigint_free ( j );
  bigint_free ( i );
  bigint_free ( h );
  bigint_free ( g );
  bigint_free ( f );
  bigint_free ( e );
  bigint_free ( d );
  bigint_free ( c );
  bigint_free ( b );
  bigint_free ( a );
}

void test_bigint_subtract ( void )
{
  BigInt * a, * b, * c, * d;

  a = bigint_init ( 9999 );
  b = bigint_init ( 3456 );
  c = bigint_init ( 6543 );
  bigint_subtract_in_place ( a, b );
  ASSERT ( bigint_compare ( a, c ) == 0, "Compare failed after subtract." );
  bigint_free ( a );
  bigint_free ( b );
  bigint_free ( c );

  a = bigint_init ( 6543 );
  b = bigint_init ( 1000000 );
  c = bigint_init ( -993457 );
  bigint_subtract_in_place ( a, b );
  ASSERT ( bigint_compare ( a, c ) == 0, "Compare failed after subtract." );
  bigint_free ( c );
  bigint_free ( b );
  bigint_free ( a );

  a = bigint_init ( -1000000 );
  b = bigint_init ( 6543 );
  c = bigint_init ( -1006543 );
  bigint_subtract_in_place ( a, b );
  ASSERT ( bigint_compare ( a, c ) == 0, "Compare failed after subtract." );
  bigint_free ( c );
  bigint_free ( b );
  bigint_free ( a );

  a = bigint_init ( -1000 );
  b = bigint_init ( -225 );
  c = bigint_init ( -775 );
  ASSERT ( a->positive == false && b->positive == false, "A & B signs failed." );
  bigint_subtract_in_place ( a, b );
  ASSERT ( a->positive == false, "A sign failed after subtract." );
  ASSERT ( bigint_compare ( a, c ) == 0, "Compare failed after subtract." );
  bigint_free ( c );
  bigint_free ( b );
  bigint_free ( a );

  a = bigint_init ( 123 );
  b = bigint_init ( 127 );
  c = bigint_init ( -4 );
  d = bigint_init ( 4 );
  ASSERT ( a->positive == true && b->positive == true, "A & B signs failed." );
  bigint_subtract_in_place ( a, b );
  ASSERT ( a->positive == false, "A sign failed after subtract." );
  ASSERT ( bigint_compare ( a, c ) == 0, "Sign comparison failed after subtract." );
  ASSERT ( bigint_compare ( a, d ) == -1, "Sign comparison failed after subtract." );
  ASSERT ( bigint_compare_magnitude ( a, c ) == 0, "Magnitude comparison failed after subtract." );
  bigint_free ( d );
  bigint_free ( c );
  bigint_free ( b );
  bigint_free ( a );
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

void test_real_bigint_add_in_place ( void )
{
  BigInt * a, * b, * c, * d;

  a = bigint_init ( 12345 );
  b = bigint_init ( 98765 );
  c = bigint_init ( 111110 );
  _real_bigint_add_in_place ( a, b );
  ASSERT ( bigint_compare ( a, c ) == 0, "wrong answer for a+b" );
  bigint_free ( c );
  bigint_free ( b );
  bigint_free ( a );

  BigInt * zero = bigint_init ( 0 );
  BigInt * one = bigint_init ( 1 );
  BigInt * two = bigint_init ( 2 );
  BigInt * three = bigint_init ( 3 );
  _real_bigint_add_in_place ( zero, one );
  ASSERT ( bigint_compare ( zero, one ) == 0, "failed to add one to zero" );
  _real_bigint_add_in_place ( zero, one );
  ASSERT ( bigint_compare ( zero, two ) == 0, "failed to add one to one" );
  ASSERT ( one->count == 1, "one has the wrong number of bits prior to add" );
  ASSERT ( bigint_low_dword ( one ) == 1, "one has wrong value prior to add" );
  ASSERT ( bigint_low_dword ( two ) == 2, "two has wrong value prior to add" );
  _real_bigint_add_in_place ( one, two );
  ASSERT ( bigint_low_dword ( one ) != 1, "one unchanged after add" );
  ASSERT ( bigint_low_dword ( two ) == 2, "two has wrong value after add" );
  ASSERT ( two->count == 2, "two has wrong number of bits after add" );
  ASSERT ( one->count == 2, "one has wrong number of bits after add" );
  ASSERT ( one->lsb->bit == true, "one lsb wrong after add" );
  ASSERT ( one->lsb->next->bit == true, "one has wrong 2nd bit after add" );
  ASSERT ( bigint_compare ( one, three ) == 0, "failed to add two to one" );
  bigint_free ( zero );
  bigint_free ( one );
  bigint_free ( two );
  bigint_free ( three );

  a = bigint_init ( 99 );
  b = bigint_init ( 365 );
  c = bigint_init ( 464 );
  ASSERT ( bigint_low_dword ( a ) == 99, "a wrong before added." );
  ASSERT ( bigint_low_dword ( b ) == 365, "b wrong before added." );
  _real_bigint_add_in_place ( a, b );
  ASSERT ( bigint_low_dword ( a ) != 99, "a unchanged by add." );
  ASSERT ( bigint_low_dword ( b ) == 365, "b wrong after added." );
  ASSERT ( bigint_low_dword ( a ) == 464, "Add failed." );
  ASSERT ( bigint_compare ( a, c ) == 0, "Equality failed after add." );
  bigint_free ( a );
  bigint_free ( b );
  
  _real_bigint_add_in_place ( c, c );
  ASSERT ( bigint_low_dword ( c ) == 928, "Self add failed." );
  d = bigint_init ( 928 );
  ASSERT ( bigint_compare ( c, d ) == 0, "Equality failed after self add." );
  bigint_free ( d );
  bigint_free ( c );

  a = bigint_init ( 12345 );
  b = bigint_init ( -98837 );
  c = bigint_init ( -86492 );
  bigint_add_in_place ( a, b );
  ASSERT ( bigint_compare ( a, c ) == 0, "g+a wrong" );
  bigint_free ( a );
  bigint_free ( b );
  bigint_free ( c );

  a = bigint_init ( -1000 );
  b = bigint_init ( 225 );
  c = bigint_init ( -775 );
  bigint_add_in_place ( a, b );
  ASSERT ( bigint_compare ( a, c ) == 0, "Compare failed after add." );
  bigint_free ( c );
  bigint_free ( b );
  bigint_free ( a );

  a = bigint_init ( 1776 );
  b = bigint_init ( -952 );
  c = bigint_init ( 824 );
  bigint_add_in_place ( a, b );
  ASSERT ( bigint_compare ( a, c ) == 0, "Compare failed after add." );
  bigint_free ( c );
  bigint_free ( b );
  bigint_free ( a );

  a = bigint_init ( -2776 );
  b = bigint_init ( 9234 );
  c = bigint_init ( 6458 );
  bigint_add_in_place ( a, b );
  ASSERT ( bigint_compare ( a, c ) == 0, "Compare failed after add." );
  bigint_free ( c );
  bigint_free ( b );
  bigint_free ( a );

  a = bigint_init ( -1209384923 );
  b = bigint_init ( -862089320 );
  c = bigint_init ( -2071474243 );
  bigint_add_in_place ( a, b );
  ASSERT ( bigint_compare ( a, c ) == 0, "Compare failed after add." );
  bigint_free ( c );
  bigint_free ( b );
  bigint_free ( a );
}

void test_bigint_from_string ( void )
{
  BigInt * a = bigint_init_from_string ( "99" );
  ASSERT ( bigint_low_dword ( a ) == 99 && bigint_positive ( a ), "wrong value from string" );
  bigint_free ( a );

  a = bigint_init_from_string ( "123456" );
  ASSERT ( bigint_low_dword ( a ) == 123456 && bigint_positive ( a ), "wrong value from string" );
  bigint_free ( a );

  a = bigint_init_from_string ( "-12345678900987654321" );
  ASSERT ( (unsigned int)bigint_low_dword ( a ) == 3697766577, "wrong value" );
  bigint_shift_right ( a, 32 );
  ASSERT ( (unsigned int)bigint_low_dword ( a ) == 2874452364, "wrong value" );
  ASSERT ( a->positive == false, "a has wrong sign" );
  bigint_free ( a );
}

void test_bitlist_compare_magnitude ( void )
{
  BigInt * a = bigint_init ( 127 );
  BigInt * b = bigint_init ( 126 );
  BigInt * c = bigint_init ( 63 );
  BigInt * d = bigint_init ( 64 );

  ASSERT ( bitlist_compare_magnitude_forward ( a->msb, b->msb, 7 ) == 1, "bitlist compare failed" );
  ASSERT ( bitlist_compare_magnitude_forward ( a->msb, b->msb, 6 ) == 0, "bitlist compare failed" );
  ASSERT ( bitlist_compare_magnitude_forward ( c->msb, a->msb, 5 ) == 0, "bitlist compare failed" );
  ASSERT ( bitlist_compare_magnitude_forward ( d->msb, b->msb, 7 ) == -1, "bitlist compare failed" );

  bigint_free ( d );
  bigint_free ( c );
  bigint_free ( b );
  bigint_free ( a );
}

void test_bigint_binary_slice ( void )
{
  BigInt * a = bigint_init ( 127 );
  BigInt * b;
 
  b = bigint_binary_slice ( a, 0, 0 );
  ASSERT ( b->count == 0, "empty slice has wrong count" );
  ASSERT ( b->lsb == NULL && b->msb == NULL, "empty slice has non-NULL MSB&LSB" );
  bigint_free ( b );

  b = bigint_binary_slice ( a, 3, 3 );
  ASSERT ( b->count == 0 && b->lsb == NULL && b->msb == NULL, "empty slice not empty" );
  bigint_free ( b );

  b = bigint_binary_slice ( a, 0, 1 );
  ASSERT ( bigint_low_dword ( b ) == 1, "failed to slice 127[0:1]" );
  bigint_free ( b );

  b = bigint_binary_slice ( a, 5, 7 );
  ASSERT ( bigint_low_dword ( b ) == 3, "failed to slice 127[5:7]" );
  bigint_free ( b );
  bigint_free ( a );

  a = bigint_init ( 1234567890 ); // 0b100 100110010110 0000001011010010
  b = bigint_binary_slice ( a, 16, 28 );
  ASSERT ( bigint_low_dword ( b ) == 2454, "failed to slice properly" );
  bigint_free ( b );

  b = bigint_binary_slice ( a, 16, 9999 );
  ASSERT ( bigint_low_dword ( b ) == 18838, "failed to curtail slice" );
  bigint_free ( b );
  bigint_free ( a );
}

void test_walk_toward_msb ( void )
{
  BigInt * a = bigint_init ( 1245 ); // 0b10011011101
  Bit const * bit = a->lsb;

  ASSERT ( bit->bit == true, "wrong LSB for 1245" );
  walk_toward_msb ( bit, 0 );
  ASSERT ( a->lsb == bit, "walk_toward_msb zero changed the bit" );
  bit = walk_toward_msb ( bit, 1 );
  ASSERT ( bit->bit == false, "wrong bit-1 for 1245" );
  bit = walk_toward_msb ( bit, 3 );
  ASSERT ( bit->bit == true, "wrong bit-5 for 1245" );
  bit = walk_toward_msb ( bit, 1 );
  ASSERT ( bit->bit == false, "wrong bit-6 for 1245" );
  bit = walk_toward_msb ( bit, 27 );
  ASSERT ( bit == NULL, "1245 has too many bits" );

  bigint_free ( a );
}

void test_reverse_bits ( void )
{
  BigInt * a = bigint_init ( 99999 ); // 0b11000011010011111
  // 0b11111001011000011

  _bigint_reverse_bits ( a );
  ASSERT ( bigint_low_dword ( a ) == 127683, "failed to reverse bits" );
  _bigint_reverse_bits ( a );
  ASSERT ( bigint_low_dword ( a ) == 99999, "failed to reverse bits" );
  bigint_pop_msb ( a );
  _bigint_reverse_bits ( a );
  ASSERT ( bigint_low_dword ( a ) == 63841, "failed to reverse after pop" );
  bigint_pop_lsb ( a );
  _bigint_reverse_bits ( a );
  ASSERT ( bigint_low_dword ( a ) == 1695, "failed to reverse after popping and reversing" );
  ASSERT ( a->msb->bit == false && a->lsb->bit == true, "wrong bits" );

  bigint_free ( a );
}

void test_bigint_tostring_base2 ( void )
{
  BigInt * a = bigint_init ( 12345 );
  char * astr = bigint_tostring_base2 ( a );
  ASSERT ( strcmp ( astr, "11000000111001" ) == 0, "wrong bits in tostring" );
  free ( astr );
  bigint_free ( a );
}

void test_bigint_remove_high_zeroes ( void )
{
  BigInt * a = bigint_init ( 129 );

  ASSERT ( bigint_low_dword ( a ) == 129, "wrong value" );
  a->msb->bit = false;
  ASSERT ( bigint_low_dword ( a ) == 1 && a->count == 8, "wrong count and value post-msb-reset" );
  
  ASSERT ( _bigint_remove_high_zeroes ( a ) == 7, "removed wrong number of bits" );
  ASSERT ( a->count == 1, "wrong count post-remove-high-zeroes" );
  ASSERT ( bigint_low_dword ( a ) == 1, "wrong value post-remove-high-zeroes" );

  bigint_free ( a );
}

void test_bigint_tostring_base10 ( void )
{
  BigInt * a = bigint_init ( 1234567890 );
  BigInt * b = bigint_init ( 987654321 );

  BigInt * c = bigint_multiply ( a, b );
  BigInt * d = bigint_multiply ( c, c );
  BigInt * e = bigint_multiply ( d, a );
  BigInt * ten = bigint_init ( -10 );
  BigInt * f = bigint_multiply ( e, ten );

  char * str_e = bigint_tostring_base10 ( f );
  ASSERT ( strcmp ( str_e, "-18355020240438438239947245929150007917788290000" ) == 0, "wrong base 10 value for -18355020240438438239947245929150007917788290000" );
  free ( str_e );

  str_e = bigint_tostring_base10 ( e );
  ASSERT ( strcmp ( str_e, "1835502024043843823994724592915000791778829000" ) == 0, "wrong base 10 value for 1835502024043843823994724592915000791778829000" );
  free ( str_e );

  bigint_free ( ten );
  bigint_free ( f );
  bigint_free ( a );
  bigint_free ( b );
  bigint_free ( c );
  bigint_free ( d );
  bigint_free ( e );
}

void test_bigint_modulo ( void )
{
  BigInt * a = bigint_init ( 9876543 );
  BigInt * b = bigint_init ( 12345 );

  BigInt * remainder = bigint_modulo ( a, b );

  ASSERT ( bigint_low_dword ( remainder ) == 543, "wrong remainder" );

  bigint_free ( remainder );
  bigint_free ( b );
  bigint_free ( a );
}

void test_factorial ( void )
{
  BigInt * a = bigint_init ( 20 );
  BigInt * b = bigint_factorial ( a );
  char * b_str = bigint_tostring_base10 ( b );
  // ASSERT ( strlen ( b_str ) == 25809, "wrong length for factorial" );
  ASSERT ( strcmp ( b_str, "2432902008176640000" ) == 0, "wrong value for 20!" );
  free ( b_str );
  bigint_free ( b );
  bigint_free ( a );

  a = bigint_init ( 200 );
  b = bigint_factorial ( a );
  b_str = bigint_tostring_base10 ( b );
  ASSERT ( strcmp ( b_str, "788657867364790503552363213932185062295135977687173263294742533244359449963403342920304284011984623904177212138919638830257642790242637105061926624952829931113462857270763317237396988943922445621451664240254033291864131227428294853277524242407573903240321257405579568660226031904170324062351700858796178922222789623703897374720000000000000000000000000000000000000000000000000" ) == 0, "wrong value for 200!" );
  free ( b_str );
  bigint_free ( b );
  bigint_free ( a );

  a = bigint_init ( 2000 );
  b = bigint_factorial ( a );
  b_str = bigint_tostring_base10 ( b );
  ASSERT ( strcmp ( b_str, "331627509245063324117539338057632403828111720810578039457193543706038077905600822400273230859732592255402352941225834109258084817415293796131386633526343688905634058556163940605117252571870647856393544045405243957467037674108722970434684158343752431580877533645127487995436859247408032408946561507233250652797655757179671536718689359056112815871601717232657156110004214012420433842573712700175883547796899921283528996665853405579854903657366350133386550401172012152635488038268152152246920995206031564418565480675946497051552288205234899995726450814065536678969532101467622671332026831552205194494461618239275204026529722631502574752048296064750927394165856283531779574482876314596450373991327334177263608852490093506621610144459709412707821313732563831572302019949914958316470942774473870327985549674298608839376326824152478834387469595829257740574539837501585815468136294217949972399813599481016556563876034227312912250384709872909626622461971076605931550201895135583165357871492290916779049702247094611937607785165110684432255905648736266530377384650390788049524600712549402614566072254136302754913671583406097831074945282217490781347709693241556111339828051358600690594619965257310741177081519922564516778571458056602185654760952377463016679422488444485798349801548032620829890965857381751888619376692828279888453584639896594213952984465291092009103710046149449915828588050761867924946385180879874512891408019340074625920057098729578599643650655895612410231018690556060308783629110505601245908998383410799367902052076858669183477906558544700148692656924631933337612428097420067172846361939249698628468719993450393889367270487127172734561700354867477509102955523953547941107421913301356819541091941462766417542161587625262858089801222443890248677182054959415751991701271767571787495861619665931878855141835782092601482071777331735396034304969082070589958701381980813035590160762908388574561288217698136182483576739218303118414719133986892842344000779246691209766731651433494437473235636572048844478331854941693030124531676232745367879322847473824485092283139952509732505979127031047683601481191102229253372697693823670057565612400290576043852852902937606479533458179666123839605262549107186663869354766108455046198102084050635827676526589492393249519685954171672419329530683673495544004586359838161043059449826627530605423580755894108278880427825951089880635410567917950974017780688782869810219010900148352061688883720250310665922068601483649830532782088263536558043605686781284169217133047141176312175895777122637584753123517230990549829210134687304205898014418063875382664169897704237759406280877253702265426530580862379301422675821187143502918637636340300173251818262076039747369595202642632364145446851113427202150458383851010136941313034856221916631623892632765815355011276307825059969158824533457435437863683173730673296589355199694458236873508830278657700879749889992343555566240682834763784685183844973648873952475103224222110561201295829657191368108693825475764118886879346725191246192151144738836269591643672490071653428228152661247800463922544945170363723627940757784542091048305461656190622174286981602973324046520201992813854882681951007282869701070737500927666487502174775372742351508748246720274170031581122805896178122160747437947510950620938556674581252518376682157712807861499255876132352950422346387878954850885764466136290394127665978044202092281337987115900896264878942413210454925003566670632909441579372986743421470507213588932019580723064781498429522595589012754823971773325722910325760929790733299545056388362640474650245080809469116072632087494143973000704111418595530278827357654819182002449697761111346318195282761590964189790958117338627206088910432945244978535147014112442143055486089639578378347325323595763291438925288393986256273242862775563140463830389168421633113445636309571965978466338551492316196335675355138403425804162919837822266909521770153175338730284610841886554138329171951332117895728541662084823682817932512931237521541926970269703299477643823386483008871530373405666383868294088487730721762268849023084934661194260180272613802108005078215741006054848201347859578102770707780655512772540501674332396066253216415004808772403047611929032210154385353138685538486425570790795341176519571188683739880683895792743749683498142923292196309777090143936843655333359307820181312993455024206044563340578606962471961505603394899523321800434359967256623927196435402872055475012079854331970674797313126813523653744085662263206768837585132782896252333284341812977624697079543436003492343159239674763638912115285406657783646213911247447051255226342701239527018127045491648045932248108858674600952306793175967755581011679940005249806303763141344412269037034987355799916009259248075052485541568266281760815446308305406677412630124441864204108373119093130001154470560277773724378067188899770851056727276781247198832857695844217588895160467868204810010047816462358220838532488134270834079868486632162720208823308727819085378845469131556021728873121907393965209260229101477527080930865364979858554010577450279289814603688431821508637246216967872282169347370599286277112447690920902988320166830170273420259765671709863311216349502171264426827119650264054228231759630874475301847194095524263411498469508073390080000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" ) == 0, "wrong value for 200!" );
  free ( b_str );
  bigint_free ( b );
  bigint_free ( a );
}

void do_tests ( void )
{
  TEST ( sanity_check_zero );
  TEST ( sanity_check_one );
  TEST ( test_initialize_bignum );
  TEST ( test_initialize_negative );
  TEST ( test_bigint_compare );
  TEST ( test_prepend );
  TEST ( test_real_bigint_add_in_place );
  TEST ( test_real_bigint_subtract_in_place );
  TEST ( test_bigint_add_in_place );
  TEST ( test_bigint_add );
  TEST ( test_bigint_copy );
  TEST ( test_bigint_pop );
  TEST ( test_bigint_shift );
  TEST ( test_bigint_multiply );
  TEST ( test_single_bit_subtract_in_place );
  TEST ( test_single_bit_add_in_place );
  TEST ( test_bigint_subtract );
  TEST ( test_bigint_from_string );
  TEST ( test_walk_toward_msb );
  TEST ( test_bigint_divide );
  TEST ( test_reverse_bits );
  TEST ( test_bitlist_compare_magnitude );
  TEST ( test_bigint_binary_slice );
  TEST ( test_bigint_tostring_base2 );
  TEST ( test_append );
  TEST ( test_bigint_remove_high_zeroes );
  TEST ( test_bigint_tostring_base10 );
  TEST ( test_bigint_modulo );
  TEST ( test_factorial );
}

