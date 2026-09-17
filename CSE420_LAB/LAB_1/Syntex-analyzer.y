%{

#include <stdio.h>
#include <stdlib.h>
#include<bits/stdc++.h>
#include"symbol_info.h"

#define YYSTYPE symbol_info*

using namespace std;

int yyparse(void);
int yylex(void);

extern FILE *yyin;

ofstream outlog;

int line_num = 1;

void yyerror(char *s)
{
    outlog << "Error at line no " << line_num << ": " << s << endl;
}

void logRule(const string &lhs, const string &rhs, const string &text)
{
    outlog << "At line no: " << line_num << " " << lhs << " : " << rhs << " " << endl << endl;
    outlog << text << endl << endl;
}

void logFactorInfo(const string &text)
{
    logRule("factor_info", "factor", text);
}

%}

%token IF FOR WHILE DO BREAK CONTINUE RETURN
%token INT FLOAT DOUBLE CHAR VOID
%token SWITCH CASE DEFAULT GOTO PRINTLN
%token ID CONST_INT CONST_FLOAT
%token ADDOP MULOP INCOP DECOP RELOP ASSIGNOP LOGICOP NOT
%token LPAREN RPAREN LCURL RCURL LTHIRD RTHIRD COMMA COLON SEMICOLON

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%

start 
    : program
    {
        outlog << "At line no: " << line_num << " start : program " << endl << endl;
    }
    ;

program 
    : program unit
    {
        logRule("program", "program unit", $1->getnameofsymbol() + "\n" + $2->getnameofsymbol());

        $$ = new symbol_info($1->getnameofsymbol() + "\n" + $2->getnameofsymbol(), "program");
    }
    | unit
    {
        logRule("program", "unit", $1->getnameofsymbol());

        $$ = new symbol_info($1->getnameofsymbol(), "program");
    }
    ;

unit
    : func_definition
    {
        logRule("unit", "func_definition", $1->getnameofsymbol());

        $$ = new symbol_info($1->getnameofsymbol(), "unit");
    }
    | declaration
    {
        logRule("unit", "declaration", $1->getnameofsymbol());

        $$ = new symbol_info($1->getnameofsymbol(), "unit");
    }
    ;

func_definition 
    : type_specifier ID LPAREN param_list RPAREN compound_statement
    {
        logRule("func_definition", "type_specifier ID LPAREN param_list RPAREN compound_statement", $1->getnameofsymbol() + " " + $2->getnameofsymbol() + "(" + $4->getnameofsymbol() + ")\n" + $6->getnameofsymbol());

        $$ = new symbol_info($1->getnameofsymbol() + " " + $2->getnameofsymbol() + "(" + $4->getnameofsymbol() + ")\n" + $6->getnameofsymbol(), "func_def");
    }
    | type_specifier ID LPAREN RPAREN compound_statement
    {
        logRule("func_definition", "type_specifier ID LPAREN RPAREN compound_statement", $1->getnameofsymbol() + " " + $2->getnameofsymbol() + "()\n" + $5->getnameofsymbol());

        $$ = new symbol_info($1->getnameofsymbol() + " " + $2->getnameofsymbol() + "()\n" + $5->getnameofsymbol(), "func_def");
    }
    ;

declaration
    : type_specifier declaration_list SEMICOLON
    {
        logRule("declaration", "type_specifier declaration_list SEMICOLON", $1->getnameofsymbol() + " " + $2->getnameofsymbol() + ";");

        $$ = new symbol_info($1->getnameofsymbol() + " " + $2->getnameofsymbol() + ";", "declaration");
    }
    ;

type_specifier
    : INT
    {
        logRule("type_specifier", "INT", "int");
        $$ = new symbol_info("int", "type");
    }
    | FLOAT
    {
        logRule("type_specifier", "FLOAT", "float");
        $$ = new symbol_info("float", "type");
    }
    | CHAR
    {
        logrule("type_specifier", "CHAR", "char");
        $$ = new symbol_info("char", "type");
    }
    | VOID
    {
        logRule("type_specifier", "VOID", "void");
        $$ = new symbol_info("void", "type");
    }
    ;

