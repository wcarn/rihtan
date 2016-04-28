/*
  Copyright 2015 William Carney

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

/* Bison source for Rihtan */

%code requires {

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
}

%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ytypes.h"

extern int yylineno;
struct tElt *LoadExternalPackage(char *packagename);
bool OuterLevel = TRUE;
%}

%union
{
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
}

%token <integer> INTEGER
%token <floatval> FLOATVAL
%token <string> IDENTIFIER
%token <string> CHARACTER
%token ASSIGN TYPE IS RANGE DOTDOT WHEN DO IF THEN END BEGIN_SYM ELSIF ELSE
%token LOOP EXIT REPEAT WHILE TRUE_SYM FALSE_SYM NULL_SYM CASE_SYM
%token FOR_SYM IN_SYM OUT_SYM UNIT_SYM ARRAY_SYM OF_SYM ARROW
%token RECORD_SYM UNION_SYM UNCHECKED_SYM
%token DIGITS MAGNITUDE PROCEDURE FUNCTION PACKAGE SUBSYSTEM SYSTEM PUBLIC_SYM CONTROLLED
%token MAIN RESTART FINAL
%token PRAGMA CLOSED GENERIC NEW CONSTANT PERSISTENT MANAGED_SYM DECLARE RENAMES SEPARATE USING ACCESS_SYM
%token SHARED_SYM WITH_SYM IAND IOR IXOR OPENLT OPENGT FREE_SYM UNIT_TEST USE_SYM ASSERT ADVISE
%token <string> COMMENT
%token <string> CODE
%token <string> HEADER_CODE
%token <string> STRING
%token SCAN_ERROR
%type <node> expr1
%type <elt> if_block
%type <elt> else_part
%type <elt> type_declaration
%type <elt> var_declaration
%type <elt> integer_var_declaration
%type <elt> unit_declaration
%type <elt> assignment
%type <elt> stmt
%type <elt> stmt_list
%type <elt> nstmt_list
%type <elt> loop_stmt
%type <elt> do_stmt
%type <elt> while_stmt
%type <elt> exit_list
%type <elt> exit_repeat_list
%type <elt> null_stmt
%type <elt> case_stmt
%type <elt> for_stmt
%type <elt> declaration_procedure
%type <elt> declaration_procedure_list
%type <elt> declaration_package
%type <elt> declaration_package_list
%type <elt> declaration_global
%type <elt> formal_parameter
%type <elt> formal_parameter_list
%type <elt> formal_parameters
%type <elt> function_return_type
%type <elt> procedure_declaration
%type <elt> function_declaration
%type <elt> package_declaration
%type <elt> package_initialisation
%type <elt> package_finalisation
%type <elt> declare_block
%type <elt> separate_stmt
%type <elt> unit_test_block
%type <elt> free_stmt
%type <elt> restart_stmt
%type <elt> optional_unit_test
%type <elt> assert_stmt
%type <elt> advise_stmt
%type <caseoption> case_option
%type <caseoption> case_option_list
%type <range> case_range
%type <range> case_range_list
%type <elt> constant_declaration
%type <reference> variable_ref
%type <reference> package_identifier
%type <reference> package_ref_list
%type <reference> package_ref_term
%type <units> unit
%type <units> unit_list
%type <units> unit_spec
%type <typespec> integer_type_spec
%type <typespec> floating_type_spec
%type <typespec> array_type_spec
%type <typespec> enumeration_type_spec
%type <typespec> record_type_spec
%type <typespec> access_type_spec
%type <typespec> type_spec
%type <typespec> basic_type_spec
%type <typespec> extended_type_spec
%type <typespec> package_type_spec
%type <typeidentifier> type_ref
%type <typereforspec> type_ref_or_spec
%type <reference> deref_list
%type <reference> deref_term
%type <enumtype> enum_symbol
%type <enumtype> enum_symbol_list
%type <arrayinit> array_initialisation
%type <arrayinitentry> array_initialisation_entry
%type <arrayinitentry> array_initialisation_list
%type <arrayinitentry> array_seq_initialisation_entry
%type <arrayinitentry> array_seq_initialisation_list
%type <recordinit> record_initialisation
%type <recordinit> field_initialisation
%type <recordinit> field_initialisation_list
%type <fieldspec> field_spec
%type <fieldspec> field_list
%type <attributechain> attribute_chain
%type <singleattribute> single_attribute
%type <rhs> rhs
%type <rhs> optional_value
%type <actual> actual_parameter_list
%type <actual> actual_parameter base_actual_parameter
%type <elt> procedure_call
%type <elt> call_with_parameter_list
%type <elt> pragma
%type <genericparam> generic_parameter generic_parameter_list
%type <genericactualparam> generic_actual_parameter generic_actual_parameter_list optional_generic_parameters
%type <representationclause> representation_clause representation_list optional_representation
%type <arrayindex> array_index
%type <typeidentifier> type_identifier
%type <shareclause> share_clause
%left AND OR NOT
%nonassoc LT GT LEQ GEQ EQ NEQ
%left IOR IXOR IAND
%left '+' '-'
%left '*' '/' MOD
%left UMINUS
%type <varmode> mode_spec
%type <elt> code_line
%type <elt> comment_line
%type <identifierlistentry> identifier_list
%type <filecommentblock> initial_comment_block

%locations

%%

program    : initial_comment_block package_declaration ';'
             { bool outer = OuterLevel;
               OuterLevel = FALSE;
               AnalyseProgram($2, outer, $1);
             }
           | initial_comment_block SHARED_SYM package_declaration ';'
             { // separate shared package
               bool outer = OuterLevel;
               OuterLevel = FALSE;
               AnalyseProgram(SetPrefix($3, PREFIX_SHARED, @2.first_line), outer, $1);
             }
           | initial_comment_block UNIT_TEST package_declaration ';'
             { // separate shared package
               bool outer = OuterLevel;
               OuterLevel = FALSE;
               AnalyseProgram(SetPrefix($3, PREFIX_UNIT_TEST, @2.first_line), outer, $1);
             }
           | initial_comment_block NOT UNIT_TEST package_declaration ';'
             { // separate shared package
               bool outer = OuterLevel;
               OuterLevel = FALSE;
               AnalyseProgram(SetPrefix($4, PREFIX_NOT_UNIT_TEST, @2.first_line), outer, $1);
             }
           ;

initial_comment_block : /* empty */
                        {
                          $$ = NULL;
                        }
                      | COMMENT initial_comment_block
                        {
                          $$ = AppendCommentLine($1, $2);
                        }
                      ;

