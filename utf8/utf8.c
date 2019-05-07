#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "opts.h"

/* UTF-8 
 *
 * UTF-8 can express 2^21 characters in UTF-8 system, thus cover all characters in unicode
 *
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
	int use_n_byte;
	codes unicode;
	codes utf8_code;
} db;

void show_utf8_bits ( codes *code )
{
	// hex
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

void decode_utf8 ( db *data )
{
	data->utf8_code.hex = 0x00000000; // reset 4-bytes

	if ( (data->unicode.hex >= 0x0000) && (data->unicode.hex <= 0x007F) )
	{
		// 1-bytes, use 7-bits, a.k.a ASCII, 0x0*******
		data->use_n_byte = 1;
		data->utf8_code.bytes[0] = (0x7F & data->unicode.hex);
	}
	else if ( (data->unicode.hex >= 0x0080) && (data->unicode.hex <= 0x07FF) )
	{
		// 2-bytes, use 11-bits, 110***** 10******
		data->use_n_byte = 2;
		data->utf8_code.bytes[0] = 0xc0;
		data->utf8_code.bytes[1] = 0x80;
		data->utf8_code.bytes[0] |= (data->unicode.hex >> 6) & 0x1F;
		data->utf8_code.bytes[1] |= (data->unicode.hex) & 0x3F ;
	}
	else if ( (data->unicode.hex >= 0x0800) && (data->unicode.hex <= 0xFFFF) )
	{
		// 3-bytes, use 16-bits, 1110**** 10****** 10******
		data->use_n_byte = 3;
		data->utf8_code.bytes[0] = 0xE0;
		data->utf8_code.bytes[1] = 0x80;
		data->utf8_code.bytes[2] = 0x80;
		data->utf8_code.bytes[0] |= (data->unicode.hex >> 12) & 0x0F;
		data->utf8_code.bytes[1] |= (data->unicode.hex >> 6) & 0x3F;
		data->utf8_code.bytes[2] |= (data->unicode.hex) & 0x3F ;
	}
	else if ( (data->unicode.hex >= 0x10000) && (data->unicode.hex <= 0x10FFFF) )
	{
		// 4-bytes, use 21-bits, 11110*** 10****** 10****** 10******
		data->use_n_byte = 4;
		data->utf8_code.bytes[0] = 0xF0;
		data->utf8_code.bytes[1] = 0x80;
		data->utf8_code.bytes[2] = 0x80;
		data->utf8_code.bytes[3] = 0x80;
		data->utf8_code.bytes[0] |= (data->unicode.hex >> 18) & 0x07;
		data->utf8_code.bytes[1] |= (data->unicode.hex >> 12) & 0x3F;
		data->utf8_code.bytes[2] |= (data->unicode.hex >> 6) & 0x3F;
		data->utf8_code.bytes[3] |= (data->unicode.hex) & 0x3F;
	}
	else
	{
		fprintf( stderr, "[Error] cannot convert unicode code=%x\n", data->unicode.hex );
		abort();
	}
}

void unit_test ()
{
	/*
	 * Type unicode in Linux --> <Ctrl> + <Shift> + u + xxxx(hex unicode)
	 *	
	 * Type unicode in Windows --> 1. unlock numlock 
	 *                             2. <Alt> + <+>(in num keyboard) + xxxx(hex unicode)
	 */
	db data;

	// flat ♭
	data.unicode.hex = 0x266d;

	printf( "unicode=U+%x\n\n", data.unicode.hex );
	show_utf8_bits ( &(data.unicode) );
	printf( "\n" );

	decode_utf8 ( &data );

	show_utf8_bits ( &(data.utf8_code) );
	printf( "decode result = " );
	fwrite( data.utf8_code.bytes, sizeof(char), data.use_n_byte, stdout );
	fwrite( "\n", sizeof(char), 1, stdout );
}

void convert_unicode_to_utf8 ( unsigned unicode )
{
	db data;

	data.unicode.hex = unicode;

	decode_utf8 ( &data );

	if ( g_opts.debug )
	{
		printf( "unicode=U+%x\n\n", data.unicode.hex );
		show_utf8_bits ( &(data.unicode) );
		printf( "\n" );
		show_utf8_bits ( &(data.utf8_code) );
		printf( "\ndecode result (%d bytes) = ", data.use_n_byte );
	}

	fwrite( data.utf8_code.bytes, sizeof(char), data.use_n_byte, stdout );
	fwrite( "\n", sizeof(char), 1, stdout );
}

int main ( int argc, char **argv )
{
	parse_cmd_options ( argc, argv );

	if ( CONVERTER_ENCODE == g_opts.mode )
	{
		convert_unicode_to_utf8( g_opts.code );
	}
	else if ( CONVERTER_DECODE == g_opts.mode )
	{
	}

	return EXIT_SUCCESS;
}

