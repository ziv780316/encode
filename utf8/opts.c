#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include "opts.h"


opt_t g_opts = {
	.mode = CONVERTER_NONE,
	.code = 0,
	.debug = false,
};

extern void unit_test ();

void show_help ()
{
	printf( "*------------------------------------*\n" 
		"*           UTF-8 converter          *\n"
		"*------------------------------------*\n" 
		"[Usage] encode unicode to UTF-8\n"
		"utf8 -e <U+****>\n"
		"\n"
		"[Options]\n"
		"  -h  =>  show help\n"
		"  -e <U+****> =>  encode UTF-8\n"
		"  -d <binary> =>  decode to U+****\n"
		"  -t =>  unit test\n"
		"  -s =>  show debug information\n"
		);
}

void parse_cmd_options ( int argc, char **argv )
{
	char *endptr;
	int c;

	while ( true )
	{
		static struct option long_options[] =
		{
			// flag options
			{"help", no_argument, 0, 'h'},
			{"unit_test", no_argument, 0, 't'},
			{"debug", no_argument, 0, 's'},

			// setting options
			{"encode", required_argument, 0, 'e'},
			{"decode", required_argument, 0, 'd'},
			{0, 0, 0, 0}
		};

		// getopt_long stores the option index here
		int option_index = 0;

		c = getopt_long( argc, argv, "hste:d:", long_options, &option_index );

		// detect the end of the options
		if ( -1 == c )
		{
			break;
		}

		switch ( c )
		{
			case 'h':
				show_help();
				exit( EXIT_SUCCESS );
				break;

			case 't':
				g_opts.mode = CONVERTER_UNIT_TEST;
				break;

			case 's':
				g_opts.debug = true;
				break;

			case 'e':
				g_opts.mode = CONVERTER_ENCODE;
				g_opts.code = (unsigned int) strtol( optarg, &endptr, 16 ); // base is hex
				break;

			case 'd':
				g_opts.mode = CONVERTER_DECODE;
				g_opts.code = *(unsigned int *) optarg;
				break;

			case '?':
				/* getopt_long already printed an error message. */
				break;

			default:
				abort ();
				break;
		}
	}

	// print any remaining command line arguments (not options)
	if (optind < argc)
	{
		fprintf( stderr, "[Warning] non-option ARGV-elements: " );
		while ( optind < argc )
		{
			fprintf( stderr, "%s ", argv[optind++] );
		}
		fprintf( stderr, "\n" );
	}

	// check mode
	if ( CONVERTER_NONE == g_opts.mode )
	{
		show_help();
		exit( EXIT_SUCCESS );
	}
}

