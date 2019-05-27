
#ifndef MCC_SYMBOL_TABLE_H
#define MCC_SYMBOL_TABLE_H

#include "mcc/dynamic_array.h"
#include "mcc/ast.h"
#include "mcc/symbol_table_semantic_error.h"

// ---------------------------------------------------------- Symbol

enum mcc_symbol_type {
    MCC_SYMBOL_TYPE_VARIABLE,
    MCC_SYMBOL_TYPE_ARRAY,
    MCC_SYMBOL_TYPE_FUNCTION,
    MCC_SYMBOL_TYPE_BUILTIN_FUNCTION
};

struct mcc_symbol_function_argument {
    enum mcc_ast_data_type arg_type;
};

struct mcc_symbol {
    char* variable_name;

    enum mcc_symbol_type symbol_type;

    // variable type or function return type
    enum mcc_ast_data_type data_type;

    union {
        long array_size;
        Dynamic_Array *func_arguments;
    };
};

struct mcc_symbol *mcc_symbol_new_symbol_variable(char* variable_name, enum mcc_ast_data_type data_type);

struct mcc_symbol *mcc_symbol_new_symbol_array(char* variable_name, enum mcc_ast_data_type data_type, long array_size);

struct mcc_symbol *mcc_symbol_new_symbol_function(
        char* variable_name,
        enum mcc_ast_data_type data_type,
        struct mcc_ast_parameter *parameter);

void mcc_symbol_delete_symbol(void* symbol);

// ---------------------------------------------------------- Symbol table

struct mcc_symbol_table {
    struct mcc_symbol_table *parent;
    char *sym_table_name;

    Dynamic_Array *symbols;
    Dynamic_Array *inner_tables;
};

struct mcc_symbol_table *mcc_symbol_table_new_table(struct mcc_symbol_table *parent);

void mcc_symbol_table_delete_table(void *parent);

struct mcc_symbol_table* mcc_symbol_table_create_inner_table(struct mcc_symbol_table *parent, char *name);

int mcc_symbol_table_insert_symbol(struct mcc_symbol_table *table, struct mcc_symbol *symbol);

struct mcc_symbol *mcc_symbol_table_get_symbol(struct mcc_symbol_table *symbol_table, char *symbol_name);


#endif //MCC_SYMBOL_TABLE_H
