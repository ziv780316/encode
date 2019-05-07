#include <stdio.h>
#include <stdlib.h>

/* UTF-8 
 * Pros: 
 * 1. first byte compatible with ASCII (backward compatibility)
 * 2. independent of byte order, so there is no Big-Endian and Little-Endian issue
 * 3. no null-byte, thus allow use null-terminated strings and more backward compatibility
*/
typedef union 
{
	unsigned int hex;
	unsigned char bytes[4];
} codes;

typedef struct
{
	codes utf8_encode;
	codes utf8_decode;
} utf8;

void show_utf8_bits ( codes *code )
{
	// hex
	printf( "hex=%x\n", code->hex );
	printf( "byte1 byte2 byte3 byte4\n" );
	for ( int i = 0; i < 4; ++i )
	{
		printf( "%hhx", (code->bytes[i] >> 4) & 0x0F );
		printf( "%hhx", (code->bytes[i] & 0x0F) );
		printf( " " );
	}
	printf( "\n" );
	
	// bits
	for ( int i = 0; i < 4; ++i )
	{
		for ( int j = 0; j < 8; ++j )
		{
			printf( "%d", ((code->bytes[i] >> (7 - j)) & 0x01) % 2 );
		}
		printf( " " );
	}
	printf( "\n" );
}

void decode_utf8 ( utf8 *data )
{
	data->utf8_decode.hex = 0x00000000; // reset 4-bytes

	if ( (data->utf8_encode.hex >= 0x0000) && (data->utf8_encode.hex <= 0x007F) )
	{
		// 1-bytes, use 7-bits, a.k.a ASCII, 0x0*******
		data->utf8_decode.bytes[0] = (0x7F & data->utf8_encode.hex);
		fwrite( data->utf8_encode.bytes, sizeof(char), 1, stdout );
	}
	else if ( (data->utf8_encode.hex >= 0x0080) && (data->utf8_encode.hex <= 0x07FF) )
	{
		// 2-bytes, use 11-bits, 110***** 10******
		fprintf( stderr, "[Error] not support 2-bytes UTF-8\n" );
		abort();
	}
	else if ( (data->utf8_encode.hex >= 0x0800) && (data->utf8_encode.hex <= 0xFFFF) )
	{
		// 3-bytes, use 16-bits, 1110**** 10****** 10******
		data->utf8_decode.bytes[0] = 0xE0;
		data->utf8_decode.bytes[1] = 0x80;
		data->utf8_decode.bytes[2] = 0x80;
		data->utf8_decode.bytes[0] |= (data->utf8_encode.hex >> 12) & 0x0F;
		data->utf8_decode.bytes[1] |= (data->utf8_encode.hex >> 6) & 0x3F;
		data->utf8_decode.bytes[2] |= (data->utf8_encode.hex) & 0x3F ;
		fwrite( data->utf8_decode.bytes, sizeof(char), 3, stdout );
		fwrite( "\n", sizeof(char), 1, stdout );
		show_utf8_bits ( &(data->utf8_encode) );
		show_utf8_bits ( &(data->utf8_decode) );
	}
	else if ( (data->utf8_encode.hex >= 0x10000) && (data->utf8_encode.hex <= 0x10FFFF) )
	{
		// 4-bytes, use 21-bits, 11110*** 10****** 10****** 10******
		fprintf( stderr, "[Error] not support 4-bytes UTF-8\n" );
		abort();
	}
	else
	{
		fprintf( stderr, "[Error] unknown utf8 code=%x\n", data->utf8_encode.hex );
		abort();
	}
}

int main ( int argc, char **argv )
{
	/*
	 * Type unicode in Linux --> <Ctrl> + <Shift> + u + xxxx(hex unicode)
	 *	
	 * Type unicode in Windows --> 1. unlock numlock 
	 *                             2. <Alt> + <+>(in num keyboard) + xxxx(hex unicode)
	 */
	utf8 data;

	// flat ♭
	data.utf8_encode.hex = 0x0000266d;

	decode_utf8 ( &data );


	return EXIT_SUCCESS;
}

