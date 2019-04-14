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
	expr -> up = op;
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

struct mcc_ast_expression *mcc_ast_new_expression_array_identifier(struct mcc_ast_identifier *identifier,
                                                                   struct mcc_ast_expression *expression)
{
	assert(identifier);
	assert(expression);

	struct mcc_ast_expression *expr = malloc(sizeof(*expr));
	assert(expr);

	expr->type = MCC_AST_EXPRESSION_TYPE_ARRAY_IDENTIFIER;
	expr->array_identifier.identifier = identifier;
	expr->array_identifier.expression = expression;
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

struct mcc_ast_literal *mcc_ast_new_literal(enum mcc_ast_literal_type type, char *value)
{
	struct mcc_ast_literal *lit = malloc(sizeof(*lit));
	if (!lit) {
		return NULL;
	}

	lit->type = type;
	lit->value = value;
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

struct mcc_ast_declaration *mcc_ast_new_declaration(enum mcc_ast_data_type type, struct mcc_ast_identifier *ident)
{
    assert(ident);

    struct mcc_ast_declaration *decl= malloc(sizeof(*decl));
    assert(decl);

    decl -> type = type;
    decl -> ident = ident;


    return decl;
}

void mcc_ast_delete_declaration(struct mcc_ast_declaration *declaration)
{
    assert(declaration);
    if (declaration->declaration_type ==
	    MCC_AST_DECLARATION_TYPE_ARRAY_DECLARATION) {
		mcc_ast_delete_literal(declaration->arr_literal);
		mcc_ast_delete_identifier(declaration->ident);
	} else {
		mcc_ast_delete_identifier(declaration->ident);
	}
	free(declaration);
}

// ------------------------------------------------------------------- Assignment

struct mcc_ast_assignment *
mcc_ast_new_assignment(struct mcc_ast_identifier *identifier,
                       struct mcc_ast_expression *rhs)
{
	assert(identifier);
	assert(rhs);

	struct mcc_ast_assignment *ass = malloc(sizeof(*ass));
	assert(ass);

	ass->type = MCC_AST_ASSIGNMENT_TYPE_NORMAL;
	ass->identifier = identifier;
	ass->normal_ass.rhs = rhs;
	return ass;
}

struct mcc_ast_assignment *
mcc_ast_new_array_assignment(struct mcc_ast_identifier *identifier,
                             struct mcc_ast_expression *index,
                             struct mcc_ast_expression *rhs)
{
	assert(identifier);
	assert(index);
	assert(rhs);

	struct mcc_ast_assignment *ass = malloc(sizeof(*ass));
	assert(ass);

	ass->type = MCC_AST_ASSIGNMENT_TYPE_ARRAY;
	ass->identifier = identifier;
	ass->array_ass.index = index;
	ass->array_ass.rhs = rhs;
	return ass;
}

void mcc_ast_delete_assignment(struct mcc_ast_assignment *assignment)
{
	assert(assignment);
	mcc_ast_delete_identifier(assignment->identifier);
	if (assignment->type == MCC_AST_ASSIGNMENT_TYPE_NORMAL) {
		mcc_ast_delete_expression(assignment->normal_ass.rhs);
	} else {
		mcc_ast_delete_expression(assignment->array_ass.index);
		mcc_ast_delete_expression(assignment->array_ass.rhs);
	}
	free(assignment);
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
    

    struct mcc_ast_statement *stmt = construct_statement();

    stmt -> type = MCC_AST_STATEMENT_TYPE_IF;
    stmt -> if_stmt = if_stmt;
    if (else_stmt != NULL) {
        stmt -> else_stmt = else_stmt;
        assert(else_stmt);
    }
    return stmt;
}

struct mcc_ast_statement *mcc_ast_new_statement_declaration(enum mcc_ast_data_type data_type,
                                                            struct mcc_ast_identifier *identifier)
{
    assert(identifier);

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

struct mcc_ast_statement *mcc_ast_new_statement(struct mcc_ast_statement *statement){

    assert(statement);

    struct mcc_ast_statement *stmt = construct_statement();

    return statement;
}

struct mcc_ast_statement_list *construct_new_statement_list(int size, int max)
{
    struct mcc_ast_statement_list *stmt_list = malloc(sizeof(*stmt_list) + max);
    stmt_list -> size = size;
    stmt_list -> max_size = max;

    return stmt_list;
}

struct mcc_ast_statement *mcc_ast_new_statement_list(struct mcc_ast_statement_list *statement_list,
                                                               struct mcc_ast_statement *next_statement)
{
    assert(next_statement);

