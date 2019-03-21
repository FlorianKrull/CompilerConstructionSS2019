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
#include <string.h>
#include "parser.tab.h"

#define YYSTYPE MCC_PARSER_STYPE
#define YYLTYPE MCC_PARSER_LTYPE

// TODO:
// - track lines
// - take line breaks into account
#define YY_USER_ACTION \
	yylloc->first_column = yylloc->last_column; \
	yylloc->last_column += yyleng;
%}

identifier     [a-zA-Z_]+

bool_literal   true|false
int_literal    [0-9]+
float_literal  [0-9]+\.[0-9]+
string_literal \"[^"]*\"
%%

<INITIAL>{
"/*" 	BEGIN(IN_COMMENT);
}
<IN_COMMENT>{ // use exclusive state to deal with multiline comments; credit goes to Peter Kirk
"*/"        BEGIN(INITIAL);
[^*]+     //eat comments in chunk
"*"         //eat the lone star
<<EOF>>     return TK_UNCLOSED_COMMENT;
}

if     { return TK_IF_KW; }
else   { return TK_ELSE_KW; }
while  { return TK_WHILE_KW; }
return { return TK_RETURN_KW; }

void   { return TK_VOID_TYPE; }  
bool   { return TK_BOOL_TYPE; }
int    { return TK_INT_TYPE; }
float  { return TK_FLOAT_TYPE; }
string { return TK_STR_TYPE; }

"("  { return TK_LPAREN; }
")"  { return TK_RPAREN; }
"["  { return TK_LBRACK; }
"]"  { return TK_RBRACK; }
"{"  { return TK_LBRACE; }
"}"  { return TK_RBRACE; }

","  { return TK_COMMA; }
";"  { return TK_SEMIC; }

"!"  { return TK_BANG; }
"&&" { return TK_LAND; }
"||" { return TK_LOR; }
"+"  { return TK_PLUS; }
"-"  { return TK_MINUS; }
"*"  { return TK_ASTER; }
"/"  { return TK_SLASH; }
"="  { return TK_EQUAL; }

"<"  { return TK_LTREL; }
">"  { return TK_GTREL; }
"<=" { return TK_LEREL; }
">=" { return TK_GEREL; }
"==" { return TK_EQREL; }
"!=" { return TK_NEREL; }


[ \t\r\n]+        { /* ignore */ }

<<EOF>>           { return TK_END; }


