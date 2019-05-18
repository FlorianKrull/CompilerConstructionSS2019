#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mcc/ast.h"
#include "mcc/parser.h"
#include "mcc/symbol_table.h"
#include "mcc/symbol_table_parse.h"
#include "mcc/symbol_table_print.h"

void print_usage(const char *prg) {
    printf("usage: %s <FILE>\n\n", prg);
    printf("  <FILE>  Input filepath or - for stdin\n");
}

int main(int argc, char *argv[]) {
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
        if (result.status != MCC_PARSER_STATUS_OK) {
            // print error message
            printf("%s", result.parser_error -> error_msg);

            return EXIT_FAILURE;
        }
        prog = result.program;
    }

    // create symbol table
    struct mcc_symbol_table_error_collector *ec =  mcc_symbol_table_new_error_collector(); 
    struct mcc_symbol_table *symbol_table = mcc_symbol_table_build(prog,ec);

   
    if (symbol_table != NULL) {
        mcc_symbol_table_print(symbol_table);
    } else {
        mcc_symbol_table_print_error(ec, stdout);
    }

    // cleanup
    mcc_ast_delete(prog);

    // TODO free result

    return EXIT_SUCCESS;
}
