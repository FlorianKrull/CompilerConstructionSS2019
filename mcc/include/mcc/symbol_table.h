
#ifndef MCC_SYMBOL_TABLE_H
#define MCC_SYMBOL_TABLE_H

#include "mcc/ast.h"

// ---------------------------------------------------------- Symbol

enum mcc_symbol_type {
    MCC_SYMBOL_TYPE_VARIABLE,
    MCC_SYMBOL_TYPE_ARRAY,
    MCC_SYMBOL_TYPE_FUNCTION
};

struct mcc_symbol_function_parameters {
    int params_size;
    enum mcc_ast_data_type *params_types;
};

struct mcc_symbol {
    char* variable_name;

    enum mcc_symbol_type symbol_type;

    // variable type or function return type
    enum mcc_ast_data_type data_type;

    union {
        int array_size;
        struct mcc_symbol_function_parameters *func_params;
    };
};

struct mcc_symbol *mcc_symbol_new_symbol_variable(char* variable_name, enum mcc_ast_data_type data_type);

struct mcc_symbol *mcc_symbol_new_symbol_array(char* variable_name, enum mcc_ast_data_type data_type, int array_size);

struct mcc_symbol *mcc_symbol_new_symbol_function(
        char* variable_name,
        enum mcc_ast_data_type data_type,
        struct mcc_ast_parameter *parameter);

void mcc_symbol_delete_symbol(struct mcc_symbol *symbol);

// ---------------------------------------------------------- Symbol table

// Symbol table implemented as a linked list

struct mcc_symbol_table {
    // use double-pointer for this as flexible arrays have to be at the end of a struct
    int symbols_size;
    int symbols_max;
    struct mcc_symbol **symbols;

    // every symbol table has one parent and can have multiple inner_tables
    struct mcc_symbol_table *parent;

    int inner_tables_size;
    int inner_tables_max;
    struct mcc_symbol_table *inner_tables[];
};

struct mcc_symbol_table *mcc_symbol_table_new_table(struct mcc_symbol_table *parent);

void mcc_symbol_table_delete_table(struct mcc_symbol_table *parent);

struct mcc_symbol_table* mcc_symbol_table_create_inner_table(struct mcc_symbol_table *parent);

int mcc_symbol_table_insert_symbol(struct mcc_symbol_table *table, struct mcc_symbol *symbol);

struct mcc_symbol *mcc_symbol_table_get_symbol(struct mcc_symbol_table *symbol_table, char *symbol_name);

#endif //MCC_SYMBOL_TABLE_H
