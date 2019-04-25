// Parser Infrastructure
//
// This defines the interface to the parser component of the compiler.
//
// It tries to convert a given text input to an AST. On success, ownership of
// the AST is transferred to the caller via the `mcc_parser_result` struct.

#ifndef MCC_PARSER_H
#define MCC_PARSER_H

#include <stdio.h>

#include "mcc/ast.h"

enum mcc_parser_status {
	MCC_PARSER_STATUS_OK,
	MCC_PARSER_STATUS_UNABLE_TO_OPEN_STREAM,
	MCC_PARSER_STATUS_UNKNOWN_ERROR,
};

// -------------------- Parser Error

struct mcc_parser_error {
	struct mcc_ast_source_location *loc;
	char *error_msg;
};

struct mcc_parser_error *new_parser_error(struct mcc_ast_source_location *loc, char *msg);

void delete_parser_error(struct mcc_parser_error *error);

// -------------------- Parser Result

struct mcc_parser_result {
	enum mcc_parser_status status;

    struct mcc_parser_error *parser_error;

	struct mcc_ast_expression *expression;
	struct mcc_ast_literal *literal;
    struct mcc_ast_function *function;
	struct mcc_ast_declaration *declaration;
	struct mcc_ast_statement *statement;
    struct mcc_ast_program *program;
};

struct mcc_parser_result mcc_parse_string(const char *input);

struct mcc_parser_result mcc_parse_file(FILE *input);


#endif // MCC_PARSER_H
