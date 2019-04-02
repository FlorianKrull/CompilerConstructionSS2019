%define api.prefix {mcc_parser_}

%define api.pure full
%lex-param   {void *scanner}
%parse-param {void *scanner} {struct mcc_ast_expression** result}

%define parse.trace
%define parse.error verbose

%code requires {
#include "include/mcc/parser.h"
}

%{
#include <string.h>
#include <stdio.h>

int mcc_parser_lex();
void mcc_parser_error();

//#define loc(ast_node, ast_sloc) \
	//(ast_node)->node.sloc.start_col = (ast_sloc).first_column;

%}

%define api.value.type union
%define api.token.prefix {TK_}

%locations

%token END 0 "EOF"

%token <long>       INT_LITERAL    "INT_TYPEeger literal"
%token <double>     FLOAT_LITERAL   "FLOAT_TYPE literal"
%token <char *>       BOOL_LITERAL    "BOOL_TYPE literal"
%token <char *>      STRING_LITERAL  "STRING_TYPEing literal"

%token <char *>    IDENTIFIER  "identifier"

%token VOID_TYPE  "void"
%token BOOL_TYPE "boolean"
%token INT_TYPE   "integer"
%token FLOAT_TYPE  "float"
%token STRING_TYPE    "string"

%token IF      "if"
%token ELSE    "else"
%token WHILE   "while"
%token RETURN  "return"


%token LPARENTH "("
%token RPARENTH ")"
%token LBRACKET "["
%token RBRACKET "]"
%token LBRACE "{"
%token RBRACE "}"

%token COMMA  ","
%token SEMICOLON  ";"

%token PLUS  "+"
%token MINUS "-"
%token ASTER "*"
%token SLASH "/"

%token NOT "!"
%token OR  "||"
%token AND "&&"
%token FOR  "for"
%token ASSIGNMENT "="

%token LESS "<"
%token GREATER ">"
%token LESS_EQ "<="
%token GREATER_EQ ">="
%token EQUALS "=="
%token NOT_EQUALS "!="

//%token INVALID "invalid charecter"
//%token UNCLOSED_COMMENT "unclosed comment"


%token <char*> UNKNWON "unkown"
//%left PLUS MINUS
//%left ASTER SLASH


//%type <struct mcc_ast_call_expr> call_expr
%type <struct mcc_ast_single_expression *> single_expression
%type <struct mcc_ast_expression *> expression
%type <struct mcc_ast_literal *> literal

%type <enum mcc_ast_binary_op> binary_op
%type <enum mcc_ast_unary_op> unary_op
%type <enum mcc_ast_literal_type> type


%type <struct mcc_ast_identifier *> identifier
%type <struct mcc_ast_statement *> statement compound_stmt if_stmt while_stmt ret_stmt declaration assignment // multi_statement
%type <struct mcc_ast_parameter_array *> parameters
//%type <STRING_TYPEuct mcc_ast_argument_array *> arguments
%type <struct mcc_ast_function *> function
//%type <STRING_TYPEuct mCc_ast_program *> program
%type <struct mcc_ast_function_def *> function_def




%start toplevel
//%right RPAREN ELSE

%%

toplevel : expression { *result = $1; }
          | function  { *result = $1; }
         ;

single_expression : literal               { $$ = mcc_ast_new_expression_literal($1); loc($$, @1); }
           | expression PLUS  expression  { $$ = mcc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_ADD, $1, $3); loc($$, @1); }
           | expression MINUS expression  { $$ = mcc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_SUB, $1, $3); loc($$, @1); }
           | expression ASTER expression  { $$ = mcc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_MUL, $1, $3); loc($$, @1); }
           | expression SLASH expression  { $$ = mcc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_DIV, $1, $3); loc($$, @1); }
           | LPARENTH expression RPARENTH { $$ = mcc_ast_new_expression_parenth($2);  loc($$, @1); }
           ;


binary_op : PLUS  { $$ = MCC_AST_BINARY_OP_ADD; }
          | MINUS { $$ = MCC_AST_BINARY_OP_SUB; }
          | ASTER { $$ = MCC_AST_BINARY_OP_MUL; }
          | SLASH { $$ = MCC_AST_BINARY_OP_DIV; }
          | AND  { $$ = MCC_AST_BINARY_OP_AND; }
          | OR   { $$ = MCC_AST_BINARY_OP_OR; }
          | LESS { $$ = MCC_AST_BINARY_OP_LESS; }
          | GREATER { $$ = MCC_AST_BINARY_OP_GREATER; }
          | GREATER_EQ { $$ = MCC_AST_BINARY_OP_GREATER_EQ; }
          | LESS_EQ { $$ = MCC_AST_BINARY_OP_LESS_EQ; }
          | EQUALS { $$ = MCC_AST_BINARY_OP_EQUALS; }
          | NOT_EQUALS { $$ = MCC_AST_BINARY_OP_NOT_EQUALS; }
          ;

literal : INT_LITERAL   { $$ = mcc_ast_new_literal_int($1); loc($$, @1); }
        | FLOAT_LITERAL { $$ = mcc_ast_new_literal_float($1); loc($$, @1); }
        | STRING_LITERAL { $$ = mcc_ast_new_literal_string($1); loc($$,@1); }
	| BOOL_LITERAL { $$ = mcc_ast_new_literal_bool($1); loc($$,@1); }
     ;


unary_op : NOT  { $$ = MCC_AST_UNARY_OP_NOT; }
	 | MINUS { $$ = MCC_AST_UNARY_OP_MINUS; }
	 ;


identifier : IDENTIFIER  { $$ = mcc_ast_new_identifier($1); }
           ;




