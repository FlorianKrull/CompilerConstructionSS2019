#include "mcc/ast.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


// ---------------------------------------------------------------- Expressions

struct mcc_ast_expression *mcc_ast_new_expression_literal(struct mcc_ast_literal *literal)
{
	assert(literal);

	struct mcc_ast_expression *expr = malloc(sizeof(*expr));
	if (!expr) {
		return NULL;
	}

	expr->type = MCC_AST_EXPRESSION_TYPE_LITERAL;
	expr->literal = literal;
	return expr;
}

struct mcc_ast_expression *mcc_ast_new_expression_binary_op(enum mcc_ast_binary_op op,
                                                            struct mcc_ast_expression *lhs,
                                                            struct mcc_ast_expression *rhs)
{
	assert(lhs);
	assert(rhs);

	struct mcc_ast_expression *expr = malloc(sizeof(*expr));
	if (!expr) {
		return NULL;
	}

	expr->type = MCC_AST_EXPRESSION_TYPE_BINARY_OP;
	expr->op = op;
	expr->lhs = lhs;
	expr->rhs = rhs;
	return expr;
}

struct mcc_ast_expression *mcc_ast_new_expression_unary_op(enum mcc_ast_unary_op op,
														   struct mcc_ast_expression *rhs)
{

	assert(rhs);

	struct mcc_ast_expression *expr = malloc(sizeof(*expr));
	if(!expr) {
		return NULL;
	}

	expr -> type = MCC_AST_EXPRESSION_TYPE_UNARY_OP;
	expr -> unary_op = op;
	expr -> rhs = rhs;
	return expr;


}

struct mcc_ast_expression *mcc_ast_new_expression_parenth(struct mcc_ast_expression *expression)
{
	assert(expression);

	struct mcc_ast_expression *expr = malloc(sizeof(*expr));
	if (!expr) {
		return NULL;
	}

	expr->type = MCC_AST_EXPRESSION_TYPE_PARENTH;
	expr->expression = expression;
	return expr;
}

void mcc_ast_delete_expression(struct mcc_ast_expression *expression)
{
	assert(expression);

	switch (expression->type) {
	case MCC_AST_EXPRESSION_TYPE_LITERAL:
		mcc_ast_delete_literal(expression->literal);
		break;

	case MCC_AST_EXPRESSION_TYPE_BINARY_OP:
		mcc_ast_delete_expression(expression->lhs);
		mcc_ast_delete_expression(expression->rhs);
		break;

	case MCC_AST_EXPRESSION_TYPE_PARENTH:
		mcc_ast_delete_expression(expression->expression);
		break;
	}

	free(expression);
}

// ------------------------------------------------------------------- Literals

struct mcc_ast_literal *mcc_ast_new_literal_int(long value)
{
	struct mcc_ast_literal *lit = malloc(sizeof(*lit));
	if (!lit) {
		return NULL;
	}

	lit->type = MCC_AST_LITERAL_TYPE_INT;
	lit->i_value = value;
	return lit;
}

struct mcc_ast_literal *mcc_ast_new_literal_float(double value)
{
	struct mcc_ast_literal *lit = malloc(sizeof(*lit));
	if (!lit) {
		return NULL;
	}

	lit->type = MCC_AST_LITERAL_TYPE_FLOAT;
	lit->f_value = value;
	return lit;
}

struct mcc_ast_literal *mcc_ast_new_literal_string(char* value)
{
	struct mcc_ast_literal *lit = malloc(sizeof(*lit));


	if (!lit) {
		return NULL;
	}

	lit -> type = MCC_AST_LITERAL_TYPE_STRING;
	lit -> s_value = value;

	return lit;
}

struct mcc_ast_literal *mcc_ast_new_literal_bool(bool value)
{
	struct mcc_ast_literal *lit = malloc(sizeof(*lit));

	if (!lit) {
		return NULL;
	}

	lit -> type = MCC_AST_LITERAL_TYPE_BOOL;
	lit -> b_value = value;

	return lit;
}



void mcc_ast_delete_literal(struct mcc_ast_literal *literal)
{
	assert(literal);
	free(literal);
}

// ------------------------------------------------------------------- Identifier

struct mcc_ast_identifier *mcc_ast_new_identifier(char *value)
{
	struct mcc_ast_identifier *id = malloc(sizeof(*id));
	if (!id) {
		return NULL;
	}

	char *str = malloc((strlen(value) + 1) * sizeof(char));
	strcpy(str, value);

	id->i_value = str;
	return id;
}

