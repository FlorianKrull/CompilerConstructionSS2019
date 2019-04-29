%define api.prefix {mcc_parser_}

%define api.pure full
%lex-param   {void *scanner}
%parse-param {void *scanner} {struct mcc_parser_result* result}

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
	(ast_node)->node.sloc.start_col = (ast_sloc).first_column; \
	(ast_node)->node.sloc.start_line = (ast_sloc).first_line; \
	(ast_node)->node.sloc.end_line = (ast_sloc).last_line; \
	(ast_node)->node.sloc.end_col = (ast_sloc).last_column;

%}

%define api.value.type union
%define api.token.prefix {TK_}

%locations

%token END 0 "EOF"
%token <char*>  INT_LITERAL   	"integer literal"
%token <char*> 	FLOAT_LITERAL 	"float literal"
%token <char*>  STRING_LITERAL	"string literal"
%token <char*> 	BOOL_LITERAL 	"bool literal"

%token <char*> IDENTIFIER "identifier"

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
%token VOID "void"

%token IF "if"
%token ELSE "else"
%token WHILE "while"
%token FOR "for"
%token RETURN "return"

%left PLUS MINUS
%left ASTER SLASH
%left OR AND
%left EQUALS NOT_EQUALS
%left LESS GREATER LESS_EQ GREATER_EQ

%precedence UNOT
%precedence UMINUS


%type <struct mcc_ast_expression *> expression single_expr binary_op call_expression
%type <struct mcc_ast_identifier *> identifier
%type <struct mcc_ast_literal *> literal
%type <struct mcc_ast_statement *> statement if_statement while_statement compound_statement
%type <enum mcc_ast_data_type> type
%type <struct mcc_ast_statement_list *> statement_list
%type <struct mcc_ast_function *> function_def
%type <struct mcc_ast_declaration *> declaration
%type <struct mcc_ast_assignment *> assignment
%type <struct mcc_ast_parameter *> parameters
%type <struct mcc_ast_argument *> argument
%type <struct mcc_ast_program *> program


%destructor { mcc_ast_delete_identifier($$); }         	identifier
%destructor { mcc_ast_delete_declaration($$); }        	declaration
%destructor {mcc_ast_delete_argument($$); }		argument

%destructor { mcc_ast_delete_function($$); } 		function_def
%destructor { mcc_ast_delete_parameter($$); }           parameters
%destructor { mcc_ast_delete_program($$); }             program


%start toplevel

%%

toplevel : program { result -> program = $1; }
         ;


binary_op : expression PLUS expression
            { $$ = mcc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_ADD,$1, $3);  loc($$, @1); }
          | expression MINUS expression
            { $$ = mcc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_SUB,$1, $3);  loc($$, @1); }
          | expression ASTER expression
            { $$ = mcc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_MUL,$1, $3);  loc($$, @1); }
          | expression SLASH expression
            { $$ = mcc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_DIV,$1, $3);  loc($$, @1); }
          | expression LESS expression
            { $$ = mcc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_LESS,$1, $3);  loc($$, @1); }
          | expression GREATER expression
            { $$ = mcc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_GREATER,$1, $3);  loc($$, @1); }
          | expression LESS_EQ expression
            { $$ = mcc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_LESS_EQUALS,$1, $3);  loc($$, @1); }
          | expression GREATER_EQ expression
            { $$ = mcc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_GREATER_EQUALS,$1, $3);  loc($$, @1); }
          | expression AND expression
            { $$ = mcc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_AND,$1, $3);  loc($$, @1); }
          | expression OR expression
            { $$ = mcc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_OR,$1, $3);  loc($$, @1); }
          | expression EQUALS expression
            { $$ = mcc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_EQUALS,$1, $3);  loc($$, @1); }
          | expression NOT_EQUALS expression
            { $$ = mcc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_NOT_EQUALS,$1, $3);  loc($$, @1); }
          ;

single_expr : literal
              { $$ = mcc_ast_new_expression_literal($1);      loc($$, @1); }
            | identifier
              { $$ = mcc_ast_new_expression_identifier($1);   loc($$, @1); }
            | identifier LBRACKET expression RBRACKET
              { $$ = mcc_ast_new_expression_bracket($1, $3);  loc($$, @1); }
            | MINUS expression %prec UMINUS
              { $$ = mcc_ast_new_expression_unary_op(MCC_AST_UNARY_OP_MINUS, $2); loc($$, @1); }
            | NOT expression %prec UNOT
              { $$ = mcc_ast_new_expression_unary_op(MCC_AST_UNARY_OP_NOT, $2); loc($$, @1); }
            | LPARENTH expression RPARENTH
              { $$ = mcc_ast_new_expression_parenth($2);      loc($$, @1); }
            ;

expression : binary_op    	{ $$ = $1; }
           | single_expr  	{ $$ = $1; }
           | call_expression 	{ $$ = $1; }
           ;


literal : BOOL_LITERAL    { $$ = mcc_ast_new_literal(MCC_AST_LITERAL_TYPE_INT, $1); loc($$, @1); }
        | INT_LITERAL     { $$ = mcc_ast_new_literal(MCC_AST_LITERAL_TYPE_INT, $1); loc($$, @1); }
        | FLOAT_LITERAL   { $$ = mcc_ast_new_literal(MCC_AST_LITERAL_TYPE_FLOAT, $1); loc($$, @1); }
        | STRING_LITERAL  { $$ = mcc_ast_new_literal(MCC_AST_LITERAL_TYPE_STRING, $1); loc($$, @1); }
        ;

