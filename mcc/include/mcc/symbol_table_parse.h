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

int mcc_symbol_table_parse_expression(
        struct mcc_ast_expression *expression,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec);

// ------------------------------------------------------------ Statement

int mcc_symbol_table_parse_compound_statement(
        struct mcc_ast_statement *compound,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec
);

int mcc_symbol_table_parse_statement(
        struct mcc_ast_statement *statement,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec,
		int create_new);


void mcc_symbol_table_add_builtins(struct mcc_symbol_table *symbol_table);

// ------------------------------------------------------------ Function

int mcc_symbol_table_add_function_declaration(
        struct mcc_ast_function *func_def,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec);

int mcc_symbol_table_parse_function(
        struct mcc_ast_function *func_def,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec
);

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

struct mcc_symbol_table* mcc_symbol_table_build_program(
		struct mcc_ast_program *program,
		struct mcc_symbol_table_error_collector *ec
);

struct mcc_symbol_table* mcc_symbol_table_build_function(
		struct mcc_ast_function *function,
		struct mcc_symbol_table_error_collector *ec
);

// clang-format off

#define mcc_symbol_table_build(x, ec) _Generic((x), \
		struct mcc_ast_function *:   mcc_symbol_table_build_function, \
        struct mcc_ast_program *:    mcc_symbol_table_build_program \
	)(x, ec)

// clang-format on


#endif //MCC_SUMBOL_TABLE_BUILD_H

