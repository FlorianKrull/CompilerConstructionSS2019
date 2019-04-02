#include "mcc/ast_visit.h"

#include "assert.h"

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
    visit_if((visitor)->order == MCC_AST_VISIT_PRE_ORDER, node, callback, visitor);

#define visit_if_post_order(node, callback, visitor) \
    visit_if((visitor)->order == MCC_AST_VISIT_POST_ORDER, node, callback, visitor);





void mcc_ast_visit_expression(struct mcc_ast_expression *expression, struct mcc_ast_visitor *visitor)
{
	assert(expression);
	assert(visitor);

	visit_if_pre_order(expression, visitor->expression, visitor);

	switch (expression->type) {
	case MCC_AST_EXPRESSION_TYPE_LITERAL:
		visit_if_pre_order(expression, visitor->expression_literal, visitor);
		mcc_ast_visit_expression(expression->literal, visitor);
		visit_if_post_order(expression, visitor->expression_literal, visitor);
		break;

	case MCC_AST_EXPRESSION_TYPE_BINARY_OP:
		visit_if_pre_order(expression, visitor->expression_binary_op, visitor);
		mcc_ast_visit_expression(expression->lhs, visitor);
		mcc_ast_visit_expression(expression->rhs, visitor);
		visit_if_post_order(expression, visitor->expression_binary_op, visitor);
		break;
	
	case MCC_AST_EXPRESSION_TYPE_UNARY_OP:
		visit_if_pre_order(expression, visitor->expression_unary_op,visitor);
		mcc_ast_visit_expression(expression->rhs, visitor);
		visit_if_post_order(expression, visitor->expression_unary_op, visitor);
		break;


	case MCC_AST_EXPRESSION_TYPE_PARENTH:
		visit_if_pre_order(expression, visitor->expression_parenth, visitor);
		mcc_ast_visit_expression(expression->expression, visitor);
		visit_if_post_order(expression, visitor->expression_parenth, visitor);
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
	};

	visit_if_post_order(literal, visitor->literal, visitor);
}

void mcc_ast_visit_declaration(struct mcc_ast_declaration *declaration, struct mcc_ast_visitor *visitor)
{
	assert(declaration);
	assert(visitor);

	visit_if_pre_order(declaration, visitor->declaration, visitor);

	mcc_ast_visit_identifier(declaration -> ident, visitor);


	visit_if_post_order(declaration, visitor->declaration, visitor);
}

void mcc_ast_visit_identifier(struct mcc_ast_identifier *identifier, struct mcc_ast_visitor *visitor)
{
	assert(identifier);
	assert(visitor);

	visit(identifier, visitor -> identifier, visitor);
}




void mcc_ast_visit_statement(struct mcc_ast_statement * statement, struct mcc_ast_visitor *visitor) {

	assert(statement);
	assert(visitor);


	switch (statement->type) {


		case (MCC_AST_STATEMENT_TYPE_IF):

			visit_if_pre_order(statement-> if_stmt, visitor->statement_if, visitor);

			mcc_ast_visit_statement(statement-> if_stmt, visitor);
			mcc_ast_visit_statement(statement -> else_stmt, visitor);

			visit_if_post_order(statement-> if_stmt, visitor->statement_if, visitor);


		case (MCC_AST_STATEMENT_TYPE_WHILE):


			visit_if_pre_order(statement-> while_stmt, visitor->statement_while, visitor);

			mcc_ast_visit_statement(statement-> while_stmt, visitor);

			visit_if_post_order(statement-> while_stmt, visitor->statement_while, visitor);


		case (MCC_AST_STATEMENT_TYPE_EXPR):

			visit_if_pre_order(statement-> expression, visitor->expression, visitor);

			mcc_ast_visit_statement(statement-> expression, visitor);

			visit_if_post_order(statement-> expression, visitor->expression, visitor);


		case (MCC_AST_STATEMENT_TYPE_COMPOUND):

			visit_if_pre_order(statement->compound_statement, visitor->statement_compound, visitor);

			mcc_ast_visit_statement(statement->compound_statement, visitor);

			visit_if_post_order(statement->compound_statement, visitor->statement_compound, visitor);


		case (MCC_AST_STATEMENT_TYPE_ASSGN):

			visit_if_pre_order(statement->id_assgn, visitor->statement_assignment, visitor);

			mcc_ast_visit_statement(statement->id_assgn, visitor);

			visit_if_post_order(statement->id_assgn, visitor->statement_assignment, visitor);


		case (MCC_AST_STATEMENT_TYPE_DECL):

			visit_if_pre_order(statement->id_decl, visitor->declaration, visitor);

			mcc_ast_visit_statement(statement->id_decl, visitor);

			visit_if_post_order(statement->id_decl, visitor->declaration, visitor);

		//case (MCC_):

	}


}



void mcc_ast_visit_compound_statement(struct mcc_ast_compund_statement *compound_statement, struct mcc_ast_visitor *visitor){

           assert(compound_statement);
	       assert(visitor);


	      visit_if_pre_order(compound_statement, visitor->statement_compound, visitor);

	      mcc_ast_visit_compound_statement(compound_statement, visitor);

	      visit_if_post_order(compound_statement, visitor -> statement_compound, visitor);


       }


void mcc_ast_visit_if_statement(struct mcc_ast_if_statement *if_statement, struct mcc_ast_visitor *visitor){

	assert(if_statement);
	assert(visitor);

	visit_if_pre_order(if_statement, visitor ->statement_if, visitor);

	mcc_ast_visit_if_statement(if_statement,visitor);

	visit_if_post_order(if_statement, visitor ->statement_if, visitor);


}


void mcc_ast_visit_while_statement(struct mcc_ast_while_statement *while_statement, struct mcc_ast_visitor * visitor){

	assert(while_statement);
	assert(visitor);


	visit_if_pre_order(while_statement, visitor -> statement_while, visitor);

	mcc_ast_visit_if_statement(while_statement,visitor);

	visit_if_post_order(while_statement, visitor -> statement_while, visitor);


}


void mcc_ast_visit_parameters(struct mcc_ast_parameters *parameters, struct mcc_ast_visitor *visitor){

   assert(parameters);
   assert(visitor);

   visit_if_pre_order(parameters, visitor ->parameters, visitor);
   mcc_ast_visit_parameters(parameters,visitor);
   visit_if_post_order(parameters, visitor ->parameters , visitor);


}