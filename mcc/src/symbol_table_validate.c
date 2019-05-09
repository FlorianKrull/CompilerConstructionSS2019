//
// Created by Clemens Paumgarten on 25.04.19.
//

#include <stdlib.h>
#include <assert.h>
#include <mcc/ast.h>
#include "mcc/symbol_table_validate.h"
#include "utils/unused.h"

// --------------------------------------- Expression

int mcc_symbol_table_validate_validate_identifier(
        struct mcc_ast_identifier *identifier,
        struct mcc_symbol_table *symbol_table,
        bool with_shadowing,
        struct mcc_symbol_table_error_collector *ec
) {
    assert(identifier);
    assert(symbol_table);
    assert(ec);

    struct mcc_symbol *s = mcc_symbol_table_get_symbol(symbol_table, identifier -> i_value);

    // identifier does not exist
    if (s == NULL) {
        mcc_symbol_table_add_error(
                ec,
                mcc_symbol_table_new_error(&(identifier ->node.sloc), MCC_SEMANTIC_ERROR_VARIABLE_NOT_DECLARED));

        return 1;
    }

    return 0;
}

int mcc_symbol_table_validate_validate_call_expression(
        struct mcc_ast_expression *expression,
        struct mcc_symbol_table *symbol_table,
        bool with_shadowing,
        struct mcc_symbol_table_error_collector *ec
) {
    assert(expression);
    assert(symbol_table);
    assert(ec);

    struct mcc_symbol *s = mcc_symbol_table_get_symbol(symbol_table, expression -> function_name -> i_value);

    // function not declared
    if (s == NULL) {
        mcc_symbol_table_add_error(
                ec,
                mcc_symbol_table_new_error(&(expression -> node.sloc), MCC_SEMANTIC_ERROR_FUNC_NOT_DECLARED));

        return 1;
    } else {
        struct mcc_ast_argument *argument = expression -> argument;
        struct mcc_symbol_function_arguments *func_args = s -> func_arguments;

        if (argument -> size != func_args -> arg_size) {
            mcc_symbol_table_add_error(
                    ec,
                    mcc_symbol_table_new_error(&(expression -> node.sloc), MCC_SEMANTIC_ERROR_WRONG_NUM_OF_ARGUMENTS));

            return 1;
        }

        for (int i = 0; i < func_args -> arg_size; i++) {
            enum mcc_ast_data_type func_arg_type = func_args -> arg_types[i];
            enum mcc_ast_data_type arg_typ = mcc_symbol_table_get_expression_return_type(argument -> expressions[i], symbol_table);

            // wrong type passed as an argument
            if (func_arg_type != arg_typ) {
                mcc_symbol_table_add_error(
                        ec,
                        mcc_symbol_table_new_error(&(expression -> node.sloc), MCC_SEMANTIC_ERROR_WRONG_ARGUMENT_TYPE));

                return 1;
            }
        }

        return 0;
    }
}

enum mcc_ast_data_type mcc_symbol_table_get_expression_return_type_binary_op(
        struct mcc_ast_expression *expression,
        struct mcc_symbol_table *symbol_table
) {

    enum mcc_ast_data_type lhs_type = mcc_symbol_table_get_expression_return_type(expression->lhs, symbol_table);
    enum mcc_ast_data_type rhs_type = mcc_symbol_table_get_expression_return_type(expression->rhs, symbol_table);

    // TODO maybe do it better - idk how
    switch(expression->op) {
        case MCC_AST_BINARY_OP_ADD:
        case MCC_AST_BINARY_OP_SUB:
        case MCC_AST_BINARY_OP_MUL:
        case MCC_AST_BINARY_OP_LESS:
        case MCC_AST_BINARY_OP_LESS_EQUALS:
        case MCC_AST_BINARY_OP_GREATER:
        case MCC_AST_BINARY_OP_GREATER_EQUALS:
        case MCC_AST_BINARY_OP_DIV:
            if(lhs_type == MCC_AST_DATA_TYPE_INT && rhs_type == MCC_AST_DATA_TYPE_INT) {
                return MCC_AST_DATA_TYPE_INT;
            } else if(lhs_type == MCC_AST_DATA_TYPE_FLOAT && rhs_type == MCC_AST_DATA_TYPE_INT) {
                return MCC_AST_DATA_TYPE_FLOAT;
            } else if(lhs_type == MCC_AST_DATA_TYPE_INT && rhs_type == MCC_AST_DATA_TYPE_FLOAT) {
                return MCC_AST_DATA_TYPE_FLOAT;
            }else{
                return MCC_AST_DATA_TYPE_INT;
            }
        case MCC_AST_BINARY_OP_EQUALS:
        case MCC_AST_BINARY_OP_NOT_EQUALS:
        case MCC_AST_BINARY_OP_AND:
        case MCC_AST_BINARY_OP_OR:
            return MCC_AST_DATA_TYPE_BOOL;

    }
}

