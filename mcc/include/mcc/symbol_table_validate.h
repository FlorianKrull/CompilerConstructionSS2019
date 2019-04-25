//
// Created by Clemens Paumgarten on 25.04.19.
//

#ifndef MCC_SYMBOL_TABLE_VALIDATE_H
#define MCC_SYMBOL_TABLE_VALIDATE_H

#include "mcc/ast.h"
#include "mcc/symbol_table.h"
#include "mcc/symbol_table_parse.h"

// --------------------------------------- Expression

/**
 * Checks if expression contains identifier or function calls and validates if they have already been declared
 * @return 1 if error, 0 if ok
 */
int mcc_symbol_table_validate_expression_existence();

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

#endif //MCC_SYMBOL_TABLE_VALIDATE_H

