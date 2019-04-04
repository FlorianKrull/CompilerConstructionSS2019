%define api.prefix {mcc_parser_}

%define api.pure full
%lex-param   {void *scanner}
%parse-param {void *scanner} {struct mcc_ast_literal** result_literal}{ struct mcc_ast_declaration** result_declaration}

%define parse.trace
%define parse.error verbose

%code requires {
#include "mcc/parser.h"
}

%{
#include <string.h>

int mcc_parser_lex();
void mcc_parser_error();

#define loc(ast_node, ast_sloc) \
	(ast_node)->node.sloc.start_col = (ast_sloc).first_column;

%}

%define api.value.type union
%define api.token.prefix {TK_}

%locations

%token END 0 "EOF"
%token <long>   INT_LITERAL   "integer literal"
%token <double> FLOAT_LITERAL "float literal"
%token <char*>   STRING_LITERAL   "string literal"
%token <char*> IDENTIFIER "identifier"
%token <bool> BOOL_LITERAL "bool literal"

%token <char*>  TYPE            "type"

%token LPARENTH "("
%token RPARENTH ")"

%token LBRACKET "["
%token RBRACKET "]"

%token LBRACE "{"
%token RBRACE "}"

%token ASSIGNMENT "="

%token PLUS  "+"
%token MINUS "-"
%token ASTER "*"
%token SLASH "/"

%token LESS "<"
%token GREATER ">"
%token LESS_EQ "<="
%token GREATER_EQ ">="
%token EQUALS "=="
%token NOT_EQUALS "!="

%token SEMICOLON ";"
%token COMMA ","

%token NOT "!"

%token AND "&&"
%token OR "||"

%token BOOL_TYPE "bool"
%token INT_TYPE "int"
%token FLOAT_TYPE "float"
%token STRING_TYPE "string"
%token VOID_TYPE "void"

%token IF "if"
%token ELSE "else"
%token WHILE "while"
%token FOR "for"
%token RETURN "return"

%type <struct mcc_ast_literal *> literal
%type <struct mcc_ast_expression *> expression binary_op unary_op
%type <struct mcc_ast_identifier *> identifier
%type <struct mcc_ast_declaration *> declaration
%type <struct mcc_ast_statement *> statement if_statement while_statement compound_statement compound_block assignment
%type <enum mcc_ast_data_type> type VOID_TYPE
%type <struct mcc_ast_function_def *> function_def
%type <struct mcc_ast_parameter *> parameters
%type <struct mcc_ast_program *> program


%start program

%%

toplevel : %empty   { result->program = mCc_ast_new_empty_program(); }
         | program  { result->program = $1; }
         ;

expression : literal                      		{ $$ = mcc_ast_new_expression_literal($1);              loc($$, @1); }
           | LPARENTH expression RPARENTH	 	{ $$ = mcc_ast_new_expression_parenth($2);              loc($$, @1); }
		   | expression binary_op expression  	{ $$ = mcc_ast_new_expression_binary_op($2, $1, $3);    loc($$, @1); }
		   | identifier 						{ $$ = $1; 			                                    loc($$, @1); }

           ;

literal : INT_LITERAL       { $$ = mcc_ast_new_literal_int($1);     loc($$, @1); }
        | FLOAT_LITERAL     { $$ = mcc_ast_new_literal_float($1);   loc($$, @1); }
		| STRING_LITERAL    { $$ = mcc_ast_new_literal_string($1);  loc($$,@1);  }
		| BOOL_LITERAL      { $$ = mcc_ast_new_literal_bool($1);    loc($$,@1);  }
		;

unary_op : NOT { $$ = MCC_AST_UNARY_OP_NOT; }
		 | MINUS  {$$ = MCC_AST_UNARY_OP_MINUS; }
		 ;

binary_op : PLUS { $$= MCC_AST_BINARY_OP_ADD; }
			| MINUS { $$ = MCC_AST_BINARY_OP_SUB; }
			| ASTER { $$ = MCC_AST_BINARY_OP_MUL; }
			| SLASH { $$ = MCC_AST_BINARY_OP_DIV; }
			| EQUALS { $$ = MCC_AST_BINARY_OP_EQUALS; }
			| NOT_EQUALS { $$ = MCC_AST_BINARY_OP_NOT_EQUALS; }
			| LESS { $$ = MCC_AST_BINARY_OP_LESS; }
			| GREATER { $$ = MCC_AST_BINARY_OP_GREATER; }
			| LESS_EQ { $$ = MCC_AST_BINARY_OP_LESS_EQUALS; }
			| GREATER_EQ { $$ = MCC_AST_BINARY_OP_GREATER_EQUALS; }
			;

type : INT_TYPE { $$ = MCC_AST_DATA_TYPE_INT; }
     | FLOAT_TYPE { $$ = MCC_AST_DATA_TYPE_FLOAT; }
     | STRING_TYPE { $$ = MCC_AST_DATA_TYPE_STRING; }
	 | BOOL_TYPE { $$ = MCC_AST_DATA_TYPE_BOOL; }
	 | VOID_TYPE { $$ = MCC_AST_DATA_TYPE_VOID;}
     ;


