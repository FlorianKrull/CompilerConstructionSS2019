#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <ctype.h>

#include "mcc/ast.h"
#include "mcc/parser.h"
#include "mcc/symbol_table.h"
#include "mcc/symbol_table_parse.h"
#include "mcc/symbol_table_print.h"
#include "mcc/printer.h"
#include "mcc/mcc_scope.h"

void print_usage(const char *prg) {
    printf("usage: %s <FILE>\n\n", prg);
    printf("  <FILE>  Input filepath or - for stdin\n");
}

int main(int argc, char *argv[]) {
    char* symbol_scope = "program";
    FILE* out = stdout;

    if (argc < 2) {
        mcc_symbol_table_print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    int c;
    static struct option options[] = {
            { "help", no_argument, 0, 'h' },
            { "output", required_argument, 0, 'o' },
            { "function", required_argument, 0, 'f' },
            { 0, 0, 0, 0 }
    };
    while ((c = getopt_long(argc, argv, "hf:o:", options, NULL)) != -1) {
        switch(c) {
            case 'h':
                mcc_symbol_table_print_usage(argv[0]);
                return EXIT_SUCCESS;
            case 'o':
                out = fopen(optarg, "rw");
                break;
            case 'f':
                symbol_scope = optarg;
                break;
            case '?':
                if(isprint(optopt))
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf(stderr,
                            "Unknown option character `\\x%x'.\n",
                            optopt);
                return 1;
            default:
                abort();
        }
    }


    // get input src
    char *input = NULL;
    if (optind < argc) {
        input = argv[optind];
    } else {
        fprintf(stderr, "%s: Missing input!\n", argv[0]);
        mcc_symbol_table_print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    FILE *in;
    if (strcmp("-", input) == 0) {
        in = stdin;
    } else {
        in = fopen(input, "r");
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
    struct mcc_symbol_table *symbol_table;

    // check if scope should be limited
    if (strcmp("program", symbol_scope) == 0) {
        symbol_table = mcc_symbol_table_build(prog, ec);
    } else {
        struct mcc_ast_function *func = mcc_limit_scope_to_function(symbol_scope, prog);
        symbol_table = mcc_symbol_table_build(func, ec);
    }


    if (symbol_table == NULL) {
        mcc_symbol_table_print_error(ec, out);

        return EXIT_FAILURE;
    }

    mcc_symbol_table_print(symbol_table,out);
    mcc_symbol_table_delete_table(symbol_table);
    mcc_ast_delete(prog);

    return EXIT_SUCCESS;
}
