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
%token <const char*> IDENTIFIER "identifier"
%token <bool> BOOL_LITERAL "bool literal"


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


/* precedence */

%left PLUS MINUS
%left ASTER SLASH
%left OR AND
%left EQUALS NOT_EQUALS
%left LESS GREATER LESS_EQ GREATER_EQ

%type <enum mcc_ast_binary_op> binary_op
%type <enum mcc_ast_unary_op> unary_op
%type <struct mcc_ast_literal *> literal
%type <struct mcc_ast_expression *> expression
%type <struct mcc_ast_statement *> statement if_statement declaration while_statement compound_statement assignment
%type <struct mcc_ast_statement_list *> statement_list
%type <struct mcc_ast_compound_statement *> compound_statement
%type <sttruct mcc_ast_if_statement *> if_statement
%type <enum mcc_ast_data_type> type
%type <struct mcc_ast_function *> function
%type <struct mcc_ast_function_def *> function_def
%type <struct mcc_ast_program *> program
%type <struct mcc_ast_parameters *>parameters
%type <strcut mcc_ast_ret_statement *> ret_statement


%start toplevel

%%

toplevel : literal { *result_literal = $1; }
         | declaration { *result_declaration = $1; }
         ;

expression : literal                      		{ $$ = mcc_ast_new_expression_literal($1);              loc($$, @1); }
           | LPARENTH expression RPARENTH	 	{ $$ = mcc_ast_new_expression_parenth($2);              loc($$, @1); }
           | expression binary_op expression  	{ $$ = mcc_ast_new_expression_binary_op($2, $1, $3);    loc($$, @1); }
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
     ;

statement : expression SEMICOLON    { $$ = mcc_ast_new_statement_expression($1); loc($$, @1); }
          | if_statement            { $$= $1;  loc($$, @1); }
		  | while_statement         { $$ = $1; loc($$, @1); }
		  | compound_statement      { $$ = $1; loc($$, @1); }
          | assignment SEMICOLON    { $$ = $1; loc($$, @1); }
          | declaration SEMICOLON   { $$ = $1; loc($$, @1); }
          | ret_statement    { $$ = $1; loc($$, @1); }
          ;

if_statement: IF LPARENTH expression RPARENTH statement { $$ = mcc_ast_new_statment_if($3, $5);                     loc($$, @1); }
            | IF LPARENTH expression RPARENTH statement ELSE statement { $$ = mcc_ast_new_statment_if($3, $5, $7);  loc($$, @1); }
            ;

declaration: type IDENTIFIER SEMICOLON { $$ = mcc_ast_new_declaration($1, $2); loc($$, @1); }
		   ;

while_statement: WHILE LPARENTH expression RPARENTH statement { $$ = mcc_ast_new_statement_while($3, $5); loc($$, @1); }
			   ;

compound_statement: LBRACE statement_list LBRACE { $$ = $2; loc($$; @1); }
				  ;

statement_list: %empty { $$ = mcc_ast_empty_node() }
			  | statement_list statement { $$ = mcc_ast_new_statement_list($1, $2); loc($$, @1); }
			  ;

assignment:  IDENTIFIER ASSIGNMENT expression 					            { $$ = mcc_ast_new_statement_assignment($1, 0, $3); 	loc($$, @1); };
          |  IDENTIFIER LBRACKET expression RBRACKET ASSIGNMENT expression  { $$ = mcc_ast_new_statement_assignment($1, $3, $6); 	loc($$, @1); };
          ;


function : function function_def  { $$ = mcc_ast_new_function($1, $2); }
         | function_def     { $$ = mcc_ast_new_function_def($1); }
	 ;

program :  %empty { $$ = mcc_ast_new_program_empty(); }
	| program function_def { $$ = mcc_ast_new_program($1, $2); loc($$, @1); }
      ;


function_def     :  VOID_TYPE identifier
                    LPARENTH parameters RPARENTH LBRACK compound_statement RBRACK    { $$ = mcc_ast_void_function_def ($2, $4, $7); }
                  |  type identifier
                    LPARENTH parameters RPARENTH LBRACK compound_statement RBRACK  { $$ = mcc_ast_type_function_def( $2, $4, $7); }
                  ;
parameters : %empty { $$ = mcc_ast_new_parameter_decl_empty();}
  	   | declaration { $$ = mcc_ast_new_parameter_array(mCc_ast_new_parameter_decl_empty(), $1); loc($$, @1); }
  	   | parameters COMMA declaration { $$ = mcc_ast_new_parameter_array($1, $3); loc($$, @1); }
  	   ;


ret_stmt : RETURN_KW SEMIC { $$ = mcc_ast_new_statement_ret(NULL);  loc($$, @1); }
	 |RETURN_KW expression SEMIC { $$ = mcc_ast_new_statement_ret($2);    loc($$, @1); }
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