package_declaration : SYSTEM IDENTIFIER IS declaration_package_list package_initialisation package_finalisation END IDENTIFIER
                      {
                        $$ = Elt(@$.filename, @$.first_line, @$.last_line, PACKAGE_DECLARATION_ELT,
                                 MakePackage(@$.filename, @$.first_line, PACKAGE_SYSTEM, $2, $4, $5, $6, $8, NULL,
                                             @1.first_line, @2.last_line, @8.first_line, @8.last_line));
                      }
                    | SUBSYSTEM IDENTIFIER IS declaration_package_list package_initialisation package_finalisation END IDENTIFIER
                      {
                        $$ = Elt(@$.filename, @$.first_line, @$.last_line, PACKAGE_DECLARATION_ELT,
                                 MakePackage(@$.filename, @$.first_line, PACKAGE_SUBSYSTEM, $2, $4, $5, $6, $8, NULL,
                                             @1.first_line, @2.last_line, @8.first_line, @8.last_line));
                      }
                    | SYSTEM IDENTIFIER USING '(' representation_list ')' IS declaration_package_list package_initialisation package_finalisation END IDENTIFIER
                      {
                        $$ = Elt(@$.filename, @$.first_line, @$.last_line, PACKAGE_DECLARATION_ELT,
                                 MakePackage(@$.filename, @$.first_line, PACKAGE_SYSTEM, $2, $8, $9, $10, $12, $5,
                                             @1.first_line, @6.last_line, @12.first_line, @12.last_line));
                      }
                    | SUBSYSTEM IDENTIFIER USING '(' representation_list ')' IS declaration_package_list package_initialisation package_finalisation END IDENTIFIER
                      {
                        $$ = Elt(@$.filename, @$.first_line, @$.last_line, PACKAGE_DECLARATION_ELT,
                                 MakePackage(@$.filename, @$.first_line, PACKAGE_SUBSYSTEM, $2, $8, $9, $10, $12, $5,
                                             @1.first_line, @6.last_line, @12.first_line, @12.last_line));
                      }
                    | PACKAGE IDENTIFIER IS declaration_package_list package_initialisation package_finalisation END IDENTIFIER
                      {
                        $$ = Elt(@$.filename, @$.first_line, @$.last_line, PACKAGE_DECLARATION_ELT,
                                 MakePackage(@$.filename, @$.first_line, PACKAGE_PACKAGE, $2, $4, $5, $6, $8,
                                             NULL,
                                             @1.first_line, @2.last_line, @7.first_line, @8.last_line));
                      }
                    | PACKAGE IDENTIFIER USING '(' representation_list ')' IS declaration_package_list package_initialisation package_finalisation END IDENTIFIER
                      {
                        $$ = Elt(@$.filename, @$.first_line, @$.last_line, PACKAGE_DECLARATION_ELT,
                                 MakePackage(@$.filename, @$.first_line, PACKAGE_PACKAGE, $2, $8, $9, $10, $12,
                                             $5,
                                             @1.first_line, @2.last_line, @11.first_line, @12.last_line));
                      }

                    | PACKAGE IDENTIFIER IS NEW package_identifier '(' generic_actual_parameter_list ')'
                      {
                        $$ = Elt(@$.filename, @$.first_line, @$.last_line, GENERIC_INSTANTIATION_ELT,
                                 MakeGenericInstantiation(PACKAGE_PACKAGE, $2, $5, $7,
                                                          @1.first_line, @8.last_line));
                      }
                    | SUBSYSTEM IDENTIFIER IS NEW package_identifier '(' generic_actual_parameter_list ')'
                      {
                        $$ = Elt(@$.filename, @$.first_line, @$.last_line, GENERIC_INSTANTIATION_ELT,
                                 MakeGenericInstantiation(PACKAGE_SUBSYSTEM, $2, $5, $7,
                                                          @1.first_line, @8.last_line));
                      }
                    | PACKAGE IDENTIFIER IS SEPARATE
                      {
                        $$ = Elt(@$.filename, @$.first_line, @$.last_line, SEPARATE_DECLARATION_ELT,
                                 MakeSeparate(PACKAGE_PACKAGE, FALSE, $2, NULL));
                      }
                    | SUBSYSTEM IDENTIFIER IS SEPARATE
                      {
                        $$ = Elt(@$.filename, @$.first_line, @$.last_line, SEPARATE_DECLARATION_ELT,
                                 MakeSeparate(PACKAGE_SUBSYSTEM, FALSE, $2, NULL));
                      }
                    | PACKAGE IDENTIFIER IS SEPARATE '(' STRING ')'
                      {
                        $$ = Elt(@$.filename, @$.first_line, @$.last_line, SEPARATE_DECLARATION_ELT,
                                 MakeSeparate(PACKAGE_PACKAGE, FALSE, $2, $6));
                      }
                    | SUBSYSTEM IDENTIFIER IS SEPARATE '(' STRING ')'
                      {
                        $$ = Elt(@$.filename, @$.first_line, @$.last_line, SEPARATE_DECLARATION_ELT,
                                 MakeSeparate(PACKAGE_SUBSYSTEM, FALSE, $2, $6));
                      }
                    | GENERIC generic_parameter_list package_declaration
                      {
                        $$ = Elt(@$.filename, @$.first_line, @$.last_line, GENERIC_DECLARATION_ELT,
                                 MakeGeneric($2, $3));
                      }
                    | GENERIC PACKAGE IDENTIFIER IS SEPARATE
                      {
                        $$ = Elt(@$.filename, @$.first_line, @$.last_line, SEPARATE_DECLARATION_ELT,
                                 MakeSeparate(PACKAGE_PACKAGE, TRUE, $3, NULL));
                      }
                    | GENERIC PACKAGE IDENTIFIER IS SEPARATE '(' STRING ')'
                      {
                        $$ = Elt(@$.filename, @$.first_line, @$.last_line, SEPARATE_DECLARATION_ELT,
                                 MakeSeparate(PACKAGE_PACKAGE, TRUE, $3, $7));
                      }
                    | GENERIC SUBSYSTEM IDENTIFIER IS SEPARATE
                      {
                        $$ = Elt(@$.filename, @$.first_line, @$.last_line, SEPARATE_DECLARATION_ELT,
                                 MakeSeparate(PACKAGE_SUBSYSTEM, TRUE, $3, NULL));
                      }
                    | GENERIC SUBSYSTEM IDENTIFIER IS SEPARATE '(' STRING ')'
                      {
                        $$ = Elt(@$.filename, @$.first_line, @$.last_line, SEPARATE_DECLARATION_ELT,
                                 MakeSeparate(PACKAGE_SUBSYSTEM, TRUE, $3, $7));
                      }
                    ;

package_initialisation : /* empty */
                         {
                           $$ = NULL;
                         }
                       | BEGIN_SYM stmt_list
                         {
                           $$ = $2;
                         }
                       ;

package_finalisation : /* empty */
                       {
                         $$ = NULL;
                       }
                     | FINAL stmt_list
                       {
                         $$ = $2;
                       }
                     ;

procedure_declaration : PROCEDURE IDENTIFIER formal_parameters optional_representation share_clause IS declaration_procedure_list BEGIN_SYM stmt_list optional_unit_test END IDENTIFIER
            {
              $$ = Elt(@$.filename, @$.first_line, @$.last_line, PROCEDURE_DECLARATION_ELT,
                       MakeProcedure(@$.filename, @$.first_line, $2, $3, NULL, $7, $9, $12, $5, $4, $10, @5.last_line));
            }
          | PROCEDURE IDENTIFIER formal_parameters optional_representation share_clause IS SEPARATE
            {
              $$ = Elt(@$.filename, @$.first_line, @$.last_line, PROCEDURE_DECLARATION_ELT,
                       MakeProcedure(@$.filename, @$.first_line, $2, $3, NULL, NULL, NULL, $2, MakeShareClause(NULL),
                                     $4, NULL, @7.last_line));
            }
          ;

function_declaration : FUNCTION IDENTIFIER formal_parameters ':' function_return_type optional_representation IS declaration_procedure_list BEGIN_SYM stmt_list optional_unit_test END IDENTIFIER
            {
              $$ = Elt(@$.filename, @$.first_line, @$.last_line, PROCEDURE_DECLARATION_ELT,
                       MakeProcedure(@$.filename, @$.first_line, $2, $3, SetVarName($5, $2), $8, $10, $13,
                                     MakeShareClause(NULL), $6, $11, @6.last_line));
            }
          | FUNCTION IDENTIFIER formal_parameters ':' function_return_type optional_representation IS SEPARATE
            {
              $$ = Elt(@$.filename, @$.first_line, @$.last_line, PROCEDURE_DECLARATION_ELT,
                       MakeProcedure(@$.filename, @$.first_line, $2, $3, SetVarName($5, $2), NULL, NULL, $2,
                                     MakeShareClause(NULL), $6, NULL, @8.last_line));
            }
          ;

share_clause         : /* empty */
                       {
                         $$ = MakeShareClause(/*FALSE,*/ NULL);
                       }
/*
                     | WITH_SYM SHARED_SYM
                       {
                         $$ = MakeShareClause(TRUE, NULL);
                       }
*/
                     | WHEN expr1
                       {
                         $$ = MakeShareClause(/*FALSE,*/ $2);
                       }
                     ;

optional_unit_test : /* empty */
                     {
                       $$ = NULL;
                     }
                   | UNIT_TEST stmt_list
                     {
                       /* The prefix is not strictly required, but adjusting the line is */
                       $$ = SetPrefix($2, PREFIX_UNIT_TEST, @$.first_line);
                     }
                   ;

function_return_type : type_identifier
                       {
                         $$ = Elt(@$.filename, @$.first_line, @$.last_line, VAR_DECLARATION_ELT,
                                  DeclareVariable(NULL, $1, NULL, NULL, NULL, MODE_LOCAL,
                                                  NULL, NAME_NONE));
                       }
                     | basic_type_spec
                       {
                         $$ = Elt(@$.filename, @$.first_line, @$.last_line, VAR_DECLARATION_ELT,
                                  DeclareVariable(NULL, NULL, SetRepresentation($1, NULL), NULL, NULL, MODE_LOCAL,
                                                  NULL, NAME_NONE));
                       }
                     ;

formal_parameters : /* Empty */
                    {
                      $$ = NULL;
                    }
                  | '(' formal_parameter_list ')'
                    {
                      $$ = $2;
                    }
                  ;

formal_parameter_list : formal_parameter
                        {
                          $$ = $1;
                        }
                      | formal_parameter ';' formal_parameter_list
                        {
                          $$ = AppendStmts($1, $3);
                        }
                      ;

formal_parameter : identifier_list ':' mode_spec type_identifier optional_representation
                   {
                     $$ = Elt(@$.filename, @$.first_line, @$.last_line, VAR_DECLARATION_ELT,
                              DeclareVariable($1, $4, NULL, NULL, NULL, $3, $5, NAME_NONE));
                   }
                 | identifier_list ':' mode_spec basic_type_spec optional_representation
                   {
                     $$ = Elt(@$.filename, @$.first_line, @$.last_line, VAR_DECLARATION_ELT,
                              DeclareVariable($1, NULL, SetRepresentation($4, NULL), NULL, NULL, $3, $5, NAME_NONE));
                   }
                 ;

