#include "mcc/printer.h"

void mcc_ast_to_dot_print_usage(const char *prg)
{
    printf("\n\nusage: %s [OPTIONS] file...\n\n", prg);
    printf("Utility for printing an abstract syntax tree in the DOT format. The output\n"
                   "can be visualised using graphviz. Errors are reported on invalid inputs.\n\n");
    printf("Use '-' as input file to read from stdin\n\n");
    printf("OPTIONS:\n");
    printf("     -h, --help                displays this help message:\n");
    printf("     -o, --output <file>       write the output to <file> (defaults to stdout)\n");
    printf("     -f, --function <name>     limit scope to the given function\n");

}


