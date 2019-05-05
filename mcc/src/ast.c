#include "mcc/ast.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Constantcs
const int PROGRAM_FUNCTION_DEF_SIZE = 4;
const int PARAMETER_DECLARATION_SIZE = 4;
const int ARGUMENT_EXPRESSION_SIZE = 4;

// ---------------------------------------------------------------- Expressions

struct mcc_ast_expression *
mcc_ast_new_expression_literal(struct mcc_ast_literal *literal)
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


struct mcc_ast_expression *
mcc_ast_new_expression_unary_op(enum mcc_ast_unary_op op,
                                struct mcc_ast_expression *expression)
{
	assert(expression);

	struct mcc_ast_expression *expr = malloc(sizeof(*expr));
	if (!expr) {
		return NULL;
	}

	expr->type = MCC_AST_EXPRESSION_TYPE_UNARY_OP;
	expr->unary_op = op;
	expr->unary_expression = expression;

    return expr;
}

struct mcc_ast_expression *
mcc_ast_new_expression_binary_op(enum mcc_ast_binary_op op,
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

struct mcc_ast_expression *
mcc_ast_new_expression_parenth(struct mcc_ast_expression *expression)
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

struct mcc_ast_expression *
mcc_ast_new_expression_bracket(struct mcc_ast_identifier *identifier,
                               struct mcc_ast_expression *expression)
{
	assert(identifier);
	assert(expression);

	struct mcc_ast_expression *expr = malloc(sizeof(*expr));
	if (!expr) {
		return NULL;
	}

	expr->type = MCC_AST_EXPRESSION_TYPE_BRACKET;
	expr->bracket_identifier = identifier;
	expr->bracket_expression = expression;
	return expr;
}

struct mcc_ast_expression *mcc_ast_new_expression_identifier(struct mcc_ast_identifier *identifier)
{
	assert(identifier);

	struct mcc_ast_expression *expr = malloc(sizeof(*expr));
	if (!expr) {
		return NULL;
	}

	expr->type = MCC_AST_EXPRESSION_TYPE_IDENTIFIER;
	expr->identifier = identifier;
	return expr;
}



void mcc_ast_delete_expression(struct mcc_ast_expression *expression)
{
	assert(expression);

	switch (expression->type) {
	case MCC_AST_EXPRESSION_TYPE_LITERAL:
		mcc_ast_delete_literal(expression->literal);
		break;

	case MCC_AST_EXPRESSION_TYPE_IDENTIFIER:
		mcc_ast_delete_identifier(expression->identifier);
		break;

	case MCC_AST_EXPRESSION_TYPE_UNARY_OP:
		mcc_ast_delete_expression(expression->unary_expression);
		break;

	case MCC_AST_EXPRESSION_TYPE_BINARY_OP:
		mcc_ast_delete_expression(expression->lhs);
		mcc_ast_delete_expression(expression->rhs);
		break;

	case MCC_AST_EXPRESSION_TYPE_PARENTH:
		mcc_ast_delete_expression(expression->expression);
		break;

	case MCC_AST_EXPRESSION_TYPE_BRACKET:
		mcc_ast_delete_identifier(expression->bracket_identifier);
		mcc_ast_delete_expression(expression->bracket_expression);
		break;
    case MCC_AST_EXPRESSION_TYPE_CALL_EXPRESSION:
        mcc_ast_delete_identifier(expression->bracket_identifier);
		if(expression->argument != NULL){
			mcc_ast_delete_argument(expression->argument);
		}
        break;
	}

	free(expression);
}

struct mcc_ast_expression *
mcc_ast_new_expression_call_expression(struct mcc_ast_identifier *function_name,
                                       struct mcc_ast_argument *argument)
{
    assert(function_name);

    struct mcc_ast_expression *expression = malloc(sizeof(*expression));

    expression -> type = MCC_AST_EXPRESSION_TYPE_CALL_EXPRESSION;
	expression -> function_name = function_name;
    expression -> argument = argument;

    return expression;
}

// ------------------------------------------------------------------- Literals

struct mcc_ast_literal *mcc_ast_new_literal_bool(bool value)
{
	struct mcc_ast_literal *lit = malloc(sizeof(*lit));
	assert(lit);

	lit->type = MCC_AST_LITERAL_TYPE_BOOL;
	lit->b_value = value;
	return lit;
}

struct mcc_ast_literal *mcc_ast_new_literal_int(long value)
{
	struct mcc_ast_literal *lit = malloc(sizeof(*lit));
	assert(lit);

	lit->type = MCC_AST_LITERAL_TYPE_INT;
	lit->i_value = value;
	return lit;
}

struct mcc_ast_literal *mcc_ast_new_literal_float(double value)
{
	struct mcc_ast_literal *lit = malloc(sizeof(*lit));
	assert(lit);

	lit->type = MCC_AST_LITERAL_TYPE_FLOAT;
	lit->f_value = value;
	return lit;
}

struct mcc_ast_literal *mcc_ast_new_literal_string(char *value)
{
	struct mcc_ast_literal *lit = malloc(sizeof(*lit));
	assert(lit);

	char *copyValue = malloc(sizeof(char) * (strlen(value) + 4));
    strcpy(copyValue, value);

	lit->type = MCC_AST_LITERAL_TYPE_STRING;
	lit->s_value = copyValue;
	return lit;
}

void mcc_ast_delete_literal(struct mcc_ast_literal *literal)
{
	assert(literal);
	if (literal->type == MCC_AST_LITERAL_TYPE_STRING) {
		free(literal->s_value);
	}
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

struct mcc_ast_declaration *mcc_ast_new_declaration(enum mcc_ast_data_type type, struct mcc_ast_literal *arr_size, struct mcc_ast_identifier *ident)
{
    assert(ident);

	struct mcc_ast_declaration *decl = malloc(sizeof(*decl));
	if (!decl) {
		return NULL;
	}

	decl -> type = type;
	decl -> ident = ident;
	decl -> arr_literal = arr_size ? arr_size : NULL;
	return decl;
}

void mcc_ast_delete_declaration(struct mcc_ast_declaration *declaration)
{
    assert(declaration);
    mcc_ast_delete_identifier(declaration->ident);
	if (declaration->arr_literal) {
		mcc_ast_delete_literal(declaration->arr_literal);
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

struct mcc_ast_statement *mcc_ast_new_statement_expression(struct mcc_ast_expression *expression)
{
	assert(expression);

	struct mcc_ast_statement *stmt = malloc(sizeof(*stmt));
    assert(stmt);

	stmt -> type = MCC_AST_STATEMENT_TYPE_EXPRESSION;
	stmt -> expression = expression;
	return stmt;
}


struct mcc_ast_statement *mcc_ast_new_statement_if(struct mcc_ast_expression *condition,
                                                   struct mcc_ast_statement *if_stmt,
                                                   struct mcc_ast_statement *else_stmt)
{
    assert(condition);
    assert(if_stmt);

    struct mcc_ast_statement *stmt = malloc(sizeof(*stmt));

    stmt -> type = MCC_AST_STATEMENT_TYPE_IF;
    stmt->if_condition = condition;
    stmt -> if_stmt = if_stmt;
    stmt->else_stmt = else_stmt;
    return stmt;
}

struct mcc_ast_statement *mcc_ast_new_statement_declaration(struct mcc_ast_declaration *declaration)
{
    assert(declaration);

    struct mcc_ast_statement *stmt = malloc(sizeof(*stmt));
    if (!stmt) {
		return NULL;
	}
    stmt-> declaration = declaration;
    stmt -> type = MCC_AST_STATEMENT_TYPE_DECL;
    
    return stmt;
}

struct mcc_ast_statement *mcc_ast_new_statement_while(struct mcc_ast_expression *condition,
                                                      struct mcc_ast_statement *while_stmt)
{
    assert(condition);
    assert(while_stmt);

    struct mcc_ast_statement *stmt = malloc(sizeof(*stmt));

    stmt -> type = MCC_AST_STATEMENT_TYPE_WHILE;
    stmt -> while_condition = condition;
    stmt -> while_stmt = while_stmt;

    return stmt;
}

struct mcc_ast_statement *mcc_ast_new_statement(struct mcc_ast_statement *statement){

    assert(statement);

    struct mcc_ast_statement *stmt = malloc(sizeof(*stmt));
    return statement;
}

struct mcc_ast_statement_list *
mcc_ast_new_statement_list(struct mcc_ast_statement *statement)
{
	assert(statement);
	struct mcc_ast_statement_list *list = malloc(sizeof(*list));
	assert(list);

	list->statement = statement;
	
	return list;
}

void mcc_ast_delete_statement_list(
    struct mcc_ast_statement_list *statement_list)
{
	assert(statement_list);
	if (statement_list->next != NULL) {
		mcc_ast_delete_statement_list(statement_list->next);
	}
	mcc_ast_delete_statement(statement_list->statement);
	free(statement_list);
}

struct mcc_ast_statement *
mcc_ast_new_statement_compound(struct mcc_ast_statement_list *statement)
{
	struct mcc_ast_statement *stmt = malloc(sizeof(*stmt));
	assert(stmt);

	stmt -> type = MCC_AST_STATEMENT_TYPE_COMPOUND;
    stmt -> statement_list = statement;

    return stmt;
}

struct mcc_ast_statement *mcc_ast_new_statement_assignment(struct mcc_ast_assignment *assignment)
{
    assert(assignment);
    
    struct mcc_ast_statement *stmt = malloc(sizeof(*stmt));
    assert(stmt);

   stmt->type = assignment->type == MCC_AST_ASSIGNMENT_TYPE_NORMAL
	                 ? MCC_AST_STATEMENT_TYPE_ASSGN
	                 : MCC_AST_STATEMENT_TYPE_ASSGN_ARR;
	stmt->assignment = assignment;
    return stmt;
}

struct mcc_ast_statement *mcc_ast_new_statement_return(struct mcc_ast_expression *expression)
{
	struct mcc_ast_statement *stmt = malloc(sizeof(*stmt));
	if (!stmt) {
		return NULL;
	}
	stmt->type = MCC_AST_STATEMENT_TYPE_RETURN;
	stmt->expression = expression;
	
	return stmt;
}

void mcc_ast_delete_statement(struct mcc_ast_statement *statement)
{
    assert(statement);

    switch(statement->type){
        case MCC_AST_STATEMENT_TYPE_EXPRESSION:
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
            if(statement -> statement_list != NULL){
                mcc_ast_delete_statement_list(statement->statement_list);
            }
			free(statement);
            break;
        default : break;
    }
}

void mcc_ast_empty_node() {
}

// ------------------------------------------------------------------- Parameters

// TODO: refactor (like program list)
struct mcc_ast_parameter *mcc_ast_new_parameter(struct mcc_ast_declaration *declaration, struct mcc_ast_parameter *params)
{
	assert(declaration);
    // assert(params);

    if (params == NULL) {
        struct mcc_ast_parameter *param = malloc(sizeof(*param) + sizeof(struct mcc_ast_declaration*) * PARAMETER_DECLARATION_SIZE);
        param -> size = 1;
        param -> max = PARAMETER_DECLARATION_SIZE;
        param -> parameters[0] = declaration;
        return param;
    }

    // add to previous params list
    if ((params -> size) > (params -> max)) {
        int size = params -> size;
        struct mcc_ast_parameter *new_params = realloc(params, sizeof(*params) + sizeof(struct mcc_ast_declaration*) * PARAMETER_DECLARATION_SIZE);
        new_params -> parameters[size] = declaration;
        new_params -> size += 1;
        new_params -> max += PARAMETER_DECLARATION_SIZE;
    } else {
        params -> parameters[(params -> size)] = declaration;
        params -> size += 1;
    }

    return params;
}

void mcc_ast_delete_parameter(struct mcc_ast_parameter *parameter) {
    if (parameter != NULL) {
        for (int i = 0; i < parameter -> size; i++) {
            mcc_ast_delete_declaration(parameter -> parameters[i]);
        }

        free(parameter);
    }
}

// ------------------------------------------------------------------- Arguments

struct mcc_ast_argument *mcc_ast_new_argument(struct mcc_ast_expression *expression) {
    assert(expression);

    struct mcc_ast_argument *argument = malloc(sizeof(*argument) + sizeof(struct mcc_ast_expression*) * ARGUMENT_EXPRESSION_SIZE);

    argument -> expressions[0] = expression;
    argument -> size = 1;
    argument -> max = ARGUMENT_EXPRESSION_SIZE;

    return argument;
}

struct mcc_ast_argument *mcc_ast_add_new_argument(struct mcc_ast_expression *expression, struct mcc_ast_argument *argument) {
    assert(argument);
    assert(expression);

    int size = argument -> size;
    int max = argument -> max;

    if (size < max) {
        argument -> expressions[size] = expression;
        argument -> size += 1;
    } else {

        argument = realloc(argument, sizeof(*argument) + sizeof(struct mcc_ast_expression*) * ARGUMENT_EXPRESSION_SIZE);
        argument -> expressions[size] = expression;
        argument -> size += 1;
        argument -> max += ARGUMENT_EXPRESSION_SIZE;
    }

    return argument;
}

void mcc_ast_delete_argument(struct mcc_ast_argument *argument) {
    assert(argument);

    for(int i = 0; i < argument -> size; i++) {
        mcc_ast_delete_expression(argument -> expressions[i]);
    }

    free(argument);
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

    struct mcc_ast_function *func = malloc(sizeof(*func));

    func -> return_type = return_type;
    func -> identifier = identifier;

    if (parameter != NULL) {
        func -> parameter = parameter;
    }

    func -> statement = statement;

    return func;
}

void mcc_ast_delete_function(struct mcc_ast_function *function) {
    assert(function);

    mcc_ast_delete_identifier(function -> identifier);

	if (function -> parameter != NULL) {
		mcc_ast_delete_parameter(function -> parameter);
	}

	if (function -> statement != NULL) {
		mcc_ast_delete_statement(function -> statement);
	}

    free(function);
}

// ------------------------------------------------------------------- Program

struct mcc_ast_program *mcc_ast_new_program(struct mcc_ast_function *function_def) {
    assert(function_def);

	struct mcc_ast_program *p = malloc(sizeof(*p) + sizeof(struct mcc_ast_function*) * PROGRAM_FUNCTION_DEF_SIZE);

    p -> size = 1;
    p -> max = PROGRAM_FUNCTION_DEF_SIZE;
    p -> function_def[0] = function_def;

    return p;
}

struct mcc_ast_program *mcc_ast_add_function(struct mcc_ast_function *function_def, struct mcc_ast_program *program)
{
    assert(program);
    assert(function_def);

    int size = program -> size;
    if (program -> size < program -> max) {
        program -> function_def[size] = function_def;
        program -> size += 1;
    } else {

        program = realloc(program, sizeof(*program) + sizeof(struct mcc_ast_function*) * PROGRAM_FUNCTION_DEF_SIZE);
        program -> max += PROGRAM_FUNCTION_DEF_SIZE;
        program -> function_def[size] = function_def;
        program -> size += 1;
    }

    return program;
}

void mcc_ast_delete_program(struct mcc_ast_program *program)
{
    assert(program);

    for (int i = 0; i < program -> size; i++) {
        mcc_ast_delete_function(program -> function_def[i]);
    }

    free(program);
}