mode_spec : IN_SYM
            {
              $$ = MODE_IN;
            }
          | OUT_SYM
            {
              $$ = MODE_OUT;
            }
          | IN_SYM OUT_SYM
            {
              $$ = MODE_IN_OUT;
            }
          | FINAL IN_SYM OUT_SYM
            {
              $$ = MODE_FINAL_IN_OUT;
            }
          ;

declaration_procedure_list : /* Empty */
                   {
                     $$ = NULL;
                   }
                 | declaration_procedure_list declaration_procedure
                   {
                     $$ = AppendStmts($1, $2);
                   }
                 ;

declaration_package_list : /* Empty */
                   {
                     $$ = NULL;
                   }
                 | declaration_package_list declaration_package
                   {
                     $$ = AppendStmts($1, $2);
                   }
                 ;

declaration_package : declaration_global
                      {
                        $$ = SetAccess($1, PRIVATE_ACCESS);
                      }
                    | PUBLIC_SYM declaration_global
                      {
                        $$ = SetAccess($2, PUBLIC_ACCESS);
                      }
                    | PUBLIC_SYM OUT_SYM var_declaration ';'
                      {
                        $$ = SetAccess(SetMode(SetPublicOut($3), MODE_GLOBAL), PUBLIC_ACCESS);
                      }
                    | PUBLIC_SYM OUT_SYM SHARED_SYM var_declaration ';'
                      {
                        $$ = SetAccess(SetMode(SetPublicOut($4), MODE_SHARED), PUBLIC_ACCESS);
                      }
                    | code_line
                      {
                        $$ = $1;
                      }
                    | pragma ';'
                      {
                        $$ = $1;
                      }
                    | comment_line
                      {
                        $$ = $1;
                      }
                    | DECLARE SEPARATE IDENTIFIER BEGIN_SYM stmt_list END IDENTIFIER ';'
                      {
                        $$ = Elt(@$.filename, @$.first_line, @$.last_line,
                                 SEPARATE_BLOCK_ELT, MakeSeparateBlock(@$.filename, @$.first_line, $3, $5, $7));
                      }
                    | DECLARE SEPARATE IDENTIFIER DECLARE declaration_package_list END IDENTIFIER ';'
                      {
                        $$ = Elt(@$.filename, @$.first_line, @$.last_line,
                                 SEPARATE_BLOCK_ELT, MakeSeparateBlock(@$.filename, @$.first_line, $3, $5, $7));
                      }
                    | USE_SYM SEPARATE package_identifier ';'
                      {
                        $$ = Elt(@$.filename, @$.first_line, @$.last_line,
                                 SEPARATE_STMT_ELT, MakeSeparateStmt($3));
                      }
                    ;

code_line : CODE
            {
              $$ = Elt(@$.filename, @$.first_line, @$.last_line, CODE_ELT, $1);
            }
          | HEADER_CODE
            {
              $$ = Elt(@$.filename, @$.first_line, @$.last_line, HEADER_CODE_ELT, $1);
            }
          ;

comment_line : COMMENT
               {
                 $$ = Elt(@$.filename, @$.first_line, @$.last_line, COMMENT_ELT, $1);
               }
             ;

declaration_global
            : type_declaration ';'
              {
                $$ = $1;
              }
            | var_declaration ';'
              {
                $$ = SetMode($1, MODE_GLOBAL);
              }
            | SHARED_SYM var_declaration ';'
              {
                $$ = SetMode($2, MODE_SHARED);
              }
            | unit_declaration ';'
              {
                $$ = $1;
              }
            | procedure_declaration ';'
              {
                $$ = $1;
              }
            | CLOSED procedure_declaration ';'
              {
                $$ = SetPrefix($2, PREFIX_CLOSED, @$.first_line);
              }
            | SHARED_SYM procedure_declaration ';'
              {
                $$ = SetPrefix($2, PREFIX_SHARED, @$.first_line);
              }
            | MAIN procedure_declaration ';'
              {
                $$ = SetPrefix($2, PREFIX_MAIN, @$.first_line);
              }
            | UNIT_TEST procedure_declaration ';'
              {
                $$ = SetPrefix($2, PREFIX_UNIT_TEST, @$.first_line);
              }
            | NOT UNIT_TEST procedure_declaration ';'
              {
                $$ = SetPrefix($3, PREFIX_NOT_UNIT_TEST, @$.first_line);
              }
            | function_declaration ';'
              {
                $$ = $1;
              }
            | CLOSED function_declaration ';'
              {
                $$ = SetPrefix($2, PREFIX_CLOSED, @$.first_line);
              }
            | SHARED_SYM function_declaration ';'
              {
                $$ = SetPrefix($2, PREFIX_SHARED, @$.first_line);
              }
            | UNIT_TEST function_declaration ';'
              {
                $$ = SetPrefix($2, PREFIX_UNIT_TEST, @$.first_line);
              }
            | NOT UNIT_TEST function_declaration ';'
              {
                $$ = SetPrefix($3, PREFIX_NOT_UNIT_TEST, @$.first_line);
              }
            | package_declaration ';'
              {
                $$ = $1;
              }
            | SHARED_SYM package_declaration ';'
              {
                $$ = SetPrefix($2, PREFIX_SHARED, @$.first_line);
              }
            | UNIT_TEST package_declaration ';'
              {
                $$ = SetPrefix($2, PREFIX_UNIT_TEST, @$.first_line);
              }
            | NOT UNIT_TEST package_declaration ';'
              {
                $$ = SetPrefix($3, PREFIX_NOT_UNIT_TEST, @$.first_line);
              }
            | constant_declaration ';'
              {
                $$ = $1;
              }
            ;

declaration_procedure : type_declaration ';'
                        {
                          $$ = $1;
                        }
                      | var_declaration ';'
                        {
                          $$ = $1; // The modes are set for local by default; allow in out for 'name for'
                        }
                      | constant_declaration ';'
                        {
                          $$ = $1;
                        }
                      | unit_declaration ';'
                        {
                          $$ = $1;
                        }
                      | code_line
                        {
                          $$ = $1;
                        }
                      | comment_line
                        {
                          $$ = $1;
                        }
                      ;

/* A statement list cannot be empty */
stmt_list : stmt nstmt_list
            {
              $$ = AppendStmts($1, $2);
            }
          ;

nstmt_list : /* empty */
             {
               $$ = NULL;
             }
           | nstmt_list stmt
             {
               $$ = AppendStmts($1, $2);
             }
           ;

stmt       : assignment ';'
           | if_block ';'
           | loop_stmt ';'
           | do_stmt ';'
           | while_stmt ';'
           | case_stmt ';'
           | for_stmt ';'
           | null_stmt ';'
           | procedure_call ';'
           | code_line
           | pragma ';'
           | declare_block ';'
           | unit_test_block ';'
           | free_stmt ';'
           | restart_stmt ';'
           | comment_line
           | separate_stmt ';'
           | assert_stmt ';'
           | advise_stmt ';'
           ;

type_declaration : TYPE IDENTIFIER
                   {
                      $$ = Elt(@$.filename, @$.first_line, @$.last_line, TYPE_DECLARATION_ELT,
                               AddType($2, NULL));
                   }
                 | TYPE IDENTIFIER IS package_type_spec
                   {
                      $$ = Elt(@$.filename, @$.first_line, @$.last_line, TYPE_DECLARATION_ELT,
                               AddType($2, $4));
                   }
                 | TYPE IDENTIFIER IS CONTROLLED USING '(' representation_list ')'
                   {
                     $$ = Elt(@$.filename, @$.first_line, @$.last_line, APPEND_REPRESENTATION_ELT, AddRepresentation($2, $7));
                   }
                 ;

