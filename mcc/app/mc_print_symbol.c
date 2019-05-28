#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mcc/ast.h"
#include "mcc/ast_print.h"
#include "mcc/parser.h"
#include "mcc/symbol_table.h"
#include "mcc/symbol_table_parse.h"


static const char *type_to_string(enum mcc_ast_data_type type){
    switch (type)
    {
    case MCC_AST_DATA_TYPE_INT: return "int";
    case MCC_AST_DATA_TYPE_STRING: return "string";
    case MCC_AST_DATA_TYPE_BOOL: return "bool";
    case MCC_AST_DATA_TYPE_FLOAT: return "float";
    case MCC_AST_DATA_TYPE_VOID : return "void";
    default: return "unknown";
    }
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		print_usage(argv[0], 0);
		return EXIT_FAILURE;
	}

	// determine input source
	FILE *in;
	if ((strcmp("-", argv[1]) == 0)) {
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
        if (result.status != MCC_PARSER_STATUS_OK) {
            // print error message
            printf("%s", result.parser_error -> error_msg);

            return EXIT_FAILURE;
        }
        prog = result.program;
    }

    struct mcc_symbol_table *table = NULL;

    // semantic check 

    {
        struct mcc_symbol_table_error_collector *ec = mcc_symbol_table_new_error_collector();
        struct  mcc_symbol_table *table =   mcc_symbol_table_build(prog,ec);

        if (table != NULL) {
        // proceed with symbol table
            mcc_symbol_table_print(table);
        }

        // cleanup
        mcc_ast_delete(prog);

        // TODO free result
        return EXIT_SUCCESS;

        
    }


}