identifier : IDENTIFIER { $$ = mcc_ast_new_identifier($1); loc($$, @1); }
           ;

statement : expression SEMICOLON                { $$ = mcc_ast_new_statement_expression($1); loc($$, @1); }
          | if_statement                        { $$= $1;  loc($$, @1); }
		  | while_statement                     { $$ = $1; loc($$, @1); }
		  | compound_statement                  { $$ = $1; loc($$, @1); }
          | assignment SEMICOLON                { $$ = $1; loc($$, @1); }
          | declaration SEMICOLON               { $$ = $1; loc($$, @1); }
          | LBRACE compound_statement RBRACE    { $$ = $2; loc($$, @1); };
		  ;

if_statement: IF LPARENTH expression RPARENTH statement { $$ = mcc_ast_new_statement_if($3, $5, NULL);                     loc($$, @1); }
            | IF LPARENTH expression RPARENTH statement ELSE statement { $$ = mcc_ast_new_statement_if($3, $5, $7);  loc($$, @1); }
            ;

declaration: type IDENTIFIER SEMICOLON 									{ $$ = mcc_ast_new_declaration($1, $2, NULL); 	loc($$, @1); }
		   | type IDENTIFIER LBRACKET INT_LITERAL RBRACKET SEMICOLON 	{ $$ = mcc_ast_new_declaration($1, $2, $4); 	loc($$, @1); };

while_statement: WHILE LPARENTH expression RPARENTH statement { $$ = mcc_ast_new_statement_while($3, $5); loc($$, @1); }
			   ;

compound_statement: statement                     { $$ = mcc_ast_new_statement_compound(NULL, $1);  loc($$, @1); }
                   | compound_statement statement { $$ = mcc_ast_new_statement_compound($1, $2);    loc($$, @1); }


/* Took this idea from : https://norasandler.com/2018/02/25/Write-a-Compiler-6.html */

/* compound_block : statement                  { $$ = mcc_ast_new_statement_compound_block($1, $1); loc($$, @1); }
		   	  | compound_block statement    { $$ = mcc_ast_add_compound_statement($1, $2); loc($$, @1); }*/
		   	  ;


assignment:  IDENTIFIER ASSIGNMENT expression 					            { $$ = mcc_ast_new_statement_assignment($1, 0, $3); 	loc($$, @1); };
          |  IDENTIFIER LBRACKET expression RBRACKET ASSIGNMENT expression  { $$ = mcc_ast_new_statement_assignment($1, $3, $6); 	loc($$, @1); };
          ;

parameters  : declaration COMMA parameters { $$ = mcc_ast_new_parameter($1); $$->next = $3; loc($$, @1); }
			| declaration                  { $$ = mcc_ast_new_parameter($1);                loc($$, @1); }
			;

function_def : type IDENTIFIER LPARENTH parameters RPARENTH LBRACE compound_statement RBRACE
                    { $$ = mcc_ast_new_function_def($1, $2, $4, $7); loc($$, @1);}
			 | type IDENTIFIER LPARENTH RPARENTH LBRACE compound_statement RBRACE
                    { $$ = mcc_ast_new_function_def($1, $2, NULL, $6); loc($$, @1);}
             | type IDENTIFIER LPARENTH RPARENTH LBRACE RBRACE
                    { $$ = mcc_ast_new_function_def($1, $2, NULL, NULL); loc($$, @1);}
			 ;

program : function_def { $$ = mcc_ast_new_program(NULL, $1); loc($$, @1); }
        | program function_def { $$ = mcc_ast_new_program($1, $2); loc($$, @1) };
		;

%%

#include <assert.h>

#include "scanner.h"
#include "utils/unused.h"

void mcc_parser_error(struct MCC_PARSER_LTYPE *yylloc, yyscan_t *scanner, const char *msg)
{
	// TODO
	UNUSED(yylloc);
	UNUSED(scanner);
	UNUSED(msg);
}

struct mcc_parser_result mcc_parse_string(const char *input)
{
	assert(input);

	FILE *in = fmemopen((void *)input, strlen(input), "r");
	if (!in) {
		return (struct mcc_parser_result){
		    .status = MCC_PARSER_STATUS_UNABLE_TO_OPEN_STREAM,
		};
	}

	struct mcc_parser_result result = mcc_parse_file(in);

	fclose(in);

	return result;
}

struct mcc_parser_result mcc_parse_file(FILE *input)
{
    printf("Parse file \n");
	assert(input);

	yyscan_t scanner;
	mcc_parser_lex_init(&scanner);
	mcc_parser_set_in(input, scanner);

	struct mcc_parser_result result = {
	    .status = MCC_PARSER_STATUS_OK,
	};

	if (yyparse(scanner, &result.literal, &result.declaration) != 0) {
		result.status = MCC_PARSER_STATUS_UNKNOWN_ERROR;
	}

	mcc_parser_lex_destroy(scanner);

	return result;
}