var_declaration : identifier_list ':' type_identifier optional_value optional_representation
                  {
                    $$ = Elt(@$.filename, @$.first_line, @$.last_line, VAR_DECLARATION_ELT,
                             DeclareVariable($1, $3, NULL, $4, NULL, MODE_LOCAL, $5, NAME_NONE));
                  }
                | identifier_list ':' type_identifier RENAMES variable_ref optional_representation
                  {
                    $$ = Elt(@$.filename, @$.first_line, @$.last_line, VAR_DECLARATION_ELT,
                             DeclareVariable($1, $3, NULL, NULL, $5, MODE_IN_OUT, $6, NAME_VAR));
                  }
                | identifier_list ':' PACKAGE RENAMES package_identifier //variable_ref
                  {
                    $$ = Elt(@$.filename, @$.first_line, @$.last_line, VAR_DECLARATION_ELT,
                             DeclareVariable($1, NULL, NULL, NULL, $5, MODE_IN_OUT, NULL, NAME_PACKAGE));
                  }
                | identifier_list ':' SUBSYSTEM RENAMES package_identifier //variable_ref
                  {
                    $$ = Elt(@$.filename, @$.first_line, @$.last_line, VAR_DECLARATION_ELT,
                             DeclareVariable($1, NULL, NULL, NULL, $5, MODE_IN_OUT, NULL, NAME_PACKAGE));
                  }
                | identifier_list ':' PROCEDURE RENAMES package_identifier //variable_ref
                  {
                    $$ = Elt(@$.filename, @$.first_line, @$.last_line, VAR_DECLARATION_ELT,
                             DeclareVariable($1, NULL, NULL, NULL, $5, MODE_IN_OUT, NULL, NAME_PROCEDURE));
                  }
                | identifier_list ':' FUNCTION RENAMES package_identifier //variable_ref
                  {
                    $$ = Elt(@$.filename, @$.first_line, @$.last_line, VAR_DECLARATION_ELT,
                             DeclareVariable($1, NULL, NULL, NULL, $5, MODE_IN_OUT, NULL, NAME_FUNCTION));
                  }
                | identifier_list ':' TYPE RENAMES type_identifier
                  {
                    $$ = Elt(@$.filename, @$.first_line, @$.last_line, VAR_DECLARATION_ELT,
                             DeclareVariable($1, $5, NULL, NULL, NULL, MODE_IN_OUT, NULL, NAME_TYPE));
                  }
                | identifier_list ':' UNIT_SYM RENAMES package_identifier //
                  {
                    $$ = Elt(@$.filename, @$.first_line, @$.last_line, VAR_DECLARATION_ELT,
                             DeclareVariable($1, NULL, NULL, NULL, $5, MODE_IN_OUT, NULL, NAME_UNIT));
                  }
                | identifier_list ':' CONSTANT RENAMES variable_ref
                  {
                    $$ = Elt(@$.filename, @$.first_line, @$.last_line, VAR_DECLARATION_ELT,
                             DeclareVariable($1, NULL, NULL, NULL, $5, MODE_IN_OUT, NULL, NAME_CONSTANT));
                  }
                // In-line anonymous types
                | identifier_list ':' basic_type_spec optional_value optional_representation
                  {
                    $$ = Elt(@$.filename, @$.first_line, @$.last_line, VAR_DECLARATION_ELT,
                             DeclareVariable($1, NULL, SetRepresentation($3, NULL), $4, NULL,
                                             MODE_LOCAL, $5, NAME_NONE));
                  }
                | identifier_list ':' CONTROLLED basic_type_spec optional_value optional_representation
                  {
                    $$ = Elt(@$.filename, @$.first_line, @$.last_line, VAR_DECLARATION_ELT,
                             DeclareVariable($1, NULL, SetControlled(SetRepresentation($4, NULL)), $5, NULL,
                                             MODE_LOCAL, $6, NAME_NONE));
                  }
                | identifier_list ':' '(' extended_type_spec ')' optional_value optional_representation
                  {
                    $$ = Elt(@$.filename, @$.first_line, @$.last_line, VAR_DECLARATION_ELT,
                             DeclareVariable($1, NULL, $4, $6, NULL, MODE_LOCAL, $7, NAME_NONE));
                  }
                | identifier_list ':' '(' CONTROLLED extended_type_spec ')' optional_value optional_representation
                  {
                    $$ = Elt(@$.filename, @$.first_line, @$.last_line, VAR_DECLARATION_ELT,
                             DeclareVariable($1, NULL, SetControlled($5), $7, NULL, MODE_LOCAL, $8,
                                             NAME_NONE));
                  }
                // Public versions of the previous four (restructure this)
                | identifier_list ':' PUBLIC_SYM basic_type_spec optional_value optional_representation
                  {
                    $$ = Elt(@$.filename, @$.first_line, @$.last_line, VAR_DECLARATION_ELT,
                             DeclareVariable($1, NULL,
                                             SetTypeSpecAccess(SetRepresentation($4, NULL), PUBLIC_ACCESS),
                                             $5, NULL, MODE_LOCAL, $6, NAME_NONE));
                  }
                | identifier_list ':' PUBLIC_SYM CONTROLLED basic_type_spec optional_value optional_representation
                  {
                    $$ = Elt(@$.filename, @$.first_line, @$.last_line, VAR_DECLARATION_ELT,
                             DeclareVariable(
                               $1, NULL,
                               SetTypeSpecAccess(SetControlled(SetRepresentation($5, NULL)), PUBLIC_ACCESS),
                               $6, NULL, MODE_LOCAL, $7, NAME_NONE));
                  }
                | identifier_list ':' PUBLIC_SYM '(' extended_type_spec ')' optional_value optional_representation
                  {
                    $$ = Elt(@$.filename, @$.first_line, @$.last_line, VAR_DECLARATION_ELT,
                             DeclareVariable(
                               $1, NULL,
                               SetTypeSpecAccess($5, PUBLIC_ACCESS), $7, NULL, MODE_LOCAL, $8, NAME_NONE));
                  }
                | identifier_list ':' PUBLIC_SYM '(' CONTROLLED extended_type_spec ')' optional_value
                    optional_representation
                  {
                    $$ = Elt(@$.filename, @$.first_line, @$.last_line, VAR_DECLARATION_ELT,
                             DeclareVariable($1, NULL, SetTypeSpecAccess(SetControlled($6), PUBLIC_ACCESS),
                                             $8, NULL, MODE_LOCAL, $9, NAME_NONE));
                  }
                ;

identifier_list : IDENTIFIER
                  {
                    $$ = MakeIdentifierListEntry($1);
                  }
                | IDENTIFIER ',' identifier_list
                  {
                    $$ = PrefixIdentifierListEntry(MakeIdentifierListEntry($1), $3);
                  }
                ;

type_identifier : package_identifier
                  {
                    $$ = MakeTypeIdentifier($1, NULL);
                  }
                | package_identifier '\'' attribute_chain
                  {
                    $$ = MakeTypeIdentifier($1, $3);
                  }
                ;

unit_declaration : UNIT_SYM IDENTIFIER
                   {
                     $$ = Elt(@$.filename, @$.first_line, @$.last_line, UNIT_DECLARATION_ELT,
                              DeclareUnit(@$.first_line, $2, NULL));
                   }
                 | UNIT_SYM IDENTIFIER IS unit_list
                   {
                     $$ = Elt(@$.filename, @$.first_line, @$.last_line, UNIT_DECLARATION_ELT,
                              DeclareUnit(@$.first_line, $2, $4));
                   }
                 ;

package_identifier : IDENTIFIER
                     {
                       $$ = MakeReference(IDENTIFIER_REFERENCE, $1, NULL, NULL);
                     }
                   | IDENTIFIER package_ref_list
                     {
                       $$ = PrefixReference(
                              MakeReference(IDENTIFIER_REFERENCE, $1, NULL, NULL), $2);
                     }
                   ;

package_ref_list : package_ref_term
                   {
                     $$ = $1;
                   }
                 | package_ref_term package_ref_list
                   {
                     $$ = PrefixReference($1, $2);
                   }
                 ;

package_ref_term : '.' IDENTIFIER
                   {
                     $$ = MakeReference(FIELD_REFERENCE, $2, NULL, NULL);
                   }
                 ;

optional_value : /* Empty */
                 {
                   $$ = NULL;
                 }
               | ASSIGN rhs
                 {
                   $$ = $2;
                 }
               ;

optional_representation : /* Empty */
                          {
                            $$ = NULL;
                          }
                        | USING '(' representation_list ')'
                          {
                            $$ = $3;
                          }
                        ;

package_type_spec : type_spec
                    {
                       $$ = $1;
                    }
                  | PUBLIC_SYM type_spec
                    {
                       $$ = SetTypeSpecAccess($2, PUBLIC_ACCESS);
                    }
                  ;

basic_type_spec : integer_type_spec
            {
              $$ = $1;
            }
          | enumeration_type_spec
            {
              $$ = $1;
            }
          | floating_type_spec
            {
              $$ = $1;
            }
          | array_type_spec
            {
              $$ = $1;
            }
          | record_type_spec
            {
              $$ = $1;
            }
          | access_type_spec
            {
              $$ = $1;
            }
          ;

extended_type_spec : basic_type_spec optional_representation
                     {
                       $$ = SetRepresentation($1, $2);
                     }
                   ;

type_spec : extended_type_spec
            {
              $$ = $1;
            }
          | CONTROLLED extended_type_spec
            {
              $$ = SetControlled($2);
            }
          ;

