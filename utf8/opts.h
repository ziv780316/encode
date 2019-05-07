#ifndef OPTS_H
#define OPTS_H

#include <stdbool.h>

typedef enum 
{
	CONVERTER_NONE,
	CONVERTER_ENCODE,
	CONVERTER_DECODE

} converter_mode;
typedef struct
{
	converter_mode mode;
	unsigned int code;
	bool debug;
} opt_t;

extern opt_t g_opts;

extern void parse_cmd_options ( int argc, char **argv );

#endif
