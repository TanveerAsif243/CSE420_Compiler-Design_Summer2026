%option noyywrap
%x COMMENT

%{
#include "symbol_info.h"
#define YYSTYPE symbol_info*
#include "y.tab.h"

extern YYSTYPE yylval;
extern int lines;
%}

delim      [ \t\v\r\f]
newline    \r?\n
ws         {delim}+
letter_    [A-Za-z_]
digit      [0-9]
id         {letter_}({letter_}|{digit})*
integers   {digit}+
floats     ({digit}+\.{digit}*|{digit}*\.{digit}+)([Ee][+-]?{digit}+)?|{digit}+[Ee][+-]?{digit}+

%%

{ws}                    { /* ignore whitespace */ }
{newline}               { lines++; }
"//"[^\n]*              { /* ignore single-line comment */ }
"/*"                    { BEGIN(COMMENT); }
<COMMENT>"*/"           { BEGIN(INITIAL); }
<COMMENT>{newline}      { lines++; }
<COMMENT>.              { /* ignore block-comment content */ }

"if"                    { return IF; }
"else"                  { return ELSE; }
"for"                   { return FOR; }
"while"                 { return WHILE; }
"do"                    { return DO; }
"break"                 { return BREAK; }
"continue"              { return CONTINUE; }
"return"                { return RETURN; }
"int"                   { return INT; }
"float"                 { return FLOAT; }
"char"                  { return CHAR; }
"void"                  { return VOID; }
"double"                { return DOUBLE; }
"switch"                { return SWITCH; }
"case"                  { return CASE; }
"default"               { return DEFAULT; }
"printf"|"println"      { return PRINTLN; }

"+"|"-"                 {
                            yylval = (YYSTYPE)new symbol_info((string)yytext, "ADDOP");
                            return ADDOP;
                        }
"*"|"/"|"%"             {
                            yylval = (YYSTYPE)new symbol_info((string)yytext, "MULOP");
                            return MULOP;
                        }
"++"                    { return INCOP; }
"--"                    { return DECOP; }
"<"|">"|"<="|">="|"=="|"!=" {
                            yylval = (YYSTYPE)new symbol_info((string)yytext, "RELOP");
                            return RELOP;
                        }
"="                     { return ASSIGNOP; }
"&&"|"||"               {
                            yylval = (YYSTYPE)new symbol_info((string)yytext, "LOGICOP");
                            return LOGICOP;
                        }
"!"                     { return NOT; }
"("                     { return LPAREN; }
")"                     { return RPAREN; }
"{"                     { return LCURL; }
"}"                     { return RCURL; }
"["                     { return LTHIRD; }
"]"                     { return RTHIRD; }
";"                     { return SEMICOLON; }
","                     { return COMMA; }

{floats}                {
                            yylval = (YYSTYPE)new symbol_info((string)yytext, "FLOAT");
                            return CONST_FLOAT;
                        }
{integers}              {
                            yylval = (YYSTYPE)new symbol_info((string)yytext, "INT");
                            return CONST_INT;
                        }
{id}                    {
                            yylval = (YYSTYPE)new symbol_info((string)yytext, "ID");
                            return ID;
                        }
.                       { /* ignore unsupported single character */ }

%%