integer_type_spec
          : RANGE expr1 DOTDOT expr1 unit_spec
            {
              $$ = MakeIntegerType(@$.first_line, $2, $4, $5);
            }
          | RANGE FOR_SYM expr1
            {
              $$ = MakeIntegerType(@$.first_line, NULL, $3, NULL);
            }
          ;
/*
package_integer_type_spec : integer_type_spec
                            {
                              $$ = $1;
                            }
                          | PUBLIC_SYM integer_type_spec
                            {
                              $$ = SetTypeSpecAccess($2, PUBLIC_ACCESS);
                            }
                          ;
*/

floating_type_spec : DIGITS INTEGER MAGNITUDE INTEGER unit_spec
                     {
                       $$ = MakeFloatingType(@$.first_line, $2, $4, $5);
                     }
                   | MAGNITUDE INTEGER DIGITS INTEGER unit_spec
                     {
                       $$ = MakeFloatingType(@$.first_line, $4, $2, $5);
                     }
                   ;

unit_spec : /* empty */
            {
              $$ = NULL;
            }
          | UNIT_SYM unit_list
            {
              $$ = $2;
            }
          | NEW UNIT_SYM
            {
              $$ = MakeUnitTerm(NULL, 1, FALSE);
            }
          ;

unit_list : unit
            {
              $$ = $1;
            }
          | unit '*' unit_list
            {
              $$ = PrefixUnitTerm($1, $3);
            }
          ;

unit : package_identifier
       {
         $$ = MakeUnitTerm($1, 1, FALSE);
       }
     | package_identifier '^' INTEGER
       {
         $$ = MakeUnitTerm($1, $3, FALSE);
       }
     | package_identifier '\'' UNIT_SYM
       {
         $$ = MakeUnitTerm($1, 1, TRUE);
       }
     | package_identifier '\'' UNIT_SYM '^' INTEGER
       {
         $$ = MakeUnitTerm($1, $5, TRUE);
       }
     ;

enumeration_type_spec : '(' enum_symbol_list ')'
                        {
                          $$ = MakeEnumType(@$.first_line, $2);
                        }
                      ;

enum_symbol_list : enum_symbol
                   {
                     $$ = $1;
                   }
                 | enum_symbol ',' enum_symbol_list
                   {
                     $$ = PrefixEnumSymbol($1, $3);
                   }
                 ;

enum_symbol : IDENTIFIER optional_representation
           {
             $$ = MakeEnumSymbol($1, $2);
           }
         ;

type_ref : type_identifier
           {
             $$ = $1;
           }
         ;

type_ref_or_spec : type_ref
                   {
                     $$ = MakeTypeRefOrSpec($1, NULL);
                   }
                 | basic_type_spec
                   {
                     $$ = MakeTypeRefOrSpec(NULL, $1);
                   }
                 | '(' extended_type_spec ')'
                   {
                     $$ = MakeTypeRefOrSpec(NULL, $2);
                   }
                 ;

/*
integer_type_ref : package_identifier
                   {
                     $$ = $1;
                   }
/*
                 | package_integer_type_spec
                   {
                     $$ = MakeTypeRef(NULL, $1);
                   }
*/
                 ;

array_index : type_identifier
              {
                $$ = MakeArrayIndex(INDEX_NAME, $1, NULL);
              }
            | integer_type_spec
              {
                $$ = MakeArrayIndex(INDEX_SPEC, NULL, $1);
              }
            ;

array_type_spec : ARRAY_SYM '[' array_index ']' OF_SYM type_ref_or_spec
                  {
                    $$ = MakeArrayType(@$.first_line, $3, $6, TRUE, TRUE);
                  }
                | ARRAY_SYM OPENLT array_index OPENGT OF_SYM type_ref_or_spec
                  {
                    $$ = MakeArrayType(@$.first_line, $3, $6, FALSE, FALSE);
                  }
                | ARRAY_SYM '[' array_index OPENGT OF_SYM type_ref_or_spec
                  {
                    $$ = MakeArrayType(@$.first_line, $3, $6, TRUE, FALSE);
                  }
                | ARRAY_SYM OPENLT array_index ']' OF_SYM type_ref_or_spec
                  {
                    $$ = MakeArrayType(@$.first_line, $3, $6, FALSE, TRUE);
                  }
                ;

record_type_spec : RECORD_SYM field_list END RECORD_SYM
                   {
                     $$ = MakeRecordType(@$.first_line, $2, REC_RECORD);
                   }
                 | UNION_SYM field_list END UNION_SYM
                   {
                     $$ = MakeRecordType(@$.first_line, $2, REC_UNION);
                   }
                 | UNCHECKED_SYM UNION_SYM field_list END UNION_SYM
                   {
                     $$ = MakeRecordType(@$.first_line, $3, REC_UNCHECKED_UNION);
                   }
                 ;

field_list : field_spec
             {
               $$ = $1;
             }
           | field_spec field_list
             {
               $$ = PrefixFieldSpec($1, $2);
             }
           ;

field_spec : IDENTIFIER ':' type_ref_or_spec optional_representation ';'
             {
               $$ = MakeFieldSpec($1, $3, $4, NULL, NULL);
             }
           | IDENTIFIER ':' NULL_SYM ';'
             {
               $$ = MakeFieldSpec($1, NULL, NULL, NULL, NULL);
             }
           | CODE
             {
               $$ = MakeFieldSpec(NULL, NULL, NULL, $1, NULL);
             }
           | COMMENT
             {
               $$ = MakeFieldSpec(NULL, NULL, NULL, NULL, $1);
             }
           ;

access_type_spec : MANAGED_SYM ACCESS_SYM OR NULL_SYM type_ref_or_spec
                   {
                     $$ = MakeAccessType(@$.first_line, $5, TRUE, FALSE, FALSE, FALSE, FALSE);
                   }
                 | MANAGED_SYM ACCESS_SYM type_ref_or_spec
                   {
                     $$ = MakeAccessType(@$.first_line, $3, FALSE, FALSE, FALSE, FALSE, FALSE);
                   }
                 | ACCESS_SYM OR NULL_SYM type_ref_or_spec
                   {
                     $$ = MakeAccessType(@$.first_line, $4, TRUE, TRUE, FALSE, FALSE, FALSE);
                   }
                 | ACCESS_SYM type_ref_or_spec
                   {
                     $$ = MakeAccessType(@$.first_line, $2, FALSE, TRUE, FALSE, FALSE, FALSE);
                   }
                 | ACCESS_SYM OR NULL_SYM NEW type_ref_or_spec
                   {
                     $$ = MakeAccessType(@$.first_line, $5, TRUE, TRUE, FALSE, FALSE, TRUE);
                   }
                 | ACCESS_SYM NEW type_ref_or_spec
                   {
                     $$ = MakeAccessType(@$.first_line, $3, FALSE, TRUE, FALSE, FALSE, TRUE);
                   }
                 | MANAGED_SYM ACCESS_SYM OR NULL_SYM CONSTANT type_ref_or_spec
                   {
                     $$ = MakeAccessType(@$.first_line, $6, TRUE, FALSE, TRUE, FALSE, FALSE);
                   }
                 | MANAGED_SYM ACCESS_SYM CONSTANT type_ref_or_spec
                   {
                     $$ = MakeAccessType(@$.first_line, $4, FALSE, FALSE, TRUE, FALSE, FALSE);
                   }
                 | ACCESS_SYM OR NULL_SYM CONSTANT type_ref_or_spec
                   {
                     $$ = MakeAccessType(@$.first_line, $5, TRUE, TRUE, TRUE, FALSE, FALSE);
                   }
                 | ACCESS_SYM CONSTANT type_ref_or_spec
                   {
                     $$ = MakeAccessType(@$.first_line, $3, FALSE, TRUE, TRUE, FALSE, FALSE);
                   }
                 | ACCESS_SYM OR NULL_SYM CONSTANT NEW type_ref_or_spec
                   {
                     $$ = MakeAccessType(@$.first_line, $6, TRUE, TRUE, TRUE, FALSE, TRUE);
                   }
                 | ACCESS_SYM CONSTANT NEW type_ref_or_spec
                   {
                     $$ = MakeAccessType(@$.first_line, $4, FALSE, TRUE, TRUE, FALSE, TRUE);
                   }
                 | PERSISTENT ACCESS_SYM type_ref_or_spec
                   {
                     $$ = MakeAccessType(@$.first_line, $3, FALSE, TRUE, FALSE, TRUE, FALSE);
                   }
                 | PERSISTENT ACCESS_SYM CONSTANT type_ref_or_spec
                   {
                     $$ = MakeAccessType(@$.first_line, $4, FALSE, TRUE, TRUE, TRUE, FALSE);
                   }
                 ;

