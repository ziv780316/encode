//
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <float.h>
#include <xmmintrin.h>
#include <pmmintrin.h>
#include <gmp.h>

char *get_double_bit_str( double x );
void denormal_number_test ();
void roundoff_guard_bit_test();
void add_roundoff_test();
void mul_roundoff_test();
void div_roundoff_test();

char *get_double_bit_str( double x ) 
{
	static char buf[64] = {0};
	char bit;
	for ( int i = 1; i <= 64; ++i )
	{
		bit = ((*(uint64_t *) &x) >> 64 - i) % 2;
		buf[i - 1] = (bit == 1 ? '1' : '0');
	}
	return buf;
}

void denormal_number_test ()
{
	uint64_t hex;
	double x_denormal;
	double x_min;
	double y;

	printf( "===========================================\n" );
	printf( "denormal number test\n" );
	printf( "===========================================\n" );

	// denormal number default
	hex = 0x0008000000000000ULL; 
	x_denormal = *(double *) &hex;

	hex = 0x0010000000000000ULL; // i.e. DBL_MIN 2^(1-1023)
	x_min = *(double *) &hex;

	// default arithmetic behavior
	y = x_denormal + x_min;
	printf( "* default denormal number arithmetic:\n" );
	printf( "DBL_MIN           =%.15le (%s)\n", DBL_MIN, get_double_bit_str(DBL_MIN) );
	printf( "x_denormal        =%.15le (%s)\n", x_denormal, get_double_bit_str(x_denormal) );
	printf( "x_min             =%.15le (%s)\n", x_min, get_double_bit_str(x_min) );
	printf( "x_min / 2         =%.15le (%s)\n", x_min / 2, get_double_bit_str( x_min / 2) );
	printf( "x_min + x_denormal=%.15le (%s)\n", y, get_double_bit_str(y) );
	printf( "\n" );

	// turn-on DAZ
	printf( "* regard denormal as zero in arithmetic (DAZ)\n" );
	_mm_setcsr( _mm_getcsr() | _MM_DENORMALS_ZERO_ON );
	y = x_denormal + x_min;
	printf( "x_min + x_denormal=%.15le (%s)\n", y, get_double_bit_str(y) );
	printf( "x_min / 2         =%.15le (%s)\n", x_min / 2, get_double_bit_str( x_min / 2) );
	printf( "\n" );
	_mm_setcsr( _mm_getcsr() & ~_MM_DENORMALS_ZERO_ON );

	// turn-on FTZ
	printf( "\n* flushd denormal to zero when it occur (FTZ)\n" );
	_mm_setcsr( _mm_getcsr() | _MM_FLUSH_ZERO_ON );
	y = x_denormal + x_min;
	printf( "x_min + x_denormal=%.15le (%s)\n", y, get_double_bit_str(y) );
	printf( "x_min / 2         =%.15le (%s)\n", x_min / 2, get_double_bit_str( x_min / 2) );
	printf( "\n" );
	_mm_setcsr( _mm_getcsr() & ~_MM_FLUSH_ZERO_ON );
}

