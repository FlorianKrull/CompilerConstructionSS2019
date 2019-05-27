#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <ctype.h>

#include "mcc/ast.h"
#include "mcc/ast_print.h"
#include "mcc/parser.h"
#include "mcc/printer.h"
#include "mcc/mcc_scope.h"

int main(int argc, char *argv[]) {
    char* ast_scope = "program";
    FILE* out = stdout;

    if (argc < 2) {
        mcc_ast_to_dot_print_usage(argv[0]);
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
                mcc_ast_to_dot_print_usage(argv[0]);
                return EXIT_SUCCESS;
            case 'o':
                out = fopen(optarg, "rw");
                break;
            case 'f':
                ast_scope = optarg;
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
        mcc_ast_to_dot_print_usage(argv[0]);
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

    // check if scope should be limited
    if (strcmp("program", ast_scope) == 0) {
        mcc_ast_print_dot(out, prog);
    } else {
        struct mcc_ast_function *func = mcc_limit_scope_to_function(ast_scope, prog);
        mcc_ast_print_dot(out, func);
    }

    // cleanup
    mcc_ast_delete_program(prog);

    return EXIT_SUCCESS;
}
