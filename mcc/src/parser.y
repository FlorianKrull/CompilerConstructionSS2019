%define api.prefix {mcc_parser_}

%define api.pure full
%lex-param   {void *scanner}
%parse-param {void *scanner} {struct mcc_ast_expression** result}

%define parse.trace
%define parse.error verbose

%code requires {
#include "../include/parser.h"
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
%token <const char *>  STRING_LITERAL "string literal"
%token <bool>  BOOLEAN                "boolean literal"

%token <const char *>    INDETIFIER  "identifier"
%token VOID_RYPE  "void"
%token BOOL_TYPE  "bool"
%token INT_TYPE   "integer"
%token FLOAT_TYPE "floating point"
%token STR_TYPE   "string"
%token IF_KW      "if"
%token ELSE_KW    "else"
%token WHILE_KW   "while"
%token RETURN_KW  "return"



%token LPARENTH "("
%token RPARENTH ")"
%token LBRACK "["
%token RBRACK "]"
%token LBRACE "{"
%token RBRACE "}"

%token COMMA  ","
%token SEMIC ";"
%token PLUS  "+"
%token MINUS "-"
%token ASTER "*"
%token SLASH "/"
%token BANG "!"
%token LAND "&&"
%token LOR   "||"
%token EQUAL "="
%token LTREL "<"
%token GTREL ">"
%token LEREL "<="
%token GEREL ">="
%token EQREL "=="
%token NEREL "!="

%token INVALID "invalid charecter"
%token UNCLOSED_COMMENT "unclosed comment"

%left PLUS MINUS
%left ASTER SLASH

%type <struct mcc_ast_expression *> expression
%type <struct mcc_ast_literal *> literal
%type <enum mCc_ast_binary_op> binary_op
%type <enum mCc_ast_unary_op> unary_op
%type <enum mCc_ast_literal_type> type
%type <enum mCc_ast_function_type> function_type


%type <struct mCc_ast_statement *> statement compound_stmt if_stmt while_stmt ret_stmt declaration assignment multi_statement
%type <struct mCc_ast_parameter_array *> parameters
%type <struct mCc_ast_argument_array *> arguments
%type <struct mCc_ast_function *> function
%type <struct mCc_ast_program *> program
%start toplevel
%right RPAREN ELSE_KW

%%

toplevel : expression { *result = $1; }
         ;

expression : literal                      { $$ = mcc_ast_new_expression_literal($1);                              loc($$, @1); }
           | expression PLUS  expression  { $$ = mcc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_ADD, $1, $3); loc($$, @1); }
           | expression MINUS expression  { $$ = mcc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_SUB, $1, $3); loc($$, @1); }
           | expression ASTER expression  { $$ = mcc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_MUL, $1, $3); loc($$, @1); }
           | expression SLASH expression  { $$ = mcc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_DIV, $1, $3); loc($$, @1); }
           | LPARENTH expression RPARENTH { $$ = mcc_ast_new_expression_parenth($2);                              loc($$, @1); }
           ;
binary_op : PLUS  { $$ = MCC_AST_BINARY_OP_ADD; }
          | MINUS { $$ = MCC_AST_BINARY_OP_SUB; }
          | ASTER { $$ = MCC_AST_BINARY_OP_MUL; }
          | SLASH { $$ = MCC_AST_BINARY_OP_DIV; }
          | LAND  { $$ = MCC_AST_BINARY_OP_AND; }
          | LOR   { $$ = MCC_AST_BINARY_OP_OR; }
          | LTREL { $$ = MCC_AST_BINARY_OP_LT; }
          | GTREL { $$ = MCC_AST_BINARY_OP_GT; }
          | GEREL { $$ = MCC_AST_BINARY_OP_GEQ; }
          | LEREL { $$ = MCC_AST_BINARY_OP_LEQ; }
          | EQREL { $$ = MCC_AST_BINARY_OP_EQ; }
          | NEREL { $$ = MCC_AST_BINARY_OP_NEQ; }
          ;

literal : INT_LITERAL   { $$ = mcc_ast_new_literal_int($1);   loc($$, @1); }
        | FLOAT_LITERAL { $$ = mcc_ast_new_literal_float($1); loc($$, @1); }
        ;

unary_op : BANG  { $$ = MCC_AST_UNARY_OP_NEG; }
	 | MINUS { $$ = MCC_AST_UNARY_OP_MIN; }
	 ;

type : BOOL_TYPE { $$ = MCC_AST_LITERAL_TYPE_BOOL; }
     | INT_TYPE  { $$ = MCC_AST_LITERAL_TYPE_INT; }
     | FLOAT_TYPE{ $$ = MCC_AST_LITERAL_TYPE_FLOAT; }
     | STR_TYPE  { $$ = MCC_AST_LITERAL_TYPE_STRING; }
     ;

