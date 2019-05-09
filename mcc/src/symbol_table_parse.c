
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "mcc/ast.h"
#include "mcc/symbol_table_parse.h"
#include "mcc/symbol_table_semantic_error.h"
#include "utils/unused.h"

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
            atoi(declaration->arr_literal->i_value));

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
        case MCC_AST_EXPRESSION_TYPE_IDENTIFIER:
            return mcc_symbol_table_validate_validate_identifier(expression -> identifier, symbol_table, true, ec);
        case MCC_AST_EXPRESSION_TYPE_CALL_EXPRESSION:
            return mcc_symbol_table_validate_validate_call_expression(
                    expression,
                    symbol_table,
                    true,
                    ec
            );
        case MCC_AST_EXPRESSION_TYPE_UNARY_OP:
        case MCC_AST_EXPRESSION_TYPE_BINARY_OP:

        case MCC_AST_EXPRESSION_TYPE_PARENTH:
        case MCC_AST_EXPRESSION_TYPE_BRACKET:
        default:
            return 0;

    }
}

// ---------------------------------------------------------- Statement

int mcc_symbol_table_parse_compound_statement(
        struct mcc_ast_statement_list *compound,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec
) {
    assert(compound);
    assert(symbol_table);
    assert(ec);

    // create it's own inner scope
    struct mcc_symbol_table *sub_table = mcc_symbol_table_create_inner_table(symbol_table);

    do {
        struct mcc_ast_statement *st = compound->statement;
        int statement_result = mcc_symbol_table_check_statement(
                st,
                sub_table,
                ec
        );

        if(statement_result == 1) {
            return 1;
        }

        compound = compound->next;
    } while(compound->next != NULL);

    return 0;
}

int mcc_symbol_table_check_statement(
        struct mcc_ast_statement *statement,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec) {

    assert(statement);
    assert(symbol_table);
    assert(ec);

    switch(statement->type) {
        case MCC_AST_STATEMENT_TYPE_EXPRESSION:
            return mcc_symbol_table_check_expression(statement->expression, symbol_table, ec);
        case MCC_AST_STATEMENT_TYPE_WHILE:
            if(mcc_symbol_table_check_expression(
                    statement->while_condition, symbol_table, ec)) {
                return mcc_symbol_table_check_statement(
                        statement->while_stmt, symbol_table, ec);
            }
            break;
        case MCC_AST_STATEMENT_TYPE_IF:
            if(mcc_symbol_table_check_expression(
                    statement->if_condition, symbol_table, ec)) {
                return mcc_symbol_table_check_statement(
                        statement->if_stmt, symbol_table, ec);
            }
            break;
        case MCC_AST_STATEMENT_TYPE_DECL:
            // TODO check for shadowing
            if(mcc_symbol_table_get_symbol(symbol_table, statement->id_decl->i_value) != NULL) {
                mcc_symbol_table_add_error(ec, mcc_symbol_table_new_error(&(statement->node.sloc),
                                                                          MCC_SEMANTIC_ERROR_VARIABLE_ALREADY_DECLARED));
            }
            if (statement->declaration->arr_literal != NULL) {
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
            // TODO create new symbol table as child
            return mcc_symbol_table_parse_compound_statement(
                    statement->statement_list,
                    symbol_table,
                    ec
            );
        default:
            return 0;
    }

    return 0;
}

// ---------------------------------------------------------- Function

// TODO: break up function into smaller pieces
int mcc_symbol_table_add_function_declaration(
        struct mcc_ast_function *func_def,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec) {
    assert(symbol_table);
    assert(ec);
    assert(func_def);

    printf("%d \n", func_def -> return_type);
    printf("mcc_symbol_table_add_function_declaration \n");

    struct mcc_symbol *fs = mcc_symbol_new_symbol_function(
            func_def->identifier->i_value,
            func_def->return_type,
            func_def->parameter);

    // check if already declared
    if(mcc_symbol_table_get_symbol(symbol_table, fs -> variable_name) == NULL) {
        // verify semantic of function (validate return type)
        if(func_def->return_type != MCC_AST_DATA_TYPE_VOID) {
            // mcc_symbol_table_validate_statement_return
        }

        mcc_symbol_table_insert_symbol(symbol_table, fs);

        // create new scope and parse function
        struct mcc_symbol_table *sub_table = mcc_symbol_table_create_inner_table(symbol_table);

        // add params to sub table
        if (func_def -> parameter != NULL && func_def -> parameter -> size > 0) {
            struct mcc_ast_parameter *p = func_def -> parameter;

            for (int i = 0; i < p -> size; i++) {
                struct mcc_ast_declaration *declaration = p -> parameters[i];

                // at this point symbol table is still empty -> adding won't result in an error
                if (declaration -> arr_literal != NULL) {
                    // array declaration
                    mcc_symbol_table_add_array_declaration(declaration, sub_table, ec);
                } else {
                    // variable declaration
                    mcc_symbol_table_add_variable_declaration(declaration, sub_table, ec);
                }
            }
        }

        // add compound statement to symbol table
        return mcc_symbol_table_check_statement(func_def -> statement, symbol_table, ec);
    } else {
        // already declared - create already declared error message
        mcc_symbol_table_add_error(ec, mcc_symbol_table_new_error(&(func_def->node.sloc),
                                                                  MCC_SEMANTIC_ERROR_FUNC_ALREADY_DECLARED));
        return 1;
    }
}

// ---------------------------------------------------------- Program

int mcc_symbol_table_parse_program(
        struct mcc_ast_program *program,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec) {
    
    UNUSED(symbol_table);
    assert(program);

    printf("mcc_symbol_table_parse_program \n");

    for(int i = 0; i < program->size; i++) {
        mcc_symbol_table_add_function_declaration(program->function_def[i], symbol_table, ec);
    }

    return 0;
}

struct mcc_symbol_table *mcc_symbol_table_build(struct mcc_ast_program *program,struct mcc_symbol_table_error_collector *ec) {
    assert(program);
    struct mcc_symbol_table *st = mcc_symbol_table_new_table(NULL,ec);
    struct mcc_symbol_table_error_collector *ec = mcc_symbol_table_new_error_collector();

    // TODO: insert built-ins
    if (mcc_symbol_table_parse_program(program, st, ec) == 0) {
        printf("Symbol table created successfully \n");
        return st;
    } else {
        // handle error collection
        printf("Symbol table not created due to errors \n");
        return NULL;
    }
}
