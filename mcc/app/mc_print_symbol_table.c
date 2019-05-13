//
// Created by Clemens Paumgarten on 04.05.19.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mcc/ast.h"
#include "mcc/parser.h"
#include "mcc/symbol_table.h"
#include "mcc/symbol_table_parse.h"

void print_usage(const char *prg) {
    printf("usage: %s <FILE>\n\n", prg);
    printf("  <FILE>  Input filepath or - for stdin\n");
}

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
        // proceed with symbol table
        for(int i = 0; i < symbol_table->symbol_container->size; i++){
            struct mcc_symbol *sym = symbol_table->symbol_container->symbols[i];
            printf("%s ", type_to_string(sym->data_type));
            printf("%s\n" , sym->variable_name);
            if(symbol_table->inner_tables != NULL){
                for(int j = 0; j  < symbol_table->inner_tables_size ; j++){
                    struct mcc_symbol *inner_sym = symbol_table->inner_tables[j];
                    printf("\t %s ",  type_to_string(inner_sym->data_type));
                    printf("\t %s\n" , inner_sym->variable_name);
                }
            }
           
        }
    }

    // cleanup
    mcc_ast_delete(prog);

    // TODO free result

    return EXIT_SUCCESS;
}
