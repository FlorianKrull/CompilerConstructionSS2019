
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "mcc/ast.h"
#include "mcc/symbol_table_parse.h"
#include "mcc/symbol_table_semantic_error.h"
#include "utils/unused.h"

// ------------------------------------------------------------ Variable

int mcc_symbol_table_add_variable_declaration(
        struct mcc_ast_declaration *declaration,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec) {
    assert(declaration);
    assert(symbol_table);
    assert(ec);

    struct mcc_symbol *vs = mcc_symbol_new_symbol_variable(declaration->ident->i_value, declaration->type);

    // check if already declared
    if(mcc_symbol_table_get_symbol(symbol_table, vs->variable_name) == NULL) {
        mcc_symbol_table_insert_symbol(symbol_table, vs);
        return 0;
    } else {
        mcc_symbol_table_add_error(ec, mcc_symbol_table_new_error(&(declaration->node.sloc),
                                                                  MCC_SEMANTIC_ERROR_VARIABLE_ALREADY_DECLARED));
        return 1;
    }
}

// ------------------------------------------------------------ Array

// returns 1 if error
int mcc_symbol_table_check_array_declaration_size(
        struct mcc_ast_declaration *declaration,
        struct mcc_symbol_table_error_collector *ec) {
    if(declaration->arr_literal != NULL) {
        // check if size is int
        if(declaration->arr_literal->type != MCC_AST_DATA_TYPE_INT) {
            mcc_symbol_table_add_error(
                    ec,
                    mcc_symbol_table_new_error(&(declaration->node.sloc), MCC_SEMANTIC_ERROR_ARRAY_SIZE_DEFINITION));

            return 1;
        }
    }

    return 0;
}

int mcc_symbol_table_add_array_declaration(
        struct mcc_ast_declaration *declaration,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec) {
    assert(declaration);
    assert(symbol_table);
    assert(ec);


    if(mcc_symbol_table_check_array_declaration_size(declaration, ec) == 1) {
        return 1;
    }

    struct mcc_symbol *vs = mcc_symbol_new_symbol_array(
            declaration->ident->i_value,
            declaration->type,
            declaration->arr_literal->i_value);

    // check if already declared
    if(mcc_symbol_table_get_symbol(symbol_table, vs->variable_name) == NULL) {

        // add array declaration to symbol table
        mcc_symbol_table_insert_symbol(symbol_table, vs);

        return 0;
    } else {
        mcc_symbol_table_add_error(ec, mcc_symbol_table_new_error(&(declaration->node.sloc),
                                                                  MCC_SEMANTIC_ERROR_ARRAY_ALREADY_DECLARED));
        return 1;
    }
}

// ---------------------------------------------------------- Statement

int mcc_symbol_table_parse_compound_statement(
        struct mcc_ast_statement *statement,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec
) {
    assert(statement);
    assert(statement->type == MCC_AST_STATEMENT_TYPE_COMPOUND);
    assert(symbol_table);
    assert(ec);

    // create it's own inner scope
    struct mcc_symbol_table *sub_table = mcc_symbol_table_create_inner_table(symbol_table);

    struct mcc_ast_statement_list *stl = statement->statement_list;

    if(stl == NULL){
        return 0;
    }

    while(stl->statement != NULL ) {
       
        int statement_result = mcc_symbol_table_parse_statement(
                stl->statement,
                sub_table,
                ec
        );

        if(statement_result == 1) {
            return 1;
        }
        if(stl->next == NULL){
            break;
        }else{
            stl = stl->next;
        }
       
    } 

    return 0;
}

