#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef union 
{
	double num;
	unsigned long long hex;
	char bytes[8];
} codes;

typedef struct
{
	codes code;
	char bits[64]; // from bits[0]=MSB bits[63]=LSB
	char *sign;
	char *exponent;
	char *fraction;
} ieee754;

void show_codes_bits ( codes *code )
{
	// bits
	for ( int i = 0; i < 8; ++i )
	{
		for ( int j = 0; j < 8; ++j )
		{
			printf( "%d", ((code->bytes[7-i] >> (7 - j)) & 0x01) % 2 );
		}
		printf( " " );
	}
	printf( "\n" );
}

void show_ieee_754 ( ieee754 *float_num )
{
	printf( "num=%.16e\n", float_num->code.num );
	printf( "%-4s  %-11s  %-52s\n", "sign", "exponent", "fraction" );
	printf( "%-4d  ", *(float_num->sign) );
	for ( int i = 0; i < 11; ++i )
	{
		printf( "%d", float_num->exponent[i] );
	}
	printf( "  " );
	for ( int i = 0; i < 52; ++i )
	{
		printf( "%d", float_num->fraction[i] );
	}
	printf( "\n" );
}

void init_ieee_754 ( ieee754 *float_num )
{
	for ( int i = 0; i < 8; ++i )
	{
		for ( int j = 0; j < 8; ++j )
		{
			float_num->bits[i*8 + j] = ((float_num->code.bytes[7-i] >> (7 - j)) & 0x01) % 2;
		}
	}

	float_num->sign = &(float_num->bits[0]); // 1-bit
	float_num->exponent = &(float_num->bits[1]); // 11-bits
	float_num->fraction = &(float_num->bits[12]); // 52-bits

	show_ieee_754( float_num );
}

bool exact_equal ( ieee754 *x, ieee754 *y )
{
	return x->code.num == y->code.num;
}

int main ( int argc, char **argv )
{
	long long eps = 1;
	ieee754 float1 = { .code.num = 1 };
	ieee754 float2 = { .code.num = 1 };
	float2.code.hex |= eps;

	init_ieee_754( &float1 );
	init_ieee_754( &float2 );

	bool is_equal;
	is_equal = exact_equal( &float1, &float2 );
	printf( "%.16e %s %.16e\n", float1.code.num, (is_equal ? "==" : "!="), float2.code.num );

	return EXIT_SUCCESS;
}

