
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <mcc/ast.h>
#include "mcc/symbol_table_parse.h"

// ------------------------------------------------------------ Variable

int mcc_symbol_table_add_variable_declaration(
        struct mcc_ast_declaration *declaration,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec) {
    assert(declaration);
    assert(symbol_table);
    assert(ec);

    struct mcc_symbol *vs = mcc_symbol_new_symbol_variable(declaration->ident->i_value, declaration->type);

    // check if already declared
    if(mcc_symbol_table_get_symbol(symbol_table, vs->variable_name) == NULL) {
        mcc_symbol_table_insert_symbol(symbol_table, vs);
        return 0;
    } else {
        mcc_symbol_table_add_error(ec, mcc_symbol_table_new_error(&(declaration->node.sloc),
                                                                  MCC_SEMANTIC_ERROR_VARIABLE_NOT_DECLARED));
        return 1;
    }
}

// ------------------------------------------------------------ Array

// returns 1 if error
int mcc_symbol_table_check_array_declaration_size(
        struct mcc_ast_declaration *declaration,
        struct mcc_symbol_table_error_collector *ec) {
    if(declaration->arr_literal != NULL) {
        // check if size is int
        if(declaration->arr_literal->type != MCC_AST_DATA_TYPE_INT) {
            mcc_symbol_table_add_error(
                    ec,
                    mcc_symbol_table_new_error(&(declaration->node.sloc), MCC_SEMANTIC_ERROR_ARRAY_SIZE_DEFINITION));

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


    if(mcc_symbol_table_check_array_declaration_size(declaration, ec) == 1) {
        return 1;
    }

    struct mcc_symbol *vs = mcc_symbol_new_symbol_array(
            declaration->ident->i_value,
            declaration->type,
            atoi(declaration->arr_literal->value));

    // check if already declared
    if(mcc_symbol_table_get_symbol(symbol_table, vs->variable_name) == NULL) {

        // add array declaration to symbol table
        mcc_symbol_table_insert_symbol(symbol_table, vs);

        return 0;
    } else {
        mcc_symbol_table_add_error(ec, mcc_symbol_table_new_error(&(declaration->node.sloc),
                                                                  MCC_SEMANTIC_ERROR_ARRAY_ALREADY_DECLARED));
        return 1;
    }
}

// ---------------------------------------------------------- Expression

int mcc_symbol_table_check_expression(
        struct mcc_ast_expression *expression,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec
) {
    assert(expression);
    assert(symbol_table);
    assert(ec);

    switch(expression->type) {
        case MCC_AST_EXPRESSION_TYPE_CALL_EXPRESSION:
            // TODO check for unknown function (remember built-ins)
            // TODO validate return type of call expression
        default:
            return 0;

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

    switch(statement->type) {
        case MCC_AST_STATEMENT_TYPE_EXPRESSION:
            return 0;
        case MCC_AST_STATEMENT_TYPE_WHILE:
        case MCC_AST_STATEMENT_TYPE_IF:
            // TODO check for bool result
        case MCC_AST_STATEMENT_TYPE_DECL:
            // TODO check for shadowing
            if(statement->declaration->arr_literal != NULL) {
                // array declaration
                return mcc_symbol_table_add_array_declaration(statement->declaration, symbol_table, ec);
            } else {
                // variable declaration
                return mcc_symbol_table_add_variable_declaration(statement->declaration, symbol_table, ec);
            }
        case MCC_AST_STATEMENT_TYPE_ASSGN:
            return mcc_symbol_table_validate_assignemt_semantic(statement->assignment, symbol_table, ec);
        case MCC_AST_STATEMENT_TYPE_ASSGN_ARR:
            // Array validation
        case MCC_AST_STATEMENT_TYPE_COMPOUND:
            return mcc_symbol_table_parse_compound_statement(
                    statement -> compound_statement,
                    statement -> compound_size,
                    symbol_table,
                    ec
            );
        default:
            // if nothing matches return 0 to continue going through ast
            return 0;
    }
}

int mcc_symbol_table_parse_compound_statement(
        struct mcc_ast_statement *compound[],
        int statement_list_size,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec
) {
    assert(compound);
    assert(symbol_table);
    assert(ec);

    // create it's own inner scope
    struct mcc_symbol_table *sub_table = mcc_symbol_table_create_inner_table(symbol_table);

    for (int i = 0; i < statement_list_size; i++) {

        // Cast should not be necessary
        struct mcc_ast_statement *st = compound[i];

        int statement_result = mcc_symbol_table_check_statement(
                st,
                sub_table,
                ec
        );


        if (statement_result == 1) {
            return 1;
        }

    }

    return 0;
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
            func_def->identifier->i_value,
            func_def->return_type,
            func_def->parameter);

    // check if already declared
    if(mcc_symbol_table_get_symbol(symbol_table, fs->variable_name) == NULL) {
        // verify semantic of function (validify return type)

        mcc_symbol_table_insert_symbol(symbol_table, fs);

        // create new scope and parse function
        // struct mcc_symbol_table *sub_table = mcc_symbol_table_create_inner_table(symbol_table);
        //

    } else {
        // already declared - create already declared error message
    }

    return 0;
}

// ---------------------------------------------------------- Program

int mcc_symbol_table_parse_program(
        struct mcc_ast_program *program,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec) {
    assert(program);
    assert(symbol_table);
    assert(ec);

    for(int i = 0; i < program->size; i++) {
        /**
         * if (mcc_symbol_table_add_function_declaration(...) == 1) return 1;
         */
    }

    return 0;
}

void build_symbol_table(struct mcc_ast_program *program) {
    struct mcc_symbol_table *st = mcc_symbol_table_new_table(NULL);
}