void roundoff_guard_bit_test ()
{
	uint64_t e;
	uint64_t hex;
	double x;
	double x_odd;
	double x_001;
	double x_010;
	double x_011;
	double x_100;
	double x_101;
	double x_110;
	double x_111;
	double y_001;
	double y_010;
	double y_011;
	double y_100;
	double y_101;
	double y_110;
	double y_111;
	double y_100_odd;

	printf( "===========================================\n" );
	printf( "roundoff guard bit test\n" );
	printf( "===========================================\n" );

	e = 0;
	hex = 0x0000000000000000ULL | ((e + 1023) << 52); 
	x = *(double *) &hex;

	e = 0;
	hex = 0x00000000000000001LL | ((e + 1023) << 52); 
	x_odd = *(double *) &hex;

	e = -3;
	hex = 0x0000000000000001ULL | ((e + 1023) << 52); 
	x_001 = *(double *) &hex;

	e = -3;
	hex = 0x0000000000000002ULL | ((e + 1023) << 52); 
	x_010 = *(double *) &hex;

	e = -3;
	hex = 0x0000000000000003ULL | ((e + 1023) << 52); 
	x_011 = *(double *) &hex;

	e = -3;
	hex = 0x0000000000000004ULL | ((e + 1023) << 52); 
	x_100 = *(double *) &hex;

	e = -3;
	hex = 0x0000000000000005ULL | ((e + 1023) << 52); 
	x_101 = *(double *) &hex;

	e = -3;
	hex = 0x0000000000000006ULL | ((e + 1023) << 52); 
	x_110 = *(double *) &hex;

	e = -3;
	hex = 0x0000000000000007ULL | ((e + 1023) << 52); 
	x_111 = *(double *) &hex;

	// guard round stick bit rounding action table 
	char *default_xmm_round_mode = "round_to_nearest";
	unsigned int rounding_mode = _MM_GET_ROUNDING_MODE();
	if ( rounding_mode & _MM_ROUND_UP )
	{
		default_xmm_round_mode = "round to +inf";
	}
	if ( rounding_mode & _MM_ROUND_DOWN )
	{
		default_xmm_round_mode = "round to -inf";
	}
	if ( rounding_mode & _MM_ROUND_TOWARD_ZERO )
	{
		default_xmm_round_mode = "round to 0";
	}
	y_001 = x + x_001;
	y_010 = x + x_010;
	y_011 = x + x_011;
	y_100 = x + x_100;
	y_100_odd = x_odd + x_100; // round to even (if LSB of mantissa is 1 then round up, therefore the LSB becomd 10 (i.e. even 2))
	y_101 = x + x_101;
	y_110 = x + x_110;
	y_111 = x + x_111;
	printf( "* guard round stick bit rounding action table (mode=%s)):\n", default_xmm_round_mode );
	printf( "x_GRS    =result\n" );
	printf( "x        =%.15le (%s)\n", x, get_double_bit_str(x) );
	printf( "x_001    =%.15le (%s)\n", x_001, get_double_bit_str(x_001) );
	printf( "x_010    =%.15le (%s)\n", x_010, get_double_bit_str(x_010) );
	printf( "x_011    =%.15le (%s)\n", x_011, get_double_bit_str(x_011) );
	printf( "x_100    =%.15le (%s)\n", x_100, get_double_bit_str(x_100) );
	printf( "x_101    =%.15le (%s)\n", x_101, get_double_bit_str(x_101) );
	printf( "x_110    =%.15le (%s)\n", x_110, get_double_bit_str(x_110) );
	printf( "x_111    =%.15le (%s)\n", x_111, get_double_bit_str(x_111) );
	printf( "y_001    =%.15le (%s)\n", y_001, get_double_bit_str(y_001) );
	printf( "y_010    =%.15le (%s)\n", y_010, get_double_bit_str(y_010) );
	printf( "y_011    =%.15le (%s)\n", y_011, get_double_bit_str(y_011) );
	printf( "y_100    =%.15le (%s)\n", y_100, get_double_bit_str(y_100) );
	printf( "y_100_odd=%.15le (%s)\n", y_100_odd, get_double_bit_str(y_100_odd) );
	printf( "y_101    =%.15le (%s)\n", y_101, get_double_bit_str(y_101) );
	printf( "y_110    =%.15le (%s)\n", y_110, get_double_bit_str(y_110) );
	printf( "y_111    =%.15le (%s)\n", y_111, get_double_bit_str(y_111) );
	printf( "\n" );

	y_001 = -x + -x_001;
	y_010 = -x + -x_010;
	y_011 = -x + -x_011;
	y_100 = -x + -x_100;
	y_100_odd = -x_odd + -x_100; // round to even (if LSB of mantissa is 1 then round up, therefore the LSB becomd 10 (i.e. even 2))
	y_101 = -x + -x_101;
	y_110 = -x + -x_110;
	y_111 = -x + -x_111;
	printf( "* guard round stick bit rounding action table (mode=%s)):\n", default_xmm_round_mode );
	printf( "x_GRS    =result\n" );
	printf( "-x       =%.15le (%s)\n", -x, get_double_bit_str(-x) );
	printf( "-x_001   =%.15le (%s)\n", -x_001, get_double_bit_str(-x_001) );
	printf( "-x_010   =%.15le (%s)\n", -x_010, get_double_bit_str(-x_010) );
	printf( "-x_011   =%.15le (%s)\n", -x_011, get_double_bit_str(-x_011) );
	printf( "-x_100   =%.15le (%s)\n", -x_100, get_double_bit_str(-x_100) );
	printf( "-x_101   =%.15le (%s)\n", -x_101, get_double_bit_str(-x_101) );
	printf( "-x_110   =%.15le (%s)\n", -x_110, get_double_bit_str(-x_110) );
	printf( "-x_111   =%.15le (%s)\n", -x_111, get_double_bit_str(-x_111) );
	printf( "y_001    =%.15le (%s)\n", y_001, get_double_bit_str(y_001) );
	printf( "y_010    =%.15le (%s)\n", y_010, get_double_bit_str(y_010) );
	printf( "y_011    =%.15le (%s)\n", y_011, get_double_bit_str(y_011) );
	printf( "y_100    =%.15le (%s)\n", y_100, get_double_bit_str(y_100) );
	printf( "y_100_odd=%.15le (%s)\n", y_100_odd, get_double_bit_str(y_100_odd) );
	printf( "y_101    =%.15le (%s)\n", y_101, get_double_bit_str(y_101) );
	printf( "y_110    =%.15le (%s)\n", y_110, get_double_bit_str(y_110) );
	printf( "y_111    =%.15le (%s)\n", y_111, get_double_bit_str(y_111) );
	printf( "\n" );

	if ( !(rounding_mode & _MM_ROUND_UP) )
	{
		_MM_SET_ROUNDING_MODE( _MM_ROUND_UP );
		y_001 = x + x_001;
		y_010 = x + x_010;
		y_011 = x + x_011;
		y_100 = x + x_100;
		y_100_odd = x_odd + x_100; // round to even (if LSB of mantissa is 1 then round up, therefore the LSB becomd 10 (i.e. even 2))
		y_101 = x + x_101;
		y_110 = x + x_110;
		y_111 = x + x_111;
		printf( "* guard round stick bit rounding action table (mode=%s)):\n", "round to +inf" );
		printf( "x_GRS    =result\n" );
		printf( "x        =%.15le (%s)\n", x, get_double_bit_str(x) );
		printf( "x_001    =%.15le (%s)\n", x_001, get_double_bit_str(x_001) );
		printf( "x_010    =%.15le (%s)\n", x_010, get_double_bit_str(x_010) );
		printf( "x_011    =%.15le (%s)\n", x_011, get_double_bit_str(x_011) );
		printf( "x_100    =%.15le (%s)\n", x_100, get_double_bit_str(x_100) );
		printf( "x_101    =%.15le (%s)\n", x_101, get_double_bit_str(x_101) );
		printf( "x_110    =%.15le (%s)\n", x_110, get_double_bit_str(x_110) );
		printf( "x_111    =%.15le (%s)\n", x_111, get_double_bit_str(x_111) );
		printf( "y_001    =%.15le (%s)\n", y_001, get_double_bit_str(y_001) );
		printf( "y_010    =%.15le (%s)\n", y_010, get_double_bit_str(y_010) );
		printf( "y_011    =%.15le (%s)\n", y_011, get_double_bit_str(y_011) );
		printf( "y_100    =%.15le (%s)\n", y_100, get_double_bit_str(y_100) );
		printf( "y_100_odd=%.15le (%s)\n", y_100_odd, get_double_bit_str(y_100_odd) );
		printf( "y_101    =%.15le (%s)\n", y_101, get_double_bit_str(y_101) );
		printf( "y_110    =%.15le (%s)\n", y_110, get_double_bit_str(y_110) );
		printf( "y_111    =%.15le (%s)\n", y_111, get_double_bit_str(y_111) );
		printf( "\n" );

		y_001 = -x + -x_001;
		y_010 = -x + -x_010;
		y_011 = -x + -x_011;
		y_100 = -x + -x_100;
		y_100_odd = -x_odd + -x_100; // round to even (if LSB of mantissa is 1 then round up, therefore the LSB becomd 10 (i.e. even 2))
		y_101 = -x + -x_101;
		y_110 = -x + -x_110;
		y_111 = -x + -x_111;
		printf( "* guard round stick bit rounding action table (mode=%s)):\n", "round to +inf" );
		printf( "x_GRS    =result\n" );
		printf( "-x       =%.15le (%s)\n", -x, get_double_bit_str(-x) );
		printf( "-x_001   =%.15le (%s)\n", -x_001, get_double_bit_str(-x_001) );
		printf( "-x_010   =%.15le (%s)\n", -x_010, get_double_bit_str(-x_010) );
		printf( "-x_011   =%.15le (%s)\n", -x_011, get_double_bit_str(-x_011) );
		printf( "-x_100   =%.15le (%s)\n", -x_100, get_double_bit_str(-x_100) );
		printf( "-x_101   =%.15le (%s)\n", -x_101, get_double_bit_str(-x_101) );
		printf( "-x_110   =%.15le (%s)\n", -x_110, get_double_bit_str(-x_110) );
		printf( "-x_111   =%.15le (%s)\n", -x_111, get_double_bit_str(-x_111) );
		printf( "y_001    =%.15le (%s)\n", y_001, get_double_bit_str(y_001) );
		printf( "y_010    =%.15le (%s)\n", y_010, get_double_bit_str(y_010) );
		printf( "y_011    =%.15le (%s)\n", y_011, get_double_bit_str(y_011) );
		printf( "y_100    =%.15le (%s)\n", y_100, get_double_bit_str(y_100) );
		printf( "y_100_odd=%.15le (%s)\n", y_100_odd, get_double_bit_str(y_100_odd) );
		printf( "y_101    =%.15le (%s)\n", y_101, get_double_bit_str(y_101) );
		printf( "y_110    =%.15le (%s)\n", y_110, get_double_bit_str(y_110) );
		printf( "y_111    =%.15le (%s)\n", y_111, get_double_bit_str(y_111) );
		printf( "\n" );
	}

	if ( !(rounding_mode & _MM_ROUND_DOWN) )
	{
		_MM_SET_ROUNDING_MODE( _MM_ROUND_DOWN );
		y_001 = x + x_001;
		y_010 = x + x_010;
		y_011 = x + x_011;
		y_100 = x + x_100;
		y_100_odd = x_odd + x_100; // round to even (if LSB of mantissa is 1 then round up, therefore the LSB becomd 10 (i.e. even 2))
		y_101 = x + x_101;
		y_110 = x + x_110;
		y_111 = x + x_111;
		printf( "* guard round stick bit rounding action table (mode=%s)):\n", "round to -inf" );
		printf( "x_GRS    =result\n" );
		printf( "x        =%.15le (%s)\n", x, get_double_bit_str(x) );
		printf( "x_001    =%.15le (%s)\n", x_001, get_double_bit_str(x_001) );
		printf( "x_010    =%.15le (%s)\n", x_010, get_double_bit_str(x_010) );
		printf( "x_011    =%.15le (%s)\n", x_011, get_double_bit_str(x_011) );
		printf( "x_100    =%.15le (%s)\n", x_100, get_double_bit_str(x_100) );
		printf( "x_101    =%.15le (%s)\n", x_101, get_double_bit_str(x_101) );
		printf( "x_110    =%.15le (%s)\n", x_110, get_double_bit_str(x_110) );
		printf( "x_111    =%.15le (%s)\n", x_111, get_double_bit_str(x_111) );
		printf( "y_001    =%.15le (%s)\n", y_001, get_double_bit_str(y_001) );
		printf( "y_010    =%.15le (%s)\n", y_010, get_double_bit_str(y_010) );
		printf( "y_011    =%.15le (%s)\n", y_011, get_double_bit_str(y_011) );
		printf( "y_100    =%.15le (%s)\n", y_100, get_double_bit_str(y_100) );
		printf( "y_100_odd=%.15le (%s)\n", y_100_odd, get_double_bit_str(y_100_odd) );
		printf( "y_101    =%.15le (%s)\n", y_101, get_double_bit_str(y_101) );
		printf( "y_110    =%.15le (%s)\n", y_110, get_double_bit_str(y_110) );
		printf( "y_111    =%.15le (%s)\n", y_111, get_double_bit_str(y_111) );
		printf( "\n" );

		y_001 = -x + -x_001;
		y_010 = -x + -x_010;
		y_011 = -x + -x_011;
		y_100 = -x + -x_100;
		y_100_odd = -x_odd + -x_100; // round to even (if LSB of mantissa is 1 then round up, therefore the LSB becomd 10 (i.e. even 2))
		y_101 = -x + -x_101;
		y_110 = -x + -x_110;
		y_111 = -x + -x_111;
		printf( "* guard round stick bit rounding action table (mode=%s)):\n", "round to -inf" );
		printf( "x_GRS    =result\n" );
		printf( "-x       =%.15le (%s)\n", -x, get_double_bit_str(-x) );
		printf( "-x_001   =%.15le (%s)\n", -x_001, get_double_bit_str(-x_001) );
		printf( "-x_010   =%.15le (%s)\n", -x_010, get_double_bit_str(-x_010) );
		printf( "-x_011   =%.15le (%s)\n", -x_011, get_double_bit_str(-x_011) );
		printf( "-x_100   =%.15le (%s)\n", -x_100, get_double_bit_str(-x_100) );
		printf( "-x_101   =%.15le (%s)\n", -x_101, get_double_bit_str(-x_101) );
		printf( "-x_110   =%.15le (%s)\n", -x_110, get_double_bit_str(-x_110) );
		printf( "-x_111   =%.15le (%s)\n", -x_111, get_double_bit_str(-x_111) );
		printf( "y_001    =%.15le (%s)\n", y_001, get_double_bit_str(y_001) );
		printf( "y_010    =%.15le (%s)\n", y_010, get_double_bit_str(y_010) );
		printf( "y_011    =%.15le (%s)\n", y_011, get_double_bit_str(y_011) );
		printf( "y_100    =%.15le (%s)\n", y_100, get_double_bit_str(y_100) );
		printf( "y_100_odd=%.15le (%s)\n", y_100_odd, get_double_bit_str(y_100_odd) );
		printf( "y_101    =%.15le (%s)\n", y_101, get_double_bit_str(y_101) );
		printf( "y_110    =%.15le (%s)\n", y_110, get_double_bit_str(y_110) );
		printf( "y_111    =%.15le (%s)\n", y_111, get_double_bit_str(y_111) );
		printf( "\n" );
	}

	if ( !(rounding_mode & _MM_ROUND_TOWARD_ZERO) )
	{
		_MM_SET_ROUNDING_MODE( _MM_ROUND_TOWARD_ZERO );
		y_001 = x + x_001;
		y_010 = x + x_010;
		y_011 = x + x_011;
		y_100 = x + x_100;
		y_100_odd = x_odd + x_100; // round to even (if LSB of mantissa is 1 then round up, therefore the LSB becomd 10 (i.e. even 2))
		y_101 = x + x_101;
		y_110 = x + x_110;
		y_111 = x + x_111;
		printf( "* guard round stick bit rounding action table (mode=%s)):\n", "round to 0" );
		printf( "x_GRS    =result\n" );
		printf( "x        =%.15le (%s)\n", x, get_double_bit_str(x) );
		printf( "x_001    =%.15le (%s)\n", x_001, get_double_bit_str(x_001) );
		printf( "x_010    =%.15le (%s)\n", x_010, get_double_bit_str(x_010) );
		printf( "x_011    =%.15le (%s)\n", x_011, get_double_bit_str(x_011) );
		printf( "x_100    =%.15le (%s)\n", x_100, get_double_bit_str(x_100) );
		printf( "x_101    =%.15le (%s)\n", x_101, get_double_bit_str(x_101) );
		printf( "x_110    =%.15le (%s)\n", x_110, get_double_bit_str(x_110) );
		printf( "x_111    =%.15le (%s)\n", x_111, get_double_bit_str(x_111) );
		printf( "y_001    =%.15le (%s)\n", y_001, get_double_bit_str(y_001) );
		printf( "y_010    =%.15le (%s)\n", y_010, get_double_bit_str(y_010) );
		printf( "y_011    =%.15le (%s)\n", y_011, get_double_bit_str(y_011) );
		printf( "y_100    =%.15le (%s)\n", y_100, get_double_bit_str(y_100) );
		printf( "y_100_odd=%.15le (%s)\n", y_100_odd, get_double_bit_str(y_100_odd) );
		printf( "y_101    =%.15le (%s)\n", y_101, get_double_bit_str(y_101) );
		printf( "y_110    =%.15le (%s)\n", y_110, get_double_bit_str(y_110) );
		printf( "y_111    =%.15le (%s)\n", y_111, get_double_bit_str(y_111) );
		printf( "\n" );

		y_001 = -x + -x_001;
		y_010 = -x + -x_010;
		y_011 = -x + -x_011;
		y_100 = -x + -x_100;
		y_100_odd = -x_odd + -x_100; // round to even (if LSB of mantissa is 1 then round up, therefore the LSB becomd 10 (i.e. even 2))
		y_101 = -x + -x_101;
		y_110 = -x + -x_110;
		y_111 = -x + -x_111;
		printf( "* guard round stick bit rounding action table (mode=%s)):\n", "round to 0" );
		printf( "x_GRS    =result\n" );
		printf( "-x       =%.15le (%s)\n", -x, get_double_bit_str(-x) );
		printf( "-x_001   =%.15le (%s)\n", -x_001, get_double_bit_str(-x_001) );
		printf( "-x_010   =%.15le (%s)\n", -x_010, get_double_bit_str(-x_010) );
		printf( "-x_011   =%.15le (%s)\n", -x_011, get_double_bit_str(-x_011) );
		printf( "-x_100   =%.15le (%s)\n", -x_100, get_double_bit_str(-x_100) );
		printf( "-x_101   =%.15le (%s)\n", -x_101, get_double_bit_str(-x_101) );
		printf( "-x_110   =%.15le (%s)\n", -x_110, get_double_bit_str(-x_110) );
		printf( "-x_111   =%.15le (%s)\n", -x_111, get_double_bit_str(-x_111) );
		printf( "y_001    =%.15le (%s)\n", y_001, get_double_bit_str(y_001) );
		printf( "y_010    =%.15le (%s)\n", y_010, get_double_bit_str(y_010) );
		printf( "y_011    =%.15le (%s)\n", y_011, get_double_bit_str(y_011) );
		printf( "y_100    =%.15le (%s)\n", y_100, get_double_bit_str(y_100) );
		printf( "y_100_odd=%.15le (%s)\n", y_100_odd, get_double_bit_str(y_100_odd) );
		printf( "y_101    =%.15le (%s)\n", y_101, get_double_bit_str(y_101) );
		printf( "y_110    =%.15le (%s)\n", y_110, get_double_bit_str(y_110) );
		printf( "y_111    =%.15le (%s)\n", y_111, get_double_bit_str(y_111) );
		printf( "\n" );
	}

	_MM_SET_ROUNDING_MODE( rounding_mode );
}

