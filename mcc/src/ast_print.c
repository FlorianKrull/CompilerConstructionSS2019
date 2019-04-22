#include "mcc/ast_print.h"

#include <assert.h>
#include <stdio.h>

#include "mcc/ast_visit.h"
#include "../include/mcc/ast.h"

const char *mcc_ast_print_unary_op(enum mcc_ast_unary_op op)
{
	switch (op) {
	case MCC_AST_UNARY_OP_MINUS: return "-";
	case MCC_AST_UNARY_OP_NOT: return "!";
	}

	return "unknown op";
}


const char *mcc_ast_print_binary_op(enum mcc_ast_binary_op op)
{
	switch (op) {
		case MCC_AST_BINARY_OP_ADD:
			return "+";
		case MCC_AST_BINARY_OP_SUB:
			return "-";
		case MCC_AST_BINARY_OP_MUL:
			return "*";
		case MCC_AST_BINARY_OP_DIV:
			return "/";
		case MCC_AST_BINARY_OP_AND:
			return "&&";
		case MCC_AST_BINARY_OP_OR:
			return "||";
		case MCC_AST_BINARY_OP_EQUALS:
			return "==";
		case MCC_AST_BINARY_OP_GREATER:
			return ">";
		case MCC_AST_BINARY_OP_GREATER_EQUALS:
			return ">=";
		case MCC_AST_BINARY_OP_LESS:
			return "<";
		case MCC_AST_BINARY_OP_LESS_EQUALS:
			return "<=";
		case MCC_AST_BINARY_OP_NOT_EQUALS:
			return "!=";
	}

	return "unknown op";
}

const char *mcc_ast_print_data_type(enum mcc_ast_data_type dt)
{
	switch (dt) {
		case MCC_AST_DATA_TYPE_INT:
			return "INT";
		case MCC_AST_DATA_TYPE_STRING:
			return "STRING";
		case MCC_AST_DATA_TYPE_BOOL:
			return "BOOL";
		case MCC_AST_DATA_TYPE_FLOAT:
			return "FLOAT";
		case MCC_AST_DATA_TYPE_VOID:
			return "VOID";
	}

	return "unknown data type";
}

const char *mcc_ast_print_statement(enum mcc_ast_statement_type stmt_type)
{
	switch (stmt_type) {
		case MCC_AST_STATEMENT_TYPE_EXPRESSION:
			return "EXPR_STMT";
		case MCC_AST_STATEMENT_TYPE_IF:
			return "IF_STMT";
		case MCC_AST_STATEMENT_TYPE_WHILE:
			return "WHILE_STMT";
		case MCC_AST_STATEMENT_TYPE_DECL:
			return "DECL_STMT";
		case MCC_AST_STATEMENT_TYPE_ASSGN:
			return "ASSGN_STMT";
		case MCC_AST_STATEMENT_TYPE_COMPOUND:
			return "COMPOUND_STMT";
        case MCC_AST_STATEMENT_TYPE_ASSGN_ARR:
            return "TYPE ARR STMT";
	}

	return "unknown statement";
}


// ---------------------------------------------------------------- DOT Printer

#define LABEL_SIZE 64

static void print_dot_begin(FILE *out)
{
	assert(out);

	fprintf(out, "digraph \"AST\" {\n"
	             "\tnodesep=0.6\n");
}

static void print_dot_end(FILE *out)
{
	assert(out);

	fprintf(out, "}\n");
}

static void print_dot_node(FILE *out, const void *node, const char *label)
{
	assert(out);
	assert(node);
	assert(label);

	fprintf(out, "\t\"%p\" [shape=box, label=\"%s\"];\n", node, label);
}

static void print_dot_edge(FILE *out, const void *src_node, const void *dst_node, const char *label)
{
	assert(out);
	assert(src_node);
	assert(dst_node);
	assert(label);

	fprintf(out, "\t\"%p\" -> \"%p\" [label=\"%s\"];\n", src_node, dst_node, label);
}

// ------------------------------------------------------------------- Expression

static void print_dot_expression_literal(struct mcc_ast_expression *expression, void *data)
{
	assert(expression);
	assert(data);

	FILE *out = data;
	print_dot_node(out, expression, "expr: lit");
	print_dot_edge(out, expression, expression->literal, "literal");
}

static void print_dot_expression_unary_op(struct mcc_ast_expression *expression,void *data)
{
	assert(expression);
	assert(data);

	char label[LABEL_SIZE] = { 0 };
	snprintf(label, sizeof(label), "expr: %s",
	         mcc_ast_print_unary_op(expression->unary_op));

	FILE *out = data;
	print_dot_node(out, expression, label);
	print_dot_edge(out, expression, expression->unary_expression, "expr");
}