type : BOOL_TYPE { $$ = MCC_AST_LITERAL_TYPE_BOOL; }
     | INT_TYPE  { $$ = MCC_AST_LITERAL_TYPE_INT; }
     | FLOAT_TYPE { $$ = MCC_AST_LITERAL_TYPE_FLOAT; }
     | STRING_TYPE  { $$ = MCC_AST_LITERAL_TYPE_STRING; }
     ;




expression : single_expression                      { $$ = $1; loc($$, @1); }
           | single_expression binary_op expression { $$ = mCc_ast_new_expression_binary_op($2, $1, $3); loc($$, @1); }
           | unary_op single_expression { $$ = mCc_ast_new_expression_unary_op($1, $2); loc($$, @1); }
           ;

/*literal : INT_TYPE_LITERAL   { $$ = mcc_ast_new_literal_int($1);   loc($$, @1); }
        | FLOAT_TYPE_LITERAL { $$ = mcc_ast_new_literal_float($1); loc($$, @1); }
        | STRING_TYPEING_LITERAL{ $$ = mcc_ast_new_literal_string($1); loc($$, @1); }
        | BOOL_TYPE_LITERAL   { $$ = mcc_new_literal_boolean($1); loc($$, $1); }
        ;*/

statement : if_stmt	{ $$ = $1; loc($$, @1); }
	  | while_stmt  { $$ = $1; loc($$, @1); }
	  | ret_stmt    { $$ = $1; loc($$, @1); }
	  | declaration SEMICOLON { $$ = $1; loc($$, @1); }
	  | assignment  SEMICOLON { $$ = $1; loc($$, @1); }
      	  | expression  SEMICOLON { $$ = mcc_ast_new_statement_expr($1); loc($$, @1); }
      	  | LBRACE compound_stmt RBRACE { $$ = $2; loc($$, @2); }
	  ;

compound_stmt : statement   { $$ = $1; loc($$, @1); }
              | compound_stmt statement { $$ = ($1,$2); loc($$, @1);}
              | %empty { $$ = mcc_ast_new_empty_compound();}
	      ;

/*multi_statement :  %empty { $$ = mcc_ast_new_statement_empty(); }
		| multi_statement statement { $$ = mcc_ast_new_statement_compound($1, $2); loc($$, @1); }
		;
*/

if_stmt : IF LPARENTH expression RPARENTH statement { $$ = mcc_ast_new_statement_if($3, $5, NULL);   loc($$, @1); }
	| IF LPARENTH expression RPARENTH statement ELSE statement { $$ = mcc_ast_new_statement_if($3, $5, $7);   loc($$, @1); }
	;

while_stmt : WHILE LPARENTH expression RPARENTH statement { $$ = mcc_ast_new_statement_while($3, $5);     loc($$, @1); };

ret_stmt : RETURN SEMICOLON { $$ = mcc_ast_new_statement_ret();  loc($$, @1); }
	 |RETURN expression SEMICOLON { $$ = mcc_ast_new_statement_ret($2);     loc($$, @1); }
	 ;

declaration : type identifier { $$ = mcc_ast_new_statement_decl($1, $2);     loc($$, @1, @2); }
	    | type LBRACKET INT_LITERAL RBRACKET identifier { $$ = mcc_ast_new_statement_decl($1, $3, $5); }
	    ;

assignment : identifier ASSIGNMENT expression { $$ = mcc_ast_new_statement_ass($1, $3, NULL);     loc($$, @1); }
	   | identifier LBRACKET expression RBRACKET ASSIGNMENT expression { $$ = mcc_ast_new_statement_ass($1, $3, $6);  loc($$, @1); }
	   ;

//arguments : expression                { $$ = mcc_ast_new_argument_single($1); loc($$, @1); }
//	  | arguments COMMA expression { $$ = mcc_ast_new_argument_array($1, $3); loc($$, @1); }
//	  ;

parameters : %empty { $$ = mcc_ast_new_parameter_decl_empty();}
	   | declaration { $$ = mcc_ast_new_parameter_array(mCc_ast_new_parameter_decl_empty(), $1); loc($$, @1); }
	   | parameters COMMA declaration { $$ = mcc_ast_new_parameter_array($1, $3); loc($$, @1); }
	   ;

function : function function_def  { $$ = mcc_ast_new_function($1, $2); }
         | function_def     { $$ = mcc_ast_new_function_def($1); }
	 ;

//program :  %empty { $$ = mcc_ast_new_program_empty(); }
//	| program function { $$ = mcc_ast_new_program($1, $2); loc($$, @1); }
 //       ;


function_def     :  VOID_TYPE identifier
                    LPARENTH parameters RPARENTH LBRACKET compound_stmt RBRACKET    { $$ = mcc_ast_void_function_def ($2, $4, $7); }
                  |  type identifier
                    LPARENTH parameters RPARENTH LBRACKET compound_stmt RBRACKET  { $$ = mcc_ast_type_function_def( $2, $4, $7); }
                  ;



/*call_expr       : identifier LPARENTH RPARENTH          { $$ = mcc_ast_new_empty_call_expr($1); }
                | identifier LPARENTH arguments RPARENTH    { $$ = mcc_ast_new_call_expr($1,$3); }

                ;
*/





%%

#include <assert.h>

#include "scanner.h"
#include "utils/unused.h"

void  mcc_parser_error(struct MCC_PARSER_LTYPE *yylloc, yyscan_t *scanner, const char *msg)
{
	// TODO
	UNUSED(yylloc);
	UNUSED(scanner);
	UNUSED(msg);
}

struct mcc_parser_result mcc_parse_STRING_TYPEing(const char *input)
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