int mcc_symbol_table_parse_statement(
        struct mcc_ast_statement *statement,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec) {

    assert(statement);
    assert(symbol_table);
    assert(ec);

    switch(statement->type) {
        case MCC_AST_STATEMENT_TYPE_EXPRESSION:
            return mcc_symbol_table_validate_expression(statement->expression, symbol_table, ec);
        case MCC_AST_STATEMENT_TYPE_WHILE:
            if(mcc_symbol_table_validate_expression(
                    statement->while_condition, symbol_table, ec) == 0) {
                if (mcc_symbol_table_validate_condition_to_type_bool(
                        statement -> while_condition,
                        symbol_table,
                        ec
                ) == 0) {
                    return mcc_symbol_table_parse_statement(
                            statement->while_stmt, symbol_table, ec);
                } else {
                    return 1;
                }
            } else {
                return 1;
            }
        case MCC_AST_STATEMENT_TYPE_IF:
            if(mcc_symbol_table_validate_expression(
                    statement->if_condition, symbol_table, ec) == 0) {
                if (mcc_symbol_table_validate_condition_to_type_bool(
                        statement -> if_condition,
                        symbol_table,
                        ec
                ) == 0) {
                    return mcc_symbol_table_parse_statement(
                            statement->if_stmt, symbol_table, ec);
                } else {
                    return 1;
                }
            } else {
                return 1;
            }
        case MCC_AST_STATEMENT_TYPE_DECL:
            // TODO extract to separate function
            if(mcc_symbol_table_get_symbol(symbol_table, statement->declaration->ident->i_value) != NULL) {
                mcc_symbol_table_add_error(ec, mcc_symbol_table_new_error(&(statement->node.sloc),
                                                                          MCC_SEMANTIC_ERROR_VARIABLE_ALREADY_DECLARED));
            }
            if (statement->declaration->arr_literal != NULL) {
                // array declaration
                return mcc_symbol_table_add_array_declaration(statement->declaration, symbol_table, ec);
            } else {
                // variable declaration
                return mcc_symbol_table_add_variable_declaration(statement->declaration, symbol_table, ec);
            }
        case MCC_AST_STATEMENT_TYPE_ASSGN:
            return mcc_symbol_table_validate_assignemt_semantic(statement->assignment, symbol_table, ec);
        case MCC_AST_STATEMENT_TYPE_ASSGN_ARR:
            return 0;
        case MCC_AST_STATEMENT_TYPE_COMPOUND:
            return mcc_symbol_table_parse_compound_statement(
                    statement,
                    symbol_table,
                    ec
            );
        default:
            return 0;
    }
}

// ---------------------------------------------------------- Function

int mcc_symbol_table_parse_function(
        struct mcc_ast_function *func_def,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec
) {
    assert(func_def);
    assert(symbol_table);
    assert(ec);

    // verify semantic of function (validate return type)
    if(func_def->return_type != MCC_AST_DATA_TYPE_VOID) {
        // mcc_symbol_table_validate_statement_return
        // TODO validate function return
    }

    // create new scope and parse function
    struct mcc_symbol_table *sub_table = mcc_symbol_table_create_inner_table(symbol_table);

    // add params to sub table
    if (func_def -> parameter != NULL && func_def -> parameter -> size > 0) {
        struct mcc_ast_parameter *p = func_def -> parameter;

        for (int i = 0; i < p -> size; i++) {
            struct mcc_ast_declaration *declaration = p -> parameters[i];

            // at this point symbol table is still empty -> adding won't result in an error
            if (declaration -> arr_literal != NULL) {
                // array declaration
                mcc_symbol_table_add_array_declaration(declaration, sub_table, ec);
            } else {
                // variable declaration
                mcc_symbol_table_add_variable_declaration(declaration, sub_table, ec);
            }
        }
    }

    return mcc_symbol_table_parse_statement(func_def -> statement, sub_table, ec);
}