void add_roundoff_test ()
{
	uint64_t e;
	uint64_t hex;
	double x_1;
	double x_2;
	double y;
	double roundoff;
	double roundoff_exact;
	double eps;

	printf( "===========================================\n" );
	printf( "add roundoff test\n" );
	printf( "===========================================\n" );

	e = -52;
	hex = 0x0000000000000000ULL | ((e + 1023) << 52); 
	eps = *(double *) &hex;
	
	e = 0;
	hex = 0x0000000000000001ULL | ((e + 1023) << 52); 
	x_1 = *(double *) &hex;

	e = 0;
	hex = 0x0000000000000000ULL | ((e + 1023) << 52); 
	x_2 = *(double *) &hex;

	// eps test
	_MM_SET_ROUNDING_MODE( _MM_ROUND_NEAREST );
	y = x_1 + x_2;
	printf( "* epsilon of float number arithmetic:\n" );
	printf( "DBL_EPSILON=%.15le (%s)\n", DBL_EPSILON, get_double_bit_str(DBL_EPSILON) );
	printf( "eps        =%.15le (%s)\n", eps, get_double_bit_str(eps) );
	printf( "x_1        =%.15le (%s)\n", x_1, get_double_bit_str(x_1) );
	printf( "x_2        =%.15le (%s)\n", x_2, get_double_bit_str( x_2) );
	printf( "x_1 + x_2  =%.15le (%s)\n", y, get_double_bit_str(y) );
	printf( "\n" );

	// round off (truncate bit |0xffffff, G=1 R=1 S=1, error=0x1|000000-0x|ffffff=0x|000001)
	e = 10;
	hex = 0x000ffff000000000ULL | ((e + 1023) << 52); 
	x_1 = *(double *) &hex;

	e = -14;
	hex = 0x0000000000ffffffULL | ((e + 1023) << 52); 
	x_2 = *(double *) &hex;

	y = x_1 + x_2;

	e = 10 - 52 - 24;
	hex = (0x8000000000000000ULL) | ((e + 1023) << 52); 
	roundoff = *(double *) &hex;

	// calculate exact roundoff error by gmp
	mpf_t x_1_gmp, x_2_gmp, y_gmp, exact_result_gmp, roundoff_gmp;
	mpf_init2( x_1_gmp, 256 );
	mpf_init2( x_2_gmp, 256 );
	mpf_init2( y_gmp, 256 );
	mpf_init2( exact_result_gmp, 256 );
	mpf_init2( roundoff_gmp, 256 );
	mpf_init_set_d( x_1_gmp, x_1 );
	mpf_init_set_d( x_2_gmp, x_2 );
	mpf_init_set_d( y_gmp, y );
	mpf_add( exact_result_gmp, x_1_gmp, x_2_gmp );
	mpf_sub( roundoff_gmp, exact_result_gmp, y_gmp );
	roundoff_exact = mpf_get_d( roundoff_gmp );
	mpf_clear( x_1_gmp );
	mpf_clear( x_2_gmp );
	mpf_clear( y_gmp );
	mpf_clear( exact_result_gmp );
	mpf_clear( roundoff_gmp );

	// roundoff test
	printf( "* add roundoff error in float number arithmetic (GRS=111):\n" );
	printf( "x_1         =%.15le (%s)\n", x_1, get_double_bit_str(x_1) );
	printf( "x_2         =%.15le (%s)\n", x_2, get_double_bit_str(x_2) );
	printf( "x_1 + x_2   =%.15le (%s)\n", y, get_double_bit_str(y) );
	printf( "roundoff    =%.15le (%s)\n", roundoff, get_double_bit_str(roundoff) );
	printf( "roundoff_gmp=%.15le (%s)\n", roundoff_exact, get_double_bit_str(roundoff_exact) );
	printf( "\n" );
}

