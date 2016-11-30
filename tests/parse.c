/*
 * Test for tree allocation
 */

#include <assert.h>
#include <string.h>

#include "../cavatina.h"

static const char *test = 
"[foo]\n"
"\n"
"arg = x\n"
"arg=y\n"
"arg=z\n"
"\n"
"[bar]\n"
"\n"
"awachifu = aśñodg\n"
"\n"
"wallawalla = kajśh\n";

int main(int argc, char *argv[])
{
	struct kv_parser parser;

	kv_init(&parser);
	if (kv_parse(&parser, test) != 0)
		return -1;
	kv_dump(&parser);
	kv_destroy(&parser);
	return 0;
}