integer_var_declaration
                : IDENTIFIER ':' type_identifier
                  {
                    $$ = Elt(@$.filename, @$.first_line, @$.last_line, VAR_DECLARATION_ELT,
                             DeclareVariable(MakeIdentifierListEntry($1), $3, NULL, NULL, NULL,
                                             MODE_LOCAL, NULL, NAME_NONE));
                  }
                | IDENTIFIER ':' integer_type_spec
                  {
                    $$ = Elt(@$.filename, @$.first_line, @$.last_line, VAR_DECLARATION_ELT,
                             DeclareVariable(MakeIdentifierListEntry($1), NULL, $3, NULL, NULL,
                                             MODE_LOCAL, NULL, NAME_NONE));
                  }
                ;

assignment : variable_ref ASSIGN NEW rhs
             {
               $$ = Elt(@$.filename, @$.first_line, @$.last_line, ASSIGNMENT_ELT, Assignment($1, $4, TRUE));
             }
           | variable_ref ASSIGN rhs
             {
               $$ = Elt(@$.filename, @$.first_line, @$.last_line, ASSIGNMENT_ELT, Assignment($1, $3, FALSE));
             }
           ;

rhs : expr1
      {
        $$ = MakeRHS(EXPR_RHS, $1, NULL, NULL);
      }
    | array_initialisation
      {
        $$ = MakeRHS(ARRAY_RHS, NULL, $1, NULL);
      }
    | record_initialisation
      {
        $$ = MakeRHS(RECORD_RHS, NULL, NULL, $1);
      }
    ;

variable_ref : IDENTIFIER
               {
                 $$ = MakeReference(IDENTIFIER_REFERENCE, $1, NULL, NULL);
               }
             | IDENTIFIER deref_list
               {
                 $$ = PrefixReference(
                        MakeReference(IDENTIFIER_REFERENCE, $1, NULL, NULL), $2);
               }
             ;

deref_list : deref_term
             {
               $$ = $1;
             }
           | deref_term deref_list
             {
               $$ = PrefixReference($1, $2);
             }
           ;

deref_term : '[' expr1 ']'
             {
               $$ = MakeReference(ARRAY_REFERENCE, NULL, $2, NULL);
             }
           | '[' expr1 DOTDOT expr1 ']'
             {
               $$ = MakeReference(ARRAY_SLICE_REFERENCE, NULL, $2, $4);
             }
           | '[' expr1 DOTDOT ']'
             {
               $$ = MakeReference(ARRAY_SLICE_REFERENCE, NULL, $2, NULL);
             }
           | '[' expr1 FOR_SYM expr1 ']'
             {
               $$ = MakeReference(ARRAY_SLICE_LENGTH_REFERENCE, NULL, $2, $4);
             }
           | '.' IDENTIFIER
             {
               $$ = MakeReference(FIELD_REFERENCE, $2, NULL, NULL);
             }
           ;

if_block : IF expr1 THEN stmt_list else_part END IF
           {
             $$ = Elt(@$.filename, @$.first_line, @$.last_line, WHEN_ELT, When($2, $4, $5, FALSE));
           }
         ;

else_part : /* empty */
            {
              $$ = NULL;
            }
          | ELSIF expr1 THEN stmt_list else_part
            {
              $$ = Elt(@$.filename, @$.first_line, @$.last_line, WHEN_ELT, When($2, $4, $5, FALSE));
            }
          | ELSE stmt_list
            {
              $$ = $2;
            }
          ;

loop_stmt : LOOP exit_repeat_list END LOOP
            {
              $$ = Elt(@$.filename, @$.first_line, @$.last_line, LOOP_ELT, Loop($2));
            }
          ;

do_stmt : DO exit_list END DO
          {
            $$ = Elt(@$.filename, @$.first_line, @$.last_line, DO_ELT, DoStmt($2));
          }
        ;

while_stmt : WHILE expr1 LOOP stmt_list END LOOP
             {
               $$ = Elt(@$.filename, @$.first_line, @$.last_line, WHILE_ELT, WhileStmt($2, $4));
             }
           ;
/*
  Exit_list is organised so that do statements cannot be empty,
  but unnecessary null statements will not be added.
  An exit statement cannot be the last statement in the list, because
  the do will exit from there anyway
*/
exit_list : stmt
            {
              $$ = $1;
            }
           | stmt exit_list
             {
               $$ = AppendStmts($1, $2);
             }
           | EXIT WHEN expr1 ';' exit_list
             {
               $$ = Elt(@$.filename, @$.first_line, @$.last_line, WHEN_ELT,
                        When($3, Elt(@$.filename, @$.first_line, @$.last_line, EXIT_ELT, NULL), $5, FALSE));
             }
           ;

/*
  Exit_repeat_list is similar to exit_list, but allows repeats as well.
  A repeat cannot be the last statement (since the loop will repeat
  unconditionally from there anyway)
*/
exit_repeat_list : stmt
            {
              $$ = $1;
            }
           | stmt exit_repeat_list
             {
               $$ = AppendStmts($1, $2);
             }
           | EXIT WHEN expr1 ';' exit_repeat_list
             {
               $$ = Elt(@$.filename, @$.first_line, @$.last_line, WHEN_ELT,
                        When($3, Elt(@$.filename, @$.first_line, @$.last_line, EXIT_ELT, NULL), $5, TRUE));
             }
           | REPEAT WHEN expr1 ';' exit_repeat_list
             {
               $$ = Elt(@$.filename, @$.first_line, @$.last_line, WHEN_ELT,
                        When($3, Elt(@$.filename, @$.first_line, @$.last_line, REPEAT_ELT, NULL), $5, FALSE));
             }
           | EXIT WHEN expr1 ';'                /* No statements follow the exit statement */
             {
               $$ = Elt(@$.filename, @$.first_line, @$.last_line, WHEN_ELT,
                        When($3, Elt(@$.filename, @$.first_line, @$.last_line, EXIT_ELT, NULL), NULL, TRUE));
             }
           ;

null_stmt : NULL_SYM
            {
              $$ = Elt(@$.filename, @$.first_line, @$.last_line, NULL_ELT, NULL);
            }
           ;

case_stmt : CASE_SYM expr1 IS case_option_list END CASE_SYM
            {
              $$ = Elt(@$.filename, @$.first_line, @$.last_line, CASE_ELT, Case($2, $4));
            }
          ;

case_option_list : /* Empty */
                   {
                     $$ = NULL;
                   }
                 | case_option_list case_option
                   {
                     $$ = AppendCaseOption($1, $2);
                   }
                 ;

case_option : WHEN case_range_list ARROW stmt_list
              {
                $$ = BuildCaseOption($2, $4);
              }
            ;

case_range_list : case_range
                  {
                    $$ = $1;
                  }
                | case_range '|' case_range_list
                  {
                    PrefixRange($1, $3);
                  }
                ;

case_range : expr1
             {
               $$ = MakeRange($1, $1);
             }
             /* Don't allow ranges (could lead to values being missed)
             | expr1 DOTDOT expr1
               {
                 $$ = MakeRange($1, $3);
               }
             */
           ;

for_stmt : FOR_SYM integer_var_declaration IN_SYM expr1 DOTDOT expr1 LOOP stmt_list END LOOP
           {
             $$ = Elt(@$.filename, @$.first_line, @$.last_line, FOR_ELT, ForStmt($2, $4, $6, $8));
           }
         | FOR_SYM integer_var_declaration LOOP stmt_list END LOOP
           {
             $$ = Elt(@$.filename, @$.first_line, @$.last_line, FOR_ELT, ForStmt($2, NULL, NULL, $4));
           }
         ;

/*
constant_expr : INTEGER
                {
                  $$ = MakeConstant($1, NULL);
                }
              | CONSTANT_IDENTIFIER
                {
                  $$ = MakeConstant(0, $1);
                }
              ;
*/

