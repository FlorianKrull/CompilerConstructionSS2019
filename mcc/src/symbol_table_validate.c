#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <mcc/ast.h>
#include "mcc/symbol_table_validate.h"
#include "utils/unused.h"

// --------------------------------------- Expression

int mcc_symbol_table_validate_identifier(
        struct mcc_ast_identifier *identifier,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec
) {
    assert(identifier);
    assert(symbol_table);
    assert(ec);

    struct mcc_symbol *s = mcc_symbol_table_get_symbol(symbol_table, identifier->i_value);

    // identifier does not exist
    if(s == NULL) {
        mcc_symbol_table_add_error(
                ec,
                mcc_symbol_table_new_error(&(identifier->node.sloc), MCC_SEMANTIC_ERROR_VARIABLE_NOT_DECLARED));

        return 1;
    }

    return 0;
}

int mcc_symbol_table_validate_call_expression(
        struct mcc_ast_expression *expression,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec
) {
    assert(expression);
    assert(symbol_table);
    assert(ec);

    struct mcc_symbol *s = mcc_symbol_table_get_symbol(symbol_table, expression->function_name->i_value);

    // function not declared
    if(s == NULL) {
        mcc_symbol_table_add_error(
                ec,
                mcc_symbol_table_new_error(&(expression->node.sloc), MCC_SEMANTIC_ERROR_FUNC_NOT_DECLARED));

        return 1;
    } else {
        struct mcc_ast_argument *argument = expression->argument;
        Dynamic_Array *func_arguments = s -> func_arguments;

        if((argument == NULL && func_arguments->size != 0) || (argument != NULL && func_arguments == NULL)) {
            mcc_symbol_table_add_error(
                    ec,
                    mcc_symbol_table_new_error(&(expression->node.sloc), MCC_SEMANTIC_ERROR_INVALID_NUM_OF_ARGUMENTS));

            return 1;
        }
        if(argument != NULL && (argument->size != func_arguments->size)) {
            mcc_symbol_table_add_error(
                    ec,
                    mcc_symbol_table_new_error(&(expression->node.sloc), MCC_SEMANTIC_ERROR_INVALID_NUM_OF_ARGUMENTS));

            return 1;
        }

        for(int i = 0; i < func_arguments-> size; i++) {
            struct mcc_symbol_function_argument *fa = func_arguments -> arr[i];

            enum mcc_ast_data_type func_arg_type = fa -> arg_type;
            enum mcc_ast_data_type arg_typ = mcc_symbol_table_get_expression_return_type(argument->expressions[i],
                                                                                         symbol_table);

            mcc_symbol_table_add_type_check(
                    ec,
                    func_arg_type,
                    arg_typ,
                    MCC_SEMANTIC_TYPE_CHECK_ARG_TYPE,
                    &(expression->node.sloc)
            );
            // wrong type passed as an argument
            if(func_arg_type != arg_typ) {
                mcc_symbol_table_add_error(
                        ec,
                        mcc_symbol_table_new_error(&(expression->node.sloc), MCC_SEMANTIC_ERROR_INVALID_ARGUMENT_TYPE));

                return 1;
            }
        }

        return 0;
    }
}

int mcc_symbol_table_validate_unary_op(
        struct mcc_ast_expression *expression,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec
) {
    assert(symbol_table);
    assert(ec);

    enum mcc_ast_unary_op u_type = expression->unary_op;
    struct mcc_ast_expression *u_exp = expression->unary_expression;

    // check if unary expression is semantically valid
    if(mcc_symbol_table_validate_expression(u_exp, symbol_table, ec) == 0) {
        enum mcc_ast_data_type data_type;
        if(u_type == MCC_AST_UNARY_OP_NOT) {


            // check if expression return type is boolean
            data_type = mcc_symbol_table_get_expression_return_type(u_exp, symbol_table);

            mcc_symbol_table_add_type_check(
                    ec,
                    MCC_AST_DATA_TYPE_BOOL,
                    data_type,
                    MCC_SEMANTIC_TYPE_CHECK_UNARY,
                    &(expression->node.sloc)
            );

            if(data_type != MCC_AST_DATA_TYPE_BOOL) {
                mcc_symbol_table_add_error(
                        ec,
                        mcc_symbol_table_new_error(&(expression->node.sloc),
                                                   MCC_SEMANTIC_ERROR_UNARY_OP_EXPECTED_BOOL));

                return 1;
            }
        } else {
            // check if expression return type is number type
            data_type = mcc_symbol_table_get_expression_return_type(u_exp, symbol_table);

            mcc_symbol_table_add_type_check(
                    ec,
                    MCC_AST_DATA_TYPE_INT,
                    data_type,
                    MCC_SEMANTIC_TYPE_CHECK_UNARY,
                    &(expression->node.sloc)
            );
            mcc_symbol_table_add_type_check(
                    ec,
                    MCC_AST_DATA_TYPE_FLOAT,
                    data_type,
                    MCC_SEMANTIC_TYPE_CHECK_UNARY,
                    &(expression->node.sloc));

            if(data_type != MCC_AST_DATA_TYPE_INT && data_type != MCC_AST_DATA_TYPE_FLOAT) {
                mcc_symbol_table_add_error(
                        ec,
                        mcc_symbol_table_new_error(&(expression->node.sloc),
                                                   MCC_SEMANTIC_ERROR_UNARY_OP_EXPECTED_NUMBER));

                return 1;
            }
        }

        return 0;
    }

    // unary op expression is not valid
    return 1;
}

