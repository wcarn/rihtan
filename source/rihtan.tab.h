/* A Bison parser, made by GNU Bison 2.4.2.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2006, 2009-2010 Free Software
   Foundation, Inc.
   
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

/* "%code requires" blocks.  */

/* Line 1685 of yacc.c  */
#line 19 "rihtan.y"


char *lexsourcefilename; /* current filename here for the lexer */

typedef struct YYLTYPE {
  int first_line;
  int first_column;
  int last_line;
  int last_column;
  char *filename;
} YYLTYPE;
# define YYLTYPE_IS_DECLARED 1 /* alert the parser that we have our own definition */

# define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                 \
      if (N)                                                           \
        {                                                              \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;       \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;     \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;        \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;      \
          (Current).filename     = YYRHSLOC (Rhs, 1).filename;         \
        }                                                              \
      else                                                             \
        { /* empty RHS */                                              \
          (Current).first_line   = (Current).last_line   =             \
            YYRHSLOC (Rhs, 0).last_line;                               \
          (Current).first_column = (Current).last_column =             \
            YYRHSLOC (Rhs, 0).last_column;                             \
          (Current).filename  = NULL;                        /* new */ \
        }                                                              \
    while (0)



/* Line 1685 of yacc.c  */
#line 75 "rihtan.tab.h"

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     INTEGER = 258,
     FLOATVAL = 259,
     IDENTIFIER = 260,
     CHARACTER = 261,
     ASSIGN = 262,
     TYPE = 263,
     IS = 264,
     RANGE = 265,
     DOTDOT = 266,
     WHEN = 267,
     DO = 268,
     IF = 269,
     THEN = 270,
     END = 271,
     BEGIN_SYM = 272,
     ELSIF = 273,
     ELSE = 274,
     LOOP = 275,
     EXIT = 276,
     REPEAT = 277,
     WHILE = 278,
     TRUE_SYM = 279,
     FALSE_SYM = 280,
     NULL_SYM = 281,
     CASE_SYM = 282,
     FOR_SYM = 283,
     IN_SYM = 284,
     OUT_SYM = 285,
     UNIT_SYM = 286,
     ARRAY_SYM = 287,
     OF_SYM = 288,
     ARROW = 289,
     RECORD_SYM = 290,
     UNION_SYM = 291,
     UNCHECKED_SYM = 292,
     DIGITS = 293,
     MAGNITUDE = 294,
     PROCEDURE = 295,
     FUNCTION = 296,
     PACKAGE = 297,
     SUBSYSTEM = 298,
     SYSTEM = 299,
     PUBLIC_SYM = 300,
     CONTROLLED = 301,
     MAIN = 302,
     RESTART = 303,
     FINAL = 304,
     PRAGMA = 305,
     CLOSED = 306,
     GENERIC = 307,
     NEW = 308,
     CONSTANT = 309,
     PERSISTENT = 310,
     MANAGED_SYM = 311,
     DECLARE = 312,
     RENAMES = 313,
     SEPARATE = 314,
     USING = 315,
     ACCESS_SYM = 316,
     SHARED_SYM = 317,
     WITH_SYM = 318,
     IAND = 319,
     IOR = 320,
     IXOR = 321,
     OPENLT = 322,
     OPENGT = 323,
     FREE_SYM = 324,
     UNIT_TEST = 325,
     USE_SYM = 326,
     ASSERT = 327,
     ADVISE = 328,
     COMMENT = 329,
     CODE = 330,
     HEADER_CODE = 331,
     STRING = 332,
     SCAN_ERROR = 333,
     NOT = 334,
     OR = 335,
     AND = 336,
     NEQ = 337,
     EQ = 338,
     GEQ = 339,
     LEQ = 340,
     GT = 341,
     LT = 342,
     MOD = 343,
     UMINUS = 344
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1685 of yacc.c  */
#line 65 "rihtan.y"

  BIGINT integer;
  BIGFLOAT floatval;
  char *string;
  struct tNode *node;
  struct tElt *elt;
  struct tRange *range;
  struct tCaseOption *caseoption;
  struct tUnitTerm *units;
  struct tTypeSpec *typespec;
  struct tReference *reference;
  struct tEnum *enumtype;
  struct tArrayInitialisation *arrayinit;
  struct tArrayInitialisationEntry *arrayinitentry;
  struct tFieldSpec *fieldspec;
  struct tFieldInitialisation *recordinit;
  struct tRHS *rhs;
  struct tProcedure *procedure;
  enum tMode varmode;
  struct tActualParameter *actual;
  struct tGenericParameter *genericparam;
  struct tGenericActualParameter *genericactualparam;
  struct tRepresentationClause *representationclause;
  struct tShareClause *shareclause;
  struct tArrayIndex *arrayindex;
  struct tTypeIdentifier *typeidentifier;
  struct tTypeRefOrSpec *typereforspec;
  struct tAttributeChainEntry *attributechain;
  struct tSingleAttribute *singleattribute;
  struct tIdentifierListEntry *identifierlistentry;
  struct tFileCommentBlock *filecommentblock;



/* Line 1685 of yacc.c  */
#line 216 "rihtan.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif

extern YYLTYPE yylloc;

