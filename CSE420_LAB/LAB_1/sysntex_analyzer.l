%option noyywrap

%{
#include<bits/stdc++.h>
#include "symbol_info.h"

#define YYSTYPE symbol_info*

#include "y.tab.h"

extern YYSTYPE yylval;

using namespace std;

void yyerror(char *);

string loglist;

extern int line_num;
extern ofstream outlog;

%}



delim       [ \t\v\r]
ws          {delim}+
newline     \n

letter_     [A-Za-z_]
digit       [0-9]

id          {letter_}({letter_}|{digit})*
integer     {digit}+
float_const (({digit}*\.{digit}+)|({digit}+\.{digit}*))([Ee][+-]?{digit}+)?|({digit}+[Ee][+-]?{digit}+)

%%

{ws}        { }

{newline}   {
                line_num++;
            }


if          {
                yylval = new symbol_info((string)yytext, "IF");
                outlog << "Line no " << line_num << ": Token <IF> Lexeme " << yytext << " found\n\n";
                return IF;
            }

else        {
                yylval = new symbol_info((string)yytext, "ELSE");
                outlog << "Line no " << line_num << ": Token <ELSE> Lexeme " << yytext << " found\n\n";
                return ELSE;
            }

for         {
                yylval = new symbol_info((string)yytext, "FOR");
                outlog << "Line no " << line_num << ": Token <FOR> Lexeme " << yytext << " found\n\n";
                return FOR;
            }

while       {
                yylval = new symbol_info((string)yytext, "WHILE");
                outlog << "Line no " << line_num << ": Token <WHILE> Lexeme " << yytext << " found\n\n";
                return WHILE;
            }

do          {
                yylval = new symbol_info((string)yytext, "DO");
                outlog << "Line no " << line_num << ": Token <DO> Lexeme " << yytext << " found\n\n";
                return DO;
            }

break       {
                yylval = new symbol_info((string)yytext, "BREAK");
                outlog << "Line no " << line_num << ": Token <BREAK> Lexeme " << yytext << " found\n\n";
                return BREAK;
            }

continue    {
                yylval = new symbol_info((string)yytext, "CONTINUE");
                outlog << "Line no " << line_num << ": Token <CONTINUE> Lexeme " << yytext << " found\n\n";
                return CONTINUE;
            }

return      {
                yylval = new symbol_info((string)yytext, "RETURN");
                outlog << "Line no " << line_num << ": Token <RETURN> Lexeme " << yytext << " found\n\n";
                return RETURN;
            }

int         {
                yylval = new symbol_info((string)yytext, "INT");
                outlog << "Line no " << line_num << ": Token <INT> Lexeme " << yytext << " found\n\n";
                return INT;
            }

float       {
                yylval = new symbol_info((string)yytext, "FLOAT");
                outlog << "Line no " << line_num << ": Token <FLOAT> Lexeme " << yytext << " found\n\n";
                return FLOAT;
            }

double      {
                yylval = new symbol_info((string)yytext, "DOUBLE");
                outlog << "Line no " << line_num << ": Token <DOUBLE> Lexeme " << yytext << " found\n\n";
                return DOUBLE;
            }

char        {
                yylval = new symbol_info((string)yytext, "CHAR");
                outlog << "Line no " << line_num << ": Token <CHAR> Lexeme " << yytext << " found\n\n";
                return CHAR;
            }

void        {
                yylval = new symbol_info((string)yytext, "VOID");
                outlog << "Line no " << line_num << ": Token <VOID> Lexeme " << yytext << " found\n\n";
                return VOID;
            }

switch      {
                yylval = new symbol_info((string)yytext, "SWITCH");
                outlog << "Line no " << line_num << ": Token <SWITCH> Lexeme " << yytext << " found\n\n";
                return SWITCH;
            }

case        {
                yylval = new symbol_info((string)yytext, "CASE");
                outlog << "Line no " << line_num << ": Token <CASE> Lexeme " << yytext << " found\n\n";
                return CASE;
            }

default     {
                yylval = new symbol_info((string)yytext, "DEFAULT");
                outlog << "Line no " << line_num << ": Token <DEFAULT> Lexeme " << yytext << " found\n\n";
                return DEFAULT;
            }

goto        {
                yylval = new symbol_info((string)yytext, "GOTO");
                outlog << "Line no " << line_num << ": Token <GOTO> Lexeme " << yytext << " found\n\n";
                return GOTO;
            }

printf      {
                yylval = new symbol_info((string)yytext, "PRINTLN");
                outlog << "Line no " << line_num << ": Token <PRINTLN> Lexeme " << yytext << " found\n\n";
                return PRINTLN;
            }