function_type : BOOL_TYPE { $$ = MCC_AST_FUNCTION_TYPE_BOOL; }
	     | INT_TYPE  { $$ = MCC_AST_FUNCTION_TYPE_INT; }
	     | FLOAT_TYPE{ $$ = MCC_AST_FUNCTION_TYPE_FLOAT; }
	     | STR_TYPE  { $$ = MCC_AST_FUNCTION_TYPE_STRING; }
	     | VOID_TYPE { $$ = MCC_AST_FUNCTION_TYPE_VOID; }
	     ;

            ;
expression : single_expr                      { $$ = $1;                                           loc($$, @1); }
           | single_expr binary_op expression { $$ = mCc_ast_new_expression_binary_op($2, $1, $3); loc($$, @1); }
           | unary_op single_expr { $$ = mCc_ast_new_expression_unary_op($1, $2); loc($$, @1); }
           ;

literal : INT_LITERAL   { $$ = mCc_ast_new_literal_int($1);   loc($$, @1); }
        | FLOAT_LITERAL { $$ = mCc_ast_new_literal_float($1); loc($$, @1); }

        | STRING_LITERAL{ $$ = mCc_ast_new_literal_string($1); loc($$, @1); }
        ;

statement : if_stmt	{ $$ = $1; loc($$, @1); }
	  | while_stmt  { $$ = $1; loc($$, @1); }
	  | ret_stmt    { $$ = $1; loc($$, @1); }
	  | declaration SEMIC { $$ = $1; loc($$, @1); }
	  | assignment  SEMIC { $$ = $1; loc($$, @1); }
      	  | expression  SEMIC { $$ = mCc_ast_new_statement_expr($1); loc($$, @1); }
      	  | compound_stmt { $$ = $1; loc($$, @1); }
	  ;

compound_stmt : LBRACE multi_statement RBRACE { $$ = $2; loc($$, @1); }
	      ;

multi_statement :  %empty { $$ = mCc_ast_new_statement_empty(); }
		| multi_statement statement { $$ = mCc_ast_new_statement_compound($1, $2); loc($$, @1); }
		;


if_stmt : IF_KW LPAREN expression RPAREN statement { $$ = mCc_ast_new_statement_if($3, $5, NULL);   loc($$, @1); }
	| IF_KW LPAREN expression RPAREN statement ELSE_KW statement { $$ = mCc_ast_new_statement_if($3, $5, $7);   loc($$, @1); }
	;

while_stmt : WHILE_KW LPAREN expression RPAREN statement { $$ = mCc_ast_new_statement_while($3, $5);     loc($$, @1); };

ret_stmt : RETURN_KW SEMIC { $$ = mCc_ast_new_statement_ret(NULL);  loc($$, @1); }
	 |RETURN_KW expression SEMIC { $$ = mCc_ast_new_statement_ret($2);     loc($$, @1); }
	 ;

declaration : type IDENTIFIER { $$ = mCc_ast_new_statement_decl($1, $2, -1);     loc($$, @1); }
	    | type LBRACK INT_LITERAL RBRACK IDENTIFIER { $$ = mCc_ast_new_statement_decl($1, $5, $3); }
	    ;

assignment : IDENTIFIER EQUAL expression { $$ = mCc_ast_new_statement_ass($1, $3, NULL);     loc($$, @1); }
	   | IDENTIFIER LBRACK expression RBRACK EQUAL expression { $$ = mCc_ast_new_statement_ass($1, $6, $3);     loc($$, @1); }
	   ;

arguments : %empty { $$ = mCc_ast_new_argument_empty(); }
	  | expression { $$ = mCc_ast_new_argument_array(mCc_ast_new_argument_empty(), $1); loc($$, @1); }
	  | arguments COMMA expression { $$ = mCc_ast_new_argument_array($1, $3); loc($$, @1); }
	  ;

parameters : %empty { $$ = mCc_ast_new_parameter_decl_empty();}
	   | declaration { $$ = mCc_ast_new_parameter_array(mCc_ast_new_parameter_decl_empty(), $1); loc($$, @1); }
	   | parameters COMMA declaration { $$ = mCc_ast_new_parameter_array($1, $3); loc($$, @1); }
	   ;

function : function_type IDENTIFIER LPAREN parameters RPAREN compound_stmt
	   { $$ = mCc_ast_new_function($1, $2, $4, $6); loc($$, @1); }
	 ;

program :  %empty { $$ = mCc_ast_new_program_empty(); }
	| program function { $$ = mCc_ast_new_program($1, $2); loc($$, @1); }
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
	assert(input);

	yyscan_t scanner;
	mcc_parser_lex_init(&scanner);
	mcc_parser_set_in(input, scanner);

	struct mcc_parser_result result = {
	    .status = MCC_PARSER_STATUS_OK,
	};

	if (yyparse(scanner, &result.expression) != 0) {
		result.status = MCC_PARSER_STATUS_UNKNOWN_ERROR;
	}

	mcc_parser_lex_destroy(scanner);

	return result;
}

