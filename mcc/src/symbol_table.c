#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "mcc/symbol_table.h"

// ---------------------------------------------------------- CONSTANTS

const int MCC_SYMBOL_TABLE_SYMBOL_SIZE = 100;
const int MCC_SYMBOL_TABLE_CHILDREN_SIZE = 5;
const int MCC_FUNC_ARG_SIZE = 5;

// ---------------------------------------------------------- Symbol

struct mcc_symbol *mcc_symbol_new_symbol_variable(char* variable_name, enum mcc_ast_data_type data_type) {
    assert(variable_name);

    struct mcc_symbol *symbol = malloc(sizeof(*symbol));

    symbol -> variable_name = variable_name;
    symbol -> data_type = data_type;
    symbol -> symbol_type = MCC_SYMBOL_TYPE_VARIABLE;

    return symbol;
}

struct mcc_symbol *mcc_symbol_new_symbol_array(char* variable_name, enum mcc_ast_data_type data_type, long array_size) {
    assert(variable_name);

    struct mcc_symbol *symbol = malloc(sizeof(*symbol));

    symbol -> variable_name = variable_name;
    symbol -> data_type = data_type;
    symbol -> array_size = array_size;

    return symbol;
}


struct mcc_symbol *mcc_symbol_new_symbol_function(
        char* variable_name,
        enum mcc_ast_data_type data_type,
        struct mcc_ast_parameter *parameter) {
    assert(variable_name);

    struct mcc_symbol *symbol = malloc(sizeof(*symbol));

    symbol -> variable_name = variable_name;
    symbol -> data_type = data_type;
    symbol -> symbol_type = MCC_SYMBOL_TYPE_FUNCTION;

    symbol -> func_arguments = mcc_create_dynamic_array(MCC_FUNC_ARG_SIZE);

    if (parameter != NULL) {
        for (int i = 0; i < parameter -> size; i++) {
            struct mcc_symbol_function_argument *fp = malloc(sizeof(*fp));
            fp -> arg_type = parameter -> parameters[i] -> type;
            mcc_add_to_array(symbol -> func_arguments, fp);
        }
    }

    return symbol;
}

void mcc_symbol_delete_symbol(void *symbol) {
    struct mcc_symbol *s = (struct mcc_symbol *) symbol;

    if (s != NULL) {
        if (s -> func_arguments) {
            mcc_delete_array(s ->func_arguments, NULL);
        }
    }
}

// ---------------------------------------------------------- Symbol Table

struct mcc_symbol_table *mcc_symbol_table_new_table(
        struct mcc_symbol_table *parent) {

    struct mcc_symbol_table *table = malloc(sizeof(*table));

    table ->inner_tables = mcc_create_dynamic_array(MCC_SYMBOL_TABLE_CHILDREN_SIZE);
    table -> symbols = mcc_create_dynamic_array(MCC_SYMBOL_TABLE_SYMBOL_SIZE);
    table -> parent = parent;

    return table;
}

void mcc_symbol_table_delete_table(void *table) {
    struct mcc_symbol_table *st = (struct mcc_symbol_table *) table;
    // Delete symbols
    assert(table);

    mcc_delete_array(st -> symbols, mcc_symbol_delete_symbol);
    mcc_delete_array(st ->inner_tables, mcc_symbol_table_delete_table);
}

struct mcc_symbol_table *mcc_symbol_table_create_inner_table(struct mcc_symbol_table *parent, char *name) {
    assert(parent);

    char *symbole_name;
    symbole_name = name;

    if(symbole_name == NULL){
        symbole_name = parent->sym_table_name;
    }

    struct mcc_symbol_table *child = mcc_symbol_table_new_table(parent);

    child->sym_table_name = symbole_name;

    mcc_add_to_array(parent -> inner_tables, child);

    return child;
}

int mcc_symbol_table_insert_symbol(struct mcc_symbol_table *table, struct mcc_symbol *symbol) {
    assert(table);
    assert(symbol);

    return mcc_add_to_array(table -> symbols, symbol);

}

struct mcc_symbol *mcc_symbol_table_get_symbol(struct mcc_symbol_table *symbol_table, char *symbol_name) {
    for (int i = 0; i < symbol_table -> symbols -> size; i++) {
        struct mcc_symbol *s = symbol_table -> symbols -> arr[i];

        if (strcmp(s -> variable_name, symbol_name) == 0) {
            return s;
        }
    }

    // not found in current symbol table
    if (symbol_table -> parent != NULL) {
        struct mcc_symbol *s = mcc_symbol_table_get_symbol(symbol_table->parent, symbol_name);

        return s;
    }

    return NULL;
}