expr1      : expr1 '+' expr1
             {
               $$ = NewNode(@2.filename, @2.first_line, _PLUS, $1, $3, 0, 0.0, NULL, NULL, NULL, NULL);
             }
           | expr1 '-' expr1
             {
               $$ = NewNode(@2.filename, @2.first_line, _MINUS, $1, $3, 0, 0.0, NULL, NULL, NULL, NULL);
             }
           | expr1 '*' expr1
             {
               $$ = NewNode(@2.filename, @2.first_line, _TIMES, $1, $3, 0, 0.0, NULL, NULL, NULL, NULL);
             }
           | expr1 '/' expr1
             {
               $$ = NewNode(@2.filename, @2.first_line, _DIVIDE, $1, $3, 0, 0.0, NULL, NULL, NULL, NULL);
             }
           | expr1 MOD expr1
             {
               $$ = NewNode(@2.filename, @2.first_line, _MOD, $1, $3, 0, 0.0, NULL, NULL, NULL, NULL);
             }
           | '-' expr1 %prec UMINUS
             {
               $$ = NewNode(@$.filename, @$.first_line, _TIMES,
                            NewNode(@$.filename, @$.first_line,
                                    _CONST_INT, NULL, NULL, -1, 0.0, NULL, NULL, NULL, NULL),
                            $2, 0, 0.0, NULL, NULL, NULL, NULL);
             }
           | expr1 IAND expr1
             {
               $$ = NewNode(@2.filename, @2.first_line, _IAND, $1, $3, 0, 0.0, NULL, NULL, NULL, NULL);
             }
           | expr1 IOR expr1
             {
               $$ = NewNode(@2.filename, @2.first_line, _IOR, $1, $3, 0, 0.0, NULL, NULL, NULL, NULL);
             }
           | expr1 IXOR expr1
             {
               $$ = NewNode(@2.filename, @2.first_line, _IXOR, $1, $3, 0, 0.0, NULL, NULL, NULL, NULL);
             }
           | expr1 LT expr1
             {
               $$ = NewNode(@2.filename, @2.first_line, _LT, $1, $3, 0, 0.0, NULL, NULL, NULL, NULL);
             }
           | expr1 GT expr1
             {
               $$ = NewNode(@2.filename, @2.first_line, _GT, $1, $3, 0, 0.0, NULL, NULL, NULL, NULL);
             }
           | expr1 LEQ expr1
             {
               $$ = NewNode(@2.filename, @2.first_line, _LEQ, $1, $3, 0, 0.0, NULL, NULL, NULL, NULL);
             }
           | expr1 GEQ expr1
             {
               $$ = NewNode(@2.filename, @2.first_line, _GEQ, $1, $3, 0, 0.0, NULL, NULL, NULL, NULL);
             }
           | expr1 EQ expr1
             {
               $$ = NewNode(@2.filename, @2.first_line, _EQ, $1, $3, 0, 0.0, NULL, NULL, NULL, NULL);
             }
           | expr1 NEQ expr1
             {
               $$ = NewNode(@2.filename, @2.first_line, _NEQ, $1, $3, 0, 0.0, NULL, NULL, NULL, NULL);
             }
           | expr1 AND expr1
             {
               $$ = NewNode(@2.filename, @2.first_line, _AND, $1, $3, 0, 0.0, NULL, NULL, NULL, NULL);
             }
           | expr1 OR expr1
             {
               $$ = NewNode(@2.filename, @2.first_line, _OR, $1, $3, 0, 0.0, NULL, NULL, NULL, NULL);
             }
           | NOT expr1
             {
               $$ = NewNode(@$.filename, @$.first_line, _NOT, $2, NULL, 0, 0.0, NULL, NULL, NULL, NULL);
             }
           | TRUE_SYM
             {
               $$ = NewNode(@$.filename, @$.first_line, _CONST_BOOL, NULL, NULL, TRUE, 0.0, NULL, NULL, NULL, NULL);
             }
           | FALSE_SYM
             {
               $$ = NewNode(@$.filename, @$.first_line, _CONST_BOOL, NULL, NULL, FALSE, 0.0, NULL, NULL, NULL, NULL);
             }
           | '(' expr1 ')'
             {
               $$ = SetEnclosedInParen($2);
             }
           | variable_ref
             {
               $$ = NewNode(@$.filename, @$.first_line, _VAR, NULL, NULL, 0, 0.0, $1, NULL, NULL, NULL);
             }
           | variable_ref '\'' attribute_chain
             {
               $$ = NewNode(@$.filename, @$.first_line, _VAR, NULL, NULL, 0, 0.0, $1, NULL, $3, NULL);
             }
/*
           | variable_ref '\'' IDENTIFIER
             {
               $$ = NewNode(@$.filename, @$.first_line, _VAR, NULL, NULL, 0, 0.0, $1, NULL, $3, NULL);
             }
           | variable_ref '\'' UNIT_SYM
             {
               $$ = NewNode(@$.filename, @$.first_line, _VAR, NULL, NULL, 0, 0.0, $1, NULL, "unit", NULL);
             }
*/
           | INTEGER
             {
               $$ = NewNode(@$.filename, @$.first_line, _CONST_INT, NULL, NULL, $1, 0.0, NULL, NULL, NULL, NULL);
             }
           | FLOATVAL
             {
               $$ = NewNode(@$.filename, @$.first_line, _CONST_FLOAT, NULL, NULL, 0, $1, NULL, NULL, NULL, NULL);
             }
           | STRING
             {
               $$ = NewNode(@$.filename, @$.first_line, _CONST_STRING, NULL, NULL, 0, 0.0, NULL, NULL, NULL, $1);
             }
           | CHARACTER
             {
               $$ = NewNode(@$.filename, @$.first_line, _CONST_CHARACTER, NULL, NULL, 0, 0.0, NULL, NULL, NULL, $1);
             }
           | NULL_SYM
             {
               $$ = NewNode(@$.filename, @$.first_line, _CONST_NULL, NULL, NULL, 0, 0.0, NULL, NULL, NULL, NULL);
             }
           | call_with_parameter_list
             {
               $$ = NewNode(@$.filename, @$.first_line, _FN_CALL, NULL, NULL, 0, 0.0, NULL, $1, NULL, NULL);
             }
           ;

attribute_chain : single_attribute
                  {
                    $$ = MakeAttributeChainEntry(@$.filename, @$.first_line, $1);
                  }
                | single_attribute '\'' attribute_chain
                  {
                    $$ = PrefixAttributeChainEntry(@$.filename, @$.first_line, $1, $3);
                  }
                ;

single_attribute : IDENTIFIER
                   {
                     $$ = MakeSingleAttribute($1, NULL);
                   }
                 | IDENTIFIER '(' IDENTIFIER ')'
                   {
                     $$ = MakeSingleAttribute($1, $3);
                   }
                 | UNIT_SYM
                   {
                     $$ = MakeSingleAttribute("unit", NULL);
                   }
                 | TYPE
                   {
                     $$ = MakeSingleAttribute("type", NULL);
                   }
				 | ACCESS_SYM
				   {
				     $$ = MakeSingleAttribute("access", NULL);
				   }
                 ;

array_initialisation : '[' RANGE IDENTIFIER ARROW rhs ']'
                       {
                          $$ = MakeArrayInitialisation($3, $5, NULL);
                       }
                     | '[' array_initialisation_list ']'
                       {
                          $$ = MakeArrayInitialisation(NULL, NULL, $2);
                       }
                     | '[' array_seq_initialisation_list ']'
                       {
                          $$ = MakeArrayInitialisation(NULL, NULL, $2);
                       }
                     ;

array_initialisation_list : array_initialisation_entry
                           {
                             $$ = $1;
                           }
                         | array_initialisation_entry ',' array_initialisation_list
                           {
                             $$ = PrefixArrayInitialisationEntry($1, $3);
                           }
                         ;

array_initialisation_entry : case_range_list ARROW rhs
                             {
                               $$ = MakeArrayInitialisationEntry($1, $3);
                             }
                           ;

array_seq_initialisation_list : array_seq_initialisation_entry
                                {
                                  $$ = $1;
                                }
                              | array_seq_initialisation_entry ',' array_seq_initialisation_list
                                {
                                  $$ = PrefixArrayInitialisationEntry($1, $3);
                                }
                              ;

array_seq_initialisation_entry : rhs
                                 {
                                   $$ = MakeArrayInitialisationEntry(NULL, $1);
                                 }
                               ;

record_initialisation : '(' field_initialisation_list ')'
                        {
                          $$ = $2;
                        }
                      | '(' ')'
                        {                 // this is possible for a record that only has '!' fields
                          $$ = NULL;
                        }
                      ;

field_initialisation_list : field_initialisation
                            {
                              $$ = $1;
                            }
                          | field_initialisation ',' field_initialisation_list
                            {
                              $$ = PrefixFieldInitialisation($1, $3);
                            }
                          ;

field_initialisation : IDENTIFIER ARROW NEW rhs
                       {
                         $$ = MakeFieldInitialisation($1, $4, TRUE);
                       }
                     | IDENTIFIER ARROW rhs
                       {
                         $$ = MakeFieldInitialisation($1, $3, FALSE);
                       }
                     | IDENTIFIER ARROW
                       {
                         $$ = MakeFieldInitialisation($1, NULL, FALSE);
                       }
                     ;

procedure_call : variable_ref
                 {
                   $$ = Elt(@$.filename, @$.first_line, @$.last_line, PROCEDURE_CALL_ELT,
                            MakeProcedureCall($1, NULL));
                 }
               | call_with_parameter_list
                 {
                   $$ = $1;
                 }
               ;

