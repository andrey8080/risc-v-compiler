%{
#include "parser.tab.h"
#include <string>
#include <cstring>

extern int yylineno;
%}

%option nounput
%option noinput

DIGIT    [0-9]
LETTER   [a-zA-Z_]
ID       {LETTER}({LETTER}|{DIGIT})*
INT      {DIGIT}+
STRING   \"([^\\\"]|\\.)*\"
WS       [ \t\r\n]+

%%

"int"           { return INT_TYPE; }
"string"        { return STRING_TYPE; }
"if"            { return IF; }
"else"          { return ELSE; }
"while"         { return WHILE; }
"print"         { return PRINT; }
"not"           { return NOT; }
"and"           { return AND; }
"or"            { return OR; }

"=="            { return EQ; }
"!="            { return NEQ; }
"<="            { return LE; }
">="            { return GE; }
"<"             { return LT; }
">"             { return GT; }
"+"             { return PLUS; }
"-"             { return MINUS; }
"*"             { return MUL; }
"/"             { return DIV; }
"="             { return ASSIGN; }
";"             { return SEMI; }
"{"             { return LBRACE; }
"}"             { return RBRACE; }
"("             { return LPAREN; }
")"             { return RPAREN; }

{INT}           { yylval.int_val = atoi(yytext); return INTEGER; }
{STRING}        { yylval.str_val = strdup(yytext); return STRING; }
{ID}            { yylval.str_val = strdup(yytext); return IDENTIFIER; }

"//".*          { /* skip single-line comment */ }
"/*"([^*]|\*+[^*/])*\*+"/"  { /* skip multi-line comment */ }

{WS}            { /* skip whitespace */ }

.               { printf("Unexpected character: %s\n", yytext); }

%%

int yywrap() {
    return 1;
}