void mul_roundoff_test ()
{
	uint64_t e;
	uint64_t hex;
	double x_1;
	double x_2;
	double y;
	double roundoff;
	double roundoff_exact;
	double eps;

	printf( "===========================================\n" );
	printf( "mul roundoff test\n" );
	printf( "===========================================\n" );

	// no round off
	e = 10;
	hex = 0x0000000000001100ULL | ((e + 1023) << 52); 
	x_1 = *(double *) &hex;

	e = -5;
	hex = 0x0000100000000000ULL | ((e + 1023) << 52); 
	x_2 = *(double *) &hex;

	y = x_1 * x_2;

	roundoff = 0;

	// calculate exact roundoff error by gmp
	mpf_t x_1_gmp, x_2_gmp, y_gmp, exact_result_gmp, roundoff_gmp;
	mpf_init2( x_1_gmp, 256 );
	mpf_init2( x_2_gmp, 256 );
	mpf_init2( y_gmp, 256 );
	mpf_init2( exact_result_gmp, 256 );
	mpf_init2( roundoff_gmp, 256 );
	mpf_init_set_d( x_1_gmp, x_1 );
	mpf_init_set_d( x_2_gmp, x_2 );
	mpf_init_set_d( y_gmp, y );
	mpf_mul( exact_result_gmp, x_1_gmp, x_2_gmp );
	mpf_sub( roundoff_gmp, exact_result_gmp, y_gmp );
	roundoff_exact = mpf_get_d( roundoff_gmp );
	mpf_clear( x_1_gmp );
	mpf_clear( x_2_gmp );
	mpf_clear( y_gmp );
	mpf_clear( exact_result_gmp );
	mpf_clear( roundoff_gmp );

	// roundoff test
	printf( "* mul w/o roundoff error in float number arithmetic:\n" );
	printf( "x_1         =%.15le (%s)\n", x_1, get_double_bit_str(x_1) );
	printf( "x_2         =%.15le (%s)\n", x_2, get_double_bit_str(x_2) );
	printf( "x_1 * x_2   =%.15le (%s)\n", y, get_double_bit_str(y) );
	printf( "roundoff    =%.15le (%s)\n", roundoff, get_double_bit_str(roundoff) );
	printf( "roundoff_gmp=%.15le (%s)\n", roundoff_exact, get_double_bit_str(roundoff_exact) );
	printf( "\n" );

	// round off (truncate bit |10001, G=1 R=0 S=1, error=1|00000-|10001=|01111)
	e = 10;
	hex = 0x0000000000001100ULL | ((e + 1023) << 52); 
	x_1 = *(double *) &hex;

	e = -5;
	hex = 0x0000008000000000ULL | ((e + 1023) << 52); 
	x_2 = *(double *) &hex;

	y = x_1 * x_2;

	e = 5 - 52 - 2;
	hex = (0x800e000000000000ULL) | ((e + 1023) << 52); 
	roundoff = *(double *) &hex;

	// calculate exact roundoff error by gmp
	mpf_init2( x_1_gmp, 256 );
	mpf_init2( x_2_gmp, 256 );
	mpf_init2( y_gmp, 256 );
	mpf_init2( exact_result_gmp, 256 );
	mpf_init2( roundoff_gmp, 256 );
	mpf_init_set_d( x_1_gmp, x_1 );
	mpf_init_set_d( x_2_gmp, x_2 );
	mpf_init_set_d( y_gmp, y );
	mpf_mul( exact_result_gmp, x_1_gmp, x_2_gmp );
	mpf_sub( roundoff_gmp, exact_result_gmp, y_gmp );
	roundoff_exact = mpf_get_d( roundoff_gmp );
	mpf_clear( x_1_gmp );
	mpf_clear( x_2_gmp );
	mpf_clear( y_gmp );
	mpf_clear( exact_result_gmp );
	mpf_clear( roundoff_gmp );

	// roundoff test
	printf( "* mul w/ roundoff error in float number arithmetic (GRS=101):\n" );
	printf( "x_1         =%.15le (%s)\n", x_1, get_double_bit_str(x_1) );
	printf( "x_2         =%.15le (%s)\n", x_2, get_double_bit_str(x_2) );
	printf( "x_1 * x_2   =%.15le (%s)\n", y, get_double_bit_str(y) );
	printf( "roundoff    =%.15le (%s)\n", roundoff, get_double_bit_str(roundoff) );
	printf( "roundoff_gmp=%.15le (%s)\n", roundoff_exact, get_double_bit_str(roundoff_exact) );
	printf( "\n" );

	// test internal use 106 bit to store immediate result (truncate bit |10000, G=1 R=0 S=0, error=0|00000-|10000=|10000)
	e = 10;
	hex = 0x0000000000001000ULL | ((e + 1023) << 52); 
	x_1 = *(double *) &hex;

	e = -5;
	hex = 0x0000008000000000ULL | ((e + 1023) << 52); 
	x_2 = *(double *) &hex;

	y = x_1 * x_2;

	e = 5 - 52 - 1;
	hex = (0x0000000000000000ULL) | ((e + 1023) << 52); 
	roundoff = *(double *) &hex;

	// calculate exact roundoff error by gmp
	mpf_init2( x_1_gmp, 256 );
	mpf_init2( x_2_gmp, 256 );
	mpf_init2( y_gmp, 256 );
	mpf_init2( exact_result_gmp, 256 );
	mpf_init2( roundoff_gmp, 256 );
	mpf_init_set_d( x_1_gmp, x_1 );
	mpf_init_set_d( x_2_gmp, x_2 );
	mpf_init_set_d( y_gmp, y );
	mpf_mul( exact_result_gmp, x_1_gmp, x_2_gmp );
	mpf_sub( roundoff_gmp, exact_result_gmp, y_gmp );
	roundoff_exact = mpf_get_d( roundoff_gmp );
	mpf_clear( x_1_gmp );
	mpf_clear( x_2_gmp );
	mpf_clear( y_gmp );
	mpf_clear( exact_result_gmp );
	mpf_clear( roundoff_gmp );

	// roundoff test
	printf( "* mul w/ roundoff error in float number arithmetic (GRS=100):\n" );
	printf( "x_1         =%.15le (%s)\n", x_1, get_double_bit_str(x_1) );
	printf( "x_2         =%.15le (%s)\n", x_2, get_double_bit_str(x_2) );
	printf( "x_1 * x_2   =%.15le (%s)\n", y, get_double_bit_str(y) );
	printf( "roundoff    =%.15le (%s)\n", roundoff, get_double_bit_str(roundoff) );
	printf( "roundoff_gmp=%.15le (%s)\n", roundoff_exact, get_double_bit_str(roundoff_exact) );
	printf( "\n" );

	// test internal use 106 bit to store immediate result (truncate bit 1|10000, G=1 R=0 S=0, error=10|00000-1|10000=|10000)
	e = 10;
	hex = 0x0000000000003000ULL | ((e + 1023) << 52); 
	x_1 = *(double *) &hex;

	e = -5;
	hex = 0x0000008000000000ULL | ((e + 1023) << 52); 
	x_2 = *(double *) &hex;

	y = x_1 * x_2;

	e = 5 - 52 - 1;
	hex = (0x8000000000000000ULL) | ((e + 1023) << 52); 
	roundoff = *(double *) &hex;

	// calculate exact roundoff error by gmp
	mpf_init2( x_1_gmp, 256 );
	mpf_init2( x_2_gmp, 256 );
	mpf_init2( y_gmp, 256 );
	mpf_init2( exact_result_gmp, 256 );
	mpf_init2( roundoff_gmp, 256 );
	mpf_init_set_d( x_1_gmp, x_1 );
	mpf_init_set_d( x_2_gmp, x_2 );
	mpf_init_set_d( y_gmp, y );
	mpf_mul( exact_result_gmp, x_1_gmp, x_2_gmp );
	mpf_sub( roundoff_gmp, exact_result_gmp, y_gmp );
	roundoff_exact = mpf_get_d( roundoff_gmp );
	mpf_clear( x_1_gmp );
	mpf_clear( x_2_gmp );
	mpf_clear( y_gmp );
	mpf_clear( exact_result_gmp );
	mpf_clear( roundoff_gmp );

	// roundoff test
	printf( "* mul w/ roundoff error in float number arithmetic (GRS=100 round to even):\n" );
	printf( "x_1         =%.15le (%s)\n", x_1, get_double_bit_str(x_1) );
	printf( "x_2         =%.15le (%s)\n", x_2, get_double_bit_str(x_2) );
	printf( "x_1 * x_2   =%.15le (%s)\n", y, get_double_bit_str(y) );
	printf( "roundoff    =%.15le (%s)\n", roundoff, get_double_bit_str(roundoff) );
	printf( "roundoff_gmp=%.15le (%s)\n", roundoff_exact, get_double_bit_str(roundoff_exact) );
	printf( "\n" );
}

