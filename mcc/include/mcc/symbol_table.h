
#ifndef MCC_SYMBOL_TABLE_H
#define MCC_SYMBOL_TABLE_H

#include "mcc/ast.h"

// ---------------------------------------------------------- Symbol

enum mcc_symbol_type {
    MCC_SYMBOL_TYPE_VARIABLE,
    MCC_SYMBOL_TYPE_ARRAY,
    MCC_SYMBOL_TYPE_FUNCTION
};

struct mcc_symbol_function_arguments {
    int arg_size;
    int arg_max;
    enum mcc_ast_data_type arg_types[];
};

struct mcc_symbol {
    char* variable_name;

    enum mcc_symbol_type symbol_type;

    // variable type or function return type
    enum mcc_ast_data_type data_type;

    union {
        long array_size;
        struct mcc_symbol_function_arguments *func_arguments;
    };
};

struct mcc_symbol *mcc_symbol_new_symbol_variable(char* variable_name, enum mcc_ast_data_type data_type);

struct mcc_symbol *mcc_symbol_new_symbol_array(char* variable_name, enum mcc_ast_data_type data_type, long array_size);

struct mcc_symbol *mcc_symbol_new_symbol_function(
        char* variable_name,
        enum mcc_ast_data_type data_type,
        struct mcc_ast_parameter *parameter);

void mcc_symbol_delete_symbol(struct mcc_symbol *symbol);

// ---------------------------------------------------------- Symbol table

// Symbol table implemented as a linked list

struct mcc_symbol_table_symbol_container {
    int size;
    int max;
    struct mcc_symbol *symbols[];
};

struct mcc_symbol_table {
    // use double-pointer for this as flexible arrays have to be at the end of a struct
    struct mcc_symbol_table_symbol_container *symbol_container;
    struct mcc_symbol_table_error_collector *ec;
    // every symbol table has one parent and can have multiple inner_tables
    struct mcc_symbol_table *parent;

    int inner_tables_size;
    int inner_tables_max;
    struct mcc_symbol_table *inner_tables[];
};

struct mcc_symbol_table *mcc_symbol_table_new_table(struct mcc_symbol_table *parent, struct mcc_symbol_table_error_collector *ec);

void mcc_symbol_table_delete_table(struct mcc_symbol_table *parent);

struct mcc_symbol_table* mcc_symbol_table_create_inner_table(struct mcc_symbol_table *parent);

int mcc_symbol_table_insert_symbol(struct mcc_symbol_table *table, struct mcc_symbol *symbol);

struct mcc_symbol *mcc_symbol_table_get_symbol(struct mcc_symbol_table *symbol_table, char *symbol_name);

#endif //MCC_SYMBOL_TABLE_H