declaration_list
    : declaration_list COMMA ID
    {
        string text = $1->getnameofsymbol() + "," + $3->getnameofsymbol();
        logRule("declaration_list", "declaration_list COMMA ID", text);
        $$ = new symbol_info(text, "declaration_list");
    }
    | declaration_list COMMA ID LTHIRD CONST_INT RTHIRD
    {
        string text = $1->getnameofsymbol() + "," + $3->getnameofsymbol() + "[" + $5->getnameofsymbol() + "]";
        logRule("declaration_list", "declaration_list COMMA ID LTHIRD CONST_INT RTHIRD", text);
        $$ = new symbol_info(text, "declaration_list");
    }
    | ID
    {
        logRule("declaration_list", "ID", $1->getnameofsymbol());
        $$ = new symbol_info($1->getnameofsymbol(), "declaration_list");
    }
    | ID LTHIRD CONST_INT RTHIRD
    {
        string text = $1->getnameofsymbol() + "[" + $3->getnameofsymbol() + "]";
        logRule("declaration_list", "ID LTHIRD CONST_INT RTHIRD", text);
        $$ = new symbol_info(text, "declaration_list");
    }
    ;

param_list
    : param_list COMMA type_specifier ID
    {
        string text = $1->getnameofsymbol() + "," + $3->getnameofsymbol() + " " + $4->getnameofsymbol();
        logRule("param_list", "param_list COMMA type_specifier ID", text);
        $$ = new symbol_info(text, "param_list");
    }
    | param_list COMMA type_specifier
    {
        string text = $1->getnameofsymbol() + "," + $3->getnameofsymbol();
        logRule("param_list", "param_list COMMA type_specifier", text);
        $$ = new symbol_info(text, "param_list");
    }
    | type_specifier ID
    {
        string text = $1->getnameofsymbol() + " " + $2->getnameofsymbol();
        logRule("param_list", "type_specifier ID", text);
        $$ = new symbol_info(text, "param_list");
    }
    | type_specifier
    {
        logRule("param_list", "type_specifier", $1->getnameofsymbol());
        $$ = new symbol_info($1->getnameofsymbol(), "param_list");
    }
    ;

compound_statement
    : LCURL statements RCURL
    {
        logRule("compound_statement", "LCURL statements RCURL", "{\n" + $2->getnameofsymbol() + "\n}");

        $$ = new symbol_info("{\n" + $2->getnameofsymbol() + "\n}", "compound_statement");
    }
    | LCURL RCURL
    {
        logRule("compound_statement", "LCURL RCURL", "{}");

        $$ = new symbol_info("{}", "compound_statement");
    }
    ;

statements
    : statements statement
    {
        string text = $1->getnameofsymbol() + "\n" + $2->getnameofsymbol();
        logRule("statements", "statements statement", text);
        $$ = new symbol_info(text, "statements");
    }
    | statement
    {
        logRule("statements", "statement", $1->getnameofsymbol());
        $$ = new symbol_info($1->getnameofsymbol(), "statements");
    }
    ;

