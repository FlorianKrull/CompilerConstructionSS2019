
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <mcc/ast.h>
#include "mcc/symbol_table_parse.h"

// ---------------------------------------------------------- Constants

const int ERROR_COLLECTOR_ERROR_MAX = 10;

// ---------------------------------------------------------- Error

struct mcc_symbol_table_error_collector *mcc_symbol_table_new_error_collector() {
    int error_size = sizeof(struct mcc_parser_error*) * ERROR_COLLECTOR_ERROR_MAX;

    struct mcc_symbol_table_error_collector *ec = malloc(sizeof(*ec) + error_size);

    ec -> error_max = ERROR_COLLECTOR_ERROR_MAX;
    ec -> error_size = 0;

    return ec;
}

void mcc_symbol_table_delete_error_collector(struct mcc_symbol_table_error_collector *ec) {
    assert(ec);

    for (int i = 0; i < ec -> error_size; ec) {
        delete_parser_error(ec -> errors[i]);
    }

    free(ec);
}

struct mcc_parser_error *mcc_symbol_table_new_error(char* msg, struct mcc_ast_source_location *loc) {
    assert(loc);
    assert(msg);

    char *error_msg = (char*) malloc(1024 * sizeof(char));

    sprintf(error_msg, "Semantic Error: \n "
            "%s \n"
            "Location: Line: %d, Column, %d \n", msg, loc -> end_line, loc -> start_col);

    struct mcc_parser_error *error = malloc(sizeof(*error));

    error -> loc = loc;
    error -> error_msg = error_msg;

    return error;
}

int mcc_symbol_table_add_error(struct mcc_symbol_table_error_collector *ec, struct mcc_parser_error *er) {
    assert(ec);
    assert(er);

    int er_size = ec -> error_size;
    int er_max = ec -> error_max;

    if (er_size < er_max) {
        ec -> errors[er_size] = er;

        return 1;
    } else {
        int error_size = sizeof(struct mcc_parser_error*) * ERROR_COLLECTOR_ERROR_MAX;
        ec = realloc(ec, sizeof(*ec) + error_size);

        ec -> errors[er_size] = er;
        ec -> error_max += ERROR_COLLECTOR_ERROR_MAX;

        return 1;
    }
}

// ------------------------------------------------------------ Variable

int mcc_symbool_table_check_variable_assignment(
        struct mcc_ast_assignment *assignment,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec
) {
    struct mcc_symbol *vs =  mcc_symbol_table_get_symbol(symbol_table, assignment -> identifier -> i_value);

    if (vs == NULL) {
        // hold there variable hasn't even be declared you little punk
    } else {
        //fsf
    }
}

int mcc_symbol_table_add_variable_declaration(
        struct mcc_ast_declaration *declaration,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec) {
    assert(declaration);
    assert(symbol_table);
    assert(ec);

    struct mcc_symbol *vs =  mcc_symbol_new_symbol_variable(declaration -> ident -> i_value, declaration -> type);

    // check if already declared
    if (mcc_symbol_table_get_symbol(symbol_table, vs -> variable_name) == NULL) {
        mcc_symbol_table_insert_symbol(symbol_table, vs);
        return 0;
    } else {
        // TODO: more detailed error message
        mcc_symbol_table_add_error(ec, mcc_symbol_table_new_error("Variable already declared", &(declaration -> node.sloc)));
        return 1;
    }
}

// ------------------------------------------------------------ Array

// returns 1 if error
int mcc_symbol_table_check_array_declaration_size(
        struct mcc_ast_declaration *declaration,
        struct mcc_symbol_table_error_collector *ec) {
    if (declaration -> arr_literal != NULL) {
        // check if size is int
        if (declaration -> arr_literal -> type != MCC_AST_DATA_TYPE_INT) {
            mcc_symbol_table_add_error(
             ec,
             mcc_symbol_table_new_error("Array declaration requires integer for array-size defintion", &(declaration -> node.sloc)));

            return 1;
        }
    }

    return 0;
}