enum mcc_ast_data_type mcc_symbol_table_get_expression_return_type_binary_op(
        struct mcc_ast_expression *expression,
        struct mcc_symbol_table *symbol_table
) {

    enum mcc_ast_data_type lhs_type = mcc_symbol_table_get_expression_return_type(expression->lhs, symbol_table);

    // since there is no implicit type conversion AND we expect the binary op to be valid (see comments in mcc_symbol_table_get_expression_return_type)
    // we can just check lhs expression
    switch(expression->op) {
        case MCC_AST_BINARY_OP_ADD:
        case MCC_AST_BINARY_OP_SUB:
        case MCC_AST_BINARY_OP_MUL:
        case MCC_AST_BINARY_OP_DIV:
            if(lhs_type == MCC_AST_DATA_TYPE_INT) {
                return MCC_AST_DATA_TYPE_INT;
            } else {
                return MCC_AST_DATA_TYPE_FLOAT;
            }
        case MCC_AST_BINARY_OP_LESS:
        case MCC_AST_BINARY_OP_LESS_EQUALS:
        case MCC_AST_BINARY_OP_GREATER:
        case MCC_AST_BINARY_OP_GREATER_EQUALS:
        case MCC_AST_BINARY_OP_EQUALS:
        case MCC_AST_BINARY_OP_NOT_EQUALS:
        case MCC_AST_BINARY_OP_AND:
        case MCC_AST_BINARY_OP_OR:
            return MCC_AST_DATA_TYPE_BOOL;
        default:
            // invalid return type for binary op
            return MCC_AST_DATA_TYPE_VOID;
    }
}

enum mcc_ast_data_type mcc_symbol_table_get_expression_return_type(
        struct mcc_ast_expression *expression,
        struct mcc_symbol_table *symbol_table
) {
    assert(expression);

    switch(expression->type) {
        case MCC_AST_EXPRESSION_TYPE_LITERAL:
            return expression->literal->type;
        case MCC_AST_EXPRESSION_TYPE_IDENTIFIER:
            return mcc_symbol_table_get_symbol(symbol_table, expression->identifier->i_value)->data_type;
        case MCC_AST_EXPRESSION_TYPE_CALL_EXPRESSION:
            return mcc_symbol_table_get_symbol(symbol_table, expression->function_name->i_value)->data_type;
        case MCC_AST_EXPRESSION_TYPE_BINARY_OP:
            // must be a valid binary operation already! (e.g. no string + number -> invalid binary operation will result in type void)
            return mcc_symbol_table_get_expression_return_type_binary_op(expression, symbol_table);
        case MCC_AST_EXPRESSION_TYPE_UNARY_OP:
            return mcc_symbol_table_get_expression_return_type(expression->unary_expression, symbol_table);
        case MCC_AST_EXPRESSION_TYPE_PARENTH:
            return mcc_symbol_table_get_expression_return_type(expression->expression, symbol_table);
        case MCC_AST_EXPRESSION_TYPE_BRACKET:
            return mcc_symbol_table_get_expression_return_type(expression->bracket_expression, symbol_table);
    }
}

int mcc_symbol_table_validate_expression_return_type(
        struct mcc_ast_expression *expression,
        struct mcc_symbol_table *symbol_table,
        enum mcc_ast_data_type expected,
        enum mcc_semantic_type_check_type type_check,
        struct mcc_symbol_table_error_collector *ec
) {

    enum mcc_ast_data_type return_type = mcc_symbol_table_get_expression_return_type(expression, symbol_table);

    mcc_symbol_table_add_type_check(
            ec,
            expected,
            return_type,
            type_check,
            &(expression->node.sloc)
    );

    return return_type == expected ? 0 : 1;
}

