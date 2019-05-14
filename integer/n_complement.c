#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

typedef union 
{
	unsigned int hex;
	char bytes[4];
} codes;

void show_codes_bits ( codes *code )
{
	// show bits from MSB to LSB
	for ( int i = 0; i < 4; ++i )
	{
		for ( int j = 0; j < 8; ++j )
		{
			printf( "%d", ((code->bytes[3-i] >> (7 - j)) & 0x01) % 2 );
		}
		printf( " " );
	}
	printf( "\n" );
}

/* n's complement
	f_comp(n, x) = (n^bits) - x
		     = f_comp( n - 1, x ) + 1
 */
void eval_n_complement ( int n, codes *integer )
{
	if ( 1 == n )
	{
		integer->hex = ~(integer->hex);
	}
	else
	{
		eval_n_complement( n - 1, integer );
		integer->hex += 0x1;
	}
}



/* 1's complement 
 * x >= 0 => x
 * x < 0  => f_1_comp(x) = 0xFFFFFFFF - x 
 *
 * pros: the arithmetic only need adder and complement (2's complement is negative of num)
 *
 * cons: there are two zero, +0 (0x00000000) and -0 (0x11111111), need compare two times to ensure zero
*/ 

/* 2's complement 
 * x >= 0 => x
 * x < 0  => f_2_comp(x) = (2^32-1) - x + 1 
 * because x + f_2_comp(x) = x + 2^32-1 -x + 1 = 2^32 = 0 (ignore overflow carrier), thus f_2_comp(x) is -x
 * 
 * pros: 1. the arithmetic only need adder and complement (2's complement is negative of num)
 *       2. only 1 zero (0x00000000)
 * 
 * exception: f(0x10000000) = 0x10000000 = -2^31
*/
void show_2_complement ( codes *integer )
{
	printf( "num=%d\n", integer->hex );
	show_codes_bits( integer );

	codes integer_2_comp = { .hex= ~(integer->hex) + 1  };
	show_codes_bits( &integer_2_comp );
	printf( "num_2_comp=%d\n", integer_2_comp.hex );

}

void unit_test ()
{
	codes integer;
	integer.hex = 0x7FFFFFFF;

	printf( "MAX\n" );
	printf( "num=%d\n", integer.hex );
	show_codes_bits( &integer );
	eval_n_complement( 2, &integer );
	printf( "num_2_comp=%d\n", integer.hex );
	show_codes_bits( &integer );

	integer.hex = 0x80000000;
	printf( "\n-MAX\n" );
	printf( "num=%d\n", integer.hex );
	show_codes_bits( &integer );
	eval_n_complement( 2, &integer );
	printf( "num_2_comp=%d\n", integer.hex );
	show_codes_bits( &integer );

	integer.hex = 0x2;
	printf( "\n2\n" );
	printf( "num=%d\n", integer.hex );
	show_codes_bits( &integer );
	eval_n_complement( 2, &integer );
	printf( "num_2_comp=%d\n", integer.hex );
	show_codes_bits( &integer );
}

int main ( int argc, char **argv )
{
	unit_test ();

	return EXIT_SUCCESS;
}

