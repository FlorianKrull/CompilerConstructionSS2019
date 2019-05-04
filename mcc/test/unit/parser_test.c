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

void StatementWhile (CuTest *tc)
{

	const char input[] = "while (i <= 2) { i = i + 1}" ;

	struct mcc_parser_result result = mcc_parse_string(input);

	CuAssertIntEquals(tc, MCC_PARSER_STATUS_OK, result.status);

	struct mcc_ast_statement *stmt = result.statement;

  CuAssertIntEquals(tc, MCC_AST_STATEMENT_TYPE_WHILE, stmt -> type );
	CuAssertTrue(tc, NULL == result.statement);

}
void StatementIf (CuTest *tc)
{

	const char input[] = "if (i == 2) { i = i + 1}" ;

	struct mcc_parser_result result = mcc_parse_string(input);

	CuAssertIntEquals(tc, MCC_PARSER_STATUS_OK, result.status);

	struct mcc_ast_statement *stmt = result.statement;

	CuAssertIntEquals(tc, MCC_AST_STATEMENT_TYPE_IF, stmt -> type );
	CuAssertTrue(tc, NULL == result.statement);

	struct mcc_ast_statement *expr = result.expression;

	CuAssertIntEquals(tc, MCC_AST_STATEMENT_TYPE_IF, expr -> type );
	CuAssertTrue(tc, NULL == result.expression);
}


void StatementIfElse(CuTest *tc){


	const char input[] = "if (i != 2) i = i + 1; else i==0;" ;

	struct mcc_parser_result result = mcc_parse_string(input);

	struct mcc_ast_statement *stmt = result.statement;

	CuAssertIntEquals(tc, MCC_AST_STATEMENT_TYPE_IF, stmt -> type );

	CuAssertTrue(tc, NULL == result.statement);

}

void StatementDeclarationInt(CuTest *tc){

	const char input[] = "int name" ;

	struct mcc_parser_result result = mcc_parse_string(input);

	CuAssertIntEquals(tc, MCC_PARSER_STATUS_OK, result.status);

	struct mcc_ast_declaration *decl = result.declaration;

	CuAssertIntEquals(tc, MCC_AST_STATEMENT_TYPE_DECL, decl -> type );
	CuAssertIntEquals(tc, decl -> type, MCC_AST_LITERAL_TYPE_INT );
	CuAssertIntEquals(tc, decl -> ident , "name");

}

void StatementDeclarationFloat(CuTest *tc){

	const char input[] = "float bar" ;

	struct mcc_parser_result result = mcc_parse_string(input);

	CuAssertIntEquals(tc, MCC_PARSER_STATUS_OK, result.status);

	struct mcc_ast_declaration *decl = result.declaration;

	CuAssertIntEquals(tc, MCC_AST_STATEMENT_TYPE_DECL, decl -> type );
	CuAssertIntEquals(tc, decl -> type, MCC_AST_LITERAL_TYPE_FLOAT);
	CuAssertIntEquals(tc, decl -> ident , "bar");

}
void StatementDeclarationString(CuTest *tc){

	const char input[] = "string hello" ;

	struct mcc_parser_result result = mcc_parse_string(input);

	CuAssertIntEquals(tc, MCC_PARSER_STATUS_OK, result.status);

	struct mcc_ast_declaration *decl = result.declaration;

	CuAssertIntEquals(tc, MCC_AST_STATEMENT_TYPE_DECL, decl -> type );
	CuAssertIntEquals(tc, decl -> type, MCC_AST_LITERAL_TYPE_STRING);
	CuAssertIntEquals(tc, decl -> ident  , "string");

}

void StatementRet(CuTest *tc){

	const char input[] = "return 1" ;

	struct mcc_parser_result result = mcc_parse_string(input);

	CuAssertIntEquals(tc, MCC_PARSER_STATUS_OK, result.status);

	struct mcc_ast_statement *stmt = result.statement;

	CuAssertIntEquals(tc, MCC_AST_STATEMENT_TYPE_DECL, stmt -> type );
	CuAssertIntEquals(tc, stmt -> type, MCC_AST_LITERAL_TYPE_INT);
	CuAssertTrue(tc, NULL == result.statement);

}

void StatementAssignment(CuTest *tc){

	const char input[] = " a = 12" ;

	struct mcc_parser_result result = mcc_parse_string(input);

	CuAssertIntEquals(tc, MCC_PARSER_STATUS_OK, result.status);

	struct mcc_ast_statement *stmt = result.statement;

	CuAssertIntEquals(tc, MCC_AST_STATEMENT_TYPE_ASSGN, stmt -> type );

	CuAssertTrue(tc, NULL == result.statement);

}



void SourceLocation_SingleLineColumn(CuTest *tc)
{
	const char input[] = "(42 + 192)";
	struct mcc_parser_result result = mcc_parse_string(input);

	CuAssertIntEquals(tc, MCC_PARSER_STATUS_OK, result.status);

	struct mcc_ast_expression *expr = result.expression;

	CuAssertIntEquals(tc, MCC_AST_EXPRESSION_TYPE_PARENTH, expr->type);
	CuAssertIntEquals(tc, 1, expr->node.sloc.start_col);

	CuAssertIntEquals(tc, MCC_AST_EXPRESSION_TYPE_BINARY_OP, expr->expression->type);
	CuAssertIntEquals(tc, 2, expr->expression->node.sloc.start_col);

	CuAssertIntEquals(tc, MCC_AST_LITERAL_TYPE_INT, expr->expression->lhs->literal->type);
	CuAssertIntEquals(tc, 2, expr->expression->lhs->literal->node.sloc.start_col);

	CuAssertIntEquals(tc, MCC_AST_LITERAL_TYPE_INT, expr->expression->rhs->literal->type);
	CuAssertIntEquals(tc, 7, expr->expression->rhs->literal->node.sloc.start_col);

	mcc_ast_delete(expr);
}

#define TESTS \
    TEST(MissingClosingParenthesis_1) \
    TEST(SourceLocation_SingleLineColumn)\
    TEST(StatementWhile)\
    TEST(StatementIf)\
    TEST(StatementIfElse)\
    TEST(StatementAssignment)\
    TEST(StatementRet)\
    TEST(StatementDeclarationString)\
    TEST(StatementDeclarationFloat)\
    TEST(StatementDeclarationInt)\

#include "main_stub.inc"
#undef TESTS