int mcc_symbol_table_validate_condition_to_type_bool(
        struct mcc_ast_expression *condition,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec
) {
    assert(condition);
    assert(symbol_table);

    int valid_return = mcc_symbol_table_validate_expression_return_type(
            condition,
            symbol_table,
            MCC_AST_DATA_TYPE_BOOL,
            MCC_SEMANTIC_TYPE_CHECK_CONDITION_BOOL,
            ec
    );

    if(valid_return) {
        mcc_symbol_table_add_error(
                ec,
                mcc_symbol_table_new_error(&(condition->node.sloc), MCC_SEMANTIC_ERROR_CONDITION_BOOL_EXPECTED));

        return 1;
    }

    return 0;
}


// both handsides of number type
int mcc_symbol_table_validate_binary_operator_handside_number_type_check(
        struct mcc_ast_expression *expression,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec
) {

    enum mcc_ast_data_type lhs_data_type = mcc_symbol_table_get_expression_return_type(expression->lhs, symbol_table);
    enum mcc_ast_data_type rhs_data_type = mcc_symbol_table_get_expression_return_type(expression-> rhs, symbol_table);

    if((lhs_data_type == MCC_AST_DATA_TYPE_INT ||
        lhs_data_type == MCC_AST_DATA_TYPE_FLOAT)
       &&
       (rhs_data_type == MCC_AST_DATA_TYPE_INT ||
        rhs_data_type == MCC_AST_DATA_TYPE_FLOAT)) {
        // both sides are number types
        return 0;
    } else {
        mcc_symbol_table_add_error(
                ec,
                mcc_symbol_table_new_error(&(expression->node.sloc),
                                           MCC_SEMANTIC_ERROR_BINARY_OP_HANDSIDE_NUMBER_TYPE));
        return 1;
    }
}

// both handsides of bool type
int mcc_symbol_table_validate_binary_operator_handside_bool_type_check(
        struct mcc_ast_expression *expression,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec
) {

    enum mcc_ast_data_type lhs_data_type = mcc_symbol_table_get_expression_return_type(expression->lhs, symbol_table);
    enum mcc_ast_data_type rhs_data_type = mcc_symbol_table_get_expression_return_type(expression-> rhs, symbol_table);

    if(lhs_data_type &&
       rhs_data_type) {
        // both sides are bool types
        return 0;
    } else {
        mcc_symbol_table_add_error(
                ec,
                mcc_symbol_table_new_error(&(expression->node.sloc), MCC_SEMANTIC_ERROR_BINARY_OP_HANDSIDE_BOOL_TYPE));
        return 1;
    }
}


int mcc_symbol_table_validate_binary_operator(
        struct mcc_ast_expression *expression,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec
) {

    // check if both handsides are semantically correct expressions
    if(mcc_symbol_table_validate_expression(expression->lhs, symbol_table, ec) == 1 ||
       mcc_symbol_table_validate_expression(expression->rhs, symbol_table, ec) == 1) {
        return 1;
    }

    enum mcc_ast_data_type lhs_data_type = mcc_symbol_table_get_expression_return_type(expression->lhs, symbol_table);
    enum mcc_ast_data_type rhs_data_type = mcc_symbol_table_get_expression_return_type(expression-> rhs, symbol_table);

    mcc_symbol_table_add_type_check(
            ec,
            lhs_data_type,
            rhs_data_type,
            MCC_SEMANIC_TYPE_CHECK_BINARY_HANDSIDE_BOTH,
            &(expression->node.sloc)
    );

    // check if both handsides have the same return type
    if(lhs_data_type != rhs_data_type) {
        mcc_symbol_table_add_error(
                ec,
                mcc_symbol_table_new_error(&(expression->node.sloc), MCC_SEMANTIC_ERROR_BINARY_OP_HANDSIDE_SAME_TYPE));

        return 1;
    }

    // handle division by 0 separately
    if(expression->op == MCC_AST_BINARY_OP_DIV) {
        if(mcc_symbol_table_get_expression_return_type(expression->rhs, symbol_table) == 0) {
            mcc_symbol_table_add_error(
                    ec,
                    mcc_symbol_table_new_error(&(expression->node.sloc), MCC_SEMANTIC_ERROR_BINARY_OP_DIV_BY_0));

            return 1;
        }
    }

    // check if handsides have valid types
    switch(expression->op) {
        case MCC_AST_BINARY_OP_ADD:
        case MCC_AST_BINARY_OP_SUB:
        case MCC_AST_BINARY_OP_MUL:
        case MCC_AST_BINARY_OP_LESS:
        case MCC_AST_BINARY_OP_LESS_EQUALS:
        case MCC_AST_BINARY_OP_GREATER:
        case MCC_AST_BINARY_OP_GREATER_EQUALS:
        case MCC_AST_BINARY_OP_DIV:
            return mcc_symbol_table_validate_binary_operator_handside_number_type_check(expression, symbol_table, ec);
        case MCC_AST_BINARY_OP_AND:
        case MCC_AST_BINARY_OP_OR:
            return mcc_symbol_table_validate_binary_operator_handside_bool_type_check(expression, symbol_table, ec);
        default:
            // eq and neq -> must have both sides same - is already check above in this function
            return 0;
    }
}

