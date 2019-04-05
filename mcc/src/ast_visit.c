#include "mcc/ast_visit.h"

#include <assert.h>

#define visit(node, callback, visitor) \
	do { \
		if (callback) { \
			(callback)(node, (visitor)->userdata); \
		} \
	} while (0)

#define visit_if(cond, node, callback, visitor) \
	do { \
		if (cond) { \
			visit(node, callback, visitor); \
		} \
	} while (0)

#define visit_if_pre_order(node, callback, visitor) \
	visit_if((visitor)->order == MCC_AST_VISIT_PRE_ORDER, node, callback, visitor)

#define visit_if_post_order(node, callback, visitor) \
	visit_if((visitor)->order == MCC_AST_VISIT_POST_ORDER, node, callback, visitor)

void mcc_ast_visit_expression(struct mcc_ast_expression *expression, struct mcc_ast_visitor *visitor)
{
	assert(expression);
	assert(visitor);

	visit_if_pre_order(expression, visitor->expression, visitor);

	switch (expression->type) {
	case MCC_AST_EXPRESSION_TYPE_LITERAL:
		visit_if_pre_order(expression, visitor->expression_literal, visitor);
		mcc_ast_visit(expression->literal, visitor);
		visit_if_post_order(expression, visitor->expression_literal, visitor);
		break;

	case MCC_AST_EXPRESSION_TYPE_BINARY_OP:
		visit_if_pre_order(expression, visitor->expression_binary_op, visitor);
		mcc_ast_visit(expression->lhs, visitor);
		mcc_ast_visit(expression->rhs, visitor);
		visit_if_post_order(expression, visitor->expression_binary_op, visitor);
		break;
	
	case MCC_AST_EXPRESSION_TYPE_UNARY_OP:
		visit_if_pre_order(expression, visitor->expression_unary_op,visitor);
		mcc_ast_visit(expression->rhs, visitor);
		visit_if_post_order(expression, visitor->expression_unary_op, visitor);
		break;

	case MCC_AST_EXPRESSION_TYPE_PARENTH:
		visit_if_pre_order(expression, visitor->expression_parenth, visitor);
		mcc_ast_visit(expression->expression, visitor);
		visit_if_post_order(expression, visitor->expression_parenth, visitor);
		break;

	case MCC_AST_EXPRESSION_TYPE_IDENTIFIER:
		visit_if_pre_order(expression, visitor->expression_identifier, visitor);
		mcc_ast_visit_identifier(expression->identifier, visitor);
		visit_if_post_order(expression, visitor->expression_identifier,
		                    visitor);
		break;
	

	}


	visit_if_post_order(expression, visitor->expression, visitor);
}

void mcc_ast_visit_literal(struct mcc_ast_literal *literal, struct mcc_ast_visitor *visitor)
{
	assert(literal);
	assert(visitor);

	visit_if_pre_order(literal, visitor->literal, visitor);

	switch (literal->type) {
	case MCC_AST_LITERAL_TYPE_INT:
		visit(literal, visitor->literal_int, visitor);
		break;

	case MCC_AST_LITERAL_TYPE_FLOAT:
		visit(literal, visitor->literal_float, visitor);
		break;
	
	case MCC_AST_LITERAL_TYPE_STRING:
		visit(literal, visitor->literal_string,visitor);
		break;

	case MCC_AST_LITERAL_TYPE_BOOL:
		visit(literal, visitor->literal_bool,visitor);
	}

	visit_if_post_order(literal, visitor->literal, visitor);
}

void mcc_ast_visit_declaration(struct mcc_ast_declaration *declaration, struct mcc_ast_visitor *visitor)
{
	assert(declaration);
	assert(visitor);

	visit_if_pre_order(declaration, visitor->declaration, visitor);
	if (declaration->array_size) {
		mcc_ast_visit_literal(declaration->array_size, visitor);
	}

	mcc_ast_visit_identifier(declaration -> identifier, visitor);


	visit_if_post_order(declaration, visitor->declaration, visitor);
}

void mcc_ast_visit_identifier(struct mcc_ast_identifier *identifier, struct mcc_ast_visitor *visitor)
{
	assert(identifier);
	assert(visitor);

	visit(identifier, visitor -> identifier, visitor);
}

void mcc_ast_visit_statement(struct mcc_ast_statement *statement , struct mcc_ast_visitor *visitor)
{
	if(statement) {

		assert(visitor);

		visit_if_pre_order(statement, visitor->statement, visitor);

		switch (statement->type) {

		case MCC_AST_STATEMENT_TYPE_IF:
			visit_if_pre_order(statement, visitor->statement_if, visitor);
			mcc_ast_visit_expression(statement->if_condition, visitor);

			mcc_ast_visit_statement(statement->if_stmt, visitor);

			visit_if_post_order(statement, visitor->statement_if, visitor);
			break;

		case MCC_AST_STATEMENT_TYPE_WHILE:
			visit_if_pre_order(statement, visitor->statement_while, visitor);
			mcc_ast_visit_expression(statement->while_condition, visitor);

			mcc_ast_visit_statement(statement->while_stmt, visitor);

			visit_if_post_order(statement, visitor->statement_while, visitor);
			break;

		case MCC_AST_STATEMENT_TYPE_DECL:
			visit_if_pre_order(statement, visitor->statement_declaration,
			                   visitor);
			mcc_ast_visit_declaration(statement->declaration, visitor);
			visit_if_post_order(statement, visitor->statement_declaration,
			                    visitor);
			break;

		case MCC_AST_STATEMENT_TYPE_ASSGN:
			visit_if_pre_order(statement, visitor->statement_assignment,
			                   visitor);
			mcc_ast_visit_identifier(statement->id_assgn, visitor);
			
			mcc_ast_visit_expression(statement->rhs_assgn, visitor);
			visit_if_post_order(statement, visitor->statement_assignment,
			                    visitor);
			break;

		case MCC_AST_STATEMENT_TYPE_EXPRESSION:
			visit_if_pre_order(statement, visitor->statement_expression,
			                   visitor);
			mcc_ast_visit_expression(statement->expression, visitor);
			visit_if_post_order(statement, visitor->statement_expression,
			                    visitor);
			break;

		case MCC_AST_STATEMENT_TYPE_COMPOUND:
			visit_if_pre_order(statement, visitor->statement_compound, visitor);
	
			visit_if_post_order(statement, visitor->statement_compound,
			                    visitor);
			break;
		}

		visit_if_post_order(statement, visitor->statement, visitor);

	}
}