// TODO: break up function into smaller pieces
int mcc_symbol_table_add_function_declaration(
        struct mcc_ast_function *func_def,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec) {
    assert(symbol_table);
    assert(ec);
    assert(func_def);

    struct mcc_symbol *fs = mcc_symbol_new_symbol_function(
            func_def->identifier->i_value,
            func_def->return_type,
            func_def->parameter);

    // check if already declared
    if(mcc_symbol_table_get_symbol(symbol_table, fs -> variable_name) == NULL) {
        mcc_symbol_table_insert_symbol(symbol_table, fs);
        return 0;
    } else {
        // already declared - create already declared error message
        mcc_symbol_table_add_error(ec, mcc_symbol_table_new_error(&(func_def->node.sloc),
                                                                  MCC_SEMANTIC_ERROR_FUNC_ALREADY_DECLARED));
        return 1;
    }
}

// ---------------------------------------------------------- Program

void add_builtin_function(struct mcc_symbol_table *symbol_table,char *variable_name, enum mcc_ast_data_type return_type, enum mcc_ast_data_type param_type){

   struct mcc_symbol *symbol = malloc(sizeof(*symbol));

    symbol -> variable_name = variable_name;
    symbol -> data_type = return_type;
    symbol -> symbol_type = MCC_SYMBOL_TYPE_FUNCTION;

    struct mcc_symbol_function_arguments *fp  = malloc(sizeof(*fp) + sizeof(enum mcc_ast_data_type*) * 4);

    if(param_type == MCC_AST_DATA_TYPE_VOID){
        fp -> arg_size = 0;
        fp -> arg_types[0] = param_type;
    }else{
        fp -> arg_size = 1;
        fp -> arg_types[0] = param_type;
    }
    symbol -> func_arguments = fp;

    mcc_symbol_table_insert_symbol(symbol_table,symbol);

}

void mcc_symbol_table_add_builtins(struct mcc_symbol_table *symbol_table) {
    add_builtin_function(symbol_table,"print_nl",MCC_AST_DATA_TYPE_VOID,MCC_AST_DATA_TYPE_VOID);
    add_builtin_function(symbol_table,"print",MCC_AST_DATA_TYPE_VOID,MCC_AST_DATA_TYPE_STRING);
    add_builtin_function(symbol_table,"print_int",MCC_AST_DATA_TYPE_VOID,MCC_AST_DATA_TYPE_INT);
    add_builtin_function(symbol_table,"print_float",MCC_AST_DATA_TYPE_VOID,MCC_AST_DATA_TYPE_FLOAT);
    add_builtin_function(symbol_table,"read_int",MCC_AST_DATA_TYPE_INT,MCC_AST_DATA_TYPE_VOID);
    add_builtin_function(symbol_table,"read_float",MCC_AST_DATA_TYPE_FLOAT,MCC_AST_DATA_TYPE_VOID);
};

int mcc_symbol_table_parse_program(
        struct mcc_ast_program *program,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec) {
    
    UNUSED(symbol_table);
    assert(program);

    printf("mcc_symbol_table_parse_program \n");

    int function_parse = 0;

    for(int i = 0; i < program->size; i++) {
        function_parse = mcc_symbol_table_add_function_declaration(program->function_def[i], symbol_table, ec);

        if (function_parse) break;
    }

    // if everything so far ok
    if (function_parse == 0) {

        // parse all functions
        for(int i = 0; i < program->size; i++) {
            function_parse = mcc_symbol_table_parse_function(program->function_def[i], symbol_table, ec);

            if (function_parse) break;
        }
    }

    // check if main exists
    if (function_parse == 0) {
        function_parse = mcc_symbol_table_validate_main(program, symbol_table, ec);
    }

    return function_parse;
}

struct mcc_symbol_table *mcc_symbol_table_build(struct mcc_ast_program *program,struct mcc_symbol_table_error_collector *ec) {
    assert(program);

    struct mcc_symbol_table *st = mcc_symbol_table_new_table(NULL,ec);

    mcc_symbol_table_add_builtins(st);

    if (mcc_symbol_table_parse_program(program, st, ec) == 0) {
        printf("Symbol table created successfully \n");
        return st;
    } else {
        // handle error collection
        printf("Symbol table not created due to errors \n");
        return NULL;
    }
}
