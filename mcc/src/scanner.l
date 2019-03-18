%option prefix="mcc_parser_"

%option batch
%option bison-bridge
%option bison-locations
%option noinput
%option nounput
%option noyywrap
%option reentrant
%option yylineno

%{
#include "parser.tab.h"

#define YYSTYPE MCC_PARSER_STYPE
#define YYLTYPE MCC_PARSER_LTYPE

// TODO
// - track lines
// - take line breaks into account

#define YY_USER_ACTION \
	yylloc->first_column = yylloc->last_column; \
	yylloc->last_column += yyleng;
%}

int_literal   [0-9]+
float_literal [0-9]+\.[0-9]+
bool_literal true|false
identifier [a-zA-Z_][a-zA-Z0-9_]*

%%

{int_literal}     { yylval->TK_INT_LITERAL = atol(yytext); return TK_INT_LITERAL; }

{float_literal}   { yylval->TK_FLOAT_LITERAL = atof(yytext); return TK_FLOAT_LITERAL; }

{bool_literal}    { yylval->TK_BOOL_LITERAL = strcmp(yytext, "true") == 0; return TK_BOOL_LITERAL; }

{string_literal}  { int len = strlen(yytext) - 1; yylval->TK_STRING_LITERAL = malloc(len);
                    memcpy(yylval->TK_STRING_LITERAL, &yytext[1], len);
                    yylval->TK_STRING_LITERAL[len - 1] = '\0';
                    return TK_STRING_LITERAL;
                  }

{identifier}      {
                    yylval->TK_ID = mCc_ast_new_identifier(yytext);
                    yylval->TK_ID->node.sloc.start_line = yylloc->first_line;
                    yylval->TK_ID->node.sloc.end_line = yylloc->last_line;
                    yylval->TK_ID->node.sloc.start_col = yylloc->first_column;
                    yylval->TK_ID->node.sloc.end_col = yylloc->last_column;
                    return TK_ID;
                  }

"+"               { return TK_PLUS; }
"-"               { return TK_MINUS; }
"*"               { return TK_ASTER; }
"/"               { return TK_SLASH; }

"("               { return TK_LPARENTH; }
")"               { return TK_RPARENTH; }

"{"               { return TK_LBRACE; }
"}"               { return TK_RBRACE; }

"["               { return TK_LBRACKET; }
"]"               { return TK_RBRACKET; }

"<"               { return TK_LESS; }
">"               { return TK_GREATER; }

"<="              { return TK_LESS_EQ; }
">="              { return TK_GREATER_EQ; }
"=="              { return TK_EQUALS; }
"!="              { return TK_NOT_EQUALS; }

"="               { return TK_ASSIGNMENT; }
";"               { return TK_SEMICOLON; }
","               { return TK_COMMA; }

"!"               { return TK_NEGATION; }

"&&"              { return TK_AND; }
"||"              { return TK_OR; }

"bool"            { return TK_BOOL_TYPE; }
"int"             { return TK_INT_TYPE; }
"float"           { return TK_FLOAT_TYPE; }
"string"          { return TK_STRING_TYPE; }
"void"            { return TK_VOID_TYPE; }

"if"              { return TK_IF; }
"else"            { return TK_ELSE; }
"while"           { return TK_WHILE; }
"return"          { return TK_RETURN; }

[ \t\r\n]+        { /* ignore */ }

<<EOF>>           { return TK_END; }

.                 { fprintf(stderr, "invalid character '%c'\n", yytext[0]); }