void div_roundoff_test ()
{
	uint64_t e;
	uint64_t hex;
	double x_1;
	double x_2;
	double y;
	double roundoff;
	double roundoff_exact;
	double eps;

	printf( "===========================================\n" );
	printf( "div roundoff test\n" );
	printf( "===========================================\n" );

	// round off (truncate bit |010101..., G=0 R=1 S=1)
	e = 0;
	hex = 0x0000000000000000ULL | ((e + 1023) << 52); 
	x_1 = *(double *) &hex;

	e = 1;
	hex = 0x0008000000000000ULL | ((e + 1023) << 52); 
	x_2 = *(double *) &hex;

	y = x_1 / x_2;

	e = -56;
	hex = (0x0005555555555555ULL) | ((e + 1023) << 52); 
	roundoff = *(double *) &hex;

	// calculate exact roundoff error by gmp
	mpf_t x_1_gmp, x_2_gmp, y_gmp, exact_result_gmp, roundoff_gmp;
	mpf_init2( x_1_gmp, 256 );
	mpf_init2( x_2_gmp, 256 );
	mpf_init2( y_gmp, 256 );
	mpf_init2( exact_result_gmp, 256 );
	mpf_init2( roundoff_gmp, 256 );
	mpf_init_set_d( x_1_gmp, x_1 );
	mpf_init_set_d( x_2_gmp, x_2 );
	mpf_init_set_d( y_gmp, y );
	mpf_div( exact_result_gmp, x_1_gmp, x_2_gmp );
	mpf_sub( roundoff_gmp, exact_result_gmp, y_gmp );
	roundoff_exact = mpf_get_d( roundoff_gmp );
	mpf_clear( x_1_gmp );
	mpf_clear( x_2_gmp );
	mpf_clear( y_gmp );
	mpf_clear( exact_result_gmp );
	mpf_clear( roundoff_gmp );

	// roundoff test
	printf( "* add roundoff error in float number arithmetic (GRS=011):\n" );
	printf( "x_1         =%.15le (%s)\n", x_1, get_double_bit_str(x_1) );
	printf( "x_2         =%.15le (%s)\n", x_2, get_double_bit_str(x_2) );
	printf( "x_1 / x_2   =%.15le (%s)\n", y, get_double_bit_str(y) );
	printf( "roundoff    =%.15le (%s)\n", roundoff, get_double_bit_str(roundoff) );
	printf( "roundoff_gmp=%.15le (%s)\n", roundoff_exact, get_double_bit_str(roundoff_exact) );
	printf( "\n" );
}


int main ( int argc, char **argv )
{
	denormal_number_test ();

	roundoff_guard_bit_test();

	mul_roundoff_test();

	div_roundoff_test();

	return EXIT_SUCCESS;
}

