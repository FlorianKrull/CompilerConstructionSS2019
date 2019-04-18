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

alpha         [a-zA-Z_]
alpha_num     [a-zA-Z0-9_]
digit         [0-9]

int_literal   {digit}+
float_literal {digit}+\.{digit}+
bool_literal  true|false
identifier {alpha}{alpha_num}*
string_literal \"(\\.|[^"\\])*\"
comment (\/\*.*\*\/)
comment_multiline ("/*"([^*]|\*+[^*/])*\*+"/")


%%


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

"!"               { return TK_NOT; }

"&&"              { return TK_AND; }
"||"              { return TK_OR; }

"bool"            { return TK_BOOL_TYPE; }
"int"             { return TK_INT_TYPE; }
"float"           { return TK_FLOAT_TYPE; }
"string"          { return TK_STRING_TYPE; }
"void"            { return TK_VOID; }

"if"              { return TK_IF; }
"else"            { return TK_ELSE; }
"while"           { return TK_WHILE; }
"return"          { return TK_RETURN; }
"for"             { return TK_FOR; }


[ \t\r\n]+        { /* ignore */ }

{identifier}      {
                    yylval->TK_IDENTIFIER = strdup(yytext);
                    return TK_IDENTIFIER;
                  }
{bool_literal}    {
                    yylval->TK_BOOL_LITERAL = strdup(yytext);
                    return TK_BOOL_LITERAL;
                  }

{int_literal}     { yylval->TK_INT_LITERAL = strdup(yytext);
                    return TK_INT_LITERAL; }

{float_literal}   { yylval->TK_FLOAT_LITERAL = strdup(yytext);
                    return TK_FLOAT_LITERAL; }



<<EOF>>           { return TK_END; }



.                 { fprintf(stderr, "invalid character '%c'\n", yytext[0]); }