statement
    : declaration
    {
        logRule("statement", "declaration", $1->getnameofsymbol());
        $$ = new symbol_info($1->getnameofsymbol(), "statement");
    }
    | expression_statement
    {
        logRule("statement", "expression_statement", $1->getnameofsymbol());
        $$ = new symbol_info($1->getnameofsymbol(), "statement");
    }
    | compound_statement
    {
        logRule("statement", "compound_statement", $1->getnameofsymbol());
        $$ = new symbol_info($1->getnameofsymbol(), "statement");
    }
    | FOR LPAREN expression_statement expression_statement expression RPAREN statement
    {
        logRule("statement", "FOR LPAREN expression_statement expression_statement expression RPAREN statement", "for(" + $3->getnameofsymbol() + $4->getnameofsymbol() + $5->getnameofsymbol() + ")\n" + $7->getnameofsymbol());

        $$ = new symbol_info("for(" + $3->getnameofsymbol() + $4->getnameofsymbol() + $5->getnameofsymbol() + ")\n" + $7->getnameofsymbol(), "statement");
    }
    | IF LPAREN expression RPAREN statement %prec LOWER_THAN_ELSE
    {
        string text = "if(" + $3->getnameofsymbol() + ")\n" + $5->getnameofsymbol();
        logRule("statement", "IF LPAREN expression RPAREN statement", text);
        $$ = new symbol_info(text, "statement");
    }
    | IF LPAREN expression RPAREN statement ELSE statement
    {
        string text = "if(" + $3->getnameofsymbol() + ")\n" + $5->getnameofsymbol() + "\nelse\n" + $7->getnameofsymbol();
        logRule("statement", "IF LPAREN expression RPAREN statement ELSE statement", text);
        $$ = new symbol_info(text, "statement");
    }
    | WHILE LPAREN expression RPAREN statement
    {
        string text = "while(" + $3->getnameofsymbol() + ")\n" + $5->getnameofsymbol();
        logRule("statement", "WHILE LPAREN expression RPAREN statement", text);
        $$ = new symbol_info(text, "statement");
    }
    | PRINTLN LPAREN ID RPAREN SEMICOLON
    {
        string text = "printf(" + $3->getnameofsymbol() + ");";
        logRule("statement", "PRINTLN LPAREN ID RPAREN SEMICOLON", text);
        $$ = new symbol_info(text, "statement");
    }
    | RETURN expression SEMICOLON
    {
        string text = "return " + $2->getnameofsymbol() + ";";
        logRule("statement", "RETURN expression SEMICOLON", text);
        $$ = new symbol_info(text, "statement");
    }
    ;

expression_statement
    : SEMICOLON
    {
        logRule("expression_statement", "SEMICOLON", ";");
        $$ = new symbol_info(";", "expression_statement");
    }
    | expression SEMICOLON
    {
        string text = $1->getnameofsymbol() + ";";
        logRule("expression_statement", "expression SEMICOLON", text);
        $$ = new symbol_info(text, "expression_statement");
    }
    ;

expression
    : logic_expression
    {
        logRule("expression", "logic_expression", $1->getnameofsymbol());
        $$ = new symbol_info($1->getnameofsymbol(), "expression");
    }
    | variable ASSIGNOP logic_expression
    {
        string text = $1->getnameofsymbol() + "=" + $3->getnameofsymbol();
        logRule("expression", "variable ASSIGNOP logic_expression", text);
        $$ = new symbol_info(text, "expression");
    }
    ;

logic_expression
    : rel_expression
    {
        logRule("logic_expression", "rel_expression", $1->getnameofsymbol());
        $$ = new symbol_info($1->getnameofsymbol(), "logic_expression");
    }
    | rel_expression LOGICOP rel_expression
    {
        string text = $1->getnameofsymbol() + $2->getnameofsymbol() + $3->getnameofsymbol();
        logRule("logic_expression", "rel_expression LOGICOP rel_expression", text);
        $$ = new symbol_info(text, "logic_expression");
    }
    ;

rel_expression
    : simple_expression
    {
        logRule("rel_expression", "simple_expression", $1->getnameofsymbol());
        $$ = new symbol_info($1->getnameofsymbol(), "rel_expression");
    }
    | simple_expression RELOP simple_expression
    {
        string text = $1->getnameofsymbol() + $2->getnameofsymbol() + $3->getnameofsymbol();
        logRule("rel_expression", "simple_expression RELOP simple_expression", text);
        $$ = new symbol_info(text, "rel_expression");
    }
    ;

simple_expression
    : term
    {
        logRule("simple_expression", "term", $1->getnameofsymbol());
        $$ = new symbol_info($1->getnameofsymbol(), "simple_expression");
    }
    | simple_expression ADDOP term
    {
        string text = $1->getnameofsymbol() + $2->getnameofsymbol() + $3->getnameofsymbol();
        logRule("simple_expression", "simple_expression ADDOP term", text);
        $$ = new symbol_info(text, "simple_expression");
    }
    ;

term
    : unary_expression
    {
        logRule("term", "unary_expression", $1->getnameofsymbol());
        $$ = new symbol_info($1->getnameofsymbol(), "term");
    }
    | term MULOP unary_expression
    {
        string text = $1->getnameofsymbol() + $2->getnameofsymbol() + $3->getnameofsymbol();
        logRule("term", "term MULOP unary_expression", text);
        $$ = new symbol_info(text, "term");
    }
    ;