identifier : IDENTIFIER { $$ = mcc_ast_new_identifier($1); loc($$, @1); }


type : INT_TYPE { $$ = MCC_AST_DATA_TYPE_INT; }
     | FLOAT_TYPE { $$ = MCC_AST_DATA_TYPE_FLOAT; }
     | STRING_TYPE { $$ = MCC_AST_DATA_TYPE_STRING; }
     | BOOL_TYPE { $$ = MCC_AST_DATA_TYPE_BOOL; }
     | VOID { $$ = MCC_AST_DATA_TYPE_VOID; }
     ;


statement : expression SEMICOLON        { $$ = mcc_ast_new_statement_expression($1); loc($$, @1); }
          | if_statement                { $$ = $1;  loc($$, @1); }
	      | while_statement             { $$ = $1; loc($$, @1); }
	      | compound_statement          { $$ = $1; loc($$, @1); }
          | assignment SEMICOLON        { $$ = mcc_ast_new_statement_assignment($1); loc($$, @1); }
          | declaration SEMICOLON       { $$ = mcc_ast_new_statement_declaration($1); loc($$, @1); }
          | RETURN expression SEMICOLON { $$ = mcc_ast_new_statement_return($2); loc($$,@1);}
          | RETURN SEMICOLON            { $$ = mcc_ast_new_statement_return(NULL); loc($$,@1);}
	      ;

if_statement: IF LPARENTH expression RPARENTH statement                 { $$ = mcc_ast_new_statement_if($3, $5,NULL);  loc($$, @1); }
            | IF LPARENTH expression RPARENTH statement ELSE statement  { $$ = mcc_ast_new_statement_if($3, $5, $7);  loc($$, @1); }
            ;

declaration:    type identifier                             { $$ = mcc_ast_new_declaration($1,NULL, $2); loc($$, @1); }
            |   type LBRACKET literal RBRACKET identifier   { $$ = mcc_ast_new_declaration($1,$3, $5); loc($$, @1); }
	        ;


while_statement: WHILE LPARENTH expression RPARENTH statement { $$ = mcc_ast_new_statement_while($3, $5); loc($$, @1); }
		;

compound_statement:  LBRACE statement_list RBRACE                         { $$ = mcc_ast_new_statement_compound($2); loc($$, @1); }  
                  |  LBRACE RBRACE                                        { $$ = mcc_ast_new_statement_compound(NULL); loc($$, @1); }
		  ;

statement_list:	 statement statement_list 	{ $$ = mcc_ast_new_statement_list($1); $$ -> next = $2; loc($$, @1); }
              |  statement 			        { $$ = mcc_ast_new_statement_list($1); loc($$, @1); }
	      ;

assignment:  identifier ASSIGNMENT expression
		    { $$ = mcc_ast_new_assignment($1, $3); 	loc($$, @1); };
          |  identifier LBRACKET expression RBRACKET ASSIGNMENT expression
          	{ $$ = mcc_ast_new_array_assignment($1, $3, $6); 	loc($$, @1); };
          ;

call_expression: identifier LPARENTH RPARENTH 		{ $$ = mcc_ast_new_expression_call_expression($1, NULL); loc($$, @1); }
	       | identifier LPARENTH argument RPARENTH  { $$ = mcc_ast_new_expression_call_expression($1, $3); loc($$, @1); }
	       ;

argument: expression COMMA argument { $$ = mcc_ast_add_new_argument($1, $3); loc($$, @1); }
	    | expression                { $$ = mcc_ast_new_argument($1) ; loc($$, @1); }


parameters: declaration COMMA parameters 	{ $$ = mcc_ast_new_parameter($1, $3); loc($$, @1); }
	      | declaration 			        { $$ = mcc_ast_new_parameter($1, NULL); loc($$, @1); }
          ;

function_def: type identifier LPARENTH parameters RPARENTH compound_statement
		{ $$ = mcc_ast_new_function($1, $2, $4, $6);   loc($$, @1);};
	    | type identifier LPARENTH RPARENTH compound_statement
		{ $$ = mcc_ast_new_function($1, $2, NULL, $5); loc($$, @1);}
	    ;

program : function_def { $$ = mcc_ast_new_program($1); loc($$, @1);}
	| function_def program { $$ = mcc_ast_add_function($1, $2); loc($$, @1); }
	;

%%
#include <assert.h>
#include "scanner.h"
#include "utils/unused.h"
void mcc_parser_error(struct MCC_PARSER_LTYPE *yylloc, yyscan_t *scanner,struct mcc_parser_result *result, const char *msg)
{

	struct mcc_ast_source_location *loc = malloc(sizeof(*loc));

	loc -> start_line = yylloc -> first_line;
	loc -> start_col = yylloc -> first_column;
	loc -> end_line = yylloc -> last_line;
	loc -> end_col = yylloc -> last_column;

	struct mcc_parser_error *error = new_parser_error(loc, (char*) msg);
	result -> parser_error = error;
	free(loc);
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

	if (yyparse(scanner, &result) != 0) {
		result.status = MCC_PARSER_STATUS_UNKNOWN_ERROR;
	}
	mcc_parser_lex_destroy(scanner);
	return result;
}