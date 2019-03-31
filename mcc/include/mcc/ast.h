// Abstract Syntax Tree (AST)
//
// Here we define the AST data structure of the compiler. It mainly consists of
// *tagged unions* for node types and enums for operators.
//
// In addition to the node type specific members, each node features a common
// member `mmc_ast_node` which serves as a *base-class*. It holds data
// independent from the actual node type, like the source location.
//
// Also note that this makes excessive use of C11's *anonymous structs and
// unions* feature.

// library to support boolean data type
#include <stdbool.h>

#ifndef MCC_AST_H
#define MCC_AST_H

// Forward Declarations
struct mcc_ast_expression;
struct mcc_ast_literal;
struct mcc_ast_statement;
struct mcc_ast_identifier;


// ------------------------------------------------------------------- AST Node

struct mcc_ast_source_location {
	int start_line;
	int start_col;
	int end_line;
	int end_col;
};


struct mcc_ast_node {
	struct mcc_ast_source_location sloc;
};

// -------------------------------------------------------------------- Types



enum mcc_ast_data_type {
    MCC_AST_DATA_TYPE_INT,
    MCC_AST_DATA_TYPE_STRING,
    MCC_AST_DATA_TYPE_BOOL,
    MCC_AST_DATA_TYPE_FLOAT
};

// ------------------------------------------------------------------ Operators

enum mcc_ast_binary_op {
	MCC_AST_BINARY_OP_ADD,
	MCC_AST_BINARY_OP_SUB,
	MCC_AST_BINARY_OP_MUL,
	MCC_AST_BINARY_OP_DIV,
    MCC_AST_BINARY_OP_AND,
    MCC_AST_BINARY_OP_OR,
    MCC_AST_BINARY_OP_EQUALS,
    MCC_AST_BINARY_OP_NOT_EQUALS,
    MCC_AST_BINARY_OP_LESS,
    MCC_AST_BINARY_OP_GREATER,
    MCC_AST_BINARY_OP_LESS_EQUALS,
    MCC_AST_BINARY_OP_GREATER_EQUALS,
};


enum mcc_ast_unary_op {
    MCC_AST_UNARY_OP_NOT,
    MCC_AST_UNARY_OP_MINUS
};

// ---------------------------------------------------------------- Expressions

enum mcc_ast_expression_type {
    MCC_AST_STATEMENT_TYPE_EXPR,
	MCC_AST_EXPRESSION_TYPE_LITERAL,
	MCC_AST_EXPRESSION_TYPE_BINARY_OP,
	MCC_AST_EXPRESSION_TYPE_UNARY_OP,
	MCC_AST_EXPRESSION_TYPE_PARENTH,
	MCC_AST_EXPRESSION_TYPE_IDENTIFIER,

};


struct mcc_ast_expression {
	struct mcc_ast_node node;

	enum mcc_ast_expression_type type;
	union {
		// MCC_AST_EXPRESSION_TYPE_LITERAL
		struct mcc_ast_literal *literal;

		// MCC_AST_EXPRESSION_TYPE_BINARY_OP
		struct {
			enum mcc_ast_binary_op op;
            enum mcc_ast_unary_op up;
			struct mcc_ast_expression *lhs;
			struct mcc_ast_expression *rhs;
		};

        // MCC_AST_EXPRESSION_TYPE_UNARY_OP
		// MCC_AST_EXPRESSION_TYPE_PARENTH
		struct mcc_ast_expression *expression;
	};
};


struct mcc_ast_expression *mcc_ast_new_expression_literal(struct mcc_ast_literal *literal);

struct mcc_ast_expression *mcc_ast_new_expression_binary_op(enum mcc_ast_binary_op op,
                                                            struct mcc_ast_expression *lhs,
                                                            struct mcc_ast_expression *rhs);

struct mcc_ast_expression *mcc_ast_new_exprssion_unary_op( enum mcc_ast_unary_op op,
														   struct mcc_ast_expression *rhs);

struct mcc_ast_expression *mcc_ast_new_expression_parenth(struct mcc_ast_expression *expression);

// struct mcc_ast_expression *mcc_ast_new_expression_call(struct mcc_ast_identifier *identifier,
// 													   struct mcc_ast_argument *argument );

void mcc_ast_delete_expression(struct mcc_ast_expression *expression);

// ------------------------------------------------------------------- Arguments

// struct mcc_ast_arguments{

// 	struct mcc_ast_node;
// 	struct mcc_ast_arguments *next_arg;
// 	struct mcc_ast_expression *expression;
// }



// ------------------------------------------------------------------- Identifier

struct mcc_ast_identifier {

	struct mcc_ast_node node;
	char *i_value;
};

