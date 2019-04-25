//
// Created by Clemens Paumgarten on 24.04.19.
//

#ifndef MCC_SUMBOL_TABLE_BUILD_H
#define MCC_SUMBOL_TABLE_BUILD_H

#include "mcc/parser.h";
#include "mcc/symbol_table.h";

struct mcc_symbol_table_error_collector {
    int error_size;
    int error_max;
    struct mcc_parser_error *errors[];
};

struct mcc_symbol_table_error_collector *mcc_symbol_table_new_error_collector();

void  mcc_symbol_table_delete_error_collector(struct mcc_symbol_table_error_collector *ec);

int mcc_symbol_table_add_error(struct mcc_symbol_table_error_collector *ec, struct mcc_parser_error *er);

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
)

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

int mcc_symbol_table_parse_program(
        struct mcc_ast_program *program,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec
);

void mcc_symbol_table_build(struct mcc_ast_program *program);

#endif //MCC_SUMBOL_TABLE_BUILD_H

