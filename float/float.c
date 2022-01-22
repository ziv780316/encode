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
void add_roundoff_test();

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

void add_roundoff_test ()
{
	uint64_t e;
	uint64_t hex;
	double x_1;
	double x_2;
	double y;
	double roundoff;
	double roundoff_gmp;
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
	y = x_1 - x_2;
	printf( "* epsilon of float number arithmetic:\n" );
	printf( "DBL_EPSILON=%.15le (%s)\n", DBL_EPSILON, get_double_bit_str(DBL_EPSILON) );
	printf( "eps        =%.15le (%s)\n", eps, get_double_bit_str(eps) );
	printf( "x_1        =%.15le (%s)\n", x_1, get_double_bit_str(x_1) );
	printf( "x_2        =%.15le (%s)\n", x_2, get_double_bit_str( x_2) );
	printf( "x_1 + x_2  =%.15le (%s)\n", y, get_double_bit_str(y) );
	printf( "\n" );

	// roundoff
	e = 10;
	hex = 0x000ffff000000000ULL | ((e + 1023) << 52); 
	x_1 = *(double *) &hex;

	e = -14;
	hex = 0x000000000000ffffULL | ((e + 1023) << 52); 
	x_2 = *(double *) &hex;

	y = x_1 + x_2;

	e = -14-37;
	hex = 0x000fffe000000000ULL | ((e + 1023) << 52); 
	roundoff = *(double *) &hex;

	// calculate exact roundoff error by gmp
	mpf_t x_1_gmp, x_2_gmp, y_gmp, exact_result_gmp;
	mpf_init2( x_1_gmp, 256 );
	mpf_init2( x_2_gmp, 256 );
	mpf_init2( y_gmp, 256 );
	mpf_init2( exact_result_gmp, 256 );
	mpf_init_set_d( x_1_gmp, x_1 );
	mpf_init_set_d( x_2_gmp, x_2 );
	mpf_init_set_d( y_gmp, y );
	mpf_add( exact_result_gmp, x_1_gmp, x_2_gmp );
	mpf_sub( exact_result_gmp, exact_result_gmp, y_gmp );
	roundoff_gmp = mpf_get_d( exact_result_gmp );
	mpf_clear( x_1_gmp );
	mpf_clear( x_2_gmp );
	mpf_clear( y_gmp );
	mpf_clear( exact_result_gmp );

	// roundoff test
	printf( "* add roundoff error in float number arithmetic:\n" );
	printf( "x_1         =%.15le (%s)\n", x_1, get_double_bit_str(x_1) );
	printf( "x_2         =%.15le (%s)\n", x_2, get_double_bit_str( x_2) );
	printf( "x_1 + x_2   =%.15le (%s)\n", y, get_double_bit_str(y) );
	printf( "roundoff    =%.15le (%s)\n", roundoff, get_double_bit_str(roundoff) );
	printf( "roundoff_gmp=%.15le (%s)\n", roundoff_gmp, get_double_bit_str(roundoff_gmp) );
	printf( "\n" );
}


int main ( int argc, char **argv )
{
	denormal_number_test ();

	add_roundoff_test();

	return EXIT_SUCCESS;
}

