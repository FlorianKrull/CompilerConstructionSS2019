#include <CuTest.h>

#include "mcc/ast.h"
#include "mcc/parser.h"

static const double EPS = 1e-3;

void BinaryOp_1(CuTest *tc)
{
	const char input[] = "192 + 3.14";
	struct mcc_parser_result result = mcc_parse_string(input);

	CuAssertIntEquals(tc, MCC_PARSER_STATUS_OK, result.status);

	struct mcc_ast_expression *expr = result.expression;

	// root
	CuAssertIntEquals(tc, MCC_AST_EXPRESSION_TYPE_BINARY_OP, expr->type);
	CuAssertIntEquals(tc, MCC_AST_BINARY_OP_ADD, expr->op);

	// root -> lhs
	CuAssertIntEquals(tc, MCC_AST_EXPRESSION_TYPE_LITERAL, expr->lhs->type);

	// root -> lhs -> literal
	CuAssertIntEquals(tc, MCC_AST_LITERAL_TYPE_INT, expr->lhs->literal->type);
	CuAssertIntEquals(tc, 192, expr->lhs->literal->i_value);

	// root -> rhs
	CuAssertIntEquals(tc, MCC_AST_EXPRESSION_TYPE_LITERAL, expr->rhs->type);

	// root -> rhs -> literal
	CuAssertIntEquals(tc, MCC_AST_LITERAL_TYPE_FLOAT, expr->rhs->literal->type);
	CuAssertDblEquals(tc, 3.14, expr->rhs->literal->f_value, EPS);

	mcc_ast_delete(expr);
}

void NestedExpression_1(CuTest *tc)
{
	const char input[] = "42 * (192 + 3.14)";
	struct mcc_parser_result result = mcc_parse_string(input);

	CuAssertIntEquals(tc, MCC_PARSER_STATUS_OK, result.status);

	struct mcc_ast_expression *expr = result.expression;

	// root
	CuAssertIntEquals(tc, MCC_AST_EXPRESSION_TYPE_BINARY_OP, expr->type);
	CuAssertIntEquals(tc, MCC_AST_BINARY_OP_MUL, expr->op);

	// root -> lhs
	CuAssertIntEquals(tc, MCC_AST_EXPRESSION_TYPE_LITERAL, expr->lhs->type);

	// root -> lhs -> literal
	CuAssertIntEquals(tc, MCC_AST_LITERAL_TYPE_INT, expr->lhs->literal->type);
	CuAssertIntEquals(tc, 42, expr->lhs->literal->i_value);

	// root -> rhs
	CuAssertIntEquals(tc, MCC_AST_EXPRESSION_TYPE_PARENTH, expr->rhs->type);

	struct mcc_ast_expression *subexpr = expr->rhs->expression;

	// subexpr
	CuAssertIntEquals(tc, MCC_AST_EXPRESSION_TYPE_BINARY_OP, subexpr->type);
	CuAssertIntEquals(tc, MCC_AST_BINARY_OP_ADD, subexpr->op);

	// subexpr -> lhs
	CuAssertIntEquals(tc, MCC_AST_EXPRESSION_TYPE_LITERAL, subexpr->lhs->type);

	// subexpr -> lhs -> literal
	CuAssertIntEquals(tc, MCC_AST_LITERAL_TYPE_INT, subexpr->lhs->literal->type);
	CuAssertIntEquals(tc, 192, subexpr->lhs->literal->i_value);

	// subexpr -> rhs
	CuAssertIntEquals(tc, MCC_AST_EXPRESSION_TYPE_LITERAL, subexpr->rhs->type);

	// subexpr -> rhs -> literal
	CuAssertIntEquals(tc, MCC_AST_LITERAL_TYPE_FLOAT, subexpr->rhs->literal->type);
	CuAssertIntEquals(tc, 3.14, subexpr->rhs->literal->f_value);

	mcc_ast_delete(expr);
}

void MissingClosingParenthesis_1(CuTest *tc)
{
	//TODO: fix memory leak

	const char input[] = "(42";
	struct mcc_parser_result result = mcc_parse_string(input);

	CuAssertTrue(tc, MCC_PARSER_STATUS_OK != result.status);
	CuAssertTrue(tc, NULL == result.expression);

	
}


void If_Statement(CuTest *tc)
{

	const char input[] = "if (i == iterations) 1;";
	struct mcc_parser_result result = mcc_parse_string(input);

	CuAssertIntEquals(tc, MCC_PARSER_STATUS_OK, result.status);


	struct mcc_ast_statement *stmt = result.statement;

	// root
	CuAssertIntEquals(tc,MCC_AST_STATEMENT_TYPE_IF, stmt->type);

	// root -> if_expression
	CuAssertIntEquals(tc, MCC_AST_EXPRESSION_TYPE_BINARY_OP, stmt->if_condition->type);


	CuAssertIntEquals(tc,MCC_AST_EXPRESSION_TYPE_IDENTIFIER, stmt->if_condition->lhs->type);
	CuAssertIntEquals(tc,MCC_AST_EXPRESSION_TYPE_IDENTIFIER, stmt->if_condition->rhs->type);
	CuAssertIntEquals(tc,MCC_AST_BINARY_OP_EQUALS, stmt->if_condition->op);

	// root -> if_statement
	CuAssertIntEquals(tc,MCC_AST_STATEMENT_TYPE_EXPRESSION, stmt->if_stmt->type);

	// root -> if_statement -> expression
	CuAssertIntEquals(tc,MCC_AST_EXPRESSION_TYPE_LITERAL,
						stmt->if_stmt->expression->type);

	// root -> if_statement -> expression -> literal
	CuAssertIntEquals(tc,MCC_AST_LITERAL_TYPE_INT,
						stmt->if_stmt->expression->literal->type);
	CuAssertTrue(tc,strcmp("1", stmt->if_stmt->expression->literal->s_value));

	// root -> else_statement
	CuAssertIntEquals(tc, NULL, stmt->else_stmt);

	
}


#define TESTS \
    TEST(MissingClosingParenthesis_1) \
		TEST(BinaryOp_1) \
		TEST(NestedExpression_1) \
		TEST(If_Statement) \
    
  

#include "main_stub.inc"
#undef TESTS