unary_expression
    : ADDOP unary_expression
    {
        string text = $1->getnameofsymbol() + $2->getnameofsymbol();
        logRule("unary_expression", "ADDOP unary_expression", text);
        $$ = new symbol_info(text, "unary_expression");
    }
    | NOT unary_expression
    {
        string text = "!" + $2->getnameofsymbol();
        logRule("unary_expression", "NOT unary_expression", text);
        $$ = new symbol_info(text, "unary_expression");
    }
    | factor
    {
        logRule("unary_expression", "factor", $1->getnameofsymbol());
        $$ = new symbol_info($1->getnameofsymbol(), "unary_expression");
    }
    ;

factor
    : variable
    {
        logRule("factor", "variable", $1->getnameofsymbol());
        logFactorInfo($1->getnameofsymbol());
        $$ = new symbol_info($1->getnameofsymbol(), "factor");
    }
    | ID LPAREN argument_list RPAREN
    {
        string text = $1->getnameofsymbol() + "(" + $3->getnameofsymbol() + ")";
        logRule("factor", "ID LPAREN argument_list RPAREN", text);
        logFactorInfo(text);
        $$ = new symbol_info(text, "factor");
    }
    | LPAREN expression RPAREN
    {
        string text = "(" + $2->getnameofsymbol() + ")";
        logRule("factor", "LPAREN expression RPAREN", text);
        logFactorInfo(text);
        $$ = new symbol_info(text, "factor");
    }
    | CONST_INT
    {
        logRule("factor", "CONST_INT", $1->getnameofsymbol());
        logFactorInfo($1->getnameofsymbol());
        $$ = new symbol_info($1->getnameofsymbol(), "factor");
    }
    | CONST_FLOAT
    {
        logRule("factor", "CONST_FLOAT", $1->getnameofsymbol());
        logFactorInfo($1->getnameofsymbol());
        $$ = new symbol_info($1->getnameofsymbol(), "factor");
    }
    | variable INCOP
    {
        string text = $1->getnameofsymbol() + "++";
        logRule("factor", "variable INCOP", text);
        logFactorInfo(text);
        $$ = new symbol_info(text, "factor");
    }
    | variable DECOP
    {
        string text = $1->getnameofsymbol() + "--";
        logRule("factor", "variable DECOP", text);
        logFactorInfo(text);
        $$ = new symbol_info(text, "factor");
    }
    ;

variable
    : ID
    {
        logRule("variable", "ID", $1->getnameofsymbol());
        $$ = new symbol_info($1->getnameofsymbol(), "variable");
    }
    | ID LTHIRD expression RTHIRD
    {
        string text = $1->getnameofsymbol() + "[" + $3->getnameofsymbol() + "]";
        logRule("variable", "ID LTHIRD expression RTHIRD", text);
        $$ = new symbol_info(text, "variable");
    }
    ;

argument_list
    : arguments
    {
        logRule("argument_list", "arguments", $1->getnameofsymbol());
        $$ = new symbol_info($1->getnameofsymbol(), "argument_list");
    }
    |
    {
        logRule("argument_list", "", "");
        $$ = new symbol_info("", "argument_list");
    }
    ;

arguments
    : arguments COMMA logic_expression
    {
        string text = $1->getnameofsymbol() + "," + $3->getnameofsymbol();
        logRule("arguments", "arguments COMMA logic_expression", text);
        $$ = new symbol_info(text, "arguments");
    }
    | logic_expression
    {
        logRule("arguments", "logic_expression", $1->getnameofsymbol());
        $$ = new symbol_info($1->getnameofsymbol(), "arguments");
    }
    ;

%%

extern FILE *yyin;
int yyparse();

int main(int c, char *v[])
{
    if(c != 2)
    {
        printf("Please provide input file name\n");
        return 1;
    }

    yyin = fopen(v[1], "r");
    outlog.open("24241370_log.txt");

    if(yyin == NULL)
    {
        printf("Cannot open input file: %s\n", v[1]);
        return 1;
    }

    int result = yyparse();

    if (result == 0) {

          printf("Parsing completed successfully\n");
          printf("Valid input\n");

    } else {
        printf("Parsing failed\n");
    }
    outlog << "Total lines: " << line_num << endl;
    outlog.close();
    fclose(yyin);
    return 0;
}