int mcc_symbol_table_validate_expression(
        struct mcc_ast_expression *expression,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec
) {
    assert(expression);
    assert(symbol_table);
    assert(ec);


    switch(expression->type) {
        case MCC_AST_EXPRESSION_TYPE_IDENTIFIER:
            return mcc_symbol_table_validate_identifier(expression->identifier, symbol_table, ec);
        case MCC_AST_EXPRESSION_TYPE_CALL_EXPRESSION:
            return mcc_symbol_table_validate_call_expression(
                    expression,
                    symbol_table,
                    ec
            );
        case MCC_AST_EXPRESSION_TYPE_UNARY_OP:
            // first check unary expression
            if(mcc_symbol_table_validate_expression(expression->unary_expression, symbol_table, ec) == 0) {
                // expression seems to be valid -> check unary
                return mcc_symbol_table_validate_unary_op(
                        expression,
                        symbol_table,
                        ec
                );
            }

            return 1;
        case MCC_AST_EXPRESSION_TYPE_BINARY_OP:
            return mcc_symbol_table_validate_binary_operator(expression, symbol_table, ec);
        case MCC_AST_EXPRESSION_TYPE_PARENTH:
            return mcc_symbol_table_validate_expression(expression->expression, symbol_table, ec);
        case MCC_AST_EXPRESSION_TYPE_BRACKET:
            return mcc_symbol_table_validate_expression(expression->bracket_expression, symbol_table, ec);
        default:
            return 0;
    }
}

// --------------------------------------- Assignment

// Variable

int mcc_symbol_table_validate_assignment_semantic(
        struct mcc_ast_assignment *assignment,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec
) {
    assert(assignment);

    struct mcc_symbol *s = mcc_symbol_table_get_symbol(symbol_table, assignment->identifier->i_value);

    // identifier not declare yet
    if(s == NULL) {
        mcc_symbol_table_add_error(
                ec,
                mcc_symbol_table_new_error(&(assignment->node.sloc), MCC_SEMANTIC_ERROR_VARIABLE_NOT_DECLARED));

        return 1;
    }

    if(s->symbol_type == MCC_SYMBOL_TYPE_ARRAY) {
        if(assignment->normal_ass.rhs->type == MCC_AST_EXPRESSION_TYPE_BINARY_OP) {
            mcc_symbol_table_add_error(
                    ec,
                    mcc_symbol_table_new_error(&(assignment->node.sloc), MCC_SEMANTIC_ERROR_ARRAY_OPERATIONS)
            );
            return 1;
        }

    }

    if(mcc_symbol_table_validate_expression(assignment->normal_ass.rhs, symbol_table, ec) == 0) {
        enum mcc_ast_data_type expected_type = s -> data_type;

        int ret_type = mcc_symbol_table_validate_expression_return_type(
                assignment->normal_ass.rhs,
                symbol_table,
                expected_type,
                MCC_SEMANIC_TYPE_CHECK_ASSIGNMENT,
                ec
        );

        if(ret_type == 1) {
            mcc_symbol_table_add_error(
                    ec,
                    mcc_symbol_table_new_error(&(assignment->node.sloc), MCC_SEMANTIC_ERROR_TYPE_ASSIGNMENT)
            );
        }

        return ret_type;
    } else {
        // TODO reasonable error message
        return 1;
    }
}

