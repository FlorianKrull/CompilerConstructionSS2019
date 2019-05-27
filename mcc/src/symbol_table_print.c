#include <stdio.h>
#include <stdlib.h>
#include<assert.h>

#include "mcc/dynamic_array.h"
#include "mcc/symbol_table_semantic_error.h"
#include "mcc/symbol_table_print.h"

const static char *type_to_string(enum mcc_ast_data_type type){
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

const static char *symbol_type_to_string(enum mcc_symbol_type type){
    switch (type)
    {
    case MCC_SYMBOL_TYPE_VARIABLE: return "variable";
    case MCC_SYMBOL_TYPE_ARRAY: return "array";
    case MCC_SYMBOL_TYPE_FUNCTION: return "function";
    default: return "unknown";
    }
}

void mcc_symbol_table_print(struct mcc_symbol_table *symbol_table, FILE *out){

     for(int i = 0; i < symbol_table->symbol_container->size; i++){
         
        struct mcc_symbol *sym = symbol_table->symbol_container->symbols[i];

        if(sym->symbol_type != MCC_SYMBOL_TYPE_FUNCTION){
            fprintf(out,"\t");
        }
       
        fprintf(out,"%*s | ", 15, symbol_type_to_string(sym->symbol_type));
		fprintf(out,"%*s | ", 6, type_to_string(sym->data_type));

        switch(sym->symbol_type){
            case MCC_SYMBOL_TYPE_VARIABLE: 
                fprintf(out,"%s", sym->variable_name); 
                break;
            case MCC_SYMBOL_TYPE_ARRAY: 
                fprintf(out,"%s[%ld]", sym->variable_name, sym->array_size);
			    break;
            case MCC_SYMBOL_TYPE_FUNCTION:
                if(sym->func_arguments){
                    fprintf(out,"%s(", sym->variable_name);
                    for(int j = 0; j < sym->func_arguments->arg_size; j++){
                        fprintf(out,"%s", type_to_string(sym->func_arguments->arg_types[j]));
                        if (j + 1 < sym->func_arguments->arg_size) {
						fprintf(out,", ");
					    }
                    }
                   fprintf(out,")");
                }else{
				    fprintf(out,"%s()", sym->variable_name);
                }
        }
        fprintf(out,"\n");
           
    }
    for (int i = 0; i < symbol_table->inner_tables_size; i++) {
		    mcc_symbol_table_print(symbol_table->inner_tables[i],out);
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
            case MCC_SEMANTIC_ERROR_INVALID_ARGUMENT_TYPE:
                error_message = "Wrong type of argument\n";
                break;
            case MCC_SEMANTIC_ERROR_INVALID_NUM_OF_ARGUMENTS:
                error_message = "Wrong number of arguments\n";
                break;
            case MCC_SEMANTIC_ERROR_VARIABLE_NOT_DECLARED:
                error_message = "Variable not declared\n";
                break;
            case MCC_SEMANTIC_ERROR_ARRAY_SIZE_DEFINITION:
                error_message = "Array size definition must be an int\n";
                break;
            case MCC_SEMANTIC_ERROR_ARRAY_OPERATIONS:
                error_message = "Operations not allowed on array\n";
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
            case MCC_SEMANTIC_ERROR_INVALID_RETURN_TYPE_IN_NON_VOID_FUNCTION:
                error_message = "Invlaid return type in non void function\n";
                break;
            case MCC_SEMANTIC_ERROR_NO_RETURN_IN_NON_VOID_FUNCTION:
                error_message = "No return in non void function \n";
                break;
        }

        fprintf(out, "Error in line %d and col %d \n", error ->sloc ->end_line, error -> sloc -> end_col);
        fprintf(out, "%s \n", error_message);
    }
}

void mcc_symbol_table_print_type_check_trace(struct mcc_symbol_table_error_collector *ec, FILE *out) {
    assert(ec);

    Dynamic_Array *type_check_arry = ec -> type_tracer;

    if (type_check_arry == NULL) {
        return;
    }

    for (int i = 0; i < type_check_arry -> index; i++) {
        struct type_check *tc = (struct type_check *) type_check_arry -> arr[i];

        int sloc = tc ->sloc ->end_line;
        const char* expected = type_to_string(tc -> target_type);
        const char* recieved = type_to_string(tc -> receiving_type);

        switch (tc ->type) {
            case MCC_SEMANTIC_TYPE_CHECK_RETURN:
                fprintf(out, "Check valid return type @ line %d: Expected: %s - Received: %s \n",
                        sloc, expected, recieved);
                break;
            case MCC_SEMANTIC_TYPE_CHECK_UNARY:
                fprintf(out, "Check valid unary operator type @ line %d: Expected: %s - Received: %s \n",
                        sloc, expected, recieved);
                break;
            case MCC_SEMANTIC_TYPE_CHECK_ARG_TYPE:
                fprintf(out, "Check valid arg type @ line %d: Expected: %s - Received: %s \n",
                        sloc, expected, recieved);
                break;
            case MCC_SEMANTIC_TYPE_CHECK_CONDITION_BOOL:
                fprintf(out, "Check condtion to be boolean @ line %d: Expected: %s - Received: %s \n",
                        sloc, expected, recieved);
                break;
            case MCC_SEMANIC_TYPE_CHECK_BINARY_HANDSIDE_BOTH:
                fprintf(out, "Check binary operator both handsides same type @ line  %d: Expected (lhs): %s - Received (rhs): %s \n",
                        sloc, expected, recieved);
                break;
            case MCC_SEMANIC_TYPE_CHECK_ASSIGNMENT:
                fprintf(out, "Check valid assignment type @ line %d: Expected (lhs): %s - Received (rhs): %s \n",
                        sloc, expected, recieved);
                break;
            default:
                continue;
        }
    }
}
