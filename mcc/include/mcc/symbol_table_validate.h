//
// Created by Clemens Paumgarten on 25.04.19.
//

#ifndef MCC_SYMBOL_TABLE_VALIDATE_H
#define MCC_SYMBOL_TABLE_VALIDATE_H

#include "mcc/ast.h"
#include "mcc/symbol_table.h"
#include "mcc/symbol_table_semantic_error.h"

// --------------------------------------- Expression

/**
 * Validate identifier (check if exists in symbol table
 * @param expression
 * @param symbol_table
 * @return enum mcc_ast_data_type
 * @return 1 if error, 0 if ok
 */
int mcc_symbol_table_validate_validate_identifier(
        struct mcc_ast_identifier *identifier,
        struct mcc_symbol_table *symbol_table,
        bool with_shadowing,
        struct mcc_symbol_table_error_collector *ec
        );


int mcc_symbol_table_validate_validate_call_expression(
        struct mcc_ast_expression *expression,
        struct mcc_symbol_table *symbol_table,
        bool with_shadowing,
        struct mcc_symbol_table_error_collector *ec
);


/**
 * Returns binary-op expression return type
 * Binary-op expression validation has to be done before
 * @param expression
 * @param symbol_table
 * @return enum mcc_ast_data_type
 */
enum mcc_ast_data_type mcc_symbol_table_get_expression_return_type_binary_op(
        struct mcc_ast_expression *expression,
        struct mcc_symbol_table *symbol_table
);

/**
 * Returns expression return type
 * Expression semantic validation has to be done before
 * @param expression
 * @param symbol_table
 * @return enum mcc_ast_data_type
 */
enum mcc_ast_data_type mcc_symbol_table_get_expression_return_type(
        struct mcc_ast_expression *expression,
        struct mcc_symbol_table *symbol_table
);

/**
 * Checks if an expression outcome matches the expected value
 * Semantic checks must be done before hand (e.g. binary operator string + integer)
 * @param expression - given expression
 * @param type - expected value
 * @return 1 if error , 0 if ok
 */
int mcc_symbol_table_validate_expression_return_type(
        struct mcc_ast_expression *expression,
        struct mcc_symbol_table *symbol_table,
        enum mcc_ast_data_type type
);

/**
 * Checks if binary operator is semantically correct
 */
int mcc_symbol_table_validate_binary_operator(
        struct mcc_ast_expression *expression,
        struct mcc_symbol_table *symbol_table
);

/**
 * Checks if an expression is semantically correct (identifier exist, binary op between string + inteeger, etc)
 * @param expression
 * @param symbol_table
 * @return 1 if error , 0 if ok
 */

int mcc_symbol_table_validate_expression_semantic(
        struct mcc_ast_expression *expression,
        struct mcc_symbol_table *symbol_table
);

// --------------------------------------- Assignment

int mcc_symbol_table_validate_assignemt_semantic(
        struct mcc_ast_assignment *assignment,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec
);

// --------------------------------------- Statement

int mcc_symbol_table_validate_statement_return(
        struct mcc_ast_statement *statement,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec
);

// --------------------------------------- Program

int mcc_symbol_table_validate_main(
        struct mcc_ast_program *program,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec
);

#endif //MCC_SYMBOL_TABLE_VALIDATE_H

