#include <stdio.h>
#include <stdlib.h>

#include "mcc/symbol_table_print.h"



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

void mcc_symbol_table_print(struct mcc_symbol_table *symbol_table){
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

void mcc_symbol_table_print_error(struct mcc_symbol_table_error_collector *ec, FILE *out) {
    // for now only one error in collector
    struct mcc_semantic_error *error = ec -> errors[0];

    if (error != NULL) {
        char *error_message = "\n";
        switch (error ->error_type) {
            case MCC_SEMANTIC_ERROR_VARIABLE_ALREADY_DECLARED:
                error_message = "Variable arleady declared\n";
                break;
            case MCC_SEMANTIC_ERROR_ARRAY_ALREADY_DECLARED:
                error_message = "Array already declared\n";
                break;
            case MCC_SEMANTIC_ERROR_FUNC_ALREADY_DECLARED:
                error_message = "Function already declared\n";
                break;
            case MCC_SEMANTIC_ERROR_FUNC_NOT_DECLARED:
                error_message = "Function not declared\n";
                break;
            case MCC_SEMANTIC_ERROR_WRONG_ARGUMENT_TYPE:
                error_message = "Wrong type of argument\n";
                break;
            case MCC_SEMANTIC_ERROR_WRONG_NUM_OF_ARGUMENTS:
                error_message = "Wrong number of arguments\n";
                break;
            case MCC_SEMANTIC_ERROR_VARIABLE_NOT_DECLARED:
                error_message = "Variable not declared\n";
                break;
            case MCC_SEMANTIC_ERROR_ARRAY_SIZE_DEFINITION:
                error_message = "Array size definition must be an int\n";
                break;
            case MCC_SEMANTIC_ERROR_TYPE_ASSIGNMENT:
                error_message = "Wrong type assigned\n";
                break;
            case MCC_SEMANTIC_ERROR_MAIN_MISSING:
                error_message = "Main missing\n";
                break;
            case MCC_SEMANTIC_ERROR_CONDITION_BOOL_EXPECTED:
                error_message = "Condition must result in a bool\n";
                break;
            case MCC_SEMANTIC_ERROR_UNARY_OP_EXPECTED_BOOL:
                error_message = "Boolean expected\n";
                break;
            case MCC_SEMANTIC_ERROR_UNARY_OP_EXPECTED_NUMBER:
                error_message = "Number type expected\n";
                break;
            case MCC_SEMANTIC_ERROR_BINARY_OP_HANDSIDE_SAME_TYPE:
                error_message = "Both parts of binary operator must be of same type\n";
                break;
            case MCC_SEMANTIC_ERROR_BINARY_OP_HANDSIDE_BOOL_TYPE:
                error_message = "Bool expected in binary operator\n";
                break;
            case MCC_SEMANTIC_ERROR_BINARY_OP_HANDSIDE_NUMBER_TYPE:
                error_message = "Number type expected in binary operator\n";
                break;
            case MCC_SEMANTIC_ERROR_BINARY_OP_DIV_BY_0:
                error_message = "Division by 0 not allowed\n";
                break;
        }

        fprintf(out, "Error in line %d and col %d \n", error ->sloc ->end_line, error -> sloc -> end_col);
        fprintf(out, error_message);
    }

};