    if (!statement_list) {
        // create new statement - start with statement array of size 10
        statement_list = construct_new_statement_list(0, 10);
        statement_list -> list[0] = next_statement;
    } else {
        int max = statement_list -> max_size;
        int current = statement_list -> size;
        // struct mcc_ast_statement *stmt_list[] = statement_list -> list;

        if (current < max) {
            statement_list -> list[current] = next_statement;
        } else {
            // double size of statement list
            statement_list = realloc(statement_list, sizeof(*statement_list) + (max * 2));

            // in case realloc fails return NULL for now
            // TODO create new statement list and free old one?
            if (!statement_list) {
                return NULL;
            }

            statement_list -> max_size = max * 2;
            statement_list -> size = current + 1;
            statement_list -> list[current] = next_statement;
        }
    }

    struct mcc_ast_statement *stmt = construct_statement();

    stmt -> type = MCC_AST_STATEMENT_TYPE_COMPOUND;
    stmt -> compound_statement = statement_list;

    return stmt;
}

struct mcc_ast_statement *mcc_ast_new_statement_assignment(struct mcc_ast_assignment *assignment)
{
    assert(assignment);
    
    struct mcc_ast_statement *stmt = construct_statement();

   stmt->type = assignment->type == MCC_AST_ASSIGNMENT_TYPE_NORMAL
	                 ? MCC_AST_STATEMENT_TYPE_ASSGN
	                 : MCC_AST_STATEMENT_TYPE_ASSGN_ARR;
	stmt->assignment = assignment;
    return stmt;
}

void mcc_ast_delete_statement(struct mcc_ast_statement *statement)
{
    assert(statement);

    switch(statement->type){
        case MMC_AST_STATEMENT_TYPE_EXPRESSION:
            mcc_ast_delete_expression(statement->expression);
            break;
        case MCC_AST_STATEMENT_TYPE_IF:
            mcc_ast_delete_expression(statement->if_condition);
            mcc_ast_delete_statement(statement->if_stmt);
            if(statement->else_stmt != NULL){
                mcc_ast_delete_statement(statement->else_stmt);
            }
            break;
	    case MCC_AST_STATEMENT_TYPE_WHILE:
            mcc_ast_delete_expression(statement->while_condition);
            mcc_ast_delete_statement(statement->while_stmt);
            break;
	    case MCC_AST_STATEMENT_TYPE_DECL:
            mcc_ast_delete_declaration(statement->declaration);
            break;
	    case MCC_AST_STATEMENT_TYPE_ASSGN:
	    case MCC_AST_STATEMENT_TYPE_ASSGN_ARR:
            mcc_ast_delete_assignment(statement->assignment);
            break;
	    case MCC_AST_STATEMENT_TYPE_COMPOUND:
            if(statement->compound_statement != NULL){
                mcc_ast_delete_statement(statement->compound_statement);
            }
            break;
        default : break;
    }
}

void mcc_ast_empty_node() {
}

// ------------------------------------------------------------------- Parameters

struct mcc_ast_parameter *mcc_ast_new_parameter(struct mcc_ast_declaration *declaration, struct mcc_ast_parameter *params)
{
	assert(declaration);
    assert(params);

    if (params == NULL) {
        struct mcc_ast_parameter *param = malloc(sizeof(*param) + sizeof(struct mcc_ast_declaration*) * 4);
        param -> size = 1;
        param -> max = 4;
        param -> parameters[0] = declaration;

        return param;
    }

    // add to previous params list
    if ((params -> size) == (params -> max)) {
        int next_max = params -> size + 4;
        int size = params -> size;
        struct mcc_ast_parameter *new_params = realloc(params, sizeof(*params) + sizeof(struct mcc_ast_declaration*) * next_max);
        new_params -> parameters[size -1] = declaration;
        new_params -> size += 1;
        new_params -> max = next_max;
    } else {
        params -> parameters[(params -> size) - 1] = declaration;
        params -> size += 1;
    }

    return params;
}

// ------------------------------------------------------------------- Function

struct mcc_ast_function *mcc_ast_new_function(
        enum mcc_ast_data_type return_type,
        struct mcc_ast_identifier *identifier,
        struct mcc_ast_parameter *parameter,
        struct mcc_ast_statement *statement
)
{
    assert(identifier);
    assert(parameter);
    assert(statement);

    struct mcc_ast_function *func = malloc(sizeof(*func));

    func -> return_type = return_type;
    func -> identifier = identifier;
    func -> parameter = parameter;
    func -> statement = statement;

    return func;
}

// ------------------------------------------------------------------- Program