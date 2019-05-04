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

/* Update line and column count *
* Reference: https://stackoverflow.com/a/22125500  */

#define YY_USER_ACTION \
yylloc->first_column = yylloc->last_column; \
yylloc->last_column += yyleng; \
for(int i = 0; yytext[i] != '\0'; i++) { \
    if(yytext[i] == '\n') { \
        yylloc->last_line++; \
        yylloc->last_column = 0; \
    } \
    else { \
        yylloc->last_column++; \
    } \
}

int line_num = 1;
int comment_caller;
%}

alpha         [a-zA-Z_]
alpha_num     [a-zA-Z0-9_]
digit         [0-9]

int_literal   {digit}+
float_literal {digit}+\.{digit}+
bool_literal  true|false
identifier {alpha}{alpha_num}*
string_literal \"(\\.|[^"\\])*\"

%x comment foo

/* Scanning comments based on documentation *
 * Documentation: ftp://ftp.gnu.org/old-gnu/Manuals/flex-2.5.4/html_mono/flex.html */

%%

"/*"         {
                comment_caller = INITIAL;
                BEGIN(comment);
             }

<foo>"/*" {
          comment_caller = foo;
          BEGIN(comment);
          }

<comment>[^*\n]*        /* eat anything that's not a '*' */
<comment>"*"+[^*/\n]*   /* eat up '*'s not followed by '/'s */
<comment>\n             ++line_num;
<comment>"*"+"/"        BEGIN(comment_caller);

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
{int_literal}     { yylval->TK_INT_LITERAL = atol(yytext); return TK_INT_LITERAL; }

{float_literal}   { yylval->TK_FLOAT_LITERAL = atof(yytext); return TK_FLOAT_LITERAL; }

{string_literal}  { yylval->TK_STRING_LITERAL = yytext; return TK_STRING_LITERAL; }

{bool_literal}    { yylval->TK_BOOL_LITERAL = strcmp(yytext, "true") == 0; return TK_BOOL_LITERAL; }



<<EOF>>           { return TK_END; }



.                 { fprintf(stderr, "invalid character '%c'\n", yytext[0]); }
