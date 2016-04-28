/* A Bison parser, made by GNU Bison 2.4.2.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 1



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 53 "rihtan.y"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ytypes.h"

extern int yylineno;
struct tElt *LoadExternalPackage(char *packagename);
bool OuterLevel = TRUE;


/* Line 189 of yacc.c  */
#line 84 "rihtan.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

/* "%code requires" blocks.  */

/* Line 209 of yacc.c  */
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



/* Line 209 of yacc.c  */
#line 144 "rihtan.tab.c"

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

/* Line 214 of yacc.c  */
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



/* Line 214 of yacc.c  */
#line 285 "rihtan.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

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


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 310 "rihtan.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
	     && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  5
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2309

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  105
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  107
/* YYNRULES -- Number of rules.  */
#define YYNRULES  348
/* YYNRULES -- Number of states.  */
#define YYNSTATES  825

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   344

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    99,
      95,    96,    90,    88,    98,    89,   100,    91,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    97,    94,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   102,     2,   103,   101,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   104,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    92,    93
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     7,    12,    17,    23,    24,    27,    36,
      45,    58,    71,    80,    93,   102,   111,   116,   121,   129,
     137,   141,   147,   156,   162,   171,   172,   175,   176,   179,
     192,   200,   214,   223,   224,   227,   228,   231,   233,   235,
     236,   240,   242,   246,   252,   258,   260,   262,   265,   269,
     270,   273,   274,   277,   279,   282,   287,   293,   295,   298,
     300,   309,   318,   323,   325,   327,   329,   332,   335,   339,
     342,   345,   349,   353,   357,   361,   366,   369,   373,   377,
     381,   386,   389,   393,   397,   402,   405,   408,   411,   414,
     417,   419,   421,   424,   425,   428,   431,   434,   437,   440,
     443,   446,   449,   452,   455,   457,   460,   463,   466,   469,
     472,   474,   477,   480,   483,   486,   491,   500,   506,   513,
     519,   525,   531,   537,   543,   549,   555,   561,   568,   576,
     585,   592,   600,   609,   619,   621,   625,   627,   631,   634,
     639,   641,   644,   646,   649,   652,   653,   656,   657,   662,
     664,   667,   669,   671,   673,   675,   677,   679,   682,   684,
     687,   693,   697,   703,   709,   710,   713,   716,   718,   722,
     724,   728,   732,   738,   742,   744,   748,   751,   753,   755,
     757,   761,   763,   765,   772,   779,   786,   793,   798,   803,
     809,   811,   814,   820,   825,   827,   829,   835,   839,   844,
     847,   853,   857,   864,   869,   875,   879,   886,   891,   895,
     900,   904,   908,   913,   917,   919,   921,   923,   925,   928,
     930,   933,   937,   943,   948,   954,   957,   965,   966,   972,
     975,   980,   985,   992,   994,   997,  1003,  1005,  1008,  1014,
    1020,  1025,  1027,  1034,  1035,  1038,  1043,  1045,  1049,  1051,
    1062,  1069,  1073,  1077,  1081,  1085,  1089,  1092,  1096,  1100,
    1104,  1108,  1112,  1116,  1120,  1124,  1128,  1132,  1136,  1139,
    1141,  1143,  1147,  1149,  1153,  1155,  1157,  1159,  1161,  1163,
    1165,  1167,  1171,  1173,  1178,  1180,  1182,  1184,  1191,  1195,
    1199,  1201,  1205,  1209,  1211,  1215,  1217,  1221,  1224,  1226,
    1230,  1235,  1239,  1242,  1244,  1246,  1250,  1255,  1257,  1261,
    1264,  1268,  1273,  1275,  1278,  1281,  1284,  1287,  1290,  1294,
    1299,  1304,  1309,  1314,  1317,  1322,  1327,  1332,  1337,  1340,
    1344,  1350,  1358,  1359,  1363,  1365,  1369,  1373,  1381,  1388,
    1393,  1397,  1403,  1410,  1413,  1416,  1418,  1422,  1426
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     106,     0,    -1,   107,   108,    94,    -1,   107,    62,   108,
      94,    -1,   107,    70,   108,    94,    -1,   107,    79,    70,
     108,    94,    -1,    -1,    74,   107,    -1,    44,     5,     9,
     121,   109,   110,    16,     5,    -1,    43,     5,     9,   121,
     109,   110,    16,     5,    -1,    44,     5,    60,    95,   210,
      96,     9,   121,   109,   110,    16,     5,    -1,    43,     5,
      60,    95,   210,    96,     9,   121,   109,   110,    16,     5,
      -1,    42,     5,     9,   121,   109,   110,    16,     5,    -1,
      42,     5,    60,    95,   210,    96,     9,   121,   109,   110,
      16,     5,    -1,    42,     5,     9,    53,   135,    95,   202,
      96,    -1,    43,     5,     9,    53,   135,    95,   202,    96,
      -1,    42,     5,     9,    59,    -1,    43,     5,     9,    59,
      -1,    42,     5,     9,    59,    95,    77,    96,    -1,    43,
       5,     9,    59,    95,    77,    96,    -1,    52,   199,   108,
      -1,    52,    42,     5,     9,    59,    -1,    52,    42,     5,
       9,    59,    95,    77,    96,    -1,    52,    43,     5,     9,
      59,    -1,    52,    43,     5,     9,    59,    95,    77,    96,
      -1,    -1,    17,   127,    -1,    -1,    49,   127,    -1,    40,
       5,   116,   139,   113,     9,   120,    17,   127,   114,    16,
       5,    -1,    40,     5,   116,   139,   113,     9,    59,    -1,
      41,     5,   116,    97,   115,   139,     9,   120,    17,   127,
     114,    16,     5,    -1,    41,     5,   116,    97,   115,   139,
       9,    59,    -1,    -1,    12,   180,    -1,    -1,    70,   127,
      -1,   133,    -1,   141,    -1,    -1,    95,   117,    96,    -1,
     118,    -1,   118,    94,   117,    -1,   132,    97,   119,   133,
     139,    -1,   132,    97,   119,   141,   139,    -1,    29,    -1,
      30,    -1,    29,    30,    -1,    49,    29,    30,    -1,    -1,
     120,   126,    -1,    -1,   121,   122,    -1,   125,    -1,    45,
     125,    -1,    45,    30,   131,    94,    -1,    45,    30,    62,
     131,    94,    -1,   123,    -1,   196,    94,    -1,   124,    -1,
      57,    59,     5,    17,   127,    16,     5,    94,    -1,    57,
      59,     5,    57,   121,    16,     5,    94,    -1,    71,    59,
     135,    94,    -1,    75,    -1,    76,    -1,    74,    -1,   130,
      94,    -1,   131,    94,    -1,    62,   131,    94,    -1,   134,
      94,    -1,   111,    94,    -1,    51,   111,    94,    -1,    62,
     111,    94,    -1,    47,   111,    94,    -1,    70,   111,    94,
      -1,    79,    70,   111,    94,    -1,   112,    94,    -1,    51,
     112,    94,    -1,    62,   112,    94,    -1,    70,   112,    94,
      -1,    79,    70,   112,    94,    -1,   108,    94,    -1,    62,
     108,    94,    -1,    70,   108,    94,    -1,    79,    70,   108,
      94,    -1,   204,    94,    -1,   130,    94,    -1,   131,    94,
      -1,   204,    94,    -1,   134,    94,    -1,   123,    -1,   124,
      -1,   129,   128,    -1,    -1,   128,   129,    -1,   161,    94,
      -1,   166,    94,    -1,   168,    94,    -1,   169,    94,    -1,
     170,    94,    -1,   174,    94,    -1,   179,    94,    -1,   173,
      94,    -1,   191,    94,    -1,   123,    -1,   196,    94,    -1,
     205,    94,    -1,   207,    94,    -1,   208,    94,    -1,   209,
      94,    -1,   124,    -1,   206,    94,    -1,   197,    94,    -1,
     198,    94,    -1,     8,     5,    -1,     8,     5,     9,   140,
      -1,     8,     5,     9,    46,    60,    95,   210,    96,    -1,
     132,    97,   133,   138,   139,    -1,   132,    97,   133,    58,
     163,   139,    -1,   132,    97,    42,    58,   135,    -1,   132,
      97,    43,    58,   135,    -1,   132,    97,    40,    58,   135,
      -1,   132,    97,    41,    58,   135,    -1,   132,    97,     8,
      58,   133,    -1,   132,    97,    31,    58,   135,    -1,   132,
      97,    54,    58,   163,    -1,   132,    97,   141,   138,   139,
      -1,   132,    97,    46,   141,   138,   139,    -1,   132,    97,
      95,   142,    96,   138,   139,    -1,   132,    97,    95,    46,
     142,    96,   138,   139,    -1,   132,    97,    45,   141,   138,
     139,    -1,   132,    97,    45,    46,   141,   138,   139,    -1,
     132,    97,    45,    95,   142,    96,   138,   139,    -1,   132,
      97,    45,    95,    46,   142,    96,   138,   139,    -1,     5,
      -1,     5,    98,   132,    -1,   135,    -1,   135,    99,   181,
      -1,    31,     5,    -1,    31,     5,     9,   147,    -1,     5,
      -1,     5,   136,    -1,   137,    -1,   137,   136,    -1,   100,
       5,    -1,    -1,     7,   162,    -1,    -1,    60,    95,   210,
      96,    -1,   143,    -1,    45,   143,    -1,   144,    -1,   149,
      -1,   145,    -1,   155,    -1,   156,    -1,   159,    -1,   141,
     139,    -1,   142,    -1,    46,   142,    -1,    10,   180,    11,
     180,   146,    -1,    10,    28,   180,    -1,    38,     3,    39,
       3,   146,    -1,    39,     3,    38,     3,   146,    -1,    -1,
      31,   147,    -1,    53,    31,    -1,   148,    -1,   148,    90,
     147,    -1,   135,    -1,   135,   101,     3,    -1,   135,    99,
      31,    -1,   135,    99,    31,   101,     3,    -1,    95,   150,
      96,    -1,   151,    -1,   151,    98,   150,    -1,     5,   139,
      -1,   133,    -1,   152,    -1,   141,    -1,    95,   142,    96,
      -1,   133,    -1,   144,    -1,    32,   102,   154,   103,    33,
     153,    -1,    32,    67,   154,    68,    33,   153,    -1,    32,
     102,   154,    68,    33,   153,    -1,    32,    67,   154,   103,
      33,   153,    -1,    35,   157,    16,    35,    -1,    36,   157,
      16,    36,    -1,    37,    36,   157,    16,    36,    -1,   158,
      -1,   158,   157,    -1,     5,    97,   153,   139,    94,    -1,
       5,    97,    26,    94,    -1,    75,    -1,    74,    -1,    56,
      61,    80,    26,   153,    -1,    56,    61,   153,    -1,    61,
      80,    26,   153,    -1,    61,   153,    -1,    61,    80,    26,
      53,   153,    -1,    61,    53,   153,    -1,    56,    61,    80,
      26,    54,   153,    -1,    56,    61,    54,   153,    -1,    61,
      80,    26,    54,   153,    -1,    61,    54,   153,    -1,    61,
      80,    26,    54,    53,   153,    -1,    61,    54,    53,   153,
      -1,    55,    61,   153,    -1,    55,    61,    54,   153,    -1,
       5,    97,   133,    -1,     5,    97,   144,    -1,   163,     7,
      53,   162,    -1,   163,     7,   162,    -1,   180,    -1,   183,
      -1,   188,    -1,     5,    -1,     5,   164,    -1,   165,    -1,
     165,   164,    -1,   102,   180,   103,    -1,   102,   180,    11,
     180,   103,    -1,   102,   180,    11,   103,    -1,   102,   180,
      28,   180,   103,    -1,   100,     5,    -1,    14,   180,    15,
     127,   167,    16,    14,    -1,    -1,    18,   180,    15,   127,
     167,    -1,    19,   127,    -1,    20,   172,    16,    20,    -1,
      13,   171,    16,    13,    -1,    23,   180,    20,   127,    16,
      20,    -1,   129,    -1,   129,   171,    -1,    21,    12,   180,
      94,   171,    -1,   129,    -1,   129,   172,    -1,    21,    12,
     180,    94,   172,    -1,    22,    12,   180,    94,   172,    -1,
      21,    12,   180,    94,    -1,    26,    -1,    27,   180,     9,
     175,    16,    27,    -1,    -1,   175,   176,    -1,    12,   177,
      34,   127,    -1,   178,    -1,   178,   104,   177,    -1,   180,
      -1,    28,   160,    29,   180,    11,   180,    20,   127,    16,
      20,    -1,    28,   160,    20,   127,    16,    20,    -1,   180,
      88,   180,    -1,   180,    89,   180,    -1,   180,    90,   180,
      -1,   180,    91,   180,    -1,   180,    92,   180,    -1,    89,
     180,    -1,   180,    64,   180,    -1,   180,    65,   180,    -1,
     180,    66,   180,    -1,   180,    87,   180,    -1,   180,    86,
     180,    -1,   180,    85,   180,    -1,   180,    84,   180,    -1,
     180,    83,   180,    -1,   180,    82,   180,    -1,   180,    81,
     180,    -1,   180,    80,   180,    -1,    79,   180,    -1,    24,
      -1,    25,    -1,    95,   180,    96,    -1,   163,    -1,   163,
      99,   181,    -1,     3,    -1,     4,    -1,    77,    -1,     6,
      -1,    26,    -1,   192,    -1,   182,    -1,   182,    99,   181,
      -1,     5,    -1,     5,    95,     5,    96,    -1,    31,    -1,
       8,    -1,    61,    -1,   102,    10,     5,    34,   162,   103,
      -1,   102,   184,   103,    -1,   102,   186,   103,    -1,   185,
      -1,   185,    98,   184,    -1,   177,    34,   162,    -1,   187,
      -1,   187,    98,   186,    -1,   162,    -1,    95,   189,    96,
      -1,    95,    96,    -1,   190,    -1,   190,    98,   189,    -1,
       5,    34,    53,   162,    -1,     5,    34,   162,    -1,     5,
      34,    -1,   163,    -1,   192,    -1,   163,    95,    96,    -1,
     163,    95,   193,    96,    -1,   194,    -1,   194,    98,   193,
      -1,   195,   139,    -1,     5,    34,   180,    -1,     5,    34,
     119,   180,    -1,   180,    -1,   119,   180,    -1,    50,   191,
      -1,    72,   193,    -1,    73,   193,    -1,   200,    94,    -1,
     200,    94,   199,    -1,     8,     5,     9,    10,    -1,     8,
       5,     9,    38,    -1,     8,     5,     9,    32,    -1,     8,
       5,     9,    61,    -1,     8,     5,    -1,    54,     5,     9,
      10,    -1,    54,     5,     9,    38,    -1,    54,     5,     9,
      61,    -1,    54,     5,     9,     5,    -1,    54,     5,    -1,
      40,     5,   116,    -1,    41,     5,   116,    97,   115,    -1,
      63,    42,     5,     9,    53,   135,   201,    -1,    -1,    95,
     202,    96,    -1,   203,    -1,   203,    98,   202,    -1,     5,
      34,   180,    -1,   132,    97,    54,     7,   180,   146,   139,
      -1,    57,   120,    17,   139,   127,    16,    -1,    17,   139,
     127,    16,    -1,    71,    59,   135,    -1,    17,    12,    70,
     127,    16,    -1,    17,    12,    79,    70,   127,    16,    -1,
      69,   163,    -1,    48,    43,    -1,   211,    -1,   211,    98,
     210,    -1,     5,    34,   180,    -1,     5,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   225,   225,   230,   236,   242,   251,   254,   260,   266,
     272,   278,   284,   291,   299,   305,   311,   316,   321,   326,
     331,   336,   341,   346,   351,   359,   362,   369,   372,   378,
     383,   391,   397,   406,   415,   422,   425,   432,   438,   447,
     450,   456,   460,   466,   471,   478,   482,   486,   490,   497,
     500,   507,   510,   516,   520,   524,   528,   532,   536,   540,
     544,   549,   554,   561,   565,   571,   578,   582,   586,   590,
     594,   598,   602,   606,   610,   614,   618,   622,   626,   630,
     634,   638,   642,   646,   650,   654,   660,   664,   668,   672,
     676,   680,   687,   694,   697,   703,   704,   705,   706,   707,
     708,   709,   710,   711,   712,   713,   714,   715,   716,   717,
     718,   719,   720,   721,   724,   729,   734,   740,   745,   750,
     755,   760,   765,   770,   775,   780,   786,   792,   798,   803,
     810,   817,   825,   832,   841,   845,   851,   855,   861,   866,
     873,   877,   884,   888,   894,   901,   904,   911,   914,   920,
     924,   930,   934,   938,   942,   946,   950,   956,   962,   966,
     973,   977,   994,   998,  1005,  1008,  1012,  1018,  1022,  1028,
    1032,  1036,  1040,  1046,  1052,  1056,  1062,  1068,  1074,  1078,
    1082,  1101,  1105,  1111,  1115,  1119,  1123,  1129,  1133,  1137,
    1143,  1147,  1153,  1157,  1161,  1165,  1171,  1175,  1179,  1183,
    1187,  1191,  1195,  1199,  1203,  1207,  1211,  1215,  1219,  1223,
    1230,  1236,  1244,  1248,  1254,  1258,  1262,  1268,  1272,  1279,
    1283,  1289,  1293,  1297,  1301,  1305,  1311,  1318,  1321,  1325,
    1331,  1337,  1343,  1354,  1358,  1362,  1374,  1378,  1382,  1387,
    1392,  1399,  1405,  1412,  1415,  1421,  1427,  1431,  1437,  1449,
    1453,  1471,  1475,  1479,  1483,  1487,  1491,  1498,  1502,  1506,
    1510,  1514,  1518,  1522,  1526,  1530,  1534,  1538,  1542,  1546,
    1550,  1554,  1558,  1562,  1576,  1580,  1584,  1588,  1592,  1596,
    1602,  1606,  1612,  1616,  1620,  1624,  1628,  1634,  1638,  1642,
    1648,  1652,  1658,  1664,  1668,  1674,  1680,  1684,  1690,  1694,
    1700,  1704,  1708,  1714,  1719,  1726,  1731,  1738,  1742,  1748,
    1754,  1758,  1762,  1766,  1772,  1778,  1784,  1790,  1794,  1800,
    1805,  1810,  1815,  1820,  1825,  1830,  1835,  1840,  1845,  1850,
    1855,  1860,  1868,  1871,  1877,  1881,  1887,  1893,  1900,  1905,
    1912,  1919,  1924,  1931,  1937,  1943,  1947,  1953,  1957
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "INTEGER", "FLOATVAL", "IDENTIFIER",
  "CHARACTER", "ASSIGN", "TYPE", "IS", "RANGE", "DOTDOT", "WHEN", "DO",
  "IF", "THEN", "END", "BEGIN_SYM", "ELSIF", "ELSE", "LOOP", "EXIT",
  "REPEAT", "WHILE", "TRUE_SYM", "FALSE_SYM", "NULL_SYM", "CASE_SYM",
  "FOR_SYM", "IN_SYM", "OUT_SYM", "UNIT_SYM", "ARRAY_SYM", "OF_SYM",
  "ARROW", "RECORD_SYM", "UNION_SYM", "UNCHECKED_SYM", "DIGITS",
  "MAGNITUDE", "PROCEDURE", "FUNCTION", "PACKAGE", "SUBSYSTEM", "SYSTEM",
  "PUBLIC_SYM", "CONTROLLED", "MAIN", "RESTART", "FINAL", "PRAGMA",
  "CLOSED", "GENERIC", "NEW", "CONSTANT", "PERSISTENT", "MANAGED_SYM",
  "DECLARE", "RENAMES", "SEPARATE", "USING", "ACCESS_SYM", "SHARED_SYM",
  "WITH_SYM", "IAND", "IOR", "IXOR", "OPENLT", "OPENGT", "FREE_SYM",
  "UNIT_TEST", "USE_SYM", "ASSERT", "ADVISE", "COMMENT", "CODE",
  "HEADER_CODE", "STRING", "SCAN_ERROR", "NOT", "OR", "AND", "NEQ", "EQ",
  "GEQ", "LEQ", "GT", "LT", "'+'", "'-'", "'*'", "'/'", "MOD", "UMINUS",
  "';'", "'('", "')'", "':'", "','", "'\\''", "'.'", "'^'", "'['", "']'",
  "'|'", "$accept", "program", "initial_comment_block",
  "package_declaration", "package_initialisation", "package_finalisation",
  "procedure_declaration", "function_declaration", "share_clause",
  "optional_unit_test", "function_return_type", "formal_parameters",
  "formal_parameter_list", "formal_parameter", "mode_spec",
  "declaration_procedure_list", "declaration_package_list",
  "declaration_package", "code_line", "comment_line", "declaration_global",
  "declaration_procedure", "stmt_list", "nstmt_list", "stmt",
  "type_declaration", "var_declaration", "identifier_list",
  "type_identifier", "unit_declaration", "package_identifier",
  "package_ref_list", "package_ref_term", "optional_value",
  "optional_representation", "package_type_spec", "basic_type_spec",
  "extended_type_spec", "type_spec", "integer_type_spec",
  "floating_type_spec", "unit_spec", "unit_list", "unit",
  "enumeration_type_spec", "enum_symbol_list", "enum_symbol", "type_ref",
  "type_ref_or_spec", "array_index", "array_type_spec", "record_type_spec",
  "field_list", "field_spec", "access_type_spec",
  "integer_var_declaration", "assignment", "rhs", "variable_ref",
  "deref_list", "deref_term", "if_block", "else_part", "loop_stmt",
  "do_stmt", "while_stmt", "exit_list", "exit_repeat_list", "null_stmt",
  "case_stmt", "case_option_list", "case_option", "case_range_list",
  "case_range", "for_stmt", "expr1", "attribute_chain", "single_attribute",
  "array_initialisation", "array_initialisation_list",
  "array_initialisation_entry", "array_seq_initialisation_list",
  "array_seq_initialisation_entry", "record_initialisation",
  "field_initialisation_list", "field_initialisation", "procedure_call",
  "call_with_parameter_list", "actual_parameter_list", "actual_parameter",
  "base_actual_parameter", "pragma", "assert_stmt", "advise_stmt",
  "generic_parameter_list", "generic_parameter",
  "optional_generic_parameters", "generic_actual_parameter_list",
  "generic_actual_parameter", "constant_declaration", "declare_block",
  "separate_stmt", "unit_test_block", "free_stmt", "restart_stmt",
  "representation_list", "representation_clause", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,    43,    45,
      42,    47,   343,   344,    59,    40,    41,    58,    44,    39,
      46,    94,    91,    93,   124
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   105,   106,   106,   106,   106,   107,   107,   108,   108,
     108,   108,   108,   108,   108,   108,   108,   108,   108,   108,
     108,   108,   108,   108,   108,   109,   109,   110,   110,   111,
     111,   112,   112,   113,   113,   114,   114,   115,   115,   116,
     116,   117,   117,   118,   118,   119,   119,   119,   119,   120,
     120,   121,   121,   122,   122,   122,   122,   122,   122,   122,
     122,   122,   122,   123,   123,   124,   125,   125,   125,   125,
     125,   125,   125,   125,   125,   125,   125,   125,   125,   125,
     125,   125,   125,   125,   125,   125,   126,   126,   126,   126,
     126,   126,   127,   128,   128,   129,   129,   129,   129,   129,
     129,   129,   129,   129,   129,   129,   129,   129,   129,   129,
     129,   129,   129,   129,   130,   130,   130,   131,   131,   131,
     131,   131,   131,   131,   131,   131,   131,   131,   131,   131,
     131,   131,   131,   131,   132,   132,   133,   133,   134,   134,
     135,   135,   136,   136,   137,   138,   138,   139,   139,   140,
     140,   141,   141,   141,   141,   141,   141,   142,   143,   143,
     144,   144,   145,   145,   146,   146,   146,   147,   147,   148,
     148,   148,   148,   149,   150,   150,   151,   152,   153,   153,
     153,   154,   154,   155,   155,   155,   155,   156,   156,   156,
     157,   157,   158,   158,   158,   158,   159,   159,   159,   159,
     159,   159,   159,   159,   159,   159,   159,   159,   159,   159,
     160,   160,   161,   161,   162,   162,   162,   163,   163,   164,
     164,   165,   165,   165,   165,   165,   166,   167,   167,   167,
     168,   169,   170,   171,   171,   171,   172,   172,   172,   172,
     172,   173,   174,   175,   175,   176,   177,   177,   178,   179,
     179,   180,   180,   180,   180,   180,   180,   180,   180,   180,
     180,   180,   180,   180,   180,   180,   180,   180,   180,   180,
     180,   180,   180,   180,   180,   180,   180,   180,   180,   180,
     181,   181,   182,   182,   182,   182,   182,   183,   183,   183,
     184,   184,   185,   186,   186,   187,   188,   188,   189,   189,
     190,   190,   190,   191,   191,   192,   192,   193,   193,   194,
     195,   195,   195,   195,   196,   197,   198,   199,   199,   200,
     200,   200,   200,   200,   200,   200,   200,   200,   200,   200,
     200,   200,   201,   201,   202,   202,   203,   204,   205,   205,
     206,   207,   207,   208,   209,   210,   210,   211,   211
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     3,     4,     4,     5,     0,     2,     8,     8,
      12,    12,     8,    12,     8,     8,     4,     4,     7,     7,
       3,     5,     8,     5,     8,     0,     2,     0,     2,    12,
       7,    13,     8,     0,     2,     0,     2,     1,     1,     0,
       3,     1,     3,     5,     5,     1,     1,     2,     3,     0,
       2,     0,     2,     1,     2,     4,     5,     1,     2,     1,
       8,     8,     4,     1,     1,     1,     2,     2,     3,     2,
       2,     3,     3,     3,     3,     4,     2,     3,     3,     3,
       4,     2,     3,     3,     4,     2,     2,     2,     2,     2,
       1,     1,     2,     0,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     1,     2,     2,     2,     2,     2,
       1,     2,     2,     2,     2,     4,     8,     5,     6,     5,
       5,     5,     5,     5,     5,     5,     5,     6,     7,     8,
       6,     7,     8,     9,     1,     3,     1,     3,     2,     4,
       1,     2,     1,     2,     2,     0,     2,     0,     4,     1,
       2,     1,     1,     1,     1,     1,     1,     2,     1,     2,
       5,     3,     5,     5,     0,     2,     2,     1,     3,     1,
       3,     3,     5,     3,     1,     3,     2,     1,     1,     1,
       3,     1,     1,     6,     6,     6,     6,     4,     4,     5,
       1,     2,     5,     4,     1,     1,     5,     3,     4,     2,
       5,     3,     6,     4,     5,     3,     6,     4,     3,     4,
       3,     3,     4,     3,     1,     1,     1,     1,     2,     1,
       2,     3,     5,     4,     5,     2,     7,     0,     5,     2,
       4,     4,     6,     1,     2,     5,     1,     2,     5,     5,
       4,     1,     6,     0,     2,     4,     1,     3,     1,    10,
       6,     3,     3,     3,     3,     3,     2,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     2,     1,
       1,     3,     1,     3,     1,     1,     1,     1,     1,     1,
       1,     3,     1,     4,     1,     1,     1,     6,     3,     3,
       1,     3,     3,     1,     3,     1,     3,     2,     1,     3,
       4,     3,     2,     1,     1,     3,     4,     1,     3,     2,
       3,     4,     1,     2,     2,     2,     2,     2,     3,     4,
       4,     4,     4,     2,     4,     4,     4,     4,     2,     3,
       5,     7,     0,     3,     1,     3,     3,     7,     6,     4,
       3,     5,     6,     2,     2,     1,     3,     3,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       6,     6,     0,     0,     7,     1,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     2,
      51,     0,    51,     0,    51,     0,   323,    39,    39,     0,
       0,   328,     0,    20,   317,     3,     4,     0,     0,    16,
      25,     0,     0,    17,    25,     0,    25,     0,     0,     0,
     329,     0,     0,     0,     0,     0,   318,     5,   140,     0,
       0,   134,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    65,    63,    64,     0,     0,
      27,     0,     0,    52,    57,    59,    53,     0,     0,     0,
       0,     0,     0,   348,     0,   345,     0,     0,    27,     0,
      27,     0,   319,   321,   320,   322,     0,    41,     0,     0,
      21,    23,   327,   324,   325,   326,     0,     0,   141,   142,
       0,     0,     0,   114,   217,     0,     0,   147,     0,     0,
     241,     0,     0,     0,    49,     0,     0,     0,     0,   104,
     110,    26,    93,     0,   303,     0,     0,     0,     0,     0,
       0,     0,     0,   304,     0,     0,     0,     0,     0,     0,
       0,     0,   138,    39,    39,     0,    54,     0,   303,   314,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    81,     0,     0,    70,    76,    66,    67,
       0,    69,    58,    85,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    40,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   330,    37,   136,
      38,   151,   153,   152,   154,   155,   156,     0,     0,     0,
     144,   143,     0,     0,   334,    18,   135,     0,     0,     0,
     218,   219,     0,   233,     0,   274,   275,   277,   269,   270,
     278,   276,     0,     0,     0,   272,     0,   279,     0,     0,
       0,     0,     0,   236,     0,     0,     0,     0,     0,   344,
       0,   343,     0,   217,    45,    46,     0,     0,   312,   315,
     307,   147,   316,    92,    95,     0,     0,    96,    97,    98,
      99,   102,   100,   101,   103,   105,   112,   113,   106,   111,
     107,   108,   109,     0,   147,     0,     0,     0,    73,    71,
      77,     0,    82,    72,    78,    68,     0,    83,    74,    79,
       0,     0,     0,     0,    28,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   145,   145,   347,    51,
     346,     0,    19,     0,    51,     0,    51,    42,     0,     0,
       0,     0,     0,     0,   195,   194,     0,   190,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   177,   179,
     178,   199,   147,     0,   174,     0,     0,     0,   332,     0,
      14,     0,     0,     0,   115,   147,   158,   149,   225,     0,
     220,     0,   234,     0,   268,   256,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   237,     0,     0,   243,     0,     0,     0,   147,    90,
      91,    50,     0,     0,     0,     0,   340,     0,    47,     0,
     313,     0,   309,    94,     0,     0,     0,   213,   214,   215,
     216,   305,     0,   169,   139,   167,    33,     0,     0,    55,
       0,    51,     0,    62,    84,    75,    80,    12,     0,     0,
       0,     0,     0,     0,     0,     0,   145,   145,     0,     0,
       0,     0,     0,     0,   147,   147,    25,    15,     9,    25,
       8,    25,   147,   147,   161,     0,   181,   182,     0,     0,
       0,     0,   191,     0,     0,     0,     0,     0,   208,     0,
       0,   197,   201,     0,   205,     0,     0,   176,   173,     0,
     282,   285,   284,   286,   137,   280,    22,    24,     0,   331,
     336,   335,     0,   150,     0,   159,   157,     0,     0,   221,
       0,   231,   271,   273,   227,   257,   258,   259,   267,   266,
     265,   264,   263,   262,   261,   260,   251,   252,   253,   254,
     255,     0,     0,     0,   339,     0,     0,   230,     0,     0,
     210,   211,     0,     0,     0,    86,    87,    89,    88,     0,
     310,    48,   308,   212,   217,   297,     0,   298,     0,   295,
       0,   246,   214,     0,   290,     0,   293,   306,     0,     0,
       0,     0,     0,   147,    56,     0,     0,   123,   124,   121,
     122,   119,   120,   145,     0,     0,   147,   147,   164,   125,
       0,   145,   146,   147,   117,   126,    27,    27,    27,    43,
      44,   164,     0,     0,     0,     0,     0,   147,   187,   188,
       0,   164,   164,   209,   203,     0,   207,     0,     0,   198,
     180,   175,     0,     0,     0,     0,   223,     0,     0,     0,
       0,     0,     0,   341,     0,   148,   240,     0,     0,     0,
       0,   244,     0,     0,     0,   311,   302,   296,     0,     0,
       0,     0,   288,     0,   289,     0,   171,   170,   168,    34,
      49,     0,     0,     0,   147,     0,   145,   130,   127,     0,
       0,   147,   145,   147,   118,     0,     0,     0,   160,     0,
       0,     0,     0,   193,     0,   189,   162,   163,     0,   196,
     200,     0,   204,     0,   281,   333,     0,   222,   224,   235,
       0,   229,     0,   342,   238,   239,   232,     0,   248,   242,
     250,     0,   338,     0,   301,     0,   299,     0,   292,   247,
     291,   294,     0,    30,     0,    49,     0,     0,   131,   145,
     147,   165,   166,   337,   147,   128,     0,     0,     0,   184,
     186,   185,   183,   192,   202,   206,   283,   116,     0,   226,
       0,     0,   300,     0,   172,     0,    32,     0,    60,    61,
     147,   132,   129,    13,    11,    10,   227,   245,     0,   287,
      35,     0,   133,   228,     0,     0,     0,    35,   249,    36,
       0,     0,    29,     0,    31
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     3,    89,    90,   195,    91,    92,   612,   816,
     227,    60,   116,   117,   287,   280,    50,    93,   149,   150,
      96,   441,   151,   293,   152,    97,    98,    99,   378,   100,
     229,   128,   129,   494,   270,   394,   379,   396,   397,   231,
     232,   711,   464,   465,   233,   383,   384,   380,   381,   508,
     234,   235,   366,   367,   236,   278,   153,   599,   265,   250,
     251,   155,   672,   156,   157,   158,   254,   274,   159,   160,
     579,   681,   600,   601,   161,   458,   534,   535,   459,   603,
     604,   605,   606,   460,   596,   597,   162,   267,   289,   290,
     291,   164,   165,   166,    24,    25,   539,   243,   244,   102,
     167,   168,   169,   170,   171,   104,   105
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -629
static const yytype_int16 yypact[] =
{
      19,    19,    78,   359,  -629,  -629,   102,   133,   149,   279,
     209,   209,    94,   104,    80,    81,    86,   199,   202,   206,
     213,   219,   227,   192,   209,   151,   154,   155,   209,  -629,
      69,   143,   127,   160,  -629,   164,   249,   168,   168,   255,
     261,   262,   267,  -629,    90,  -629,  -629,   181,   272,   186,
    1627,   281,   272,   189,  1627,   281,  1627,   281,   135,   283,
    -629,   193,   230,   232,    65,   284,  -629,  -629,   195,   201,
     220,   205,   295,  1945,   299,   301,   308,  1727,   274,   310,
     173,   264,   456,   303,   265,  -629,  -629,  -629,   260,   231,
     282,   238,   241,  -629,  -629,  -629,  -629,   244,   245,   243,
     254,   258,   276,   315,   263,   268,   277,   294,   282,   278,
     282,   280,  -629,  -629,  -629,  -629,   285,   286,   287,  1339,
     298,   305,  -629,  -629,  -629,  -629,   324,   374,  -629,   195,
     384,   311,   283,   381,    -8,  1880,   938,   101,  1848,   938,
    -629,   938,   405,   370,  -629,   310,   358,   965,   965,  -629,
    -629,  -629,  -629,   321,    38,   326,   330,   331,   342,   343,
     345,   354,   360,  -629,   361,   366,   368,   369,   378,   388,
     390,   391,   444,   168,   168,    44,  -629,   393,   375,  -629,
     394,   395,   470,   397,   398,   399,   400,   386,   408,   410,
     411,   272,   303,  -629,  1945,   490,  -629,  -629,  -629,  -629,
    1043,  -629,  -629,  -629,   938,   498,   281,   384,   414,   502,
     510,   504,   512,  -629,   283,   159,   851,   -16,    41,    41,
     489,   528,   529,   472,   473,  1097,   530,  -629,  -629,   438,
    -629,  -629,  -629,  -629,  -629,  -629,  -629,   464,   466,   272,
    -629,  -629,   511,   450,   449,  -629,  -629,   651,   552,   938,
    -629,    -8,   553,  1880,   548,  -629,  -629,  -629,  -629,  -629,
    -629,  -629,   938,   938,   938,    61,  1526,  -629,   106,   474,
    1945,   556,   562,  1848,   559,  1729,  1445,   479,   163,  -629,
      83,  -629,   272,    25,   547,  -629,   550,   938,  2177,  -629,
     487,   526,  -629,  1945,  -629,   144,   843,  -629,  -629,  -629,
    -629,  -629,  -629,  -629,  -629,  -629,  -629,  -629,  -629,  -629,
    -629,  -629,  -629,   272,   526,   491,   283,   493,  -629,  -629,
    -629,    64,  -629,  -629,  -629,  -629,  1085,  -629,  -629,  -629,
     495,   496,   497,   499,  -629,   589,   537,   538,   539,   541,
     543,   544,  1391,  1462,    43,  1168,    46,   596,  2177,  -629,
    -629,   508,  -629,   603,  -629,   604,  -629,  -629,  1339,   938,
    1484,    74,    74,   513,  -629,  -629,   599,    41,   601,    41,
     582,   588,  1203,  1132,  1348,   517,   602,  1383,  -629,  -629,
    -629,  -629,   526,   533,   532,    68,   535,   536,   540,   938,
    -629,   384,  1421,  1452,  -629,   526,  -629,  -629,  -629,   869,
    -629,   938,  -629,   620,  2188,  -629,  2055,    68,  1945,   938,
     938,   938,   938,   938,   938,   938,   938,   938,   938,   938,
     938,   938,   938,   938,   938,  1945,   567,   281,   625,   938,
     938,  -629,   622,  1945,  -629,    74,  1945,   938,   526,  -629,
    -629,  -629,   551,   554,   555,   563,  -629,   973,  -629,   614,
    2177,   965,  -629,  -629,   650,   816,   756,  -629,  2177,  -629,
    -629,  -629,   566,    92,  -629,   560,   639,  1339,   564,  -629,
    1945,  -629,   606,  -629,  -629,  -629,  -629,  -629,   272,   272,
     272,   272,   272,   272,  1462,  1215,   596,   596,   938,   310,
    1462,   569,   650,   310,   526,   526,  1627,  -629,  -629,  1627,
    -629,  1627,   526,   526,  2177,   938,  -629,  -629,    14,    71,
    1257,   624,  -629,   630,   652,   664,   666,  1348,  -629,  1348,
     645,  -629,  -629,  1348,  -629,   273,   583,  -629,  -629,   530,
     585,  -629,  -629,  -629,  -629,   579,  -629,  -629,   384,  -629,
    2177,  -629,  1462,  -629,   586,  -629,  -629,   557,   938,  -629,
    2084,  -629,  -629,  -629,   223,   389,   389,   389,  2188,  2188,
    2217,  2217,  2217,  2217,  2217,  2217,   136,   136,  -629,  -629,
    -629,   668,  1945,   595,  -629,  2115,  2146,  -629,   676,   194,
    -629,  -629,   677,  1513,  1945,  -629,  -629,  -629,  -629,   938,
    2177,  -629,  -629,  -629,    32,  -629,   598,   584,   690,  -629,
     665,   594,  1942,   597,   607,   605,   611,  -629,   670,   699,
     272,   938,   695,   526,  -629,   694,  1677,  -629,  -629,  -629,
    -629,  -629,  -629,   596,  1462,   615,   526,   526,  1984,  -629,
     617,   596,  -629,   526,  -629,  -629,   282,   282,   282,  -629,
    -629,  1984,   683,   688,   689,   692,   629,   526,  -629,  -629,
     698,    52,    52,  -629,  -629,  1267,  -629,  1348,  1304,  -629,
    -629,  -629,   721,    68,   632,   281,  -629,  1997,  2026,  1880,
     938,  1945,   715,  -629,   720,  -629,  1848,  1848,   718,   938,
     714,  -629,   722,   938,   727,  2177,   812,  -629,   744,   716,
     650,   938,  -629,   938,  -629,   650,   653,  -629,  -629,  2177,
     696,   742,   751,   753,   526,   667,   596,  -629,  -629,   272,
     733,   526,   596,   526,  -629,   752,   754,   755,  -629,  1348,
    1348,  1348,  1348,  -629,   673,  -629,  -629,  -629,  1348,  -629,
    -629,  1348,  -629,   678,  -629,  -629,   679,  -629,  -629,  -629,
    1565,  -629,   758,  -629,  -629,  -629,  -629,   735,  2177,  -629,
    -629,  1760,  -629,   650,  -629,   739,  -629,   650,  -629,  -629,
    -629,  -629,   773,  -629,   352,   719,   685,   707,  -629,   596,
     526,  -629,  -629,  -629,   526,  -629,   778,   779,   797,  -629,
    -629,  -629,  -629,  -629,  -629,  -629,  -629,  -629,  1945,  -629,
    1945,  1945,  -629,   700,  -629,  1945,  -629,   356,  -629,  -629,
     526,  -629,  -629,  -629,  -629,  -629,   223,  -629,   788,  -629,
     737,  1945,  -629,  -629,   789,  1945,   792,   737,  -629,  -629,
     805,   795,  -629,   807,  -629
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -629,  -629,   813,    37,   -25,   -98,   -26,   -20,  -629,     7,
     362,     4,   612,  -629,  -174,  -628,     2,  -629,   -50,   -49,
     748,  -629,  -168,  -629,  -118,  -269,   -79,   -24,   -82,  -255,
     -39,   701,  -629,  -324,   -97,  -629,   -76,  -306,   436,  -293,
    -629,  -432,  -566,  -629,  -629,   314,  -629,  -629,  -263,   477,
    -629,  -629,  -164,  -629,  -629,  -629,  -629,  -267,   -71,   593,
    -629,  -629,    28,  -629,  -629,  -629,  -234,  -243,  -629,  -629,
    -629,  -629,  -594,  -629,  -629,   376,  -369,  -629,  -629,   157,
    -629,   158,  -629,  -629,   171,  -629,   781,   -58,  -115,  -629,
    -629,   -32,  -629,  -629,   808,  -629,  -629,  -175,  -629,  -253,
    -629,  -629,  -629,  -629,  -629,   -41,  -629
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -249
static const yytype_int16 yytable[] =
{
      94,    95,   154,   186,    94,    95,    94,    95,   178,    69,
     209,   442,   211,   106,   109,   163,   111,   253,   101,   402,
     273,   163,   101,   495,   101,   444,   334,   445,   457,   108,
     431,   110,   351,   292,    54,   118,    56,   228,   553,   491,
      13,   358,    61,   230,   698,   295,   363,    26,    27,    71,
     488,   361,   177,   492,   180,   368,   184,   189,   187,   447,
     181,    43,   185,   190,   154,    47,   686,   154,   507,   507,
     122,   526,   764,   530,   281,   123,   531,   163,     5,    68,
     163,   470,   642,   709,   216,   747,   362,   545,    71,    30,
      32,    72,   248,     1,   249,    34,   317,   759,    17,   532,
     438,   489,   428,   124,   493,   710,   316,    14,   246,   518,
     521,   522,   524,   268,    74,   364,   365,   643,   346,   183,
     188,   471,    48,   154,   347,   248,   125,   249,    49,   533,
      18,    19,   248,   296,   249,   253,   163,   797,    15,   644,
      31,    33,   581,   771,    22,   112,    35,   255,   256,   134,
     257,   187,   330,    23,    16,   273,   296,    85,    86,    87,
     407,   269,   626,   627,    28,   350,   332,   113,   258,   259,
     260,   395,   333,   114,   645,   453,   425,   314,   315,   625,
      52,   462,   154,   436,   630,   426,    53,   593,   284,   285,
     118,   608,   437,   609,   452,   163,   115,   454,    29,   154,
     388,   443,   154,   512,    36,   514,   679,    37,   286,   718,
     680,    38,   163,    75,    76,   163,   541,   466,    39,   726,
     727,   261,   154,   262,    40,   632,   422,   423,   424,   331,
     439,   440,    41,   263,    42,   163,   545,   468,    51,   455,
     554,   670,   671,   446,   346,    44,   456,   647,    45,    46,
     347,     6,     7,     8,   653,    55,   654,   571,    58,    57,
     656,     9,   659,    59,    62,   578,   486,   487,   582,   395,
      63,    64,    65,   589,   463,    67,   502,    68,    68,   506,
     506,    70,   503,   216,   107,   527,   103,    17,    71,   120,
     119,   121,   187,   126,   734,   127,   130,   131,   546,   704,
     133,   395,   615,   132,   172,   217,   173,   713,   218,   219,
     220,   221,   222,   174,    75,   134,   395,   395,   705,    18,
      19,    20,    21,   182,   191,   193,   657,   658,   223,   224,
     192,   194,   196,    22,   225,   197,   592,   154,   198,   199,
     200,   584,    23,    75,    76,     6,     7,     8,   201,   204,
     163,   496,   202,   580,   154,     9,   499,    71,   501,   205,
      72,    71,   154,   664,    72,   154,   206,   163,   377,   795,
     203,   208,   207,   811,   210,   163,   212,   239,   163,   240,
     214,   213,   770,    74,   215,   228,   573,    74,   774,   242,
     247,   230,   729,   237,   730,   732,   617,   634,   635,   154,
     238,     6,     7,     8,   674,   639,   640,   245,   623,   395,
     277,     9,   163,   279,   395,   294,   684,   282,   629,   754,
     297,    10,   633,   758,   298,   299,    85,    86,    87,    11,
      85,    86,    87,   744,   745,   739,   300,   301,    12,   302,
     618,   619,   620,   621,   622,   800,    94,    95,   303,    94,
      95,    94,    95,   313,   304,   305,   779,   780,   781,   782,
     306,    71,   307,   308,   101,   784,   395,   101,   785,   101,
     296,   636,   309,   616,   637,   321,   638,   420,   421,   422,
     423,   424,   310,   326,   311,   312,   792,   318,   319,   320,
     793,   322,   323,   324,   325,   442,    75,    76,     6,     7,
       8,   154,   327,   741,   328,   329,   335,   349,     9,   444,
     352,   445,   266,   154,   163,   275,   701,   276,   353,   354,
     355,   356,    68,   288,   288,   369,   163,   216,   442,   707,
     708,   370,   371,   372,   373,   382,   714,   385,   715,   716,
     717,   386,   444,   387,   445,   389,   390,   391,   395,   217,
     724,   253,   218,   219,   220,   221,   222,   398,   273,   273,
     255,   256,   134,   257,   403,   401,    94,    95,   429,   427,
     523,   463,   223,   224,   430,   432,   435,   448,   225,   449,
     348,   258,   259,   260,   101,   451,   269,   469,   467,   473,
     474,   475,   360,   476,   477,   478,   479,   480,   154,   481,
     154,   482,   483,   492,   497,   154,   154,   768,   498,   500,
     510,   163,   377,   163,   773,   511,   775,   513,   163,   163,
     806,   515,   807,   808,   736,   399,   516,   810,   525,   528,
     529,   536,   537,   551,   261,   538,   262,   572,   404,   405,
     406,   574,   577,   817,   591,   585,   263,   819,   586,   587,
     610,   611,   264,   255,   256,   134,   257,   588,   614,   648,
     666,   216,   607,   450,   489,   631,   649,   651,   650,   652,
     463,   655,   288,   801,   258,   259,   260,   802,   663,   660,
     662,   665,   688,   217,   673,   443,   218,   219,   220,   221,
     222,   675,   678,   682,   687,   689,   392,   393,   691,   690,
     692,   696,   697,   812,   700,   693,   223,   224,   694,   695,
     702,   706,   225,   712,   439,   440,   719,   154,   443,   154,
     154,   720,   721,   723,   154,   722,   733,   261,   735,   262,
     163,   742,   163,   163,   725,   504,   743,   163,   746,   263,
     154,   749,   750,   752,   154,   455,   226,   439,   440,   755,
     757,   765,   456,   163,   762,   763,   766,   163,   767,   255,
     256,   134,   257,   769,   772,   540,   598,   783,   776,   790,
     777,   778,   789,   686,   786,   787,   794,   550,   796,   798,
     258,   259,   260,   803,   804,   555,   556,   557,   558,   559,
     560,   561,   562,   563,   564,   565,   566,   567,   568,   569,
     570,   799,   805,   809,   814,   575,   576,   815,   820,   818,
     822,   823,   824,   583,     4,   255,   256,   134,   257,   255,
     256,   594,   257,   590,   821,   176,   357,   288,   543,   613,
     241,   406,   602,   261,   813,   262,   258,   259,   260,   509,
     258,   259,   260,   661,   400,   263,   255,   256,   283,   257,
     760,   455,    66,   761,   255,   256,   134,   257,   456,   756,
     179,     0,     0,     0,   628,   753,     0,   258,   259,   260,
       0,     0,   284,   285,     0,   258,   259,   260,     0,   359,
     547,   641,     0,     0,     0,     0,     0,     0,     0,   261,
       0,   262,   286,   261,     0,   262,     0,   548,     0,     0,
       0,   263,     0,     0,     0,   263,     0,   455,     0,     0,
       0,   264,   595,     0,   456,     0,     0,     0,     0,     0,
     261,     0,   262,   667,   668,     0,     0,     0,   261,     0,
     262,     0,   263,   409,   410,   411,     0,     0,   264,   461,
     263,   255,   256,   134,   257,     0,   264,     0,     0,   412,
     413,   414,   415,   416,   417,   418,   419,   420,   421,   422,
     423,   424,   258,   259,   260,   685,     0,     0,   255,   256,
     283,   257,   549,     0,     0,     0,   255,   256,   134,   257,
       0,     0,     0,     0,     0,     0,     0,   699,     0,   258,
     259,   260,     0,     0,   284,   285,     0,   258,   259,   260,
       0,     0,   284,   285,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   286,   261,     0,   262,     0,     0,
       0,     0,   286,     0,     0,     0,     0,   263,     0,     0,
       0,     0,     0,   264,     0,     0,     0,     0,     0,     0,
       0,     0,   261,     0,   262,     0,   740,     0,    68,     0,
     261,   336,   262,   216,   263,   748,     0,     0,     0,   751,
     264,     0,   263,     0,     0,     0,     0,   748,   264,   748,
       0,     0,     0,     0,   337,   217,     0,     0,   218,   219,
     220,   221,   222,   338,   339,   340,   341,     0,   342,   343,
      68,     0,     0,   336,     0,   216,     0,   344,   223,   224,
       0,     0,    68,     0,   225,     0,     0,   216,     0,     0,
       0,     0,     0,     0,     0,     0,   337,   217,     0,     0,
     218,   219,   220,   221,   222,   338,   339,   340,   341,   217,
     342,   343,   218,   219,   220,   221,   222,    68,   345,   472,
     223,   224,   216,     0,     0,     0,   225,     0,     0,     0,
     374,   375,   223,   224,     0,     0,     0,     0,   225,     0,
       0,     0,     0,     0,   217,     0,     0,   218,   219,   220,
     221,   222,     0,   382,     0,     0,     0,   376,   216,     0,
     345,     0,     0,     0,     0,     0,   519,   223,   224,     0,
       0,     0,   377,   225,     0,     0,     0,     0,     0,     0,
     217,     0,     0,   218,   219,   220,   221,   222,    68,     0,
       0,     0,   520,   216,   490,     0,     0,     0,     0,     0,
     382,     0,     0,   223,   224,   216,     0,   377,     0,   225,
       0,     0,     0,     0,     0,   217,     0,     0,   218,   219,
     220,   221,   222,     0,     0,     0,     0,   217,     0,     0,
     218,   219,   220,   221,   222,     0,     0,   517,   223,   224,
       0,   624,    68,   226,   225,     0,     0,   216,     0,     0,
     223,   224,    68,     0,     0,     0,   225,   216,     0,     0,
       0,     0,     0,   646,     0,     0,     0,     0,     0,   217,
       0,     0,   218,   219,   220,   221,   222,     0,   377,   217,
       0,     0,   218,   219,   220,   221,   222,     0,     0,    68,
     226,     0,   223,   224,   216,     0,     0,     0,   225,     0,
       0,   728,   223,   224,     0,     0,     0,     0,   225,     0,
       0,     0,     0,     0,     0,     0,   217,     0,     0,   218,
     219,   220,   221,   222,    68,     0,     0,     0,     0,   216,
       0,     0,   377,    68,     0,     0,     0,   731,   216,   223,
     224,     0,   377,     0,     0,   225,     0,     0,     0,     0,
       0,   217,     0,     0,   218,   219,   220,   221,   222,     0,
     217,     0,     0,   218,   219,   220,   221,   222,   382,     0,
       0,     0,     0,   216,   223,   224,     0,     0,     0,   377,
     225,   216,     0,   223,   224,     0,     0,     0,     0,   225,
       0,     0,     0,     0,     0,   217,     0,     0,   218,   219,
     220,   221,   222,   217,     0,     0,   218,   219,   220,   221,
     222,   216,     0,     0,   226,     0,     0,   484,   223,   224,
       0,     0,     0,   377,   225,     0,   223,   224,     0,     0,
       0,     0,   225,   217,   434,     0,   218,   219,   220,   221,
     222,     0,   216,     0,     0,     0,     0,   542,     0,     0,
       0,     0,   216,     0,     0,     0,   223,   224,   226,     0,
       0,     0,   225,     0,   217,     0,   485,   218,   219,   220,
     221,   222,     0,     0,   217,   505,     0,   218,   219,   220,
     221,   222,     0,     0,     0,     0,     0,   223,   224,   409,
     410,   411,   544,   225,     0,     0,   226,   223,   224,     0,
       0,     0,     0,   225,   683,   412,   413,   414,   415,   416,
     417,   418,   419,   420,   421,   422,   423,   424,     0,     0,
       0,   408,     0,     0,     0,     0,     0,   226,   409,   410,
     411,     0,     0,     0,     0,     0,     0,   226,     0,     0,
       0,     0,     0,     0,   412,   413,   414,   415,   416,   417,
     418,   419,   420,   421,   422,   423,   424,   409,   410,   411,
     788,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     409,   410,   411,   412,   413,   414,   415,   416,   417,   418,
     419,   420,   421,   422,   423,   424,   412,   413,   414,   415,
     416,   417,   418,   419,   420,   421,   422,   423,   424,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   409,
     410,   411,    71,     0,     0,    72,     0,     0,     0,     0,
       0,     0,     0,     0,    73,   412,   413,   414,   415,   416,
     417,   418,   419,   420,   421,   422,   423,   424,    74,     0,
       0,     0,     0,     0,     0,     0,     0,    75,    76,     6,
       7,     8,    77,     0,    78,     0,     0,    79,    80,     9,
       0,     0,    71,     0,    81,    72,     0,     0,     0,    82,
       0,     0,     0,   703,     0,     0,     0,    83,    84,     0,
       0,    85,    86,    87,     0,     0,    88,     0,    74,     0,
       0,     0,     0,     0,     0,     0,     0,    75,    76,     6,
       7,     8,    77,     0,    78,     0,     0,    79,    80,     9,
       0,     0,    71,     0,    81,    72,     0,     0,     0,    82,
       0,     0,     0,     0,     0,     0,     0,    83,    84,   433,
       0,    85,    86,    87,     0,     0,    88,   175,    74,     0,
       0,     0,     0,     0,     0,     0,     0,    75,    76,     6,
       7,     8,     0,     0,    78,     0,     0,     0,    80,     9,
     791,     0,     0,     0,     0,     0,     0,     0,     0,    82,
       0,     0,     0,   409,   410,   411,     0,    83,     0,     0,
       0,     0,     0,     0,     0,     0,    88,     0,     0,   412,
     413,   414,   415,   416,   417,   418,   419,   420,   421,   422,
     423,   424,     0,     0,   409,   410,   411,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     412,   413,   414,   415,   416,   417,   418,   419,   420,   421,
     422,   423,   424,   134,     0,     0,     0,     0,     0,     0,
       0,   135,   136,     0,     0,   137,     0,     0,   138,   271,
     272,   139,     0,     0,   140,   141,   142,     0,     0,     0,
       0,     0,     0,     0,     0,   134,     0,     0,     0,     0,
       0,     0,     0,   135,   136,     0,   143,   137,    79,     0,
     138,   252,     0,   139,     0,   144,   140,   141,   142,     0,
       0,     0,     0,     0,     0,     0,     0,   145,     0,   146,
     147,   148,    85,    86,    87,     0,     0,     0,   143,     0,
      79,     0,     0,     0,     0,     0,     0,   144,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   145,
     134,   146,   147,   148,    85,    86,    87,     0,   135,   136,
       0,     0,   137,     0,     0,   138,     0,     0,   139,     0,
       0,   140,   141,   142,     0,     0,  -248,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   143,     0,    79,     0,     0,     0,     0,
       0,     0,   144,     0,     0,     0,   409,   410,   411,     0,
       0,     0,     0,     0,   145,   709,   146,   147,   148,    85,
      86,    87,   412,   413,   414,   415,   416,   417,   418,   419,
     420,   421,   422,   423,   424,     0,     0,   710,     0,     0,
       0,     0,     0,     0,     0,     0,  -248,     0,   409,   410,
     411,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   409,   410,   411,   412,   413,   414,   415,   416,   417,
     418,   419,   420,   421,   422,   423,   424,   412,   413,   414,
     415,   416,   417,   418,   419,   420,   421,   422,   423,   424,
     409,   410,   411,     0,     0,     0,     0,     0,     0,     0,
     737,     0,     0,     0,     0,     0,   412,   413,   414,   415,
     416,   417,   418,   419,   420,   421,   422,   423,   424,   409,
     410,   411,     0,     0,     0,     0,     0,     0,     0,   738,
       0,     0,     0,     0,     0,   412,   413,   414,   415,   416,
     417,   418,   419,   420,   421,   422,   423,   424,   409,   410,
     411,   552,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   412,   413,   414,   415,   416,   417,
     418,   419,   420,   421,   422,   423,   424,     0,   669,   409,
     410,   411,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   412,   413,   414,   415,   416,
     417,   418,   419,   420,   421,   422,   423,   424,     0,   676,
     409,   410,   411,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   412,   413,   414,   415,
     416,   417,   418,   419,   420,   421,   422,   423,   424,     0,
     677,   409,   410,   411,     0,     0,     0,     0,     0,     0,
       0,     0,   409,   410,   411,     0,     0,   412,   413,   414,
     415,   416,   417,   418,   419,   420,   421,   422,   423,   424,
     414,   415,   416,   417,   418,   419,   420,   421,   422,   423,
     424,   409,   410,   411,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  -249,
    -249,  -249,  -249,  -249,  -249,   420,   421,   422,   423,   424
};

static const yytype_int16 yycheck[] =
{
      50,    50,    73,    82,    54,    54,    56,    56,    79,    48,
     108,   280,   110,    52,    55,    73,    57,   135,    50,   253,
     138,    79,    54,   347,    56,   280,   194,   280,   295,    54,
     273,    56,   207,   148,    32,    59,    34,   119,   407,   345,
       3,   215,    38,   119,   610,     7,     5,    10,    11,     5,
       7,    67,    78,     7,    80,   219,    82,    83,    82,    34,
      80,    24,    82,    83,   135,    28,    34,   138,   361,   362,
       5,   377,   700,     5,   145,    10,     8,   135,     0,     5,
     138,    17,    68,    31,    10,   679,   102,   393,     5,     9,
       9,     8,   100,    74,   102,     9,   175,   691,     8,    31,
      17,    58,   270,    38,    58,    53,    62,     5,   132,   372,
     373,   374,   375,    12,    31,    74,    75,   103,   200,    82,
      83,    57,    53,   194,   200,   100,    61,   102,    59,    61,
      40,    41,   100,    95,   102,   253,   194,   765,     5,    68,
      60,    60,   435,   709,    54,    10,    60,     3,     4,     5,
       6,   175,   191,    63,     5,   273,    95,    74,    75,    76,
      99,    60,   486,   487,    70,   206,   192,    32,    24,    25,
      26,   247,   192,    38,   103,   293,    70,   173,   174,   485,
      53,   296,   253,    20,   490,    79,    59,   454,    29,    30,
     214,    99,    29,   101,   291,   253,    61,    53,    94,   270,
     239,   280,   273,   367,     5,   369,    12,     5,    49,   641,
      16,     5,   270,    40,    41,   273,   391,   314,     5,   651,
     652,    77,   293,    79,     5,   492,    90,    91,    92,   192,
     280,   280,     5,    89,    42,   293,   542,   316,    95,    95,
     408,    18,    19,   282,   326,    94,   102,   510,    94,    94,
     326,    42,    43,    44,   517,    95,   519,   425,     9,    95,
     523,    52,   525,    95,     9,   433,   342,   343,   436,   345,
       9,     9,     5,   447,   313,    94,   358,     5,     5,   361,
     362,    95,   358,    10,    95,   382,     5,     8,     5,    59,
      97,    59,   316,     9,   663,   100,    95,    77,   395,   623,
       5,   377,   470,    98,     5,    32,     5,   631,    35,    36,
      37,    38,    39,     5,    40,     5,   392,   393,   624,    40,
      41,    42,    43,    59,    59,    94,    53,    54,    55,    56,
      70,    49,    94,    54,    61,    94,   451,   408,    94,    94,
      97,   438,    63,    40,    41,    42,    43,    44,    94,    34,
     408,   349,    94,   435,   425,    52,   354,     5,   356,    96,
       8,     5,   433,   538,     8,   436,    98,   425,    95,    17,
      94,    77,    95,    17,    96,   433,    96,    53,   436,     5,
      94,    96,   706,    31,    97,   467,   427,    31,   712,     5,
       9,   467,   655,    95,   657,   658,   478,   494,   495,   470,
      95,    42,    43,    44,   572,   502,   503,    96,   484,   485,
       5,    52,   470,    43,   490,    94,   584,    59,   489,   686,
      94,    62,   493,   690,    94,    94,    74,    75,    76,    70,
      74,    75,    76,   676,   677,   669,    94,    94,    79,    94,
     479,   480,   481,   482,   483,   769,   496,   496,    94,   499,
     499,   501,   501,     9,    94,    94,   719,   720,   721,   722,
      94,     5,    94,    94,   496,   728,   542,   499,   731,   501,
      95,   496,    94,   471,   499,     5,   501,    88,    89,    90,
      91,    92,    94,    97,    94,    94,   753,    94,    94,    94,
     757,    94,    94,    94,    94,   764,    40,    41,    42,    43,
      44,   572,    94,   671,    94,    94,    16,     9,    52,   764,
      96,   764,   136,   584,   572,   139,   613,   141,    16,     9,
      16,     9,     5,   147,   148,    36,   584,    10,   797,   626,
     627,     3,     3,    61,    61,     5,   633,    99,   636,   637,
     638,    77,   797,    77,   797,    34,    96,    98,   624,    32,
     647,   669,    35,    36,    37,    38,    39,     5,   676,   677,
       3,     4,     5,     6,    16,    12,   616,   616,    12,    95,
      53,   610,    55,    56,    12,    16,    97,    30,    61,    29,
     204,    24,    25,    26,   616,    98,    60,    94,    97,    94,
      94,    94,   216,    94,     5,    58,    58,    58,   669,    58,
     671,    58,    58,     7,    96,   676,   677,   704,     5,     5,
      97,   669,    95,   671,   711,    16,   713,    16,   676,   677,
     788,    39,   790,   791,   665,   249,    38,   795,    26,    96,
      98,    96,    96,    13,    77,    95,    79,    70,   262,   263,
     264,    16,    20,   811,    30,    94,    89,   815,    94,    94,
      90,    12,    95,     3,     4,     5,     6,    94,    94,    35,
     103,    10,    96,   287,    58,    96,    36,     3,    16,     3,
     709,    26,   296,   770,    24,    25,    26,   774,    99,    96,
      95,    95,    98,    32,    16,   764,    35,    36,    37,    38,
      39,    96,    16,    16,    96,     5,    45,    46,   104,    34,
     103,    31,     3,   800,     9,    98,    55,    56,   103,    98,
      16,    96,    61,    96,   764,   764,    33,   788,   797,   790,
     791,    33,    33,    94,   795,    33,     5,    77,    96,    79,
     788,    16,   790,   791,    36,   359,    16,   795,    20,    89,
     811,    27,    20,    16,   815,    95,    95,   797,   797,     5,
      34,     9,   102,   811,   101,    59,     5,   815,     5,     3,
       4,     5,     6,    96,    31,   389,    10,    94,    16,    34,
      16,    16,    14,    34,    96,    96,     3,   401,    59,    94,
      24,    25,    26,     5,     5,   409,   410,   411,   412,   413,
     414,   415,   416,   417,   418,   419,   420,   421,   422,   423,
     424,    94,     5,   103,    16,   429,   430,    70,    16,    20,
       5,    16,     5,   437,     1,     3,     4,     5,     6,     3,
       4,     5,     6,   447,   817,    77,   214,   451,   392,   467,
     129,   455,   456,    77,   806,    79,    24,    25,    26,   362,
      24,    25,    26,   529,   251,    89,     3,     4,     5,     6,
     693,    95,    44,   695,     3,     4,     5,     6,   102,   688,
      79,    -1,    -1,    -1,   488,    53,    -1,    24,    25,    26,
      -1,    -1,    29,    30,    -1,    24,    25,    26,    -1,    28,
      11,   505,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    77,
      -1,    79,    49,    77,    -1,    79,    -1,    28,    -1,    -1,
      -1,    89,    -1,    -1,    -1,    89,    -1,    95,    -1,    -1,
      -1,    95,    96,    -1,   102,    -1,    -1,    -1,    -1,    -1,
      77,    -1,    79,   547,   548,    -1,    -1,    -1,    77,    -1,
      79,    -1,    89,    64,    65,    66,    -1,    -1,    95,    96,
      89,     3,     4,     5,     6,    -1,    95,    -1,    -1,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    24,    25,    26,   589,    -1,    -1,     3,     4,
       5,     6,   103,    -1,    -1,    -1,     3,     4,     5,     6,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   611,    -1,    24,
      25,    26,    -1,    -1,    29,    30,    -1,    24,    25,    26,
      -1,    -1,    29,    30,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    49,    77,    -1,    79,    -1,    -1,
      -1,    -1,    49,    -1,    -1,    -1,    -1,    89,    -1,    -1,
      -1,    -1,    -1,    95,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    77,    -1,    79,    -1,   670,    -1,     5,    -1,
      77,     8,    79,    10,    89,   679,    -1,    -1,    -1,   683,
      95,    -1,    89,    -1,    -1,    -1,    -1,   691,    95,   693,
      -1,    -1,    -1,    -1,    31,    32,    -1,    -1,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    -1,    45,    46,
       5,    -1,    -1,     8,    -1,    10,    -1,    54,    55,    56,
      -1,    -1,     5,    -1,    61,    -1,    -1,    10,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    31,    32,    -1,    -1,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    32,
      45,    46,    35,    36,    37,    38,    39,     5,    95,    54,
      55,    56,    10,    -1,    -1,    -1,    61,    -1,    -1,    -1,
      53,    54,    55,    56,    -1,    -1,    -1,    -1,    61,    -1,
      -1,    -1,    -1,    -1,    32,    -1,    -1,    35,    36,    37,
      38,    39,    -1,     5,    -1,    -1,    -1,    80,    10,    -1,
      95,    -1,    -1,    -1,    -1,    -1,    54,    55,    56,    -1,
      -1,    -1,    95,    61,    -1,    -1,    -1,    -1,    -1,    -1,
      32,    -1,    -1,    35,    36,    37,    38,    39,     5,    -1,
      -1,    -1,    80,    10,    46,    -1,    -1,    -1,    -1,    -1,
       5,    -1,    -1,    55,    56,    10,    -1,    95,    -1,    61,
      -1,    -1,    -1,    -1,    -1,    32,    -1,    -1,    35,    36,
      37,    38,    39,    -1,    -1,    -1,    -1,    32,    -1,    -1,
      35,    36,    37,    38,    39,    -1,    -1,    54,    55,    56,
      -1,    46,     5,    95,    61,    -1,    -1,    10,    -1,    -1,
      55,    56,     5,    -1,    -1,    -1,    61,    10,    -1,    -1,
      -1,    -1,    -1,    26,    -1,    -1,    -1,    -1,    -1,    32,
      -1,    -1,    35,    36,    37,    38,    39,    -1,    95,    32,
      -1,    -1,    35,    36,    37,    38,    39,    -1,    -1,     5,
      95,    -1,    55,    56,    10,    -1,    -1,    -1,    61,    -1,
      -1,    54,    55,    56,    -1,    -1,    -1,    -1,    61,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    32,    -1,    -1,    35,
      36,    37,    38,    39,     5,    -1,    -1,    -1,    -1,    10,
      -1,    -1,    95,     5,    -1,    -1,    -1,    53,    10,    55,
      56,    -1,    95,    -1,    -1,    61,    -1,    -1,    -1,    -1,
      -1,    32,    -1,    -1,    35,    36,    37,    38,    39,    -1,
      32,    -1,    -1,    35,    36,    37,    38,    39,     5,    -1,
      -1,    -1,    -1,    10,    55,    56,    -1,    -1,    -1,    95,
      61,    10,    -1,    55,    56,    -1,    -1,    -1,    -1,    61,
      -1,    -1,    -1,    -1,    -1,    32,    -1,    -1,    35,    36,
      37,    38,    39,    32,    -1,    -1,    35,    36,    37,    38,
      39,    10,    -1,    -1,    95,    -1,    -1,    46,    55,    56,
      -1,    -1,    -1,    95,    61,    -1,    55,    56,    -1,    -1,
      -1,    -1,    61,    32,     9,    -1,    35,    36,    37,    38,
      39,    -1,    10,    -1,    -1,    -1,    -1,    46,    -1,    -1,
      -1,    -1,    10,    -1,    -1,    -1,    55,    56,    95,    -1,
      -1,    -1,    61,    -1,    32,    -1,    95,    35,    36,    37,
      38,    39,    -1,    -1,    32,    11,    -1,    35,    36,    37,
      38,    39,    -1,    -1,    -1,    -1,    -1,    55,    56,    64,
      65,    66,    60,    61,    -1,    -1,    95,    55,    56,    -1,
      -1,    -1,    -1,    61,    11,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    -1,    -1,
      -1,    15,    -1,    -1,    -1,    -1,    -1,    95,    64,    65,
      66,    -1,    -1,    -1,    -1,    -1,    -1,    95,    -1,    -1,
      -1,    -1,    -1,    -1,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    64,    65,    66,
      15,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      64,    65,    66,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    64,
      65,    66,     5,    -1,    -1,     8,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    17,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    31,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    40,    41,    42,
      43,    44,    45,    -1,    47,    -1,    -1,    50,    51,    52,
      -1,    -1,     5,    -1,    57,     8,    -1,    -1,    -1,    62,
      -1,    -1,    -1,    16,    -1,    -1,    -1,    70,    71,    -1,
      -1,    74,    75,    76,    -1,    -1,    79,    -1,    31,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    40,    41,    42,
      43,    44,    45,    -1,    47,    -1,    -1,    50,    51,    52,
      -1,    -1,     5,    -1,    57,     8,    -1,    -1,    -1,    62,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    70,    71,    20,
      -1,    74,    75,    76,    -1,    -1,    79,    30,    31,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    40,    41,    42,
      43,    44,    -1,    -1,    47,    -1,    -1,    -1,    51,    52,
      20,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    62,
      -1,    -1,    -1,    64,    65,    66,    -1,    70,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    79,    -1,    -1,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    -1,    -1,    64,    65,    66,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,     5,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    13,    14,    -1,    -1,    17,    -1,    -1,    20,    21,
      22,    23,    -1,    -1,    26,    27,    28,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     5,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    13,    14,    -1,    48,    17,    50,    -1,
      20,    21,    -1,    23,    -1,    57,    26,    27,    28,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    69,    -1,    71,
      72,    73,    74,    75,    76,    -1,    -1,    -1,    48,    -1,
      50,    -1,    -1,    -1,    -1,    -1,    -1,    57,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    69,
       5,    71,    72,    73,    74,    75,    76,    -1,    13,    14,
      -1,    -1,    17,    -1,    -1,    20,    -1,    -1,    23,    -1,
      -1,    26,    27,    28,    -1,    -1,    34,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    48,    -1,    50,    -1,    -1,    -1,    -1,
      -1,    -1,    57,    -1,    -1,    -1,    64,    65,    66,    -1,
      -1,    -1,    -1,    -1,    69,    31,    71,    72,    73,    74,
      75,    76,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    -1,    -1,    53,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   104,    -1,    64,    65,
      66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    64,    65,    66,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      64,    65,    66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     103,    -1,    -1,    -1,    -1,    -1,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    64,
      65,    66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   103,
      -1,    -1,    -1,    -1,    -1,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    64,    65,
      66,    96,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    -1,    94,    64,
      65,    66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    -1,    94,
      64,    65,    66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    -1,
      94,    64,    65,    66,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    64,    65,    66,    -1,    -1,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    64,    65,    66,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    74,   106,   107,   107,     0,    42,    43,    44,    52,
      62,    70,    79,   108,     5,     5,     5,     8,    40,    41,
      42,    43,    54,    63,   199,   200,   108,   108,    70,    94,
       9,    60,     9,    60,     9,    60,     5,     5,     5,     5,
       5,     5,    42,   108,    94,    94,    94,   108,    53,    59,
     121,    95,    53,    59,   121,    95,   121,    95,     9,    95,
     116,   116,     9,     9,     9,     5,   199,    94,     5,   135,
      95,     5,     8,    17,    31,    40,    41,    45,    47,    50,
      51,    57,    62,    70,    71,    74,    75,    76,    79,   108,
     109,   111,   112,   122,   123,   124,   125,   130,   131,   132,
     134,   196,   204,     5,   210,   211,   135,    95,   109,   210,
     109,   210,    10,    32,    38,    61,   117,   118,   132,    97,
      59,    59,     5,    10,    38,    61,     9,   100,   136,   137,
      95,    77,    98,     5,     5,    13,    14,    17,    20,    23,
      26,    27,    28,    48,    57,    69,    71,    72,    73,   123,
     124,   127,   129,   161,   163,   166,   168,   169,   170,   173,
     174,   179,   191,   192,   196,   197,   198,   205,   206,   207,
     208,   209,     5,     5,     5,    30,   125,   111,   163,   191,
     111,   112,    59,   108,   111,   112,   131,   132,   108,   111,
     112,    59,    70,    94,    49,   110,    94,    94,    94,    94,
      97,    94,    94,    94,    34,    96,    98,    95,    77,   110,
      96,   110,    96,    96,    94,    97,    10,    32,    35,    36,
      37,    38,    39,    55,    56,    61,    95,   115,   133,   135,
     141,   144,   145,   149,   155,   156,   159,    95,    95,    53,
       5,   136,     5,   202,   203,    96,   132,     9,   100,   102,
     164,   165,    21,   129,   171,     3,     4,     6,    24,    25,
      26,    77,    79,    89,    95,   163,   180,   192,    12,    60,
     139,    21,    22,   129,   172,   180,   180,     5,   160,    43,
     120,   163,    59,     5,    29,    30,    49,   119,   180,   193,
     194,   195,   193,   128,    94,     7,    95,    94,    94,    94,
      94,    94,    94,    94,    94,    94,    94,    94,    94,    94,
      94,    94,    94,     9,   116,   116,    62,   131,    94,    94,
      94,     5,    94,    94,    94,    94,    97,    94,    94,    94,
     135,   108,   111,   112,   127,    16,     8,    31,    40,    41,
      42,    43,    45,    46,    54,    95,   133,   141,   180,     9,
     210,   202,    96,    16,     9,    16,     9,   117,   119,    28,
     180,    67,   102,     5,    74,    75,   157,   158,   157,    36,
       3,     3,    61,    61,    53,    54,    80,    95,   133,   141,
     152,   153,     5,   150,   151,    99,    77,    77,   135,    34,
      96,    98,    45,    46,   140,   141,   142,   143,     5,   180,
     164,    12,   171,    16,   180,   180,   180,    99,    15,    64,
      65,    66,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    70,    79,    95,   127,    12,
      12,   172,    16,    20,     9,    97,    20,    29,    17,   123,
     124,   126,   130,   131,   134,   204,   135,    34,    30,    29,
     180,    98,   139,   129,    53,    95,   102,   162,   180,   183,
     188,    96,   193,   135,   147,   148,   139,    97,   131,    94,
      17,    57,    54,    94,    94,    94,    94,     5,    58,    58,
      58,    58,    58,    58,    46,    95,   141,   141,     7,    58,
      46,   142,     7,    58,   138,   138,   121,    96,     5,   121,
       5,   121,   133,   141,   180,    11,   133,   144,   154,   154,
      97,    16,   157,    16,   157,    39,    38,    54,   153,    54,
      80,   153,   153,    53,   153,    26,   142,   139,    96,    98,
       5,     8,    31,    61,   181,   182,    96,    96,    95,   201,
     180,   202,    46,   143,    60,   142,   139,    11,    28,   103,
     180,    13,    96,   181,   127,   180,   180,   180,   180,   180,
     180,   180,   180,   180,   180,   180,   180,   180,   180,   180,
     180,   127,    70,   210,    16,   180,   180,    20,   127,   175,
     133,   144,   127,   180,   139,    94,    94,    94,    94,   119,
     180,    30,   193,   162,     5,    96,   189,   190,    10,   162,
     177,   178,   180,   184,   185,   186,   187,    96,    99,   101,
      90,    12,   113,   115,    94,   127,   121,   133,   135,   135,
     135,   135,   135,   141,    46,   142,   138,   138,   180,   163,
     142,    96,   162,   163,   139,   139,   109,   109,   109,   139,
     139,   180,    68,   103,    68,   103,    26,   153,    35,    36,
      16,     3,     3,   153,   153,    26,   153,    53,    54,   153,
      96,   150,    95,    99,   202,    95,   103,   180,   180,    94,
      18,    19,   167,    16,   127,    96,    94,    94,    16,    12,
      16,   176,    16,    11,   127,   180,    34,    96,    98,     5,
      34,   104,   103,    98,   103,    98,    31,     3,   147,   180,
       9,   139,    16,    16,   138,   142,    96,   139,   139,    31,
      53,   146,    96,   138,   139,   110,   110,   110,   146,    33,
      33,    33,    33,    94,   139,    36,   146,   146,    54,   153,
     153,    53,   153,     5,   181,    96,   210,   103,   103,   171,
     180,   127,    16,    16,   172,   172,    20,   177,   180,    27,
      20,   180,    16,    53,   162,     5,   189,    34,   162,   177,
     184,   186,   101,    59,   120,     9,     5,     5,   139,    96,
     138,   147,    31,   139,   138,   139,    16,    16,    16,   153,
     153,   153,   153,    94,   153,   153,    96,    96,    15,    14,
      34,    20,   162,   162,     3,    17,    59,   120,    94,    94,
     138,   139,   139,     5,     5,     5,   127,   127,   127,   103,
     127,    17,   139,   167,    16,    70,   114,   127,    20,   127,
      16,   114,     5,    16,     5
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, Location); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
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


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Location data for the lookahead symbol.  */
YYLTYPE yylloc;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.
       `yyls': related to locations.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[2];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;
  yylsp = yyls;

#if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  /* Initialize the default location before parsing starts.  */
  yylloc.first_line   = yylloc.last_line   = 1;
  yylloc.first_column = yylloc.last_column = 1;
#endif

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;
	YYLTYPE *yyls1 = yyls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);

	yyls = yyls1;
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
	YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;
  *++yylsp = yylloc;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1464 of yacc.c  */
#line 226 "rihtan.y"
    { bool outer = OuterLevel;
               OuterLevel = FALSE;
               AnalyseProgram((yyvsp[(2) - (3)].elt), outer, (yyvsp[(1) - (3)].filecommentblock));
             ;}
    break;

  case 3:

/* Line 1464 of yacc.c  */
#line 231 "rihtan.y"
    { // separate shared package
               bool outer = OuterLevel;
               OuterLevel = FALSE;
               AnalyseProgram(SetPrefix((yyvsp[(3) - (4)].elt), PREFIX_SHARED, (yylsp[(2) - (4)]).first_line), outer, (yyvsp[(1) - (4)].filecommentblock));
             ;}
    break;

  case 4:

/* Line 1464 of yacc.c  */
#line 237 "rihtan.y"
    { // separate shared package
               bool outer = OuterLevel;
               OuterLevel = FALSE;
               AnalyseProgram(SetPrefix((yyvsp[(3) - (4)].elt), PREFIX_UNIT_TEST, (yylsp[(2) - (4)]).first_line), outer, (yyvsp[(1) - (4)].filecommentblock));
             ;}
    break;

  case 5:

/* Line 1464 of yacc.c  */
#line 243 "rihtan.y"
    { // separate shared package
               bool outer = OuterLevel;
               OuterLevel = FALSE;
               AnalyseProgram(SetPrefix((yyvsp[(4) - (5)].elt), PREFIX_NOT_UNIT_TEST, (yylsp[(2) - (5)]).first_line), outer, (yyvsp[(1) - (5)].filecommentblock));
             ;}
    break;

  case 6:

/* Line 1464 of yacc.c  */
#line 251 "rihtan.y"
    {
                          (yyval.filecommentblock) = NULL;
                        ;}
    break;

  case 7:

/* Line 1464 of yacc.c  */
#line 255 "rihtan.y"
    {
                          (yyval.filecommentblock) = AppendCommentLine((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].filecommentblock));
                        ;}
    break;

  case 8:

/* Line 1464 of yacc.c  */
#line 261 "rihtan.y"
    {
                        (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, PACKAGE_DECLARATION_ELT,
                                 MakePackage((yyloc).filename, (yyloc).first_line, PACKAGE_SYSTEM, (yyvsp[(2) - (8)].string), (yyvsp[(4) - (8)].elt), (yyvsp[(5) - (8)].elt), (yyvsp[(6) - (8)].elt), (yyvsp[(8) - (8)].string), NULL,
                                             (yylsp[(1) - (8)]).first_line, (yylsp[(2) - (8)]).last_line, (yylsp[(8) - (8)]).first_line, (yylsp[(8) - (8)]).last_line));
                      ;}
    break;

  case 9:

/* Line 1464 of yacc.c  */
#line 267 "rihtan.y"
    {
                        (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, PACKAGE_DECLARATION_ELT,
                                 MakePackage((yyloc).filename, (yyloc).first_line, PACKAGE_SUBSYSTEM, (yyvsp[(2) - (8)].string), (yyvsp[(4) - (8)].elt), (yyvsp[(5) - (8)].elt), (yyvsp[(6) - (8)].elt), (yyvsp[(8) - (8)].string), NULL,
                                             (yylsp[(1) - (8)]).first_line, (yylsp[(2) - (8)]).last_line, (yylsp[(8) - (8)]).first_line, (yylsp[(8) - (8)]).last_line));
                      ;}
    break;

  case 10:

/* Line 1464 of yacc.c  */
#line 273 "rihtan.y"
    {
                        (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, PACKAGE_DECLARATION_ELT,
                                 MakePackage((yyloc).filename, (yyloc).first_line, PACKAGE_SYSTEM, (yyvsp[(2) - (12)].string), (yyvsp[(8) - (12)].elt), (yyvsp[(9) - (12)].elt), (yyvsp[(10) - (12)].elt), (yyvsp[(12) - (12)].string), (yyvsp[(5) - (12)].representationclause),
                                             (yylsp[(1) - (12)]).first_line, (yylsp[(6) - (12)]).last_line, (yylsp[(12) - (12)]).first_line, (yylsp[(12) - (12)]).last_line));
                      ;}
    break;

  case 11:

/* Line 1464 of yacc.c  */
#line 279 "rihtan.y"
    {
                        (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, PACKAGE_DECLARATION_ELT,
                                 MakePackage((yyloc).filename, (yyloc).first_line, PACKAGE_SUBSYSTEM, (yyvsp[(2) - (12)].string), (yyvsp[(8) - (12)].elt), (yyvsp[(9) - (12)].elt), (yyvsp[(10) - (12)].elt), (yyvsp[(12) - (12)].string), (yyvsp[(5) - (12)].representationclause),
                                             (yylsp[(1) - (12)]).first_line, (yylsp[(6) - (12)]).last_line, (yylsp[(12) - (12)]).first_line, (yylsp[(12) - (12)]).last_line));
                      ;}
    break;

  case 12:

/* Line 1464 of yacc.c  */
#line 285 "rihtan.y"
    {
                        (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, PACKAGE_DECLARATION_ELT,
                                 MakePackage((yyloc).filename, (yyloc).first_line, PACKAGE_PACKAGE, (yyvsp[(2) - (8)].string), (yyvsp[(4) - (8)].elt), (yyvsp[(5) - (8)].elt), (yyvsp[(6) - (8)].elt), (yyvsp[(8) - (8)].string),
                                             NULL,
                                             (yylsp[(1) - (8)]).first_line, (yylsp[(2) - (8)]).last_line, (yylsp[(7) - (8)]).first_line, (yylsp[(8) - (8)]).last_line));
                      ;}
    break;

  case 13:

/* Line 1464 of yacc.c  */
#line 292 "rihtan.y"
    {
                        (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, PACKAGE_DECLARATION_ELT,
                                 MakePackage((yyloc).filename, (yyloc).first_line, PACKAGE_PACKAGE, (yyvsp[(2) - (12)].string), (yyvsp[(8) - (12)].elt), (yyvsp[(9) - (12)].elt), (yyvsp[(10) - (12)].elt), (yyvsp[(12) - (12)].string),
                                             (yyvsp[(5) - (12)].representationclause),
                                             (yylsp[(1) - (12)]).first_line, (yylsp[(2) - (12)]).last_line, (yylsp[(11) - (12)]).first_line, (yylsp[(12) - (12)]).last_line));
                      ;}
    break;

  case 14:

/* Line 1464 of yacc.c  */
#line 300 "rihtan.y"
    {
                        (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, GENERIC_INSTANTIATION_ELT,
                                 MakeGenericInstantiation(PACKAGE_PACKAGE, (yyvsp[(2) - (8)].string), (yyvsp[(5) - (8)].reference), (yyvsp[(7) - (8)].genericactualparam),
                                                          (yylsp[(1) - (8)]).first_line, (yylsp[(8) - (8)]).last_line));
                      ;}
    break;

  case 15:

/* Line 1464 of yacc.c  */
#line 306 "rihtan.y"
    {
                        (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, GENERIC_INSTANTIATION_ELT,
                                 MakeGenericInstantiation(PACKAGE_SUBSYSTEM, (yyvsp[(2) - (8)].string), (yyvsp[(5) - (8)].reference), (yyvsp[(7) - (8)].genericactualparam),
                                                          (yylsp[(1) - (8)]).first_line, (yylsp[(8) - (8)]).last_line));
                      ;}
    break;

  case 16:

/* Line 1464 of yacc.c  */
#line 312 "rihtan.y"
    {
                        (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, SEPARATE_DECLARATION_ELT,
                                 MakeSeparate(PACKAGE_PACKAGE, FALSE, (yyvsp[(2) - (4)].string), NULL));
                      ;}
    break;

  case 17:

/* Line 1464 of yacc.c  */
#line 317 "rihtan.y"
    {
                        (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, SEPARATE_DECLARATION_ELT,
                                 MakeSeparate(PACKAGE_SUBSYSTEM, FALSE, (yyvsp[(2) - (4)].string), NULL));
                      ;}
    break;

  case 18:

/* Line 1464 of yacc.c  */
#line 322 "rihtan.y"
    {
                        (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, SEPARATE_DECLARATION_ELT,
                                 MakeSeparate(PACKAGE_PACKAGE, FALSE, (yyvsp[(2) - (7)].string), (yyvsp[(6) - (7)].string)));
                      ;}
    break;

  case 19:

/* Line 1464 of yacc.c  */
#line 327 "rihtan.y"
    {
                        (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, SEPARATE_DECLARATION_ELT,
                                 MakeSeparate(PACKAGE_SUBSYSTEM, FALSE, (yyvsp[(2) - (7)].string), (yyvsp[(6) - (7)].string)));
                      ;}
    break;

  case 20:

/* Line 1464 of yacc.c  */
#line 332 "rihtan.y"
    {
                        (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, GENERIC_DECLARATION_ELT,
                                 MakeGeneric((yyvsp[(2) - (3)].genericparam), (yyvsp[(3) - (3)].elt)));
                      ;}
    break;

  case 21:

/* Line 1464 of yacc.c  */
#line 337 "rihtan.y"
    {
                        (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, SEPARATE_DECLARATION_ELT,
                                 MakeSeparate(PACKAGE_PACKAGE, TRUE, (yyvsp[(3) - (5)].string), NULL));
                      ;}
    break;

  case 22:

/* Line 1464 of yacc.c  */
#line 342 "rihtan.y"
    {
                        (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, SEPARATE_DECLARATION_ELT,
                                 MakeSeparate(PACKAGE_PACKAGE, TRUE, (yyvsp[(3) - (8)].string), (yyvsp[(7) - (8)].string)));
                      ;}
    break;

  case 23:

/* Line 1464 of yacc.c  */
#line 347 "rihtan.y"
    {
                        (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, SEPARATE_DECLARATION_ELT,
                                 MakeSeparate(PACKAGE_SUBSYSTEM, TRUE, (yyvsp[(3) - (5)].string), NULL));
                      ;}
    break;

  case 24:

/* Line 1464 of yacc.c  */
#line 352 "rihtan.y"
    {
                        (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, SEPARATE_DECLARATION_ELT,
                                 MakeSeparate(PACKAGE_SUBSYSTEM, TRUE, (yyvsp[(3) - (8)].string), (yyvsp[(7) - (8)].string)));
                      ;}
    break;

  case 25:

/* Line 1464 of yacc.c  */
#line 359 "rihtan.y"
    {
                           (yyval.elt) = NULL;
                         ;}
    break;

  case 26:

/* Line 1464 of yacc.c  */
#line 363 "rihtan.y"
    {
                           (yyval.elt) = (yyvsp[(2) - (2)].elt);
                         ;}
    break;

  case 27:

/* Line 1464 of yacc.c  */
#line 369 "rihtan.y"
    {
                         (yyval.elt) = NULL;
                       ;}
    break;

  case 28:

/* Line 1464 of yacc.c  */
#line 373 "rihtan.y"
    {
                         (yyval.elt) = (yyvsp[(2) - (2)].elt);
                       ;}
    break;

  case 29:

/* Line 1464 of yacc.c  */
#line 379 "rihtan.y"
    {
              (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, PROCEDURE_DECLARATION_ELT,
                       MakeProcedure((yyloc).filename, (yyloc).first_line, (yyvsp[(2) - (12)].string), (yyvsp[(3) - (12)].elt), NULL, (yyvsp[(7) - (12)].elt), (yyvsp[(9) - (12)].elt), (yyvsp[(12) - (12)].string), (yyvsp[(5) - (12)].shareclause), (yyvsp[(4) - (12)].representationclause), (yyvsp[(10) - (12)].elt), (yylsp[(5) - (12)]).last_line));
            ;}
    break;

  case 30:

/* Line 1464 of yacc.c  */
#line 384 "rihtan.y"
    {
              (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, PROCEDURE_DECLARATION_ELT,
                       MakeProcedure((yyloc).filename, (yyloc).first_line, (yyvsp[(2) - (7)].string), (yyvsp[(3) - (7)].elt), NULL, NULL, NULL, (yyvsp[(2) - (7)].string), MakeShareClause(NULL),
                                     (yyvsp[(4) - (7)].representationclause), NULL, (yylsp[(7) - (7)]).last_line));
            ;}
    break;

  case 31:

/* Line 1464 of yacc.c  */
#line 392 "rihtan.y"
    {
              (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, PROCEDURE_DECLARATION_ELT,
                       MakeProcedure((yyloc).filename, (yyloc).first_line, (yyvsp[(2) - (13)].string), (yyvsp[(3) - (13)].elt), SetVarName((yyvsp[(5) - (13)].elt), (yyvsp[(2) - (13)].string)), (yyvsp[(8) - (13)].elt), (yyvsp[(10) - (13)].elt), (yyvsp[(13) - (13)].string),
                                     MakeShareClause(NULL), (yyvsp[(6) - (13)].representationclause), (yyvsp[(11) - (13)].elt), (yylsp[(6) - (13)]).last_line));
            ;}
    break;

  case 32:

/* Line 1464 of yacc.c  */
#line 398 "rihtan.y"
    {
              (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, PROCEDURE_DECLARATION_ELT,
                       MakeProcedure((yyloc).filename, (yyloc).first_line, (yyvsp[(2) - (8)].string), (yyvsp[(3) - (8)].elt), SetVarName((yyvsp[(5) - (8)].elt), (yyvsp[(2) - (8)].string)), NULL, NULL, (yyvsp[(2) - (8)].string),
                                     MakeShareClause(NULL), (yyvsp[(6) - (8)].representationclause), NULL, (yylsp[(8) - (8)]).last_line));
            ;}
    break;

  case 33:

/* Line 1464 of yacc.c  */
#line 406 "rihtan.y"
    {
                         (yyval.shareclause) = MakeShareClause(/*FALSE,*/ NULL);
                       ;}
    break;

  case 34:

/* Line 1464 of yacc.c  */
#line 416 "rihtan.y"
    {
                         (yyval.shareclause) = MakeShareClause(/*FALSE,*/ (yyvsp[(2) - (2)].node));
                       ;}
    break;

  case 35:

/* Line 1464 of yacc.c  */
#line 422 "rihtan.y"
    {
                       (yyval.elt) = NULL;
                     ;}
    break;

  case 36:

/* Line 1464 of yacc.c  */
#line 426 "rihtan.y"
    {
                       /* The prefix is not strictly required, but adjusting the line is */
                       (yyval.elt) = SetPrefix((yyvsp[(2) - (2)].elt), PREFIX_UNIT_TEST, (yyloc).first_line);
                     ;}
    break;

  case 37:

/* Line 1464 of yacc.c  */
#line 433 "rihtan.y"
    {
                         (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, VAR_DECLARATION_ELT,
                                  DeclareVariable(NULL, (yyvsp[(1) - (1)].typeidentifier), NULL, NULL, NULL, MODE_LOCAL,
                                                  NULL, NAME_NONE));
                       ;}
    break;

  case 38:

/* Line 1464 of yacc.c  */
#line 439 "rihtan.y"
    {
                         (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, VAR_DECLARATION_ELT,
                                  DeclareVariable(NULL, NULL, SetRepresentation((yyvsp[(1) - (1)].typespec), NULL), NULL, NULL, MODE_LOCAL,
                                                  NULL, NAME_NONE));
                       ;}
    break;

  case 39:

/* Line 1464 of yacc.c  */
#line 447 "rihtan.y"
    {
                      (yyval.elt) = NULL;
                    ;}
    break;

  case 40:

/* Line 1464 of yacc.c  */
#line 451 "rihtan.y"
    {
                      (yyval.elt) = (yyvsp[(2) - (3)].elt);
                    ;}
    break;

  case 41:

/* Line 1464 of yacc.c  */
#line 457 "rihtan.y"
    {
                          (yyval.elt) = (yyvsp[(1) - (1)].elt);
                        ;}
    break;

  case 42:

/* Line 1464 of yacc.c  */
#line 461 "rihtan.y"
    {
                          (yyval.elt) = AppendStmts((yyvsp[(1) - (3)].elt), (yyvsp[(3) - (3)].elt));
                        ;}
    break;

  case 43:

/* Line 1464 of yacc.c  */
#line 467 "rihtan.y"
    {
                     (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, VAR_DECLARATION_ELT,
                              DeclareVariable((yyvsp[(1) - (5)].identifierlistentry), (yyvsp[(4) - (5)].typeidentifier), NULL, NULL, NULL, (yyvsp[(3) - (5)].varmode), (yyvsp[(5) - (5)].representationclause), NAME_NONE));
                   ;}
    break;

  case 44:

/* Line 1464 of yacc.c  */
#line 472 "rihtan.y"
    {
                     (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, VAR_DECLARATION_ELT,
                              DeclareVariable((yyvsp[(1) - (5)].identifierlistentry), NULL, SetRepresentation((yyvsp[(4) - (5)].typespec), NULL), NULL, NULL, (yyvsp[(3) - (5)].varmode), (yyvsp[(5) - (5)].representationclause), NAME_NONE));
                   ;}
    break;

  case 45:

/* Line 1464 of yacc.c  */
#line 479 "rihtan.y"
    {
              (yyval.varmode) = MODE_IN;
            ;}
    break;

  case 46:

/* Line 1464 of yacc.c  */
#line 483 "rihtan.y"
    {
              (yyval.varmode) = MODE_OUT;
            ;}
    break;

  case 47:

/* Line 1464 of yacc.c  */
#line 487 "rihtan.y"
    {
              (yyval.varmode) = MODE_IN_OUT;
            ;}
    break;

  case 48:

/* Line 1464 of yacc.c  */
#line 491 "rihtan.y"
    {
              (yyval.varmode) = MODE_FINAL_IN_OUT;
            ;}
    break;

  case 49:

/* Line 1464 of yacc.c  */
#line 497 "rihtan.y"
    {
                     (yyval.elt) = NULL;
                   ;}
    break;

  case 50:

/* Line 1464 of yacc.c  */
#line 501 "rihtan.y"
    {
                     (yyval.elt) = AppendStmts((yyvsp[(1) - (2)].elt), (yyvsp[(2) - (2)].elt));
                   ;}
    break;

  case 51:

/* Line 1464 of yacc.c  */
#line 507 "rihtan.y"
    {
                     (yyval.elt) = NULL;
                   ;}
    break;

  case 52:

/* Line 1464 of yacc.c  */
#line 511 "rihtan.y"
    {
                     (yyval.elt) = AppendStmts((yyvsp[(1) - (2)].elt), (yyvsp[(2) - (2)].elt));
                   ;}
    break;

  case 53:

/* Line 1464 of yacc.c  */
#line 517 "rihtan.y"
    {
                        (yyval.elt) = SetAccess((yyvsp[(1) - (1)].elt), PRIVATE_ACCESS);
                      ;}
    break;

  case 54:

/* Line 1464 of yacc.c  */
#line 521 "rihtan.y"
    {
                        (yyval.elt) = SetAccess((yyvsp[(2) - (2)].elt), PUBLIC_ACCESS);
                      ;}
    break;

  case 55:

/* Line 1464 of yacc.c  */
#line 525 "rihtan.y"
    {
                        (yyval.elt) = SetAccess(SetMode(SetPublicOut((yyvsp[(3) - (4)].elt)), MODE_GLOBAL), PUBLIC_ACCESS);
                      ;}
    break;

  case 56:

/* Line 1464 of yacc.c  */
#line 529 "rihtan.y"
    {
                        (yyval.elt) = SetAccess(SetMode(SetPublicOut((yyvsp[(4) - (5)].elt)), MODE_SHARED), PUBLIC_ACCESS);
                      ;}
    break;

  case 57:

/* Line 1464 of yacc.c  */
#line 533 "rihtan.y"
    {
                        (yyval.elt) = (yyvsp[(1) - (1)].elt);
                      ;}
    break;

  case 58:

/* Line 1464 of yacc.c  */
#line 537 "rihtan.y"
    {
                        (yyval.elt) = (yyvsp[(1) - (2)].elt);
                      ;}
    break;

  case 59:

/* Line 1464 of yacc.c  */
#line 541 "rihtan.y"
    {
                        (yyval.elt) = (yyvsp[(1) - (1)].elt);
                      ;}
    break;

  case 60:

/* Line 1464 of yacc.c  */
#line 545 "rihtan.y"
    {
                        (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line,
                                 SEPARATE_BLOCK_ELT, MakeSeparateBlock((yyloc).filename, (yyloc).first_line, (yyvsp[(3) - (8)].string), (yyvsp[(5) - (8)].elt), (yyvsp[(7) - (8)].string)));
                      ;}
    break;

  case 61:

/* Line 1464 of yacc.c  */
#line 550 "rihtan.y"
    {
                        (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line,
                                 SEPARATE_BLOCK_ELT, MakeSeparateBlock((yyloc).filename, (yyloc).first_line, (yyvsp[(3) - (8)].string), (yyvsp[(5) - (8)].elt), (yyvsp[(7) - (8)].string)));
                      ;}
    break;

  case 62:

/* Line 1464 of yacc.c  */
#line 555 "rihtan.y"
    {
                        (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line,
                                 SEPARATE_STMT_ELT, MakeSeparateStmt((yyvsp[(3) - (4)].reference)));
                      ;}
    break;

  case 63:

/* Line 1464 of yacc.c  */
#line 562 "rihtan.y"
    {
              (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, CODE_ELT, (yyvsp[(1) - (1)].string));
            ;}
    break;

  case 64:

/* Line 1464 of yacc.c  */
#line 566 "rihtan.y"
    {
              (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, HEADER_CODE_ELT, (yyvsp[(1) - (1)].string));
            ;}
    break;

  case 65:

/* Line 1464 of yacc.c  */
#line 572 "rihtan.y"
    {
                 (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, COMMENT_ELT, (yyvsp[(1) - (1)].string));
               ;}
    break;

  case 66:

/* Line 1464 of yacc.c  */
#line 579 "rihtan.y"
    {
                (yyval.elt) = (yyvsp[(1) - (2)].elt);
              ;}
    break;

  case 67:

/* Line 1464 of yacc.c  */
#line 583 "rihtan.y"
    {
                (yyval.elt) = SetMode((yyvsp[(1) - (2)].elt), MODE_GLOBAL);
              ;}
    break;

  case 68:

/* Line 1464 of yacc.c  */
#line 587 "rihtan.y"
    {
                (yyval.elt) = SetMode((yyvsp[(2) - (3)].elt), MODE_SHARED);
              ;}
    break;

  case 69:

/* Line 1464 of yacc.c  */
#line 591 "rihtan.y"
    {
                (yyval.elt) = (yyvsp[(1) - (2)].elt);
              ;}
    break;

  case 70:

/* Line 1464 of yacc.c  */
#line 595 "rihtan.y"
    {
                (yyval.elt) = (yyvsp[(1) - (2)].elt);
              ;}
    break;

  case 71:

/* Line 1464 of yacc.c  */
#line 599 "rihtan.y"
    {
                (yyval.elt) = SetPrefix((yyvsp[(2) - (3)].elt), PREFIX_CLOSED, (yyloc).first_line);
              ;}
    break;

  case 72:

/* Line 1464 of yacc.c  */
#line 603 "rihtan.y"
    {
                (yyval.elt) = SetPrefix((yyvsp[(2) - (3)].elt), PREFIX_SHARED, (yyloc).first_line);
              ;}
    break;

  case 73:

/* Line 1464 of yacc.c  */
#line 607 "rihtan.y"
    {
                (yyval.elt) = SetPrefix((yyvsp[(2) - (3)].elt), PREFIX_MAIN, (yyloc).first_line);
              ;}
    break;

  case 74:

/* Line 1464 of yacc.c  */
#line 611 "rihtan.y"
    {
                (yyval.elt) = SetPrefix((yyvsp[(2) - (3)].elt), PREFIX_UNIT_TEST, (yyloc).first_line);
              ;}
    break;

  case 75:

/* Line 1464 of yacc.c  */
#line 615 "rihtan.y"
    {
                (yyval.elt) = SetPrefix((yyvsp[(3) - (4)].elt), PREFIX_NOT_UNIT_TEST, (yyloc).first_line);
              ;}
    break;

  case 76:

/* Line 1464 of yacc.c  */
#line 619 "rihtan.y"
    {
                (yyval.elt) = (yyvsp[(1) - (2)].elt);
              ;}
    break;

  case 77:

/* Line 1464 of yacc.c  */
#line 623 "rihtan.y"
    {
                (yyval.elt) = SetPrefix((yyvsp[(2) - (3)].elt), PREFIX_CLOSED, (yyloc).first_line);
              ;}
    break;

  case 78:

/* Line 1464 of yacc.c  */
#line 627 "rihtan.y"
    {
                (yyval.elt) = SetPrefix((yyvsp[(2) - (3)].elt), PREFIX_SHARED, (yyloc).first_line);
              ;}
    break;

  case 79:

/* Line 1464 of yacc.c  */
#line 631 "rihtan.y"
    {
                (yyval.elt) = SetPrefix((yyvsp[(2) - (3)].elt), PREFIX_UNIT_TEST, (yyloc).first_line);
              ;}
    break;

  case 80:

/* Line 1464 of yacc.c  */
#line 635 "rihtan.y"
    {
                (yyval.elt) = SetPrefix((yyvsp[(3) - (4)].elt), PREFIX_NOT_UNIT_TEST, (yyloc).first_line);
              ;}
    break;

  case 81:

/* Line 1464 of yacc.c  */
#line 639 "rihtan.y"
    {
                (yyval.elt) = (yyvsp[(1) - (2)].elt);
              ;}
    break;

  case 82:

/* Line 1464 of yacc.c  */
#line 643 "rihtan.y"
    {
                (yyval.elt) = SetPrefix((yyvsp[(2) - (3)].elt), PREFIX_SHARED, (yyloc).first_line);
              ;}
    break;

  case 83:

/* Line 1464 of yacc.c  */
#line 647 "rihtan.y"
    {
                (yyval.elt) = SetPrefix((yyvsp[(2) - (3)].elt), PREFIX_UNIT_TEST, (yyloc).first_line);
              ;}
    break;

  case 84:

/* Line 1464 of yacc.c  */
#line 651 "rihtan.y"
    {
                (yyval.elt) = SetPrefix((yyvsp[(3) - (4)].elt), PREFIX_NOT_UNIT_TEST, (yyloc).first_line);
              ;}
    break;

  case 85:

/* Line 1464 of yacc.c  */
#line 655 "rihtan.y"
    {
                (yyval.elt) = (yyvsp[(1) - (2)].elt);
              ;}
    break;

  case 86:

/* Line 1464 of yacc.c  */
#line 661 "rihtan.y"
    {
                          (yyval.elt) = (yyvsp[(1) - (2)].elt);
                        ;}
    break;

  case 87:

/* Line 1464 of yacc.c  */
#line 665 "rihtan.y"
    {
                          (yyval.elt) = (yyvsp[(1) - (2)].elt); // The modes are set for local by default; allow in out for 'name for'
                        ;}
    break;

  case 88:

/* Line 1464 of yacc.c  */
#line 669 "rihtan.y"
    {
                          (yyval.elt) = (yyvsp[(1) - (2)].elt);
                        ;}
    break;

  case 89:

/* Line 1464 of yacc.c  */
#line 673 "rihtan.y"
    {
                          (yyval.elt) = (yyvsp[(1) - (2)].elt);
                        ;}
    break;

  case 90:

/* Line 1464 of yacc.c  */
#line 677 "rihtan.y"
    {
                          (yyval.elt) = (yyvsp[(1) - (1)].elt);
                        ;}
    break;

  case 91:

/* Line 1464 of yacc.c  */
#line 681 "rihtan.y"
    {
                          (yyval.elt) = (yyvsp[(1) - (1)].elt);
                        ;}
    break;

  case 92:

/* Line 1464 of yacc.c  */
#line 688 "rihtan.y"
    {
              (yyval.elt) = AppendStmts((yyvsp[(1) - (2)].elt), (yyvsp[(2) - (2)].elt));
            ;}
    break;

  case 93:

/* Line 1464 of yacc.c  */
#line 694 "rihtan.y"
    {
               (yyval.elt) = NULL;
             ;}
    break;

  case 94:

/* Line 1464 of yacc.c  */
#line 698 "rihtan.y"
    {
               (yyval.elt) = AppendStmts((yyvsp[(1) - (2)].elt), (yyvsp[(2) - (2)].elt));
             ;}
    break;

  case 114:

/* Line 1464 of yacc.c  */
#line 725 "rihtan.y"
    {
                      (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, TYPE_DECLARATION_ELT,
                               AddType((yyvsp[(2) - (2)].string), NULL));
                   ;}
    break;

  case 115:

/* Line 1464 of yacc.c  */
#line 730 "rihtan.y"
    {
                      (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, TYPE_DECLARATION_ELT,
                               AddType((yyvsp[(2) - (4)].string), (yyvsp[(4) - (4)].typespec)));
                   ;}
    break;

  case 116:

/* Line 1464 of yacc.c  */
#line 735 "rihtan.y"
    {
                     (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, APPEND_REPRESENTATION_ELT, AddRepresentation((yyvsp[(2) - (8)].string), (yyvsp[(7) - (8)].representationclause)));
                   ;}
    break;

  case 117:

/* Line 1464 of yacc.c  */
#line 741 "rihtan.y"
    {
                    (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, VAR_DECLARATION_ELT,
                             DeclareVariable((yyvsp[(1) - (5)].identifierlistentry), (yyvsp[(3) - (5)].typeidentifier), NULL, (yyvsp[(4) - (5)].rhs), NULL, MODE_LOCAL, (yyvsp[(5) - (5)].representationclause), NAME_NONE));
                  ;}
    break;

  case 118:

/* Line 1464 of yacc.c  */
#line 746 "rihtan.y"
    {
                    (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, VAR_DECLARATION_ELT,
                             DeclareVariable((yyvsp[(1) - (6)].identifierlistentry), (yyvsp[(3) - (6)].typeidentifier), NULL, NULL, (yyvsp[(5) - (6)].reference), MODE_IN_OUT, (yyvsp[(6) - (6)].representationclause), NAME_VAR));
                  ;}
    break;

  case 119:

/* Line 1464 of yacc.c  */
#line 751 "rihtan.y"
    {
                    (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, VAR_DECLARATION_ELT,
                             DeclareVariable((yyvsp[(1) - (5)].identifierlistentry), NULL, NULL, NULL, (yyvsp[(5) - (5)].reference), MODE_IN_OUT, NULL, NAME_PACKAGE));
                  ;}
    break;

  case 120:

/* Line 1464 of yacc.c  */
#line 756 "rihtan.y"
    {
                    (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, VAR_DECLARATION_ELT,
                             DeclareVariable((yyvsp[(1) - (5)].identifierlistentry), NULL, NULL, NULL, (yyvsp[(5) - (5)].reference), MODE_IN_OUT, NULL, NAME_PACKAGE));
                  ;}
    break;

  case 121:

/* Line 1464 of yacc.c  */
#line 761 "rihtan.y"
    {
                    (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, VAR_DECLARATION_ELT,
                             DeclareVariable((yyvsp[(1) - (5)].identifierlistentry), NULL, NULL, NULL, (yyvsp[(5) - (5)].reference), MODE_IN_OUT, NULL, NAME_PROCEDURE));
                  ;}
    break;

  case 122:

/* Line 1464 of yacc.c  */
#line 766 "rihtan.y"
    {
                    (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, VAR_DECLARATION_ELT,
                             DeclareVariable((yyvsp[(1) - (5)].identifierlistentry), NULL, NULL, NULL, (yyvsp[(5) - (5)].reference), MODE_IN_OUT, NULL, NAME_FUNCTION));
                  ;}
    break;

  case 123:

/* Line 1464 of yacc.c  */
#line 771 "rihtan.y"
    {
                    (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, VAR_DECLARATION_ELT,
                             DeclareVariable((yyvsp[(1) - (5)].identifierlistentry), (yyvsp[(5) - (5)].typeidentifier), NULL, NULL, NULL, MODE_IN_OUT, NULL, NAME_TYPE));
                  ;}
    break;

  case 124:

/* Line 1464 of yacc.c  */
#line 776 "rihtan.y"
    {
                    (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, VAR_DECLARATION_ELT,
                             DeclareVariable((yyvsp[(1) - (5)].identifierlistentry), NULL, NULL, NULL, (yyvsp[(5) - (5)].reference), MODE_IN_OUT, NULL, NAME_UNIT));
                  ;}
    break;

  case 125:

/* Line 1464 of yacc.c  */
#line 781 "rihtan.y"
    {
                    (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, VAR_DECLARATION_ELT,
                             DeclareVariable((yyvsp[(1) - (5)].identifierlistentry), NULL, NULL, NULL, (yyvsp[(5) - (5)].reference), MODE_IN_OUT, NULL, NAME_CONSTANT));
                  ;}
    break;

  case 126:

/* Line 1464 of yacc.c  */
#line 787 "rihtan.y"
    {
                    (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, VAR_DECLARATION_ELT,
                             DeclareVariable((yyvsp[(1) - (5)].identifierlistentry), NULL, SetRepresentation((yyvsp[(3) - (5)].typespec), NULL), (yyvsp[(4) - (5)].rhs), NULL,
                                             MODE_LOCAL, (yyvsp[(5) - (5)].representationclause), NAME_NONE));
                  ;}
    break;

  case 127:

/* Line 1464 of yacc.c  */
#line 793 "rihtan.y"
    {
                    (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, VAR_DECLARATION_ELT,
                             DeclareVariable((yyvsp[(1) - (6)].identifierlistentry), NULL, SetControlled(SetRepresentation((yyvsp[(4) - (6)].typespec), NULL)), (yyvsp[(5) - (6)].rhs), NULL,
                                             MODE_LOCAL, (yyvsp[(6) - (6)].representationclause), NAME_NONE));
                  ;}
    break;

  case 128:

/* Line 1464 of yacc.c  */
#line 799 "rihtan.y"
    {
                    (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, VAR_DECLARATION_ELT,
                             DeclareVariable((yyvsp[(1) - (7)].identifierlistentry), NULL, (yyvsp[(4) - (7)].typespec), (yyvsp[(6) - (7)].rhs), NULL, MODE_LOCAL, (yyvsp[(7) - (7)].representationclause), NAME_NONE));
                  ;}
    break;

  case 129:

/* Line 1464 of yacc.c  */
#line 804 "rihtan.y"
    {
                    (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, VAR_DECLARATION_ELT,
                             DeclareVariable((yyvsp[(1) - (8)].identifierlistentry), NULL, SetControlled((yyvsp[(5) - (8)].typespec)), (yyvsp[(7) - (8)].rhs), NULL, MODE_LOCAL, (yyvsp[(8) - (8)].representationclause),
                                             NAME_NONE));
                  ;}
    break;

  case 130:

/* Line 1464 of yacc.c  */
#line 811 "rihtan.y"
    {
                    (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, VAR_DECLARATION_ELT,
                             DeclareVariable((yyvsp[(1) - (6)].identifierlistentry), NULL,
                                             SetTypeSpecAccess(SetRepresentation((yyvsp[(4) - (6)].typespec), NULL), PUBLIC_ACCESS),
                                             (yyvsp[(5) - (6)].rhs), NULL, MODE_LOCAL, (yyvsp[(6) - (6)].representationclause), NAME_NONE));
                  ;}
    break;

  case 131:

/* Line 1464 of yacc.c  */
#line 818 "rihtan.y"
    {
                    (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, VAR_DECLARATION_ELT,
                             DeclareVariable(
                               (yyvsp[(1) - (7)].identifierlistentry), NULL,
                               SetTypeSpecAccess(SetControlled(SetRepresentation((yyvsp[(5) - (7)].typespec), NULL)), PUBLIC_ACCESS),
                               (yyvsp[(6) - (7)].rhs), NULL, MODE_LOCAL, (yyvsp[(7) - (7)].representationclause), NAME_NONE));
                  ;}
    break;

  case 132:

/* Line 1464 of yacc.c  */
#line 826 "rihtan.y"
    {
                    (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, VAR_DECLARATION_ELT,
                             DeclareVariable(
                               (yyvsp[(1) - (8)].identifierlistentry), NULL,
                               SetTypeSpecAccess((yyvsp[(5) - (8)].typespec), PUBLIC_ACCESS), (yyvsp[(7) - (8)].rhs), NULL, MODE_LOCAL, (yyvsp[(8) - (8)].representationclause), NAME_NONE));
                  ;}
    break;

  case 133:

/* Line 1464 of yacc.c  */
#line 834 "rihtan.y"
    {
                    (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, VAR_DECLARATION_ELT,
                             DeclareVariable((yyvsp[(1) - (9)].identifierlistentry), NULL, SetTypeSpecAccess(SetControlled((yyvsp[(6) - (9)].typespec)), PUBLIC_ACCESS),
                                             (yyvsp[(8) - (9)].rhs), NULL, MODE_LOCAL, (yyvsp[(9) - (9)].representationclause), NAME_NONE));
                  ;}
    break;

  case 134:

/* Line 1464 of yacc.c  */
#line 842 "rihtan.y"
    {
                    (yyval.identifierlistentry) = MakeIdentifierListEntry((yyvsp[(1) - (1)].string));
                  ;}
    break;

  case 135:

/* Line 1464 of yacc.c  */
#line 846 "rihtan.y"
    {
                    (yyval.identifierlistentry) = PrefixIdentifierListEntry(MakeIdentifierListEntry((yyvsp[(1) - (3)].string)), (yyvsp[(3) - (3)].identifierlistentry));
                  ;}
    break;

  case 136:

/* Line 1464 of yacc.c  */
#line 852 "rihtan.y"
    {
                    (yyval.typeidentifier) = MakeTypeIdentifier((yyvsp[(1) - (1)].reference), NULL);
                  ;}
    break;

  case 137:

/* Line 1464 of yacc.c  */
#line 856 "rihtan.y"
    {
                    (yyval.typeidentifier) = MakeTypeIdentifier((yyvsp[(1) - (3)].reference), (yyvsp[(3) - (3)].attributechain));
                  ;}
    break;

  case 138:

/* Line 1464 of yacc.c  */
#line 862 "rihtan.y"
    {
                     (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, UNIT_DECLARATION_ELT,
                              DeclareUnit((yyloc).first_line, (yyvsp[(2) - (2)].string), NULL));
                   ;}
    break;

  case 139:

/* Line 1464 of yacc.c  */
#line 867 "rihtan.y"
    {
                     (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, UNIT_DECLARATION_ELT,
                              DeclareUnit((yyloc).first_line, (yyvsp[(2) - (4)].string), (yyvsp[(4) - (4)].units)));
                   ;}
    break;

  case 140:

/* Line 1464 of yacc.c  */
#line 874 "rihtan.y"
    {
                       (yyval.reference) = MakeReference(IDENTIFIER_REFERENCE, (yyvsp[(1) - (1)].string), NULL, NULL);
                     ;}
    break;

  case 141:

/* Line 1464 of yacc.c  */
#line 878 "rihtan.y"
    {
                       (yyval.reference) = PrefixReference(
                              MakeReference(IDENTIFIER_REFERENCE, (yyvsp[(1) - (2)].string), NULL, NULL), (yyvsp[(2) - (2)].reference));
                     ;}
    break;

  case 142:

/* Line 1464 of yacc.c  */
#line 885 "rihtan.y"
    {
                     (yyval.reference) = (yyvsp[(1) - (1)].reference);
                   ;}
    break;

  case 143:

/* Line 1464 of yacc.c  */
#line 889 "rihtan.y"
    {
                     (yyval.reference) = PrefixReference((yyvsp[(1) - (2)].reference), (yyvsp[(2) - (2)].reference));
                   ;}
    break;

  case 144:

/* Line 1464 of yacc.c  */
#line 895 "rihtan.y"
    {
                     (yyval.reference) = MakeReference(FIELD_REFERENCE, (yyvsp[(2) - (2)].string), NULL, NULL);
                   ;}
    break;

  case 145:

/* Line 1464 of yacc.c  */
#line 901 "rihtan.y"
    {
                   (yyval.rhs) = NULL;
                 ;}
    break;

  case 146:

/* Line 1464 of yacc.c  */
#line 905 "rihtan.y"
    {
                   (yyval.rhs) = (yyvsp[(2) - (2)].rhs);
                 ;}
    break;

  case 147:

/* Line 1464 of yacc.c  */
#line 911 "rihtan.y"
    {
                            (yyval.representationclause) = NULL;
                          ;}
    break;

  case 148:

/* Line 1464 of yacc.c  */
#line 915 "rihtan.y"
    {
                            (yyval.representationclause) = (yyvsp[(3) - (4)].representationclause);
                          ;}
    break;

  case 149:

/* Line 1464 of yacc.c  */
#line 921 "rihtan.y"
    {
                       (yyval.typespec) = (yyvsp[(1) - (1)].typespec);
                    ;}
    break;

  case 150:

/* Line 1464 of yacc.c  */
#line 925 "rihtan.y"
    {
                       (yyval.typespec) = SetTypeSpecAccess((yyvsp[(2) - (2)].typespec), PUBLIC_ACCESS);
                    ;}
    break;

  case 151:

/* Line 1464 of yacc.c  */
#line 931 "rihtan.y"
    {
              (yyval.typespec) = (yyvsp[(1) - (1)].typespec);
            ;}
    break;

  case 152:

/* Line 1464 of yacc.c  */
#line 935 "rihtan.y"
    {
              (yyval.typespec) = (yyvsp[(1) - (1)].typespec);
            ;}
    break;

  case 153:

/* Line 1464 of yacc.c  */
#line 939 "rihtan.y"
    {
              (yyval.typespec) = (yyvsp[(1) - (1)].typespec);
            ;}
    break;

  case 154:

/* Line 1464 of yacc.c  */
#line 943 "rihtan.y"
    {
              (yyval.typespec) = (yyvsp[(1) - (1)].typespec);
            ;}
    break;

  case 155:

/* Line 1464 of yacc.c  */
#line 947 "rihtan.y"
    {
              (yyval.typespec) = (yyvsp[(1) - (1)].typespec);
            ;}
    break;

  case 156:

/* Line 1464 of yacc.c  */
#line 951 "rihtan.y"
    {
              (yyval.typespec) = (yyvsp[(1) - (1)].typespec);
            ;}
    break;

  case 157:

/* Line 1464 of yacc.c  */
#line 957 "rihtan.y"
    {
                       (yyval.typespec) = SetRepresentation((yyvsp[(1) - (2)].typespec), (yyvsp[(2) - (2)].representationclause));
                     ;}
    break;

  case 158:

/* Line 1464 of yacc.c  */
#line 963 "rihtan.y"
    {
              (yyval.typespec) = (yyvsp[(1) - (1)].typespec);
            ;}
    break;

  case 159:

/* Line 1464 of yacc.c  */
#line 967 "rihtan.y"
    {
              (yyval.typespec) = SetControlled((yyvsp[(2) - (2)].typespec));
            ;}
    break;

  case 160:

/* Line 1464 of yacc.c  */
#line 974 "rihtan.y"
    {
              (yyval.typespec) = MakeIntegerType((yyloc).first_line, (yyvsp[(2) - (5)].node), (yyvsp[(4) - (5)].node), (yyvsp[(5) - (5)].units));
            ;}
    break;

  case 161:

/* Line 1464 of yacc.c  */
#line 978 "rihtan.y"
    {
              (yyval.typespec) = MakeIntegerType((yyloc).first_line, NULL, (yyvsp[(3) - (3)].node), NULL);
            ;}
    break;

  case 162:

/* Line 1464 of yacc.c  */
#line 995 "rihtan.y"
    {
                       (yyval.typespec) = MakeFloatingType((yyloc).first_line, (yyvsp[(2) - (5)].integer), (yyvsp[(4) - (5)].integer), (yyvsp[(5) - (5)].units));
                     ;}
    break;

  case 163:

/* Line 1464 of yacc.c  */
#line 999 "rihtan.y"
    {
                       (yyval.typespec) = MakeFloatingType((yyloc).first_line, (yyvsp[(4) - (5)].integer), (yyvsp[(2) - (5)].integer), (yyvsp[(5) - (5)].units));
                     ;}
    break;

  case 164:

/* Line 1464 of yacc.c  */
#line 1005 "rihtan.y"
    {
              (yyval.units) = NULL;
            ;}
    break;

  case 165:

/* Line 1464 of yacc.c  */
#line 1009 "rihtan.y"
    {
              (yyval.units) = (yyvsp[(2) - (2)].units);
            ;}
    break;

  case 166:

/* Line 1464 of yacc.c  */
#line 1013 "rihtan.y"
    {
              (yyval.units) = MakeUnitTerm(NULL, 1, FALSE);
            ;}
    break;

  case 167:

/* Line 1464 of yacc.c  */
#line 1019 "rihtan.y"
    {
              (yyval.units) = (yyvsp[(1) - (1)].units);
            ;}
    break;

  case 168:

/* Line 1464 of yacc.c  */
#line 1023 "rihtan.y"
    {
              (yyval.units) = PrefixUnitTerm((yyvsp[(1) - (3)].units), (yyvsp[(3) - (3)].units));
            ;}
    break;

  case 169:

/* Line 1464 of yacc.c  */
#line 1029 "rihtan.y"
    {
         (yyval.units) = MakeUnitTerm((yyvsp[(1) - (1)].reference), 1, FALSE);
       ;}
    break;

  case 170:

/* Line 1464 of yacc.c  */
#line 1033 "rihtan.y"
    {
         (yyval.units) = MakeUnitTerm((yyvsp[(1) - (3)].reference), (yyvsp[(3) - (3)].integer), FALSE);
       ;}
    break;

  case 171:

/* Line 1464 of yacc.c  */
#line 1037 "rihtan.y"
    {
         (yyval.units) = MakeUnitTerm((yyvsp[(1) - (3)].reference), 1, TRUE);
       ;}
    break;

  case 172:

/* Line 1464 of yacc.c  */
#line 1041 "rihtan.y"
    {
         (yyval.units) = MakeUnitTerm((yyvsp[(1) - (5)].reference), (yyvsp[(5) - (5)].integer), TRUE);
       ;}
    break;

  case 173:

/* Line 1464 of yacc.c  */
#line 1047 "rihtan.y"
    {
                          (yyval.typespec) = MakeEnumType((yyloc).first_line, (yyvsp[(2) - (3)].enumtype));
                        ;}
    break;

  case 174:

/* Line 1464 of yacc.c  */
#line 1053 "rihtan.y"
    {
                     (yyval.enumtype) = (yyvsp[(1) - (1)].enumtype);
                   ;}
    break;

  case 175:

/* Line 1464 of yacc.c  */
#line 1057 "rihtan.y"
    {
                     (yyval.enumtype) = PrefixEnumSymbol((yyvsp[(1) - (3)].enumtype), (yyvsp[(3) - (3)].enumtype));
                   ;}
    break;

  case 176:

/* Line 1464 of yacc.c  */
#line 1063 "rihtan.y"
    {
             (yyval.enumtype) = MakeEnumSymbol((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].representationclause));
           ;}
    break;

  case 177:

/* Line 1464 of yacc.c  */
#line 1069 "rihtan.y"
    {
             (yyval.typeidentifier) = (yyvsp[(1) - (1)].typeidentifier);
           ;}
    break;

  case 178:

/* Line 1464 of yacc.c  */
#line 1075 "rihtan.y"
    {
                     (yyval.typereforspec) = MakeTypeRefOrSpec((yyvsp[(1) - (1)].typeidentifier), NULL);
                   ;}
    break;

  case 179:

/* Line 1464 of yacc.c  */
#line 1079 "rihtan.y"
    {
                     (yyval.typereforspec) = MakeTypeRefOrSpec(NULL, (yyvsp[(1) - (1)].typespec));
                   ;}
    break;

  case 180:

/* Line 1464 of yacc.c  */
#line 1083 "rihtan.y"
    {
                     (yyval.typereforspec) = MakeTypeRefOrSpec(NULL, (yyvsp[(2) - (3)].typespec));
                   ;}
    break;

  case 181:

/* Line 1464 of yacc.c  */
#line 1102 "rihtan.y"
    {
                (yyval.arrayindex) = MakeArrayIndex(INDEX_NAME, (yyvsp[(1) - (1)].typeidentifier), NULL);
              ;}
    break;

  case 182:

/* Line 1464 of yacc.c  */
#line 1106 "rihtan.y"
    {
                (yyval.arrayindex) = MakeArrayIndex(INDEX_SPEC, NULL, (yyvsp[(1) - (1)].typespec));
              ;}
    break;

  case 183:

/* Line 1464 of yacc.c  */
#line 1112 "rihtan.y"
    {
                    (yyval.typespec) = MakeArrayType((yyloc).first_line, (yyvsp[(3) - (6)].arrayindex), (yyvsp[(6) - (6)].typereforspec), TRUE, TRUE);
                  ;}
    break;

  case 184:

/* Line 1464 of yacc.c  */
#line 1116 "rihtan.y"
    {
                    (yyval.typespec) = MakeArrayType((yyloc).first_line, (yyvsp[(3) - (6)].arrayindex), (yyvsp[(6) - (6)].typereforspec), FALSE, FALSE);
                  ;}
    break;

  case 185:

/* Line 1464 of yacc.c  */
#line 1120 "rihtan.y"
    {
                    (yyval.typespec) = MakeArrayType((yyloc).first_line, (yyvsp[(3) - (6)].arrayindex), (yyvsp[(6) - (6)].typereforspec), TRUE, FALSE);
                  ;}
    break;

  case 186:

/* Line 1464 of yacc.c  */
#line 1124 "rihtan.y"
    {
                    (yyval.typespec) = MakeArrayType((yyloc).first_line, (yyvsp[(3) - (6)].arrayindex), (yyvsp[(6) - (6)].typereforspec), FALSE, TRUE);
                  ;}
    break;

  case 187:

/* Line 1464 of yacc.c  */
#line 1130 "rihtan.y"
    {
                     (yyval.typespec) = MakeRecordType((yyloc).first_line, (yyvsp[(2) - (4)].fieldspec), REC_RECORD);
                   ;}
    break;

  case 188:

/* Line 1464 of yacc.c  */
#line 1134 "rihtan.y"
    {
                     (yyval.typespec) = MakeRecordType((yyloc).first_line, (yyvsp[(2) - (4)].fieldspec), REC_UNION);
                   ;}
    break;

  case 189:

/* Line 1464 of yacc.c  */
#line 1138 "rihtan.y"
    {
                     (yyval.typespec) = MakeRecordType((yyloc).first_line, (yyvsp[(3) - (5)].fieldspec), REC_UNCHECKED_UNION);
                   ;}
    break;

  case 190:

/* Line 1464 of yacc.c  */
#line 1144 "rihtan.y"
    {
               (yyval.fieldspec) = (yyvsp[(1) - (1)].fieldspec);
             ;}
    break;

  case 191:

/* Line 1464 of yacc.c  */
#line 1148 "rihtan.y"
    {
               (yyval.fieldspec) = PrefixFieldSpec((yyvsp[(1) - (2)].fieldspec), (yyvsp[(2) - (2)].fieldspec));
             ;}
    break;

  case 192:

/* Line 1464 of yacc.c  */
#line 1154 "rihtan.y"
    {
               (yyval.fieldspec) = MakeFieldSpec((yyvsp[(1) - (5)].string), (yyvsp[(3) - (5)].typereforspec), (yyvsp[(4) - (5)].representationclause), NULL, NULL);
             ;}
    break;

  case 193:

/* Line 1464 of yacc.c  */
#line 1158 "rihtan.y"
    {
               (yyval.fieldspec) = MakeFieldSpec((yyvsp[(1) - (4)].string), NULL, NULL, NULL, NULL);
             ;}
    break;

  case 194:

/* Line 1464 of yacc.c  */
#line 1162 "rihtan.y"
    {
               (yyval.fieldspec) = MakeFieldSpec(NULL, NULL, NULL, (yyvsp[(1) - (1)].string), NULL);
             ;}
    break;

  case 195:

/* Line 1464 of yacc.c  */
#line 1166 "rihtan.y"
    {
               (yyval.fieldspec) = MakeFieldSpec(NULL, NULL, NULL, NULL, (yyvsp[(1) - (1)].string));
             ;}
    break;

  case 196:

/* Line 1464 of yacc.c  */
#line 1172 "rihtan.y"
    {
                     (yyval.typespec) = MakeAccessType((yyloc).first_line, (yyvsp[(5) - (5)].typereforspec), TRUE, FALSE, FALSE, FALSE, FALSE);
                   ;}
    break;

  case 197:

/* Line 1464 of yacc.c  */
#line 1176 "rihtan.y"
    {
                     (yyval.typespec) = MakeAccessType((yyloc).first_line, (yyvsp[(3) - (3)].typereforspec), FALSE, FALSE, FALSE, FALSE, FALSE);
                   ;}
    break;

  case 198:

/* Line 1464 of yacc.c  */
#line 1180 "rihtan.y"
    {
                     (yyval.typespec) = MakeAccessType((yyloc).first_line, (yyvsp[(4) - (4)].typereforspec), TRUE, TRUE, FALSE, FALSE, FALSE);
                   ;}
    break;

  case 199:

/* Line 1464 of yacc.c  */
#line 1184 "rihtan.y"
    {
                     (yyval.typespec) = MakeAccessType((yyloc).first_line, (yyvsp[(2) - (2)].typereforspec), FALSE, TRUE, FALSE, FALSE, FALSE);
                   ;}
    break;

  case 200:

/* Line 1464 of yacc.c  */
#line 1188 "rihtan.y"
    {
                     (yyval.typespec) = MakeAccessType((yyloc).first_line, (yyvsp[(5) - (5)].typereforspec), TRUE, TRUE, FALSE, FALSE, TRUE);
                   ;}
    break;

  case 201:

/* Line 1464 of yacc.c  */
#line 1192 "rihtan.y"
    {
                     (yyval.typespec) = MakeAccessType((yyloc).first_line, (yyvsp[(3) - (3)].typereforspec), FALSE, TRUE, FALSE, FALSE, TRUE);
                   ;}
    break;

  case 202:

/* Line 1464 of yacc.c  */
#line 1196 "rihtan.y"
    {
                     (yyval.typespec) = MakeAccessType((yyloc).first_line, (yyvsp[(6) - (6)].typereforspec), TRUE, FALSE, TRUE, FALSE, FALSE);
                   ;}
    break;

  case 203:

/* Line 1464 of yacc.c  */
#line 1200 "rihtan.y"
    {
                     (yyval.typespec) = MakeAccessType((yyloc).first_line, (yyvsp[(4) - (4)].typereforspec), FALSE, FALSE, TRUE, FALSE, FALSE);
                   ;}
    break;

  case 204:

/* Line 1464 of yacc.c  */
#line 1204 "rihtan.y"
    {
                     (yyval.typespec) = MakeAccessType((yyloc).first_line, (yyvsp[(5) - (5)].typereforspec), TRUE, TRUE, TRUE, FALSE, FALSE);
                   ;}
    break;

  case 205:

/* Line 1464 of yacc.c  */
#line 1208 "rihtan.y"
    {
                     (yyval.typespec) = MakeAccessType((yyloc).first_line, (yyvsp[(3) - (3)].typereforspec), FALSE, TRUE, TRUE, FALSE, FALSE);
                   ;}
    break;

  case 206:

/* Line 1464 of yacc.c  */
#line 1212 "rihtan.y"
    {
                     (yyval.typespec) = MakeAccessType((yyloc).first_line, (yyvsp[(6) - (6)].typereforspec), TRUE, TRUE, TRUE, FALSE, TRUE);
                   ;}
    break;

  case 207:

/* Line 1464 of yacc.c  */
#line 1216 "rihtan.y"
    {
                     (yyval.typespec) = MakeAccessType((yyloc).first_line, (yyvsp[(4) - (4)].typereforspec), FALSE, TRUE, TRUE, FALSE, TRUE);
                   ;}
    break;

  case 208:

/* Line 1464 of yacc.c  */
#line 1220 "rihtan.y"
    {
                     (yyval.typespec) = MakeAccessType((yyloc).first_line, (yyvsp[(3) - (3)].typereforspec), FALSE, TRUE, FALSE, TRUE, FALSE);
                   ;}
    break;

  case 209:

/* Line 1464 of yacc.c  */
#line 1224 "rihtan.y"
    {
                     (yyval.typespec) = MakeAccessType((yyloc).first_line, (yyvsp[(4) - (4)].typereforspec), FALSE, TRUE, TRUE, TRUE, FALSE);
                   ;}
    break;

  case 210:

/* Line 1464 of yacc.c  */
#line 1231 "rihtan.y"
    {
                    (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, VAR_DECLARATION_ELT,
                             DeclareVariable(MakeIdentifierListEntry((yyvsp[(1) - (3)].string)), (yyvsp[(3) - (3)].typeidentifier), NULL, NULL, NULL,
                                             MODE_LOCAL, NULL, NAME_NONE));
                  ;}
    break;

  case 211:

/* Line 1464 of yacc.c  */
#line 1237 "rihtan.y"
    {
                    (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, VAR_DECLARATION_ELT,
                             DeclareVariable(MakeIdentifierListEntry((yyvsp[(1) - (3)].string)), NULL, (yyvsp[(3) - (3)].typespec), NULL, NULL,
                                             MODE_LOCAL, NULL, NAME_NONE));
                  ;}
    break;

  case 212:

/* Line 1464 of yacc.c  */
#line 1245 "rihtan.y"
    {
               (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, ASSIGNMENT_ELT, Assignment((yyvsp[(1) - (4)].reference), (yyvsp[(4) - (4)].rhs), TRUE));
             ;}
    break;

  case 213:

/* Line 1464 of yacc.c  */
#line 1249 "rihtan.y"
    {
               (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, ASSIGNMENT_ELT, Assignment((yyvsp[(1) - (3)].reference), (yyvsp[(3) - (3)].rhs), FALSE));
             ;}
    break;

  case 214:

/* Line 1464 of yacc.c  */
#line 1255 "rihtan.y"
    {
        (yyval.rhs) = MakeRHS(EXPR_RHS, (yyvsp[(1) - (1)].node), NULL, NULL);
      ;}
    break;

  case 215:

/* Line 1464 of yacc.c  */
#line 1259 "rihtan.y"
    {
        (yyval.rhs) = MakeRHS(ARRAY_RHS, NULL, (yyvsp[(1) - (1)].arrayinit), NULL);
      ;}
    break;

  case 216:

/* Line 1464 of yacc.c  */
#line 1263 "rihtan.y"
    {
        (yyval.rhs) = MakeRHS(RECORD_RHS, NULL, NULL, (yyvsp[(1) - (1)].recordinit));
      ;}
    break;

  case 217:

/* Line 1464 of yacc.c  */
#line 1269 "rihtan.y"
    {
                 (yyval.reference) = MakeReference(IDENTIFIER_REFERENCE, (yyvsp[(1) - (1)].string), NULL, NULL);
               ;}
    break;

  case 218:

/* Line 1464 of yacc.c  */
#line 1273 "rihtan.y"
    {
                 (yyval.reference) = PrefixReference(
                        MakeReference(IDENTIFIER_REFERENCE, (yyvsp[(1) - (2)].string), NULL, NULL), (yyvsp[(2) - (2)].reference));
               ;}
    break;

  case 219:

/* Line 1464 of yacc.c  */
#line 1280 "rihtan.y"
    {
               (yyval.reference) = (yyvsp[(1) - (1)].reference);
             ;}
    break;

  case 220:

/* Line 1464 of yacc.c  */
#line 1284 "rihtan.y"
    {
               (yyval.reference) = PrefixReference((yyvsp[(1) - (2)].reference), (yyvsp[(2) - (2)].reference));
             ;}
    break;

  case 221:

/* Line 1464 of yacc.c  */
#line 1290 "rihtan.y"
    {
               (yyval.reference) = MakeReference(ARRAY_REFERENCE, NULL, (yyvsp[(2) - (3)].node), NULL);
             ;}
    break;

  case 222:

/* Line 1464 of yacc.c  */
#line 1294 "rihtan.y"
    {
               (yyval.reference) = MakeReference(ARRAY_SLICE_REFERENCE, NULL, (yyvsp[(2) - (5)].node), (yyvsp[(4) - (5)].node));
             ;}
    break;

  case 223:

/* Line 1464 of yacc.c  */
#line 1298 "rihtan.y"
    {
               (yyval.reference) = MakeReference(ARRAY_SLICE_REFERENCE, NULL, (yyvsp[(2) - (4)].node), NULL);
             ;}
    break;

  case 224:

/* Line 1464 of yacc.c  */
#line 1302 "rihtan.y"
    {
               (yyval.reference) = MakeReference(ARRAY_SLICE_LENGTH_REFERENCE, NULL, (yyvsp[(2) - (5)].node), (yyvsp[(4) - (5)].node));
             ;}
    break;

  case 225:

/* Line 1464 of yacc.c  */
#line 1306 "rihtan.y"
    {
               (yyval.reference) = MakeReference(FIELD_REFERENCE, (yyvsp[(2) - (2)].string), NULL, NULL);
             ;}
    break;

  case 226:

/* Line 1464 of yacc.c  */
#line 1312 "rihtan.y"
    {
             (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, WHEN_ELT, When((yyvsp[(2) - (7)].node), (yyvsp[(4) - (7)].elt), (yyvsp[(5) - (7)].elt), FALSE));
           ;}
    break;

  case 227:

/* Line 1464 of yacc.c  */
#line 1318 "rihtan.y"
    {
              (yyval.elt) = NULL;
            ;}
    break;

  case 228:

/* Line 1464 of yacc.c  */
#line 1322 "rihtan.y"
    {
              (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, WHEN_ELT, When((yyvsp[(2) - (5)].node), (yyvsp[(4) - (5)].elt), (yyvsp[(5) - (5)].elt), FALSE));
            ;}
    break;

  case 229:

/* Line 1464 of yacc.c  */
#line 1326 "rihtan.y"
    {
              (yyval.elt) = (yyvsp[(2) - (2)].elt);
            ;}
    break;

  case 230:

/* Line 1464 of yacc.c  */
#line 1332 "rihtan.y"
    {
              (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, LOOP_ELT, Loop((yyvsp[(2) - (4)].elt)));
            ;}
    break;

  case 231:

/* Line 1464 of yacc.c  */
#line 1338 "rihtan.y"
    {
            (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, DO_ELT, DoStmt((yyvsp[(2) - (4)].elt)));
          ;}
    break;

  case 232:

/* Line 1464 of yacc.c  */
#line 1344 "rihtan.y"
    {
               (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, WHILE_ELT, WhileStmt((yyvsp[(2) - (6)].node), (yyvsp[(4) - (6)].elt)));
             ;}
    break;

  case 233:

/* Line 1464 of yacc.c  */
#line 1355 "rihtan.y"
    {
              (yyval.elt) = (yyvsp[(1) - (1)].elt);
            ;}
    break;

  case 234:

/* Line 1464 of yacc.c  */
#line 1359 "rihtan.y"
    {
               (yyval.elt) = AppendStmts((yyvsp[(1) - (2)].elt), (yyvsp[(2) - (2)].elt));
             ;}
    break;

  case 235:

/* Line 1464 of yacc.c  */
#line 1363 "rihtan.y"
    {
               (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, WHEN_ELT,
                        When((yyvsp[(3) - (5)].node), Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, EXIT_ELT, NULL), (yyvsp[(5) - (5)].elt), FALSE));
             ;}
    break;

  case 236:

/* Line 1464 of yacc.c  */
#line 1375 "rihtan.y"
    {
              (yyval.elt) = (yyvsp[(1) - (1)].elt);
            ;}
    break;

  case 237:

/* Line 1464 of yacc.c  */
#line 1379 "rihtan.y"
    {
               (yyval.elt) = AppendStmts((yyvsp[(1) - (2)].elt), (yyvsp[(2) - (2)].elt));
             ;}
    break;

  case 238:

/* Line 1464 of yacc.c  */
#line 1383 "rihtan.y"
    {
               (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, WHEN_ELT,
                        When((yyvsp[(3) - (5)].node), Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, EXIT_ELT, NULL), (yyvsp[(5) - (5)].elt), TRUE));
             ;}
    break;

  case 239:

/* Line 1464 of yacc.c  */
#line 1388 "rihtan.y"
    {
               (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, WHEN_ELT,
                        When((yyvsp[(3) - (5)].node), Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, REPEAT_ELT, NULL), (yyvsp[(5) - (5)].elt), FALSE));
             ;}
    break;

  case 240:

/* Line 1464 of yacc.c  */
#line 1393 "rihtan.y"
    {
               (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, WHEN_ELT,
                        When((yyvsp[(3) - (4)].node), Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, EXIT_ELT, NULL), NULL, TRUE));
             ;}
    break;

  case 241:

/* Line 1464 of yacc.c  */
#line 1400 "rihtan.y"
    {
              (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, NULL_ELT, NULL);
            ;}
    break;

  case 242:

/* Line 1464 of yacc.c  */
#line 1406 "rihtan.y"
    {
              (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, CASE_ELT, Case((yyvsp[(2) - (6)].node), (yyvsp[(4) - (6)].caseoption)));
            ;}
    break;

  case 243:

/* Line 1464 of yacc.c  */
#line 1412 "rihtan.y"
    {
                     (yyval.caseoption) = NULL;
                   ;}
    break;

  case 244:

/* Line 1464 of yacc.c  */
#line 1416 "rihtan.y"
    {
                     (yyval.caseoption) = AppendCaseOption((yyvsp[(1) - (2)].caseoption), (yyvsp[(2) - (2)].caseoption));
                   ;}
    break;

  case 245:

/* Line 1464 of yacc.c  */
#line 1422 "rihtan.y"
    {
                (yyval.caseoption) = BuildCaseOption((yyvsp[(2) - (4)].range), (yyvsp[(4) - (4)].elt));
              ;}
    break;

  case 246:

/* Line 1464 of yacc.c  */
#line 1428 "rihtan.y"
    {
                    (yyval.range) = (yyvsp[(1) - (1)].range);
                  ;}
    break;

  case 247:

/* Line 1464 of yacc.c  */
#line 1432 "rihtan.y"
    {
                    PrefixRange((yyvsp[(1) - (3)].range), (yyvsp[(3) - (3)].range));
                  ;}
    break;

  case 248:

/* Line 1464 of yacc.c  */
#line 1438 "rihtan.y"
    {
               (yyval.range) = MakeRange((yyvsp[(1) - (1)].node), (yyvsp[(1) - (1)].node));
             ;}
    break;

  case 249:

/* Line 1464 of yacc.c  */
#line 1450 "rihtan.y"
    {
             (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, FOR_ELT, ForStmt((yyvsp[(2) - (10)].elt), (yyvsp[(4) - (10)].node), (yyvsp[(6) - (10)].node), (yyvsp[(8) - (10)].elt)));
           ;}
    break;

  case 250:

/* Line 1464 of yacc.c  */
#line 1454 "rihtan.y"
    {
             (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, FOR_ELT, ForStmt((yyvsp[(2) - (6)].elt), NULL, NULL, (yyvsp[(4) - (6)].elt)));
           ;}
    break;

  case 251:

/* Line 1464 of yacc.c  */
#line 1472 "rihtan.y"
    {
               (yyval.node) = NewNode((yylsp[(2) - (3)]).filename, (yylsp[(2) - (3)]).first_line, _PLUS, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), 0, 0.0, NULL, NULL, NULL, NULL);
             ;}
    break;

  case 252:

/* Line 1464 of yacc.c  */
#line 1476 "rihtan.y"
    {
               (yyval.node) = NewNode((yylsp[(2) - (3)]).filename, (yylsp[(2) - (3)]).first_line, _MINUS, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), 0, 0.0, NULL, NULL, NULL, NULL);
             ;}
    break;

  case 253:

/* Line 1464 of yacc.c  */
#line 1480 "rihtan.y"
    {
               (yyval.node) = NewNode((yylsp[(2) - (3)]).filename, (yylsp[(2) - (3)]).first_line, _TIMES, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), 0, 0.0, NULL, NULL, NULL, NULL);
             ;}
    break;

  case 254:

/* Line 1464 of yacc.c  */
#line 1484 "rihtan.y"
    {
               (yyval.node) = NewNode((yylsp[(2) - (3)]).filename, (yylsp[(2) - (3)]).first_line, _DIVIDE, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), 0, 0.0, NULL, NULL, NULL, NULL);
             ;}
    break;

  case 255:

/* Line 1464 of yacc.c  */
#line 1488 "rihtan.y"
    {
               (yyval.node) = NewNode((yylsp[(2) - (3)]).filename, (yylsp[(2) - (3)]).first_line, _MOD, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), 0, 0.0, NULL, NULL, NULL, NULL);
             ;}
    break;

  case 256:

/* Line 1464 of yacc.c  */
#line 1492 "rihtan.y"
    {
               (yyval.node) = NewNode((yyloc).filename, (yyloc).first_line, _TIMES,
                            NewNode((yyloc).filename, (yyloc).first_line,
                                    _CONST_INT, NULL, NULL, -1, 0.0, NULL, NULL, NULL, NULL),
                            (yyvsp[(2) - (2)].node), 0, 0.0, NULL, NULL, NULL, NULL);
             ;}
    break;

  case 257:

/* Line 1464 of yacc.c  */
#line 1499 "rihtan.y"
    {
               (yyval.node) = NewNode((yylsp[(2) - (3)]).filename, (yylsp[(2) - (3)]).first_line, _IAND, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), 0, 0.0, NULL, NULL, NULL, NULL);
             ;}
    break;

  case 258:

/* Line 1464 of yacc.c  */
#line 1503 "rihtan.y"
    {
               (yyval.node) = NewNode((yylsp[(2) - (3)]).filename, (yylsp[(2) - (3)]).first_line, _IOR, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), 0, 0.0, NULL, NULL, NULL, NULL);
             ;}
    break;

  case 259:

/* Line 1464 of yacc.c  */
#line 1507 "rihtan.y"
    {
               (yyval.node) = NewNode((yylsp[(2) - (3)]).filename, (yylsp[(2) - (3)]).first_line, _IXOR, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), 0, 0.0, NULL, NULL, NULL, NULL);
             ;}
    break;

  case 260:

/* Line 1464 of yacc.c  */
#line 1511 "rihtan.y"
    {
               (yyval.node) = NewNode((yylsp[(2) - (3)]).filename, (yylsp[(2) - (3)]).first_line, _LT, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), 0, 0.0, NULL, NULL, NULL, NULL);
             ;}
    break;

  case 261:

/* Line 1464 of yacc.c  */
#line 1515 "rihtan.y"
    {
               (yyval.node) = NewNode((yylsp[(2) - (3)]).filename, (yylsp[(2) - (3)]).first_line, _GT, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), 0, 0.0, NULL, NULL, NULL, NULL);
             ;}
    break;

  case 262:

/* Line 1464 of yacc.c  */
#line 1519 "rihtan.y"
    {
               (yyval.node) = NewNode((yylsp[(2) - (3)]).filename, (yylsp[(2) - (3)]).first_line, _LEQ, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), 0, 0.0, NULL, NULL, NULL, NULL);
             ;}
    break;

  case 263:

/* Line 1464 of yacc.c  */
#line 1523 "rihtan.y"
    {
               (yyval.node) = NewNode((yylsp[(2) - (3)]).filename, (yylsp[(2) - (3)]).first_line, _GEQ, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), 0, 0.0, NULL, NULL, NULL, NULL);
             ;}
    break;

  case 264:

/* Line 1464 of yacc.c  */
#line 1527 "rihtan.y"
    {
               (yyval.node) = NewNode((yylsp[(2) - (3)]).filename, (yylsp[(2) - (3)]).first_line, _EQ, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), 0, 0.0, NULL, NULL, NULL, NULL);
             ;}
    break;

  case 265:

/* Line 1464 of yacc.c  */
#line 1531 "rihtan.y"
    {
               (yyval.node) = NewNode((yylsp[(2) - (3)]).filename, (yylsp[(2) - (3)]).first_line, _NEQ, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), 0, 0.0, NULL, NULL, NULL, NULL);
             ;}
    break;

  case 266:

/* Line 1464 of yacc.c  */
#line 1535 "rihtan.y"
    {
               (yyval.node) = NewNode((yylsp[(2) - (3)]).filename, (yylsp[(2) - (3)]).first_line, _AND, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), 0, 0.0, NULL, NULL, NULL, NULL);
             ;}
    break;

  case 267:

/* Line 1464 of yacc.c  */
#line 1539 "rihtan.y"
    {
               (yyval.node) = NewNode((yylsp[(2) - (3)]).filename, (yylsp[(2) - (3)]).first_line, _OR, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), 0, 0.0, NULL, NULL, NULL, NULL);
             ;}
    break;

  case 268:

/* Line 1464 of yacc.c  */
#line 1543 "rihtan.y"
    {
               (yyval.node) = NewNode((yyloc).filename, (yyloc).first_line, _NOT, (yyvsp[(2) - (2)].node), NULL, 0, 0.0, NULL, NULL, NULL, NULL);
             ;}
    break;

  case 269:

/* Line 1464 of yacc.c  */
#line 1547 "rihtan.y"
    {
               (yyval.node) = NewNode((yyloc).filename, (yyloc).first_line, _CONST_BOOL, NULL, NULL, TRUE, 0.0, NULL, NULL, NULL, NULL);
             ;}
    break;

  case 270:

/* Line 1464 of yacc.c  */
#line 1551 "rihtan.y"
    {
               (yyval.node) = NewNode((yyloc).filename, (yyloc).first_line, _CONST_BOOL, NULL, NULL, FALSE, 0.0, NULL, NULL, NULL, NULL);
             ;}
    break;

  case 271:

/* Line 1464 of yacc.c  */
#line 1555 "rihtan.y"
    {
               (yyval.node) = SetEnclosedInParen((yyvsp[(2) - (3)].node));
             ;}
    break;

  case 272:

/* Line 1464 of yacc.c  */
#line 1559 "rihtan.y"
    {
               (yyval.node) = NewNode((yyloc).filename, (yyloc).first_line, _VAR, NULL, NULL, 0, 0.0, (yyvsp[(1) - (1)].reference), NULL, NULL, NULL);
             ;}
    break;

  case 273:

/* Line 1464 of yacc.c  */
#line 1563 "rihtan.y"
    {
               (yyval.node) = NewNode((yyloc).filename, (yyloc).first_line, _VAR, NULL, NULL, 0, 0.0, (yyvsp[(1) - (3)].reference), NULL, (yyvsp[(3) - (3)].attributechain), NULL);
             ;}
    break;

  case 274:

/* Line 1464 of yacc.c  */
#line 1577 "rihtan.y"
    {
               (yyval.node) = NewNode((yyloc).filename, (yyloc).first_line, _CONST_INT, NULL, NULL, (yyvsp[(1) - (1)].integer), 0.0, NULL, NULL, NULL, NULL);
             ;}
    break;

  case 275:

/* Line 1464 of yacc.c  */
#line 1581 "rihtan.y"
    {
               (yyval.node) = NewNode((yyloc).filename, (yyloc).first_line, _CONST_FLOAT, NULL, NULL, 0, (yyvsp[(1) - (1)].floatval), NULL, NULL, NULL, NULL);
             ;}
    break;

  case 276:

/* Line 1464 of yacc.c  */
#line 1585 "rihtan.y"
    {
               (yyval.node) = NewNode((yyloc).filename, (yyloc).first_line, _CONST_STRING, NULL, NULL, 0, 0.0, NULL, NULL, NULL, (yyvsp[(1) - (1)].string));
             ;}
    break;

  case 277:

/* Line 1464 of yacc.c  */
#line 1589 "rihtan.y"
    {
               (yyval.node) = NewNode((yyloc).filename, (yyloc).first_line, _CONST_CHARACTER, NULL, NULL, 0, 0.0, NULL, NULL, NULL, (yyvsp[(1) - (1)].string));
             ;}
    break;

  case 278:

/* Line 1464 of yacc.c  */
#line 1593 "rihtan.y"
    {
               (yyval.node) = NewNode((yyloc).filename, (yyloc).first_line, _CONST_NULL, NULL, NULL, 0, 0.0, NULL, NULL, NULL, NULL);
             ;}
    break;

  case 279:

/* Line 1464 of yacc.c  */
#line 1597 "rihtan.y"
    {
               (yyval.node) = NewNode((yyloc).filename, (yyloc).first_line, _FN_CALL, NULL, NULL, 0, 0.0, NULL, (yyvsp[(1) - (1)].elt), NULL, NULL);
             ;}
    break;

  case 280:

/* Line 1464 of yacc.c  */
#line 1603 "rihtan.y"
    {
                    (yyval.attributechain) = MakeAttributeChainEntry((yyloc).filename, (yyloc).first_line, (yyvsp[(1) - (1)].singleattribute));
                  ;}
    break;

  case 281:

/* Line 1464 of yacc.c  */
#line 1607 "rihtan.y"
    {
                    (yyval.attributechain) = PrefixAttributeChainEntry((yyloc).filename, (yyloc).first_line, (yyvsp[(1) - (3)].singleattribute), (yyvsp[(3) - (3)].attributechain));
                  ;}
    break;

  case 282:

/* Line 1464 of yacc.c  */
#line 1613 "rihtan.y"
    {
                     (yyval.singleattribute) = MakeSingleAttribute((yyvsp[(1) - (1)].string), NULL);
                   ;}
    break;

  case 283:

/* Line 1464 of yacc.c  */
#line 1617 "rihtan.y"
    {
                     (yyval.singleattribute) = MakeSingleAttribute((yyvsp[(1) - (4)].string), (yyvsp[(3) - (4)].string));
                   ;}
    break;

  case 284:

/* Line 1464 of yacc.c  */
#line 1621 "rihtan.y"
    {
                     (yyval.singleattribute) = MakeSingleAttribute("unit", NULL);
                   ;}
    break;

  case 285:

/* Line 1464 of yacc.c  */
#line 1625 "rihtan.y"
    {
                     (yyval.singleattribute) = MakeSingleAttribute("type", NULL);
                   ;}
    break;

  case 286:

/* Line 1464 of yacc.c  */
#line 1629 "rihtan.y"
    {
				     (yyval.singleattribute) = MakeSingleAttribute("access", NULL);
				   ;}
    break;

  case 287:

/* Line 1464 of yacc.c  */
#line 1635 "rihtan.y"
    {
                          (yyval.arrayinit) = MakeArrayInitialisation((yyvsp[(3) - (6)].string), (yyvsp[(5) - (6)].rhs), NULL);
                       ;}
    break;

  case 288:

/* Line 1464 of yacc.c  */
#line 1639 "rihtan.y"
    {
                          (yyval.arrayinit) = MakeArrayInitialisation(NULL, NULL, (yyvsp[(2) - (3)].arrayinitentry));
                       ;}
    break;

  case 289:

/* Line 1464 of yacc.c  */
#line 1643 "rihtan.y"
    {
                          (yyval.arrayinit) = MakeArrayInitialisation(NULL, NULL, (yyvsp[(2) - (3)].arrayinitentry));
                       ;}
    break;

  case 290:

/* Line 1464 of yacc.c  */
#line 1649 "rihtan.y"
    {
                             (yyval.arrayinitentry) = (yyvsp[(1) - (1)].arrayinitentry);
                           ;}
    break;

  case 291:

/* Line 1464 of yacc.c  */
#line 1653 "rihtan.y"
    {
                             (yyval.arrayinitentry) = PrefixArrayInitialisationEntry((yyvsp[(1) - (3)].arrayinitentry), (yyvsp[(3) - (3)].arrayinitentry));
                           ;}
    break;

  case 292:

/* Line 1464 of yacc.c  */
#line 1659 "rihtan.y"
    {
                               (yyval.arrayinitentry) = MakeArrayInitialisationEntry((yyvsp[(1) - (3)].range), (yyvsp[(3) - (3)].rhs));
                             ;}
    break;

  case 293:

/* Line 1464 of yacc.c  */
#line 1665 "rihtan.y"
    {
                                  (yyval.arrayinitentry) = (yyvsp[(1) - (1)].arrayinitentry);
                                ;}
    break;

  case 294:

/* Line 1464 of yacc.c  */
#line 1669 "rihtan.y"
    {
                                  (yyval.arrayinitentry) = PrefixArrayInitialisationEntry((yyvsp[(1) - (3)].arrayinitentry), (yyvsp[(3) - (3)].arrayinitentry));
                                ;}
    break;

  case 295:

/* Line 1464 of yacc.c  */
#line 1675 "rihtan.y"
    {
                                   (yyval.arrayinitentry) = MakeArrayInitialisationEntry(NULL, (yyvsp[(1) - (1)].rhs));
                                 ;}
    break;

  case 296:

/* Line 1464 of yacc.c  */
#line 1681 "rihtan.y"
    {
                          (yyval.recordinit) = (yyvsp[(2) - (3)].recordinit);
                        ;}
    break;

  case 297:

/* Line 1464 of yacc.c  */
#line 1685 "rihtan.y"
    {                 // this is possible for a record that only has '!' fields
                          (yyval.recordinit) = NULL;
                        ;}
    break;

  case 298:

/* Line 1464 of yacc.c  */
#line 1691 "rihtan.y"
    {
                              (yyval.recordinit) = (yyvsp[(1) - (1)].recordinit);
                            ;}
    break;

  case 299:

/* Line 1464 of yacc.c  */
#line 1695 "rihtan.y"
    {
                              (yyval.recordinit) = PrefixFieldInitialisation((yyvsp[(1) - (3)].recordinit), (yyvsp[(3) - (3)].recordinit));
                            ;}
    break;

  case 300:

/* Line 1464 of yacc.c  */
#line 1701 "rihtan.y"
    {
                         (yyval.recordinit) = MakeFieldInitialisation((yyvsp[(1) - (4)].string), (yyvsp[(4) - (4)].rhs), TRUE);
                       ;}
    break;

  case 301:

/* Line 1464 of yacc.c  */
#line 1705 "rihtan.y"
    {
                         (yyval.recordinit) = MakeFieldInitialisation((yyvsp[(1) - (3)].string), (yyvsp[(3) - (3)].rhs), FALSE);
                       ;}
    break;

  case 302:

/* Line 1464 of yacc.c  */
#line 1709 "rihtan.y"
    {
                         (yyval.recordinit) = MakeFieldInitialisation((yyvsp[(1) - (2)].string), NULL, FALSE);
                       ;}
    break;

  case 303:

/* Line 1464 of yacc.c  */
#line 1715 "rihtan.y"
    {
                   (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, PROCEDURE_CALL_ELT,
                            MakeProcedureCall((yyvsp[(1) - (1)].reference), NULL));
                 ;}
    break;

  case 304:

/* Line 1464 of yacc.c  */
#line 1720 "rihtan.y"
    {
                   (yyval.elt) = (yyvsp[(1) - (1)].elt);
                 ;}
    break;

  case 305:

/* Line 1464 of yacc.c  */
#line 1727 "rihtan.y"
    {
                   (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, PROCEDURE_CALL_ELT,
                            MakeProcedureCall((yyvsp[(1) - (3)].reference), NULL));
                 ;}
    break;

  case 306:

/* Line 1464 of yacc.c  */
#line 1732 "rihtan.y"
    {
                   (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, PROCEDURE_CALL_ELT,
                            MakeProcedureCall((yyvsp[(1) - (4)].reference), (yyvsp[(3) - (4)].actual)));
                 ;}
    break;

  case 307:

/* Line 1464 of yacc.c  */
#line 1739 "rihtan.y"
    {
                          (yyval.actual) = (yyvsp[(1) - (1)].actual);
                        ;}
    break;

  case 308:

/* Line 1464 of yacc.c  */
#line 1743 "rihtan.y"
    {
                          (yyval.actual) = PrefixActualParameter((yyvsp[(1) - (3)].actual), (yyvsp[(3) - (3)].actual));
                        ;}
    break;

  case 309:

/* Line 1464 of yacc.c  */
#line 1749 "rihtan.y"
    {
                     (yyval.actual) = AddRepresentationToActualParameter((yyloc).filename, (yyloc).first_line, (yyvsp[(1) - (2)].actual), (yyvsp[(2) - (2)].representationclause));
                   ;}
    break;

  case 310:

/* Line 1464 of yacc.c  */
#line 1755 "rihtan.y"
    {
                     (yyval.actual) = MakeActualParameter((yyvsp[(1) - (3)].string), (yyvsp[(3) - (3)].node), MODE_UNSPECIFIED);
                   ;}
    break;

  case 311:

/* Line 1464 of yacc.c  */
#line 1759 "rihtan.y"
    {
                     (yyval.actual) = MakeActualParameter((yyvsp[(1) - (4)].string), (yyvsp[(4) - (4)].node), (yyvsp[(3) - (4)].varmode));
                   ;}
    break;

  case 312:

/* Line 1464 of yacc.c  */
#line 1763 "rihtan.y"
    {
                     (yyval.actual) = MakeActualParameter(NULL, (yyvsp[(1) - (1)].node), MODE_UNSPECIFIED);
                   ;}
    break;

  case 313:

/* Line 1464 of yacc.c  */
#line 1767 "rihtan.y"
    {
                     (yyval.actual) = MakeActualParameter(NULL, (yyvsp[(2) - (2)].node), (yyvsp[(1) - (2)].varmode));
                   ;}
    break;

  case 314:

/* Line 1464 of yacc.c  */
#line 1773 "rihtan.y"
    {
            (yyval.elt) = MakePragma((yyvsp[(2) - (2)].elt));
         ;}
    break;

  case 315:

/* Line 1464 of yacc.c  */
#line 1779 "rihtan.y"
    {
                (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, ASSERT_ELT, (yyvsp[(2) - (2)].actual));
              ;}
    break;

  case 316:

/* Line 1464 of yacc.c  */
#line 1785 "rihtan.y"
    {
                (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, ADVISE_ELT, (yyvsp[(2) - (2)].actual));
              ;}
    break;

  case 317:

/* Line 1464 of yacc.c  */
#line 1791 "rihtan.y"
    {
              (yyval.genericparam) = (yyvsp[(1) - (2)].genericparam);
            ;}
    break;

  case 318:

/* Line 1464 of yacc.c  */
#line 1795 "rihtan.y"
    {
              (yyval.genericparam) = PrefixGenericParameter((yyvsp[(1) - (3)].genericparam), (yyvsp[(3) - (3)].genericparam));
            ;}
    break;

  case 319:

/* Line 1464 of yacc.c  */
#line 1801 "rihtan.y"
    {
                      (yyval.genericparam) = MakeGenericParameter((yyloc).filename, (yyloc).first_line,
                                                (yyvsp[(2) - (4)].string), GENERIC_RANGE_TYPE, NULL, NULL, NULL, NULL, NULL);
                    ;}
    break;

  case 320:

/* Line 1464 of yacc.c  */
#line 1806 "rihtan.y"
    {
                      (yyval.genericparam) = MakeGenericParameter((yyloc).filename, (yyloc).first_line,
                                                (yyvsp[(2) - (4)].string), GENERIC_DIGITS_TYPE, NULL, NULL, NULL, NULL, NULL);
                    ;}
    break;

  case 321:

/* Line 1464 of yacc.c  */
#line 1811 "rihtan.y"
    {
                      (yyval.genericparam) = MakeGenericParameter((yyloc).filename, (yyloc).first_line,
                                                (yyvsp[(2) - (4)].string), GENERIC_ARRAY_TYPE, NULL, NULL, NULL, NULL, NULL);
                    ;}
    break;

  case 322:

/* Line 1464 of yacc.c  */
#line 1816 "rihtan.y"
    {
                      (yyval.genericparam) = MakeGenericParameter((yyloc).filename, (yyloc).first_line,
                                                (yyvsp[(2) - (4)].string), GENERIC_ACCESS_TYPE, NULL, NULL, NULL, NULL, NULL);
                    ;}
    break;

  case 323:

/* Line 1464 of yacc.c  */
#line 1821 "rihtan.y"
    {
                      (yyval.genericparam) = MakeGenericParameter((yyloc).filename, (yyloc).first_line,
                                                (yyvsp[(2) - (2)].string), GENERIC_TYPE, NULL, NULL, NULL, NULL, NULL);
                    ;}
    break;

  case 324:

/* Line 1464 of yacc.c  */
#line 1826 "rihtan.y"
    {
                      (yyval.genericparam) = MakeGenericParameter((yyloc).filename, (yyloc).first_line,
                                                (yyvsp[(2) - (4)].string), GENERIC_RANGE_CONSTANT, NULL, NULL, NULL, NULL, NULL);
                    ;}
    break;

  case 325:

/* Line 1464 of yacc.c  */
#line 1831 "rihtan.y"
    {
                      (yyval.genericparam) = MakeGenericParameter((yyloc).filename, (yyloc).first_line,
                                                (yyvsp[(2) - (4)].string), GENERIC_DIGITS_CONSTANT, NULL, NULL, NULL, NULL, NULL);
                    ;}
    break;

  case 326:

/* Line 1464 of yacc.c  */
#line 1836 "rihtan.y"
    {
                      (yyval.genericparam) = MakeGenericParameter((yyloc).filename, (yyloc).first_line,
                                                (yyvsp[(2) - (4)].string), GENERIC_ACCESS_CONSTANT, NULL, NULL, NULL, NULL, NULL);
                    ;}
    break;

  case 327:

/* Line 1464 of yacc.c  */
#line 1841 "rihtan.y"
    {
                      (yyval.genericparam) = MakeGenericParameter((yyloc).filename, (yyloc).first_line,
                                                (yyvsp[(2) - (4)].string), GENERIC_CONSTANT, (yyvsp[(4) - (4)].string), NULL, NULL, NULL, NULL);
                    ;}
    break;

  case 328:

/* Line 1464 of yacc.c  */
#line 1846 "rihtan.y"
    {
                      (yyval.genericparam) = MakeGenericParameter((yyloc).filename, (yyloc).first_line,
                                                (yyvsp[(2) - (2)].string), GENERIC_CONSTANT, NULL, NULL, NULL, NULL, NULL);
                    ;}
    break;

  case 329:

/* Line 1464 of yacc.c  */
#line 1851 "rihtan.y"
    {
                      (yyval.genericparam) = MakeGenericParameter((yyloc).filename, (yyloc).first_line,
                                                (yyvsp[(2) - (3)].string), GENERIC_PROCEDURE, NULL, NULL, (yyvsp[(3) - (3)].elt), NULL, NULL);
                    ;}
    break;

  case 330:

/* Line 1464 of yacc.c  */
#line 1856 "rihtan.y"
    {
                      (yyval.genericparam) = MakeGenericParameter((yyloc).filename, (yyloc).first_line,
                                                (yyvsp[(2) - (5)].string), GENERIC_FUNCTION, NULL, NULL, (yyvsp[(3) - (5)].elt), (yyvsp[(5) - (5)].elt), NULL);
                    ;}
    break;

  case 331:

/* Line 1464 of yacc.c  */
#line 1861 "rihtan.y"
    {
                      (yyval.genericparam) = MakeGenericParameter((yyloc).filename, (yyloc).first_line,
                                                (yyvsp[(3) - (7)].string), GENERIC_GENERIC_PACKAGE, NULL, (yyvsp[(6) - (7)].reference), NULL, NULL, (yyvsp[(7) - (7)].genericactualparam));
                    ;}
    break;

  case 332:

/* Line 1464 of yacc.c  */
#line 1868 "rihtan.y"
    {
                                 (yyval.genericactualparam) = NULL;
                               ;}
    break;

  case 333:

/* Line 1464 of yacc.c  */
#line 1872 "rihtan.y"
    {
                                 (yyval.genericactualparam) = (yyvsp[(2) - (3)].genericactualparam);
                               ;}
    break;

  case 334:

/* Line 1464 of yacc.c  */
#line 1878 "rihtan.y"
    {
                                  (yyval.genericactualparam) = (yyvsp[(1) - (1)].genericactualparam);
                                ;}
    break;

  case 335:

/* Line 1464 of yacc.c  */
#line 1882 "rihtan.y"
    {
                                  (yyval.genericactualparam) = PrefixGenericActualParameter((yyvsp[(1) - (3)].genericactualparam), (yyvsp[(3) - (3)].genericactualparam));
                                ;}
    break;

  case 336:

/* Line 1464 of yacc.c  */
#line 1888 "rihtan.y"
    {
                             (yyval.genericactualparam) = MakeGenericActualParameter((yyvsp[(1) - (3)].string), (yyvsp[(3) - (3)].node));
                           ;}
    break;

  case 337:

/* Line 1464 of yacc.c  */
#line 1894 "rihtan.y"
    {
                         (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, CONSTANT_DECLARATION_ELT,
                                  MakeConstant((yyvsp[(1) - (7)].identifierlistentry), (yyvsp[(5) - (7)].node), (yyvsp[(6) - (7)].units), (yyvsp[(7) - (7)].representationclause)));
                       ;}
    break;

  case 338:

/* Line 1464 of yacc.c  */
#line 1901 "rihtan.y"
    {
                  (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line,
                           DECLARE_BLOCK_ELT, MakeDeclareBlock((yyvsp[(2) - (6)].elt), (yyvsp[(5) - (6)].elt), (yyvsp[(4) - (6)].representationclause), TRUE, TRUE));
                ;}
    break;

  case 339:

/* Line 1464 of yacc.c  */
#line 1906 "rihtan.y"
    {
                  (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line,
                           DECLARE_BLOCK_ELT, MakeDeclareBlock(NULL, (yyvsp[(3) - (4)].elt), (yyvsp[(2) - (4)].representationclause), TRUE, TRUE));
                ;}
    break;

  case 340:

/* Line 1464 of yacc.c  */
#line 1913 "rihtan.y"
    {
                  (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line,
                           SEPARATE_STMT_ELT, MakeSeparateStmt((yyvsp[(3) - (3)].reference)));
                ;}
    break;

  case 341:

/* Line 1464 of yacc.c  */
#line 1920 "rihtan.y"
    {
                    (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line,
                             DECLARE_BLOCK_ELT, MakeDeclareBlock(NULL, (yyvsp[(4) - (5)].elt), NULL, TRUE, FALSE));
                  ;}
    break;

  case 342:

/* Line 1464 of yacc.c  */
#line 1925 "rihtan.y"
    {
                    (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line,
                             DECLARE_BLOCK_ELT, MakeDeclareBlock(NULL, (yyvsp[(5) - (6)].elt), NULL, FALSE, TRUE));
                  ;}
    break;

  case 343:

/* Line 1464 of yacc.c  */
#line 1932 "rihtan.y"
    {
              (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, FREE_ELT, (yyvsp[(2) - (2)].reference));
            ;}
    break;

  case 344:

/* Line 1464 of yacc.c  */
#line 1938 "rihtan.y"
    {
                 (yyval.elt) = Elt((yyloc).filename, (yyloc).first_line, (yyloc).last_line, RESTART_ELT, NULL);
               ;}
    break;

  case 345:

/* Line 1464 of yacc.c  */
#line 1944 "rihtan.y"
    {
                        (yyval.representationclause) = (yyvsp[(1) - (1)].representationclause);
                      ;}
    break;

  case 346:

/* Line 1464 of yacc.c  */
#line 1948 "rihtan.y"
    {
                        (yyval.representationclause) = PrefixRepresentationClause((yyvsp[(1) - (3)].representationclause), (yyvsp[(3) - (3)].representationclause));
                      ;}
    break;

  case 347:

/* Line 1464 of yacc.c  */
#line 1954 "rihtan.y"
    {
                          (yyval.representationclause) = MakeRepresentationClause((yyvsp[(1) - (3)].string), (yyvsp[(3) - (3)].node));
                        ;}
    break;

  case 348:

/* Line 1464 of yacc.c  */
#line 1958 "rihtan.y"
    {
                          (yyval.representationclause) = MakeRepresentationClause(
                                 (yyvsp[(1) - (1)].string), NewNode((yyloc).filename, (yyloc).first_line, _CONST_BOOL,
                                             NULL, NULL, TRUE, 0.0, NULL, NULL, NULL, NULL));
                        ;}
    break;



/* Line 1464 of yacc.c  */
#line 5674 "rihtan.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }

  yyerror_range[0] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval, &yylloc);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[0] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      yyerror_range[0] = *yylsp;
      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;

  yyerror_range[1] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, (yyerror_range - 1), 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, &yylloc);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



