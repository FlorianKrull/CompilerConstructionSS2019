#ifndef MCC_TAC_BUILD_H
#define MCC_TAC_BUILD_H

#include "ast.h"
#include <stdio.h>


enum mcc_tac_operation{

    MCC_TAC_BOOL_LITERAL,
	MCC_TAC_INT_LITERAL,
	MCC_TAC_STRING_LITERAL,
	MCC_TAC_FLOAT_LITERAL,
    MCC_TAC_IDENTIFIER,
	MCC_TAC_UNARY_EXPRESSION,
	MCC_TAC_BINARY_EXPRESSION,
    MCC_TAC_NEGATE_INT,
	MCC_TAC_NEGATE_FLOAT,
	MCC_TAC_NOT_BOOL,
	MCC_TAC_PLUS_INT,
	MCC_TAC_PLUS_FLOAT,
	MCC_TAC_MINUS_INT,
	MCC_TAC_MINUS_FLOAT,
	MCC_TAC_MUL_INT,
	MCC_TAC_MUL_FLOAT,
	MCC_TAC_DIV_INT,
	MCC_TAC_DIV_FLOAT,
	MCC_TAC_EQ,
	MCC_TAC_NEQ,
	MCC_TAC_GT,
	MCC_TAC_LT,
	MCC_TAC_LTEQ,
	MCC_TAC_GTEQ,
	MCC_TAC_LT_FLOAT,
	MCC_TAC_LTEQ_FLOAT,
	MCC_TAC_GT_FLOAT,
	MCC_TAC_GTEQ_FLOAT,
	MCC_TAC_EQ_FLOAT,
	MCC_TAC_NEQ_FLOAT,
	MCC_TAC_AND,
	MCC_TAC_OR,
	MCC_TAC_VARIABLE_DECLARATION,
	MCC_TAC_VARIABLE_ASSIGNMENT,
    MCC_TAC_JMP

};

struct mcc_tac{

    enum mcc_tac_operation tac_op;
    char *arg1;
	char *arg2;
	char *result;

    struct mcc_tac *next;
	struct mcc_tac *prev;

};



#endif