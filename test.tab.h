/* A Bison parser, made by GNU Bison 3.7.5.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_TEST_TAB_H_INCLUDED
# define YY_YY_TEST_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    INTEGER = 258,                 /* INTEGER  */
    FLOATING = 259,                /* FLOATING  */
    STRING = 260,                  /* STRING  */
    EMPTY = 261,                   /* EMPTY  */
    CHAR = 262,                    /* CHAR  */
    ID = 263,                      /* ID  */
    INT = 264,                     /* INT  */
    FLOAT = 265,                   /* FLOAT  */
    ELEM = 266,                    /* ELEM  */
    SET = 267,                     /* SET  */
    IF = 268,                      /* IF  */
    ELSE = 269,                    /* ELSE  */
    FOR = 270,                     /* FOR  */
    READ = 271,                    /* READ  */
    WRITE = 272,                   /* WRITE  */
    WRITELN = 273,                 /* WRITELN  */
    IN = 274,                      /* IN  */
    IS_SET = 275,                  /* IS_SET  */
    ADD = 276,                     /* ADD  */
    REMOVE = 277,                  /* REMOVE  */
    EXISTS = 278,                  /* EXISTS  */
    FORALL = 279,                  /* FORALL  */
    RETURN = 280,                  /* RETURN  */
    OPENPAR = 281,                 /* OPENPAR  */
    CLOSEPAR = 282,                /* CLOSEPAR  */
    OPENCURLY = 283,               /* OPENCURLY  */
    CLOSECURLY = 284,              /* CLOSECURLY  */
    OPENBRAC = 285,                /* OPENBRAC  */
    CLOSEBRAC = 286,               /* CLOSEBRAC  */
    LESS = 287,                    /* LESS  */
    LE = 288,                      /* LE  */
    EQ = 289,                      /* EQ  */
    NEQ = 290,                     /* NEQ  */
    GREATER = 291,                 /* GREATER  */
    GE = 292,                      /* GE  */
    PLUS = 293,                    /* PLUS  */
    MINUS = 294,                   /* MINUS  */
    DBS = 295,                     /* DBS  */
    AST = 296,                     /* AST  */
    BS = 297,                      /* BS  */
    ASSIGN = 298,                  /* ASSIGN  */
    COMMA = 299,                   /* COMMA  */
    SEMICOLON = 300,               /* SEMICOLON  */
    OR = 301,                      /* OR  */
    AND = 302,                     /* AND  */
    NOT = 303,                     /* NOT  */
    AMP = 304,                     /* AMP  */
    PCENT = 305,                   /* PCENT  */
    VOID = 306                     /* VOID  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 419 "test.y"

	double val;	
	char *text;
	//bool boolean;
	struct no *node;

#line 122 "test.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_TEST_TAB_H_INCLUDED  */
