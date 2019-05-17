//
// Created by Clemens Paumgarten on 25.04.19.
//

#ifndef MCC_SYMBOL_TABLE_SEMANTIC_ERROR_H
#define MCC_SYMBOL_TABLE_SEMANTIC_ERROR_H

#include "mcc/ast.h"


enum mcc_semantic_error_type {
    MCC_SEMANTIC_ERROR_VARIABLE_ALREADY_DECLARED,
    MCC_SEMANTIC_ERROR_ARRAY_ALREADY_DECLARED,
    MCC_SEMANTIC_ERROR_FUNC_ALREADY_DECLARED,
    MCC_SEMANTIC_ERROR_FUNC_NOT_DECLARED,
    MCC_SEMANTIC_ERROR_WRONG_ARGUMENT_TYPE,
    MCC_SEMANTIC_ERROR_WRONG_NUM_OF_ARGUMENTS,
    MCC_SEMANTIC_ERROR_VARIABLE_NOT_DECLARED,
    MCC_SEMANTIC_ERROR_ARRAY_SIZE_DEFINITION,
    MCC_SEMANTIC_ERROR_TYPE_ASSIGNMENT,
    MCC_SEMANTIC_ERROR_MAIN_MISSINIG,
    MCC_SEMANTIC_ERROR_UNARY_OP_EXPECTED_BOOL,
    MCC_SEMANTIC_ERROR_UNARY_OP_EXPECTED_NUMBER_TYPE,
    MCC_SEMANTIC_ERROR_BINARY_OP_HANDSIDE_SAME_TYPE,
    MCC_SEMANTIC_ERROR_BINARY_OP_HANDSIDE_BOOL_TYPE,
    MCC_SEMANTIC_ERROR_BINARY_OP_HANDSIDE_NUMBER_TYPE,
    MCC_SEMANTIC_ERROR_BINARY_OP_DIV_BY_0
};

struct mcc_semantic_error {
    struct mcc_ast_source_location *sloc;
    enum mcc_semantic_error_type error_type;
};

struct mcc_symbol_table_error_collector {
    int error_size;
    int error_max;
    struct mcc_semantic_error *errors[];
};

struct mcc_symbol_table_error_collector *mcc_symbol_table_new_error_collector();

struct mcc_semantic_error *mcc_symbol_table_new_error(struct mcc_ast_source_location *loc, enum mcc_semantic_error_type type);

void mcc_symbol_table_delete_error_collector(struct mcc_symbol_table_error_collector *ec);

int mcc_symbol_table_add_error(struct mcc_symbol_table_error_collector *ec, struct mcc_semantic_error *er);

#endif //MCC_SYMBOL_TABLE_SEMANTIC_ERROR_H