static void print_dot_expression_binary_op(struct mcc_ast_expression *expression, void *data)
{
	assert(expression);
	assert(data);

	char label[LABEL_SIZE] = {0};
	snprintf(label, sizeof(label), "expr: %s", mcc_ast_print_binary_op(expression->op));

	FILE *out = data;
	print_dot_node(out, expression, label);
	print_dot_edge(out, expression, expression->lhs, "lhs");
	print_dot_edge(out, expression, expression->rhs, "rhs");
}

static void print_dot_expression_identifier(struct mcc_ast_expression *expression, void *data)
{
	assert(expression);
	assert(data);

	FILE *out = data;
	print_dot_node(out, expression, "expr: id");
	print_dot_edge(out, expression, expression->identifier, "identifier");
}

static void print_dot_expression_parenth(struct mcc_ast_expression *expression, void *data)
{
	assert(expression);
	assert(data);

	FILE *out = data;
	print_dot_node(out, expression, "( )");
	print_dot_edge(out, expression, expression->expression, "expression");
}

// ------------------------------------------------------------------- Literal

static void print_dot_literal(struct mcc_ast_literal *literal, void *data)
{
	assert(literal);
	assert(data);

	char label[LABEL_SIZE] = { 0 };
	snprintf(label, sizeof(label), "%s", literal->value);

	FILE *out = data;
	print_dot_node(out, literal, label);
}

// ------------------------------------------------------------------- Identifier

static void print_dot_identifier(struct mcc_ast_identifier *identifier,void *data)
{
	assert(identifier);
	assert(data);

	char label[LABEL_SIZE] = { 0 };
	snprintf(label, sizeof(label), "%s", identifier->i_value);

	FILE *out = data;
	print_dot_node(out, identifier, label);
}

// ------------------------------------------------------------------- Statement

static void print_dot_statement_expression(struct mcc_ast_statement *statement,void *data)
{
	assert(statement);
	assert(data);

	FILE *out = data;
	print_dot_node(out, statement, "stmt: expr");
	print_dot_edge(out, statement, statement->expression, "expression");
}

static void print_dot_statement_if(struct mcc_ast_statement *statement, void *data)
{
	assert(statement);
	assert(data);

	FILE *out = data;
	print_dot_edge(out, statement, statement -> if_condition, "if_condition");
	print_dot_edge(out, statement, statement -> if_stmt, "if_statement");

}

static void print_dot_statement_assignment( struct mcc_ast_statement *statement, void *data)
{
	assert(statement);
	assert(data);
	assert(statement->type == MCC_AST_STATEMENT_TYPE_ASSGN ||
	       statement->type == MCC_AST_STATEMENT_TYPE_ASSGN_ARR);

	FILE *out = data;
	print_dot_edge(out, statement, statement->assignment->identifier,
	               "identifier");
	if (statement->assignment->type == MCC_AST_ASSIGNMENT_TYPE_NORMAL) {
		/* snprintf(label, sizeof(label), "="); */
		print_dot_edge(out, statement, statement->assignment->normal_ass.rhs,
		               "rhs");
	} else {
		/* snprintf(label, sizeof(label), "[]="); */
		print_dot_edge(out, statement, statement->assignment->array_ass.index,
		               "index");
		print_dot_edge(out, statement, statement->assignment->array_ass.rhs,
		               "rhs");
	}
}

static void print_dot_statement_while(struct mcc_ast_statement *statement, void *data)
{
	assert(statement);
	assert(data);

	FILE *out = data;
	print_dot_node(out, statement, "stmt: while");
	print_dot_edge(out, statement, statement->while_condition, "expression");
	print_dot_edge(out, statement, statement->while_stmt, "statement");
}

static void print_dot_statement_declaration(struct mcc_ast_statement *statement,void *data)
{
	assert(statement);
	assert(data);

	FILE *out = data;
	print_dot_node(out, statement, "stmt: decl");
	print_dot_edge(out, statement, statement->declaration, "declaration");
}

//static void
//print_dot_statement_list(struct mcc_ast_statement_list *statement_list,void *data)
//{
//	assert(statement_list);
//	assert(data);
//
//	char label[LABEL_SIZE] = { 0 };
//	snprintf(label, sizeof(label), "statement list");
//
//	FILE *out = data;
//	print_dot_node(out, statement_list, label);
//	print_dot_edge(out, statement_list, statement_list->statement, "statement");
//	if (statement_list->next != NULL) {
//		print_dot_edge(out, statement_list, statement_list->next,
//		               "next statement");
//	}
//}

static void print_dot_statement_compound( struct mcc_ast_statement *statement, void *data)
{
	assert(statement);
	assert(data);
	assert(statement->type == MCC_AST_STATEMENT_TYPE_COMPOUND);

	FILE *out = data;
	if (statement->compound_statement != NULL) {
		print_dot_edge(out, statement, statement->compound_statement, "stmt list");
	}
}