// NOT USED
//int mcc_symbol_table_validate_assignment_array_semantic(
//        struct mcc_ast_assignment *assignment,
//        struct mcc_symbol_table *symbol_table,
//        struct mcc_symbol_table_error_collector *ec
//) {
//
//    struct mcc_symbol *s = mcc_symbol_table_get_symbol(symbol_table, assignment->identifier->i_value);
//    if(mcc_symbol_table_validate_expression(assignment->array_ass.rhs, symbol_table, ec) == 0) {
//        enum mcc_ast_data_type expected_type = s->data_type;
//
//        int ret_type = mcc_symbol_table_validate_expression_return_type(
//                assignment->array_ass.rhs,
//                symbol_table,
//                expected_type,
//                MCC_SEMANIC_TYPE_CHECK_ASSIGNMENT,
//                ec
//        );
//
//        if(ret_type == 1) {
//            mcc_symbol_table_add_error(
//                    ec,
//                    mcc_symbol_table_new_error(&(assignment->node.sloc), MCC_SEMANTIC_ERROR_TYPE_ASSIGNMENT)
//            );
//        }
//
//    }
//
//
//    if(assignment->array_ass.rhs->type == MCC_AST_EXPRESSION_TYPE_BINARY_OP) {
//        mcc_symbol_table_add_error(
//                ec,
//                mcc_symbol_table_new_error(&(assignment->node.sloc), MCC_SEMANTIC_ERROR_ARRAY_OPERATIONS)
//        );
//        return 1;
//    }
//
//    return 0;
//}

// --------------------------------------- Statement

static int validate_return_expression(
        struct mcc_ast_expression *expression,
        enum mcc_ast_data_type return_type,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec
) {
    int valid_return = mcc_symbol_table_validate_expression_return_type(
            expression,
            symbol_table,
            return_type,
            MCC_SEMANTIC_TYPE_CHECK_RETURN,
            ec
    );

    if(valid_return == 1) {
        mcc_symbol_table_add_error(
                ec,
                mcc_symbol_table_new_error(&(expression->node.sloc),
                                           MCC_SEMANTIC_ERROR_INVALID_RETURN_TYPE_IN_NON_VOID_FUNCTION)
        );

        return 1;
    }

    return 0;
}

int mcc_symbol_table_validate_statement_return(
        struct mcc_ast_statement *statement,
        enum mcc_ast_data_type return_type,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec) {
    struct mcc_ast_statement_list *st_l = NULL;
    struct mcc_ast_statement_list *next = NULL;
    assert(statement);
    // according to parser a function can only have a statement list as a body
    if(statement != NULL && statement->type == MCC_AST_STATEMENT_TYPE_COMPOUND) {
        st_l = statement->statement_list;
    }

    while(st_l != NULL) {
        next = st_l->next;

        if(next == NULL) {
            if(st_l->statement->type == MCC_AST_STATEMENT_TYPE_RETURN) {
                return validate_return_expression(
                        st_l->statement->expression,
                        return_type,
                        symbol_table,
                        ec
                );
            } else if(st_l->statement->type == MCC_AST_STATEMENT_TYPE_WHILE) {
                return mcc_symbol_table_validate_statement_return(
                        st_l->statement->while_stmt,
                        return_type,
                        symbol_table,
                        ec
                );
            } else if(st_l->statement->type == MCC_AST_STATEMENT_TYPE_IF) {
                int valid_if = mcc_symbol_table_validate_statement_return(
                        st_l->statement->if_stmt,
                        return_type,
                        symbol_table,
                        ec
                );

                int valid_else = 0;
                if(st_l->statement->else_stmt != NULL) {
                    valid_else = mcc_symbol_table_validate_statement_return(
                            st_l->statement->else_stmt,
                            return_type,
                            symbol_table,
                            ec
                    );
                }

                return valid_if | valid_else;
            } else {
                mcc_symbol_table_add_error(
                        ec,
                        mcc_symbol_table_new_error(&(statement->node.sloc),
                                                   MCC_SEMANTIC_ERROR_NO_RETURN_IN_NON_VOID_FUNCTION)
                );
                return 1;
            }
        } else {
            st_l = st_l->next;
        }
    }

    // should not get to here - if - there was no return anywhere
    mcc_symbol_table_add_error(
            ec,
            mcc_symbol_table_new_error(&(statement->node.sloc),
                                       MCC_SEMANTIC_ERROR_NO_RETURN_IN_NON_VOID_FUNCTION)
    );

    return 1;
}

// Program

int mcc_symbol_table_validate_main(
        struct mcc_ast_program *program,
        struct mcc_symbol_table *symbol_table,
        struct mcc_symbol_table_error_collector *ec
) {
    UNUSED(symbol_table);
    UNUSED(ec);
    struct mcc_symbol *s = mcc_symbol_table_get_symbol(symbol_table, "main");

    if(!s) {
        mcc_symbol_table_add_error(
                ec,
                mcc_symbol_table_new_error(&(program->node.sloc), MCC_SEMANTIC_ERROR_MAIN_MISSING)
        );
        return 1;
    } else {
        return 0;
    }
}

