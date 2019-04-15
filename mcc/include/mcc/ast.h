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
    MCC_AST_DATA_TYPE_FLOAT,
    MCC_AST_DATA_TYPE_VOID
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
	MCC_AST_EXPRESSION_TYPE_ARRAY_IDENTIFIER,

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

		struct {
			struct mcc_ast_identifier *identifier;
			struct mcc_ast_expression *expression;
		} array_identifier;
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
enum mcc_ast_declaration_type {
	MCC_AST_DECLARATION_TYPE_DECLARATION,
	MCC_AST_DECLARATION_TYPE_ARRAY_DECLARATION
};

struct mcc_ast_declaration {
	
	struct mcc_ast_node node;

	enum mcc_ast_declaration_type declaration_type;

	enum mcc_ast_data_type type;

	struct mcc_ast_identifier *ident;

	struct mcc_ast_literal *arr_literal;

};

struct mcc_ast_declaration *mcc_ast_new_declaration(enum mcc_ast_data_type type, struct mcc_ast_identifier *ident);


// ------------------------------------------------------------------- Assignments

enum mcc_ast_assignment_type {
	MCC_AST_ASSIGNMENT_TYPE_NORMAL,
	MCC_AST_ASSIGNMENT_TYPE_ARRAY,
};

struct mcc_ast_assignment {
	struct mcc_ast_node node;
	enum mcc_ast_assignment_type type;
	struct mcc_ast_identifier *identifier;
	union {
		struct {
			struct mcc_ast_expression *rhs;
		} normal_ass;

		struct {
			struct mcc_ast_expression *index;
			struct mcc_ast_expression *rhs;
		} array_ass;
	};
};

struct mcc_ast_assignment *
mcc_ast_new_assignment(struct mcc_ast_identifier *identifier,
                       struct mcc_ast_expression *rhs);

struct mcc_ast_assignment *
mcc_ast_new_array_assignment(struct mcc_ast_identifier *identifier,
                             struct mcc_ast_expression *index,
                             struct mcc_ast_expression *rhs);

void mcc_ast_delete_assignment(struct mcc_ast_assignment *assignment);

// ------------------------------------------------------------------- Statements

enum mcc_ast_statement_type {
    MMC_AST_STATEMENT_TYPE_EXPRESSION,
    MCC_AST_STATEMENT_TYPE_IF,
	MCC_AST_STATEMENT_TYPE_WHILE,
	MCC_AST_STATEMENT_TYPE_DECL,
	MCC_AST_STATEMENT_TYPE_ASSGN,
	MCC_AST_STATEMENT_TYPE_ASSGN_ARR,
	MCC_AST_STATEMENT_TYPE_COMPOUND,
	// MCC_AST_STATEMENT_TYPE_BLOCK
};

struct mcc_ast_statement_list {

	struct mcc_ast_node node;
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

		struct mcc_ast_assignment *assignment;

		struct mcc_ast_declaration *declaration;

        struct mcc_ast_statement_list *compound_statement;
    };
};

struct mcc_ast_statement *mcc_ast_new_statement_expression (struct mcc_ast_expression *expression);

struct mcc_ast_statement *mcc_ast_new_statement_if(struct mcc_ast_expression *condition,
												   struct mcc_ast_statement *if_stmt,
                                                   struct mcc_ast_statement *else_stmt);

struct mcc_ast_statement *mcc_ast_new_statement_while(struct mcc_ast_expression *condition,
												   	  struct mcc_ast_statement *while_stmt);

struct mcc_ast_statement *mcc_ast_new_statement_assignment(struct mcc_ast_assignment *assignment);

struct mcc_ast_statement *mcc_ast_new_statement_declaration(enum mcc_ast_data_type data_type,
															struct mcc_ast_identifier *identifier);

struct mcc_ast_statement *mcc_ast_new_statement_list(struct mcc_ast_statement_list *statement_list,
                                                                struct mcc_ast_statement *next_statement);


void mcc_ast_delete_statement(struct mcc_ast_statement *statement);

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
	char *value;
};

struct mcc_ast_literal *mcc_ast_new_literal(enum mcc_ast_literal_type,char* value);


void mcc_ast_delete_literal(struct mcc_ast_literal *literal);

void mcc_ast_empty_node();


// -------------------------------------------------------------------- Assignment




// -------------------------------------------------------------------- Parameter

// int PARAMETER_BLOCK_SIZE = 4;
struct mcc_ast_parameter {
	struct mcc_ast_node node;
    int max;
    int size;
	struct mcc_ast_declaration *parameters[];
};

struct mcc_ast_parameter *mcc_ast_new_parameter(struct mcc_ast_declaration *declaration, struct mcc_ast_parameter *params);

void mcc_ast_delete_parameter(struct mcc_ast_parameter *parameter);


// -------------------------------------------------------------------- Function

struct mcc_ast_function {
    struct mcc_ast_node node;

    enum mcc_ast_data_type return_type;
    struct mcc_ast_identifier *identifier;
   	struct mcc_ast_parameter *parameter;
	struct mcc_ast_statement *statement;
};

struct mcc_ast_function *mcc_ast_new_function(
        enum mcc_ast_data_type return_type,
        struct mcc_ast_identifier *identifier,
        struct mcc_ast_parameter *parameter,
        struct mcc_ast_statement *statement
);


// -------------------------------------------------------------------- Program

struct mcc_ast_program {
	struct mcc_ast_node node;
    int max;
    int size;
	struct mcc_ast_function *function_def[];
};

<<<<<<< HEAD
struct mcc_ast_program *mcc_ast_new_program(struct mcc_ast_function_def_list *function_def_list);
=======
struct mcc_ast_program *mcc_ast_new_program(struct mcc_ast_function *function_def);

struct mcc_ast_program *mcc_ast_add_function(struct mcc_ast_function *function_def, struct mcc_ast_program *program);
>>>>>>> d69a8212fba56a48cc06bfc3ee9cdac30a47faf9

void mcc_ast_delete_program(struct mcc_ast_program *program);

// -------------------------------------------------------------------- Utility

// clang-format off

#define mcc_ast_delete(x) _Generic((x), \
		struct mcc_ast_expression *: mcc_ast_delete_expression, \
		struct mcc_ast_literal *:    mcc_ast_delete_literal, \
		struct mcc_ast_statement *: mcc_ast_delete_statement \
	)(x)

// clang-format on

#endif // MCC_AST_H