{float_const} {
                yylval = new symbol_info((string)yytext, "CONST_FLOAT");
                outlog << "Line no " << line_num << ": Token <CONST_FLOAT> Lexeme " << yytext << " found\n\n";
                return CONST_FLOAT;
            }

{integer}   {
                yylval = new symbol_info((string)yytext, "CONST_INT");
                outlog << "Line no " << line_num << ": Token <CONST_INT> Lexeme " << yytext << " found\n\n";
                return CONST_INT;
            }



{id}        {
                yylval = new symbol_info((string)yytext, "ID");
                outlog << "Line no " << line_num << ": Token <ID> Lexeme " << yytext << " found\n\n";
                return ID;
            }



"++"        {
                yylval = new symbol_info((string)yytext, "INCOP");
                outlog << "Line no " << line_num << ": Token <INCOP> Lexeme " << yytext << " found\n\n";
                return INCOP;
            }

"--"        {
                yylval = new symbol_info((string)yytext, "DECOP");
                outlog << "Line no " << line_num << ": Token <DECOP> Lexeme " << yytext << " found\n\n";
                return DECOP;
            }

"<="|">="|"=="|"!="|"<"|">" {
                yylval = new symbol_info((string)yytext, "RELOP");
                outlog << "Line no " << line_num << ": Token <RELOP> Lexeme " << yytext << " found\n\n";
                return RELOP;
            }

"="         {
                yylval = new symbol_info((string)yytext, "ASSIGNOP");
                outlog << "Line no " << line_num << ": Token <ASSIGNOP> Lexeme " << yytext << " found\n\n";
                return ASSIGNOP;
            }

"&&"|"||"   {
                yylval = new symbol_info((string)yytext, "LOGICOP");
                outlog << "Line no " << line_num << ": Token <LOGICOP> Lexeme " << yytext << " found\n\n";
                return LOGICOP;
            }

"!"         {
                yylval = new symbol_info((string)yytext, "NOT");
                outlog << "Line no " << line_num << ": Token <NOT> Lexeme " << yytext << " found\n\n";
                return NOT;
            }

"*"|"/"|"%" {
                yylval = new symbol_info((string)yytext, "MULOP");
                outlog << "Line no " << line_num << ": Token <MULOP> Lexeme " << yytext << " found\n\n";
                return MULOP;
            }

"+"|"-"     {
                yylval = new symbol_info((string)yytext, "ADDOP");
                outlog << "Line no " << line_num << ": Token <ADDOP> Lexeme " << yytext << " found\n\n";
                return ADDOP;
            }



"("         {
                yylval = new symbol_info((string)yytext, "LPAREN");
                outlog << "Line no " << line_num << ": Token <LPAREN> Lexeme " << yytext << " found\n\n";
                return LPAREN;
            }

")"         {
                yylval = new symbol_info((string)yytext, "RPAREN");
                outlog << "Line no " << line_num << ": Token <RPAREN> Lexeme " << yytext << " found\n\n";
                return RPAREN;
            }

"{"         {
                yylval = new symbol_info((string)yytext, "LCURL");
                outlog << "Line no " << line_num << ": Token <LCURL> Lexeme " << yytext << " found\n\n";
                return LCURL;
            }

"}"         {
                yylval = new symbol_info((string)yytext, "RCURL");
                outlog << "Line no " << line_num << ": Token <RCURL> Lexeme " << yytext << " found\n\n";
                return RCURL;
            }

"["         {
                yylval = new symbol_info((string)yytext, "LTHIRD");
                outlog << "Line no " << line_num << ": Token <LTHIRD> Lexeme " << yytext << " found\n\n";
                return LTHIRD;
            }

"]"         {
                yylval = new symbol_info((string)yytext, "RTHIRD");
                outlog << "Line no " << line_num << ": Token <RTHIRD> Lexeme " << yytext << " found\n\n";
                return RTHIRD;
            }

","         {
                yylval = new symbol_info((string)yytext, "COMMA");
                outlog << "Line no " << line_num << ": Token <COMMA> Lexeme " << yytext << " found\n\n";
                return COMMA;
            }

":"         {
                yylval = new symbol_info((string)yytext, "COLON");
                outlog << "Line no " << line_num << ": Token <COLON> Lexeme " << yytext << " found\n\n";
                return COLON;
            }

";"         {
                yylval = new symbol_info((string)yytext, "SEMICOLON");
                outlog << "Line no " << line_num << ": Token <SEMICOLON> Lexeme " << yytext << " found\n\n";
                return SEMICOLON;
            }



.           {
                outlog << "Line no " << line_num << ": Unrecognized character " << yytext << "\n\n";
            }

%%