void mcc_ast_delete_identifier(struct mcc_ast_identifier *identifier)
{
	assert(identifier);
	free(identifier->i_value);
	free(identifier);
}

// ------------------------------------------------------------------- Declaration

struct mcc_ast_declaration *mcc_ast_new_declaration(enum mcc_ast_data_type type, struct mcc_ast_literal *array_size,struct mcc_ast_identifier *identifier)
{
    assert(identifier);

    struct mcc_ast_declaration *decl= malloc(sizeof(*decl));
	if (!decl) {
		return NULL;
	}

    decl -> type = type;
	decl-> array_size = array_size ? array_size : NULL;
    decl -> identifier = identifier;

    return decl;
}


// ------------------------------------------------------------------- Statements

struct mcc_ast_statement *construct_statement()
{
    struct mcc_ast_statement *stmt = malloc(sizeof(*stmt));
    if (!stmt)
        return NULL;

    return stmt;
}

struct mcc_ast_statement *mcc_ast_new_statement_expression(struct mcc_ast_expression *expression)
{
	assert(expression);

	struct mcc_ast_statement *stmt = construct_statement();

	stmt -> type = MMC_AST_STATEMENT_TYPE_EXPRESSION;
	stmt -> expression = expression;
	return stmt;
}


struct mcc_ast_statement *mcc_ast_new_statement_if(struct mcc_ast_expression *condition,
                                                   struct mcc_ast_statement *if_stmt,
                                                   struct mcc_ast_statement *else_stmt)
{
    assert(condition);
    assert(if_stmt);
    assert(else_stmt);

    struct mcc_ast_statement *stmt = construct_statement();

    stmt -> type = MCC_AST_STATEMENT_TYPE_IF;
    stmt -> if_stmt = if_stmt;
    if (else_stmt) {
        stmt -> else_stmt = else_stmt;
    }

    return stmt;
}

struct mcc_ast_statement *mcc_ast_new_statement_declaration(enum mcc_ast_data_type data_type,
                                                            struct mcc_ast_identifier *identifier)
{
    assert(identifier);

    printf("Parse declaration");

    struct mcc_ast_statement *stmt = construct_statement();

    stmt -> type = MCC_AST_STATEMENT_TYPE_DECL;
    stmt -> data_type = data_type;
    stmt -> id_decl = identifier;

    return stmt;
}

struct mcc_ast_statement *mcc_ast_new_statement_while(struct mcc_ast_expression *condition,
                                                      struct mcc_ast_statement *while_stmt)
{
    assert(condition);
    assert(while_stmt);

    struct mcc_ast_statement *stmt = construct_statement();

    stmt -> type = MCC_AST_STATEMENT_TYPE_WHILE;
    stmt -> while_condition = condition;
    stmt -> while_stmt = while_stmt;

    return stmt;
}


struct mcc_ast_statement *mcc_ast_new_statement_assignment(struct mcc_ast_identifier *id_assgn,
                                                           struct mcc_ast_expression *lhs_assgn,
                                                           struct mcc_ast_expression *rhs_assgn)
{
    assert(id_assgn);
    assert(lhs_assgn);
    assert(rhs_assgn);

    struct mcc_ast_statement *stmt = construct_statement();

    stmt -> type = MCC_AST_STATEMENT_TYPE_ASSGN;
    stmt -> id_assgn = id_assgn;
    stmt -> lhs_assgn = lhs_assgn;
    stmt -> rhs_assgn = rhs_assgn;

    return stmt;
}

void mcc_ast_empty_node() {
}

// ------------------------------------------------------------------- Function Definition

struct mcc_ast_function_def *mcc_ast_new_function_def( enum mcc_ast_data_type type, 
														struct mcc_ast_identifier *identifier,
														struct mcc_ast_parameter *parameter,
														struct mcc_ast_statement *compound_statement)
{



}


// ------------------------------------------------------------------- Parameters

struct mcc_ast_parameter *mcc_ast_new_parameter(struct mcc_ast_declaration *declaration)
{
	assert(declaration);

	struct mcc_ast_parameter *param = malloc(sizeof(*param));
	assert(param);

	param->declaration = declaration;
	param->next = NULL;
	return param;
}

void mcc_ast_delete_parameter(struct mcc_ast_parameter *parameter)
{
	assert(parameter);
	mcc_ast_delete_declaration(parameter->declaration);
	if (parameter->next != NULL) {
		mcc_ast_delete_parameter(parameter->next);
	}
	free(parameter);
}