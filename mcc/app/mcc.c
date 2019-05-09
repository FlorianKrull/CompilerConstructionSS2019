#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mcc/ast.h"
#include "mcc/parser.h"

void print_usage(const char *prg)
{
	printf("\n\nusage: %s [OPTIONS] file...s\n\n", prg);
    printf("The mC compiler. It takes mC input files and produces an executable.\n\n");
	printf("<FILE> Input filepath or - for stdin\n\n");
}


int main(int argc, char *argv[])
{
	if (argc < 2) {
		print_usage(argv[0]);
		return EXIT_FAILURE;
	}

	// determine input source
	FILE *in;
	if (strcmp("-", argv[1]) == 0) {
		in = stdin;
	} else {
		in = fopen(argv[1], "r");
		if (!in) {
			perror("fopen");
			return EXIT_FAILURE;
		}
	}

	struct mcc_ast_program *prog = NULL;

	// parsing phase
	{
		struct mcc_parser_result result = mcc_parse_file(in);
		fclose(in);
		if (result.status != MCC_PARSER_STATUS_OK) {
			 printf("NOT OK");
			return EXIT_FAILURE;
		}
		prog = result.program;
	}

	// TODO:
	// - run semantic checks
	{
		
	}


	// - create three-address code
	// - output assembly code
	// - invoke backend compiler

	// cleanup
	mcc_ast_delete_program(prog);

	return EXIT_SUCCESS;
}
