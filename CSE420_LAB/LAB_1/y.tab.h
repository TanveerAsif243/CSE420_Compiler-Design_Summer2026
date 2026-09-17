/* A Bison parser, made by GNU Bison 2.7.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2012 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     IF = 258,
     FOR = 259,
     WHILE = 260,
     DO = 261,
     BREAK = 262,
     CONTINUE = 263,
     RETURN = 264,
     INT = 265,
     FLOAT = 266,
     DOUBLE = 267,
     CHAR = 268,
     VOID = 269,
     SWITCH = 270,
     CASE = 271,
     DEFAULT = 272,
     GOTO = 273,
     PRINTLN = 274,
     ID = 275,
     CONST_INT = 276,
     CONST_FLOAT = 277,
     ADDOP = 278,
     MULOP = 279,
     INCOP = 280,
     DECOP = 281,
     RELOP = 282,
     ASSIGNOP = 283,
     LOGICOP = 284,
     NOT = 285,
     LPAREN = 286,
     RPAREN = 287,
     LCURL = 288,
     RCURL = 289,
     LTHIRD = 290,
     RTHIRD = 291,
     COMMA = 292,
     COLON = 293,
     SEMICOLON = 294,
     LOWER_THAN_ELSE = 295,
     ELSE = 296
   };
#endif
/* Tokens.  */
#define IF 258
#define FOR 259
#define WHILE 260
#define DO 261
#define BREAK 262
#define CONTINUE 263
#define RETURN 264
#define INT 265
#define FLOAT 266
#define DOUBLE 267
#define CHAR 268
#define VOID 269
#define SWITCH 270
#define CASE 271
#define DEFAULT 272
#define GOTO 273
#define PRINTLN 274
#define ID 275
#define CONST_INT 276
#define CONST_FLOAT 277
#define ADDOP 278
#define MULOP 279
#define INCOP 280
#define DECOP 281
#define RELOP 282
#define ASSIGNOP 283
#define LOGICOP 284
#define NOT 285
#define LPAREN 286
#define RPAREN 287
#define LCURL 288
#define RCURL 289
#define LTHIRD 290
#define RTHIRD 291
#define COMMA 292
#define COLON 293
#define SEMICOLON 294
#define LOWER_THAN_ELSE 295
#define ELSE 296



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