enum mcc_ast_data_type mcc_symbol_table_get_expression_return_type(
        struct mcc_ast_expression *expression,
        struct mcc_symbol_table *symbol_table
) {
    switch(expression->type) {
        case MCC_AST_EXPRESSION_TYPE_LITERAL:
            return expression->literal->type;
        case MCC_AST_EXPRESSION_TYPE_IDENTIFIER:
            return mcc_symbol_table_get_symbol(symbol_table, expression->identifier->i_value)->data_type;
        case MCC_AST_EXPRESSION_TYPE_CALL_EXPRESSION:
            return mcc_symbol_table_get_symbol(symbol_table, expression->function_name->i_value)->data_type;
        case MCC_AST_EXPRESSION_TYPE_UNARY_OP:
        case MCC_AST_EXPRESSION_TYPE_BINARY_OP:
            return mcc_symbol_table_get_expression_return_type_binary_op(expression, symbol_table);
        case MCC_AST_EXPRESSION_TYPE_PARENTH:
            return mcc_symbol_table_get_expression_return_type(expression->expression, symbol_table);
        case MCC_AST_EXPRESSION_TYPE_BRACKET:
            // array index ? - should that alway be INT?
            return MCC_AST_DATA_TYPE_INT;
    }
}

int mcc_symbol_table_validate_expression_return_type(
        struct mcc_ast_expression *expression,
        struct mcc_symbol_table *symbol_table,
        enum mcc_ast_data_type type
) {
    return mcc_symbol_table_get_expression_return_type(expression, symbol_table) == type ? 0 : 1;
}

// both handsides of same type
int mcc_symbol_table_validate_binary_operator_handside_equals_type_check(
        struct mcc_ast_expression *expression,
        struct mcc_symbol_table *symbol_table
) {
    if(mcc_symbol_table_get_expression_return_type(expression->lhs, symbol_table) ==
       mcc_symbol_table_get_expression_return_type(expression->rhs, symbol_table)) {
        return 0;
    } else {
        return 1;
    }
}

// both handsides of number type
int mcc_symbol_table_validate_binary_operator_handside_number_type_check(
        struct mcc_ast_expression *expression,
        struct mcc_symbol_table *symbol_table
) {
    if(((mcc_symbol_table_get_expression_return_type(expression->lhs, symbol_table) == MCC_AST_DATA_TYPE_INT) ||
        (mcc_symbol_table_get_expression_return_type(expression->lhs, symbol_table) == MCC_AST_DATA_TYPE_FLOAT))
       &&
       ((mcc_symbol_table_get_expression_return_type(expression->rhs, symbol_table) == MCC_AST_DATA_TYPE_INT) ||
        (mcc_symbol_table_get_expression_return_type(expression->rhs, symbol_table) == MCC_AST_DATA_TYPE_FLOAT))) {
        // both sides are number types
        return 0;
    } else {
        return 1;
    }
}

// both handsides of bool type
int mcc_symbol_table_validate_binary_operator_handside_bool_type_check(
        struct mcc_ast_expression *expression,
        struct mcc_symbol_table *symbol_table
) {
    if((mcc_symbol_table_get_expression_return_type(expression->lhs, symbol_table) == MCC_AST_DATA_TYPE_BOOL) &&
        (mcc_symbol_table_get_expression_return_type(expression->rhs, symbol_table) == MCC_AST_DATA_TYPE_BOOL)) {
        // both sides are bool types
        return 0;
    } else {
        return 1;
    }
}