call_with_parameter_list :
                 variable_ref '(' ')'
                 {
                   $$ = Elt(@$.filename, @$.first_line, @$.last_line, PROCEDURE_CALL_ELT,
                            MakeProcedureCall($1, NULL));
                 }
               | variable_ref '(' actual_parameter_list ')'
                 {
                   $$ = Elt(@$.filename, @$.first_line, @$.last_line, PROCEDURE_CALL_ELT,
                            MakeProcedureCall($1, $3));
                 }
               ;

actual_parameter_list : actual_parameter
                        {
                          $$ = $1;
                        }
                      | actual_parameter ',' actual_parameter_list
                        {
                          $$ = PrefixActualParameter($1, $3);
                        }
                      ;

actual_parameter : base_actual_parameter optional_representation
                   {
                     $$ = AddRepresentationToActualParameter(@$.filename, @$.first_line, $1, $2);
                   }
                 ;

base_actual_parameter : IDENTIFIER ARROW expr1
                   {
                     $$ = MakeActualParameter($1, $3, MODE_UNSPECIFIED);
                   }
                 | IDENTIFIER ARROW mode_spec expr1
                   {
                     $$ = MakeActualParameter($1, $4, $3);
                   }
                 | expr1
                   {
                     $$ = MakeActualParameter(NULL, $1, MODE_UNSPECIFIED);
                   }
                 | mode_spec expr1
                   {
                     $$ = MakeActualParameter(NULL, $2, $1);
                   }
                 ;

pragma : PRAGMA procedure_call
         {
            $$ = MakePragma($2);
         }
       ;
       
assert_stmt : ASSERT actual_parameter_list
              {
                $$ = Elt(@$.filename, @$.first_line, @$.last_line, ASSERT_ELT, $2);
              }
            ;

advise_stmt : ADVISE actual_parameter_list
              {
                $$ = Elt(@$.filename, @$.first_line, @$.last_line, ADVISE_ELT, $2);
              }
            ;

generic_parameter_list : generic_parameter ';'
            {
              $$ = $1;
            }
          | generic_parameter ';' generic_parameter_list
            {
              $$ = PrefixGenericParameter($1, $3);
            }
          ;

generic_parameter : TYPE IDENTIFIER IS RANGE
                    {
                      $$ = MakeGenericParameter(@$.filename, @$.first_line,
                                                $2, GENERIC_RANGE_TYPE, NULL, NULL, NULL, NULL, NULL);
                    }
                  | TYPE IDENTIFIER IS DIGITS
                    {
                      $$ = MakeGenericParameter(@$.filename, @$.first_line,
                                                $2, GENERIC_DIGITS_TYPE, NULL, NULL, NULL, NULL, NULL);
                    }
                  | TYPE IDENTIFIER IS ARRAY_SYM
                    {
                      $$ = MakeGenericParameter(@$.filename, @$.first_line,
                                                $2, GENERIC_ARRAY_TYPE, NULL, NULL, NULL, NULL, NULL);
                    }
                  | TYPE IDENTIFIER IS ACCESS_SYM
                    {
                      $$ = MakeGenericParameter(@$.filename, @$.first_line,
                                                $2, GENERIC_ACCESS_TYPE, NULL, NULL, NULL, NULL, NULL);
                    }
                  | TYPE IDENTIFIER
                    {
                      $$ = MakeGenericParameter(@$.filename, @$.first_line,
                                                $2, GENERIC_TYPE, NULL, NULL, NULL, NULL, NULL);
                    }
                  | CONSTANT IDENTIFIER IS RANGE
                    {
                      $$ = MakeGenericParameter(@$.filename, @$.first_line,
                                                $2, GENERIC_RANGE_CONSTANT, NULL, NULL, NULL, NULL, NULL);
                    }
                  | CONSTANT IDENTIFIER IS DIGITS
                    {
                      $$ = MakeGenericParameter(@$.filename, @$.first_line,
                                                $2, GENERIC_DIGITS_CONSTANT, NULL, NULL, NULL, NULL, NULL);
                    }
                  | CONSTANT IDENTIFIER IS ACCESS_SYM
                    {
                      $$ = MakeGenericParameter(@$.filename, @$.first_line,
                                                $2, GENERIC_ACCESS_CONSTANT, NULL, NULL, NULL, NULL, NULL);
                    }
                  | CONSTANT IDENTIFIER IS IDENTIFIER
                    {
                      $$ = MakeGenericParameter(@$.filename, @$.first_line,
                                                $2, GENERIC_CONSTANT, $4, NULL, NULL, NULL, NULL);
                    }
                  | CONSTANT IDENTIFIER
                    {
                      $$ = MakeGenericParameter(@$.filename, @$.first_line,
                                                $2, GENERIC_CONSTANT, NULL, NULL, NULL, NULL, NULL);
                    }
                  | PROCEDURE IDENTIFIER formal_parameters
                    {
                      $$ = MakeGenericParameter(@$.filename, @$.first_line,
                                                $2, GENERIC_PROCEDURE, NULL, NULL, $3, NULL, NULL);
                    }
                  | FUNCTION IDENTIFIER formal_parameters ':' function_return_type
                    {
                      $$ = MakeGenericParameter(@$.filename, @$.first_line,
                                                $2, GENERIC_FUNCTION, NULL, NULL, $3, $5, NULL);
                    }
                  | WITH_SYM PACKAGE IDENTIFIER IS NEW package_identifier optional_generic_parameters
                    {
                      $$ = MakeGenericParameter(@$.filename, @$.first_line,
                                                $3, GENERIC_GENERIC_PACKAGE, NULL, $6, NULL, NULL, $7);
                    }
                  ;

optional_generic_parameters:   /* */
                               {
                                 $$ = NULL;
                               }
                             | '(' generic_actual_parameter_list ')'
                               {
                                 $$ = $2;
                               }
                             ;

generic_actual_parameter_list : generic_actual_parameter
                                {
                                  $$ = $1;
                                }
                              | generic_actual_parameter ',' generic_actual_parameter_list
                                {
                                  $$ = PrefixGenericActualParameter($1, $3);
                                }
                              ;

generic_actual_parameter : IDENTIFIER ARROW expr1
                           {
                             $$ = MakeGenericActualParameter($1, $3);
                           }
                         ;

constant_declaration : identifier_list ':' CONSTANT ASSIGN expr1 unit_spec optional_representation
                       {
                         $$ = Elt(@$.filename, @$.first_line, @$.last_line, CONSTANT_DECLARATION_ELT,
                                  MakeConstant($1, $5, $6, $7));
                       }
                     ;

declare_block : DECLARE declaration_procedure_list BEGIN_SYM optional_representation stmt_list END
                {
                  $$ = Elt(@$.filename, @$.first_line, @$.last_line,
                           DECLARE_BLOCK_ELT, MakeDeclareBlock($2, $5, $4, TRUE, TRUE));
                }
              | BEGIN_SYM optional_representation stmt_list END
                {
                  $$ = Elt(@$.filename, @$.first_line, @$.last_line,
                           DECLARE_BLOCK_ELT, MakeDeclareBlock(NULL, $3, $2, TRUE, TRUE));
                }
              ;

separate_stmt : USE_SYM SEPARATE package_identifier
                {
                  $$ = Elt(@$.filename, @$.first_line, @$.last_line,
                           SEPARATE_STMT_ELT, MakeSeparateStmt($3));
                }
              ;

unit_test_block : BEGIN_SYM WHEN UNIT_TEST stmt_list END
                  {
                    $$ = Elt(@$.filename, @$.first_line, @$.last_line,
                             DECLARE_BLOCK_ELT, MakeDeclareBlock(NULL, $4, NULL, TRUE, FALSE));
                  }
                | BEGIN_SYM WHEN NOT UNIT_TEST stmt_list END
                  {
                    $$ = Elt(@$.filename, @$.first_line, @$.last_line,
                             DECLARE_BLOCK_ELT, MakeDeclareBlock(NULL, $5, NULL, FALSE, TRUE));
                  }
                ;

free_stmt : FREE_SYM variable_ref
            {
              $$ = Elt(@$.filename, @$.first_line, @$.last_line, FREE_ELT, $2);
            }
          ;

restart_stmt : RESTART SUBSYSTEM
               {
                 $$ = Elt(@$.filename, @$.first_line, @$.last_line, RESTART_ELT, NULL);
               }
             ;

representation_list : representation_clause
                      {
                        $$ = $1;
                      }
                    | representation_clause ',' representation_list
                      {
                        $$ = PrefixRepresentationClause($1, $3);
                      }
                    ;

representation_clause : IDENTIFIER ARROW expr1
                        {
                          $$ = MakeRepresentationClause($1, $3);
                        }
                      | IDENTIFIER
                        {
                          $$ = MakeRepresentationClause(
                                 $1, NewNode(@$.filename, @$.first_line, _CONST_BOOL,
                                             NULL, NULL, TRUE, 0.0, NULL, NULL, NULL, NULL));
                        }
                      ;

