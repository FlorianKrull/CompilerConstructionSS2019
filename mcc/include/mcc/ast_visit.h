// AST Visitor Infrastructure
//
// This module defines a visitor utility for traversing the AST.
//
// Instantiate the `mcc_ast_visitor` struct with the desired configuration and
// callbacks. Use this instance with the functions declared below. Each
// callback is optional, just set it to NULL.

#ifndef MCC_AST_VISIT_H
#define MCC_AST_VISIT_H

#include "mcc/ast.h"

enum mcc_ast_visit_traversal {
	MCC_AST_VISIT_DEPTH_FIRST,
	// TODO: MCC_AST_VISIT_BREADTH_FIRST,
};

enum mcc_ast_visit_order {
	MCC_AST_VISIT_PRE_ORDER,
	MCC_AST_VISIT_POST_ORDER,
};

// Callbacks
typedef void (*mcc_ast_visit_expression_cb)(struct mcc_ast_expression *, void *userData);
typedef void (*mcc_ast_visit_literal_cb)(struct mcc_ast_literal *, void *userData);
typedef void (*mcc_ast_visit_declaration_cb)(struct mcc_ast_declaration *, void *userData);
typedef void (*mcc_ast_visit_identifier_cb)(struct mcc_ast_identifier *, void *userData);
typedef void (*mcc_ast_visit_statement_cb)(struct mcc_ast_statement *, void *userData);
typedef void (*mcc_ast_visit_statement_list_cb)(struct mcc_ast_statement_list *,void *userData);
typedef void (*mcc_ast_visit_assignment_cb)(struct mcc_ast_assignment *, void *userData);
typedef void (*mcc_ast_visit_function_cb)(struct mcc_ast_function *, void *userData);
typedef void (*mcc_ast_visit_parameter_cb)(struct mcc_ast_parameter *, void *userData);
typedef void (*mcc_ast_visit_argument_cb)(struct mcc_ast_argument *, void *userData);
typedef void (*mcc_ast_visit_program_cb)(struct mcc_ast_program *, void *userData);

struct mcc_ast_visitor {
	enum mcc_ast_visit_traversal traversal;
	enum mcc_ast_visit_order order;

	// This will be passed to every callback along with the corresponding AST
	// node. Use it to share data while traversing the tree.
	void *userdata;

	mcc_ast_visit_expression_cb expression;
	mcc_ast_visit_expression_cb expression_literal;
	mcc_ast_visit_expression_cb expression_binary_op;
	mcc_ast_visit_expression_cb expression_unary_op;
	mcc_ast_visit_expression_cb expression_identifier;
	mcc_ast_visit_expression_cb expression_parenth;
	mcc_ast_visit_expression_cb expression_call_expression;

	mcc_ast_visit_statement_cb statement;
	mcc_ast_visit_statement_cb statement_if;
	mcc_ast_visit_statement_cb statement_if_else;
	mcc_ast_visit_statement_cb statement_assignment;
	mcc_ast_visit_statement_cb statement_expression;
	mcc_ast_visit_statement_cb statement_while;
	mcc_ast_visit_statement_cb statement_compound;
	mcc_ast_visit_statement_cb statement_declaration;
	mcc_ast_visit_statement_cb statement_return;

	mcc_ast_visit_statement_list_cb statement_list;

	mcc_ast_visit_literal_cb literal;
	mcc_ast_visit_literal_cb literal_bool;
	mcc_ast_visit_literal_cb literal_int;
	mcc_ast_visit_literal_cb literal_float;
	mcc_ast_visit_literal_cb literal_string;
	mcc_ast_visit_declaration_cb declaration;
	mcc_ast_visit_assignment_cb assignment;
	mcc_ast_visit_identifier_cb identifier;
	mcc_ast_visit_argument_cb argument;

	mcc_ast_visit_function_cb function;
	mcc_ast_visit_parameter_cb parameter;
	mcc_ast_visit_program_cb program;
};

void mcc_ast_visit_expression(struct mcc_ast_expression *expression, struct mcc_ast_visitor *visitor);

void mcc_ast_visit_literal(struct mcc_ast_literal *literal, struct mcc_ast_visitor *visitor);

void mcc_ast_visit_declaration(struct mcc_ast_declaration *declaration, struct mcc_ast_visitor *visitor);

void mcc_ast_visit_assignment(struct mcc_ast_assignment *assignment, struct mcc_ast_visitor *visitor);

void mcc_ast_visit_identifier(struct mcc_ast_identifier *identifier, struct mcc_ast_visitor *visitor);

void mcc_ast_visit_statement(struct mcc_ast_statement *statement,struct mcc_ast_visitor *visitor);

void mcc_ast_visit_statement_list(struct mcc_ast_statement_list *statement_list,struct mcc_ast_visitor *visitor);

void mcc_ast_visit_parameter(struct mcc_ast_parameter *parameter, struct mcc_ast_visitor *visitor);

void mcc_ast_visit_argument(struct mcc_ast_argument *argument, struct mcc_ast_visitor *visitor);

void mcc_ast_visit_function(struct mcc_ast_function *function, struct mcc_ast_visitor *visitor);

void mcc_ast_visit_program(struct mcc_ast_program *program, struct mcc_ast_visitor *visitor);

// clang-format off

#define mcc_ast_visit(x, visitor) _Generic((x), \
		struct mcc_ast_expression *: 	mcc_ast_visit_expression, \
		struct mcc_ast_literal *:    	mcc_ast_visit_literal, \
		struct mcc_ast_declaration *:   mcc_ast_visit_declaration, \
		struct mcc_ast_identifier *:	mcc_ast_visit_identifier, \
		struct mcc_ast_statement *:		mcc_ast_visit_statement, \
		struct mcc_ast_parameter *: 	mcc_ast_visit_parameter, \
		struct mcc_ast_function *: 		mcc_ast_visit_function, \
		struct mcc_ast_program *:		mcc_ast_visit_program \
	)(x, visitor)

// clang-format on

#endif // MCC_AST_VISIT_H