struct mcc_ast_identifier *mcc_ast_new_identifier(char *value);


// ------------------------------------------------------------------- Declaration

struct mcc_ast_declaration {
	struct mcc_ast_node *node;

	enum mcc_ast_data_type type;

	struct mcc_ast_identifier *ident;

};

struct mcc_ast_declaration *mcc_ast_new_declaration(enum mcc_ast_data_type type, struct mcc_ast_identifier *ident);


// ------------------------------------------------------------------- Statements

enum mcc_ast_statement_type {
    MMC_AST_STATEMENT_TYPE_EXPRESSION,
    MCC_AST_STATEMENT_TYPE_IF,
	MCC_AST_STATEMENT_TYPE_WHILE,
	MCC_AST_STATEMENT_TYPE_DECL,
	MCC_AST_STATEMENT_TYPE_ASSGN,
	MCC_AST_STATEMENT_TYPE_COMPOUND,
	// MCC_AST_STATEMENT_TYPE_BLOCK
};

struct mcc_ast_statement_list {
    int size;
    int max_size;
    struct mcc_ast_statement *list[];
};

struct mcc_ast_statement {
    struct mcc_ast_node node;

    enum mcc_ast_statement_type  type;

    union {
        struct mcc_ast_expression *expression;

		struct {
            enum mcc_ast_data_type data_type;
            struct mcc_ast_identifier *id_decl;
        };

		struct {
			struct mcc_ast_expression *if_condition;
			struct mcc_ast_statement *if_stmt;
			struct mcc_ast_statement *else_stmt;
		};

		struct {
			struct mcc_ast_expression *while_condition;
			struct mcc_ast_statement *while_stmt;
		};

		struct {
			struct mcc_ast_identifier *id_assgn;
			struct mcc_ast_expression *lhs_assgn;
			struct mcc_ast_expression *rhs_assgn;
		};

        struct mcc_ast_statement_list *compound_statement;
    };
};

struct mcc_ast_statement *mcc_ast_new_statement_expression (struct mcc_ast_expression *expression);

struct mcc_ast_statement *mcc_ast_new_statement_if(struct mcc_ast_expression *condition,
												   struct mcc_ast_statement *if_stmt,
                                                   struct mcc_ast_statement *else_stmt);

struct mcc_ast_statement *mcc_ast_new_statement_while(struct mcc_ast_expression *condition,
												   	  struct mcc_ast_statement *while_stmt);

struct mcc_ast_statement *mcc_ast_new_statement_assignment(struct mcc_ast_identifier *id_assgn,
														   struct mcc_ast_expression *lhs_assgn,
														   struct mcc_ast_expression *rhs_assgn );

struct mcc_ast_statement *mcc_ast_new_statement_declaration(enum mcc_ast_data_type data_type,
															struct mcc_ast_identifier *identifier);

struct mcc_ast_statement *mcc_ast_new_statement_statement_list(struct mcc_ast_statement_list *statement_list,
                                                                struct mcc_ast_statement *next_statement);




struct mcc_ast_statement mcc_ast_new_block_statement();

// ------------------------------------------------------------------- Literals

enum mcc_ast_literal_type {
	MCC_AST_LITERAL_TYPE_INT,
	MCC_AST_LITERAL_TYPE_FLOAT,
	MCC_AST_LITERAL_TYPE_STRING,
	MCC_AST_LITERAL_TYPE_BOOL
};

struct mcc_ast_literal {
	struct mcc_ast_node node;

	enum mcc_ast_literal_type type;
	union {
		// MCC_AST_LITERAL_TYPE_INT
		long i_value;

		// MCC_AST_LITERAL_TYPE_FLOAT
		double f_value;

		// MCC_AST_LITERAL_TYPE_STRING
		char* s_value;

		// MCC_AST_LITERAL_TYPE_BOOL
		bool b_value;
	};
};

struct mcc_ast_literal *mcc_ast_new_literal_int(long value);

struct mcc_ast_literal *mcc_ast_new_literal_float(double value);

struct mcc_ast_literal *mcc_ast_new_literal_string(char* value);

struct mcc_ast_literal *mcc_ast_new_literal_bool(bool value);

void mcc_ast_delete_literal(struct mcc_ast_literal *literal);

void mcc_ast_empty_node();


// -------------------------------------------------------------------- Assignment



// -------------------------------------------------------------------- Utility

// clang-format off

#define mcc_ast_delete(x) _Generic((x), \
		struct mcc_ast_expression *: mcc_ast_delete_expression, \
		struct mcc_ast_literal *:    mcc_ast_delete_literal \
	)(x)

// clang-format on

#endif // MCC_AST_H