int mcc_symbol_table_validate_binary_operator(
        struct mcc_ast_expression *expression,
        struct mcc_symbol_table *symbol_table
) {

    // check if both handsides exist (or have any other errors
    if(mcc_symbol_table_validate_expression_semantic(expression->lhs, symbol_table) == 1 ||
       mcc_symbol_table_validate_expression_semantic(expression->rhs, symbol_table) == 1) {
        // i wi
        return 1;
    }

    // check if both side exist in symbol table and check types
    // TODO maybe do it better - idk how
    switch(expression->op) {
        case MCC_AST_BINARY_OP_ADD:
        case MCC_AST_BINARY_OP_SUB:
        case MCC_AST_BINARY_OP_MUL:
        case MCC_AST_BINARY_OP_LESS:
        case MCC_AST_BINARY_OP_LESS_EQUALS:
        case MCC_AST_BINARY_OP_GREATER:
        case MCC_AST_BINARY_OP_GREATER_EQUALS:
            return mcc_symbol_table_validate_binary_operator_handside_number_type_check(expression, symbol_table);

        case MCC_AST_BINARY_OP_DIV:
            // TODO: checck for devicion by zero right mustnt be 0
        case MCC_AST_BINARY_OP_EQUALS:
        case MCC_AST_BINARY_OP_NOT_EQUALS:
            return mcc_symbol_table_validate_binary_operator_handside_equals_type_check(expression, symbol_table);
        case MCC_AST_BINARY_OP_AND:
        case MCC_AST_BINARY_OP_OR:
            return mcc_symbol_table_validate_binary_operator_handside_bool_type_check(expression, symbol_table);
        default:
            return 1;
    }
}

int mcc_symbol_table_validate_expression_semantic(
        struct mcc_ast_expression *expression,
        struct mcc_symbol_table *symbol_table
) {
    switch(expression->type) {
        case MCC_AST_EXPRESSION_TYPE_IDENTIFIER:
            return mcc_symbol_table_get_symbol(symbol_table, expression->identifier->i_value) == NULL ? 1 : 0;
        case MCC_AST_EXPRESSION_TYPE_CALL_EXPRESSION:
            return mcc_symbol_table_get_symbol(symbol_table, expression->function_name->i_value) == NULL ? 1 : 0;
        case MCC_AST_EXPRESSION_TYPE_UNARY_OP:
            // not sure yet
            return 0;
        case MCC_AST_EXPRESSION_TYPE_BINARY_OP:
            return mcc_symbol_table_validate_binary_operator(expression, symbol_table);
        case MCC_AST_EXPRESSION_TYPE_PARENTH:
            return mcc_symbol_table_validate_expression_semantic(expression, symbol_table);
        case MCC_AST_EXPRESSION_TYPE_BRACKET:
            // Don't know what to do yet
        default:
            return 0;
    }
}

// --------------------------------------- Assignment

// Variable

int mcc_symbol_table_validate_assignemt_semantic(
        struct mcc_ast_assignment *assignment,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec
) {
    assert(assignment);

    struct mcc_symbol *s = mcc_symbol_table_get_symbol(symbol_table, assignment -> identifier -> i_value);

    // identifier not declare yet
    if (s == NULL) {
        mcc_symbol_table_add_error(
                ec,
                mcc_symbol_table_new_error(&(assignment -> node.sloc), MCC_SEMANTIC_ERROR_VARIABLE_NOT_DECLARED));

        return 1;
    }

    if (mcc_symbol_table_validate_expression_semantic(assignment -> normal_ass.rhs, symbol_table) == 0) {
        enum mcc_ast_data_type expected_type = s -> data_type;
        int ret_type = mcc_symbol_table_validate_expression_return_type(
                assignment -> normal_ass.rhs,
                symbol_table, expected_type);

        if (ret_type == 1) {
                mcc_symbol_table_add_error(
                        ec,
                        mcc_symbol_table_new_error(&(assignment ->node.sloc), MCC_SEMANTIC_ERROR_TYPE_ASSIGNMENT)
                );
        }

        return ret_type;
    } else {
        // expression somehow broke
        // TODO reasonable error message
        return 1;
    }
}

// Array

int mcc_symbol_table_validate_assignemt_array_semantic(
        struct mcc_ast_assignment *assignment,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec
) {
    UNUSED(assignment);
    UNUSED(symbol_table);
    UNUSED(ec);
    // TODO array validation

    return 0;
}

// --------------------------------------- Statement


// Program

int mcc_symbol_table_validate_main(
        struct mcc_ast_program *program,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec
) {
   UNUSED(symbol_table);
   UNUSED(ec);
   struct mcc_symbol *s = mcc_symbol_table_get_symbol(symbol_table,"main");

    if(!s){
        mcc_symbol_table_add_error(
                        ec,
                        mcc_symbol_table_new_error(&(program ->node.sloc), MCC_SEMANTIC_ERROR_MAIN_MISSINIG)
        );
        return 1;
    }
    else{
        return 0;
    }

}