int mcc_symbol_table_add_array_declaration(
        struct mcc_ast_declaration *declaration,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec) {
    assert(declaration);
    assert(symbol_table);
    assert(ec);


    if (mcc_symbol_table_check_array_declaration_size(declaration, ec) == 1) {
        return 1;
    }

    struct mcc_symbol *vs =  mcc_symbol_new_symbol_array(
            declaration -> ident -> i_value,
            declaration -> type,
            atoi(declaration -> arr_literal -> value));

    // check if already declared
    if (mcc_symbol_table_get_symbol(symbol_table, vs -> variable_name) == NULL) {

        // add array declaration to symbol table
        mcc_symbol_table_insert_symbol(symbol_table, vs);

        return 0;
    } else {
        // TODO: more detailed error message
        mcc_symbol_table_add_error(ec, mcc_symbol_table_new_error("Array was already declared", &(declaration -> node.sloc)));
        return 1;
    }
}

// ---------------------------------------------------------- Expression

int mcc_symbol_table_validate_expression(
        struct mcc_ast_expression *expression,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec) {
    assert(expression);
    assert(symbol_table);
    assert(ec);

    switch (expression -> type) {
    case MCC_AST_EXPRESSION_TYPE_LITERAL:

    case MCC_AST_EXPRESSION_TYPE_IDENTIFIER:
    case MCC_AST_EXPRESSION_TYPE_CALL_EXPRESSION:
    case MCC_AST_EXPRESSION_TYPE_UNARY_OP:
    case MCC_AST_EXPRESSION_TYPE_BINARY_OP:
    case MCC_AST_EXPRESSION_TYPE_PARENTH:

    case MCC_AST_EXPRESSION_TYPE_BRACKET:

        break;
    }

}

// ---------------------------------------------------------- Statement

int mcc_symbol_table_check_statement(
        struct mcc_ast_statement *statement,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec) {

    assert(statement);
    assert(symbol_table);
    assert(ec);

    switch(statement -> type) {
    case MCC_AST_STATEMENT_TYPE_EXPRESSION:

        break;
    case MCC_AST_STATEMENT_TYPE_DECL:
        if (statement -> declaration -> arr_literal != NULL) {
            // array declaration
            return mcc_symbol_table_add_array_declaration(statement -> declaration, symbol_table, ec);
        } else {
            // variable declaration
            return mcc_symbol_table_add_variable_declaration(statement -> declaration, symbol_table, ec);
        }
    case MCC_AST_STATEMENT_TYPE_ASSGN:

        // check validity of assignment
    case MCC_AST_STATEMENT_TYPE_ASSGN_ARR:
        return mcc_symbol_table_add_array_declaration(statement -> declaration, symbol_table, ec);
    case MCC_AST_STATEMENT_TYPE_COMPOUND:
        // go through statement list and check
        break;
    default:
        // if nothing matches return 0 to continue going through ast
        return 0;
    }
}

// ---------------------------------------------------------- Function

int mcc_symbol_table_add_function_declaration(
        struct mcc_ast_function *func_def,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec) {
    assert(symbol_table);
    assert(ec);
    assert(func_def);

    struct mcc_symbol *fs = mcc_symbol_new_symbol_function(
            func_def -> identifier -> i_value,
            func_def -> return_type,
            func_def -> parameter);

    // check if already declared
    if (mcc_symbol_table_get_symbol(symbol_table, fs -> variable_name) == NULL) {
        // verify semantic of function (validify return type)

        mcc_symbol_table_insert_symbol(symbol_table, fs);

        // create new scope and parse function
        struct mcc_symbol_table *sub_table = mcc_symbol_table_create_inner_table(symbol_table);
        //

    } else {
        // already declared - create already declared error message
    }
}

// ---------------------------------------------------------- Program

int mcc_symbol_table_parse_program(
        struct mcc_ast_program *program,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec) {
    assert(program);

    for (int i = 0; i < program -> size; i++) {
        /**
         * if (mcc_symbol_table_add_function_declaration(...) == 1) return 1;
         */
    }
}

void build_symbol_table(struct mcc_ast_program *program) {
    struct mcc_symbol_table *st = mcc_symbol_table_new_table(NULL);
}