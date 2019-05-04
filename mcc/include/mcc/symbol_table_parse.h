//
// Created by Clemens Paumgarten on 24.04.19.
//

#ifndef MCC_SUMBOL_TABLE_BUILD_H
#define MCC_SUMBOL_TABLE_BUILD_H

#include "mcc/parser.h"
#include "mcc/symbol_table.h"
#include "mcc/symbol_table_semantic_error.h"
#include "mcc/symbol_table_validate.h"

// ------------------------------------------------------------ Array

int mcc_symbol_table_check_array_declaration_size(
        struct mcc_ast_declaration *declaration,
        struct mcc_symbol_table_error_collector *ec);

int mcc_symbol_table_add_array_declaration(
        struct mcc_ast_declaration *declaration,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec);


// ------------------------------------------------------------ Variable

int mcc_symbool_table_check_variable_assignment(
        struct mcc_ast_assignment *assignment,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec
);

int mcc_symbol_table_add_variable_declaration(
        struct mcc_ast_declaration *declaration,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec);

// ------------------------------------------------------------ Expression

int mcc_symbol_table_check_expression(
        struct mcc_ast_expression *expression,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec);

// ------------------------------------------------------------ Statement

int mcc_symbol_table_parse_compound_statement(
        struct mcc_ast_statement_list *compound,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec
);

int mcc_symbol_table_check_statement(
        struct mcc_ast_statement *statement,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec);

// ------------------------------------------------------------ Function

int mcc_symbol_table_verify_function_def();

int mcc_symbol_table_add_function_declaration(
        struct mcc_ast_function *func_def,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec);

/**
 * Parses program and containing functions
 * Add declarations and function definitions to symbol table and applies semantic validation on statements and expressions
 * @param program - mcc_ast_program pointer
 * @param symbol_table - mcc_symbol_table pointer
 * @param ec - mcc_symbol_table_error_collector error collector that contains semantic errors
 * @return 1 if semantic check throws an error, 0 if parsing passed successfully
 */
int mcc_symbol_table_parse_program(
        struct mcc_ast_program *program,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec
);

struct mcc_symbol_table *mcc_smybol_table_build(struct mcc_ast_program *program);

#endif //MCC_SUMBOL_TABLE_BUILD_H