// ------------------------------------------------------------------- Declaration

static void print_dot_declaration(struct mcc_ast_declaration *declaration,
                                  void *data)
{
	assert(declaration);
	assert(data);

	char label[LABEL_SIZE] = { 0 };
	snprintf(label, sizeof(label), "decl: %s",
	         mcc_ast_print_data_type(declaration->type));

	FILE *out = data;
	print_dot_node(out, declaration, label);
	print_dot_edge(out, declaration, declaration->ident, "identifier");
	if (declaration->arr_literal) {
		print_dot_edge(out, declaration, declaration->arr_literal, "array size");
	}
}

// ------------------------------------------------------------------- Parameter

static void print_dot_parameter(struct mcc_ast_parameter *parameter, void *data)
{
	assert(parameter);
	assert(data);

	FILE *out = data;
	print_dot_node(out, parameter, "param: decl");
    for (int i = 0; i < parameter->size; ++i) {
		print_dot_edge(out, parameter, parameter->parameters[i], "declaration");
	}
}

// ------------------------------------------------------------------- Function

static void print_dot_function(struct mcc_ast_function *function, void *data)
{
	assert(function);
	assert(data);

	char label[LABEL_SIZE] = { 0 };
	snprintf(label, sizeof(label), "func: %s",mcc_ast_print_data_type(function->return_type));

	FILE *out = data;
	print_dot_node(out, function, label);
	print_dot_edge(out, function, function->identifier, "identifier");

	if (function->parameter != NULL) {
		print_dot_edge(out, function, function->parameter, "parameter");
	}
	if (function->statement != NULL) {
		print_dot_edge(out, function, function->statement, "body");
	}
}

// ------------------------------------------------------------------- Program

static void print_dot_program(struct mcc_ast_program *program, void *data)
{
	assert(program);
	assert(data);

	FILE *out = data;
	print_dot_node(out, program, "program");
	for (int i = 0; i < program->size; i++) {
		print_dot_edge(out, program, program->function_def[i],
		               "function_definition");
	}
}

// Setup an AST Visitor for printing.
static struct mcc_ast_visitor print_dot_visitor(FILE *out)
{
	assert(out);

	return (struct mcc_ast_visitor){
	    .traversal = MCC_AST_VISIT_DEPTH_FIRST,
	    .order = MCC_AST_VISIT_PRE_ORDER,

	    .userdata = out,

	    .expression_literal = print_dot_expression_literal,
		.expression_unary_op = print_dot_expression_unary_op,
	    .expression_binary_op = print_dot_expression_binary_op,
	    .expression_parenth = print_dot_expression_parenth,
		.expression_identifier = print_dot_expression_identifier,

		.literal = print_dot_literal,
		.identifier = print_dot_identifier,

		.statement_expression = print_dot_statement_expression,
		.statement_assignment = print_dot_statement_assignment,
		.statement_declaration = print_dot_statement_declaration,
		.statement_if = print_dot_statement_if,
		.statement_while = print_dot_statement_while,
		.statement_compound = print_dot_statement_compound,
		
		.declaration = print_dot_declaration,

		.function = print_dot_function,
		.parameter = print_dot_parameter,
		.program = print_dot_program,
	};
}


void mcc_ast_print_dot_expression(FILE *out, struct mcc_ast_expression *expression)
{
	assert(out);
	assert(expression);

	print_dot_begin(out);

	struct mcc_ast_visitor visitor = print_dot_visitor(out);
	mcc_ast_visit(expression, &visitor);

	print_dot_end(out);
}

void mcc_ast_print_dot_literal(FILE *out, struct mcc_ast_literal *literal)
{
	assert(out);
	assert(literal);

	print_dot_begin(out);

	struct mcc_ast_visitor visitor = print_dot_visitor(out);
	mcc_ast_visit(literal, &visitor);

	print_dot_end(out);
}

void mcc_ast_print_dot_statement(FILE *out, struct mcc_ast_statement *statement)
{
	assert(out);
	assert(statement);

	print_dot_begin(out);

	struct mcc_ast_visitor visitor = print_dot_visitor(out);
	mcc_ast_visit_statement(statement, &visitor);

	print_dot_end(out);
}

void mcc_ast_print_dot_declaration(FILE *out, struct mcc_ast_declaration *declaration)
{
	assert(out);
	assert(declaration);

	print_dot_begin(out);

	struct mcc_ast_visitor visitor = print_dot_visitor(out);
	mcc_ast_visit(declaration, &visitor);

	print_dot_end(out);
}

void mcc_ast_print_dot_program(FILE *out, struct mcc_ast_program *program)
{
	assert(out);
    assert(program);

	print_dot_begin(out);

	struct mcc_ast_visitor visitor = print_dot_visitor(out);
	mcc_ast_visit(program, &visitor);

	print_dot_end(out);
}
