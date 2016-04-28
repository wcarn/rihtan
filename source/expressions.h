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

/* Routines mainly for analysing expressions for Rihtan */

#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "ytypes.h"

enum tTargetBaseType
{ TARGET_INTEGER, TARGET_FLOAT };

struct tTargetType
{
  enum tTargetBaseType BaseType;
  char *CName;
  char *AccessCName;
  // Integers
  BIGINT Min;
  BIGINT Max;
  // Floats
  BIGINT Digits;
  BIGINT Magnitude;
  //
  bool UseForArithmetic;
  //
  bool Preferred;               // for arithmetic
  //
  char *RepresentationName;     // for 'using' clauses
  //
  char *PrintfFormat;           // For auto-generated test code
  //
  unsigned SizeInBits;
  //
  bool AtomicType;              // Atomic accesses
};

// Store LHS - RHS in [Low, High]
struct tRelation
{
  struct tVar *LHS, *RHS;
  enum tAttribute LHSAttr, RHSAttr;
  BIGINT Low, High;
  bool LowSet, HighSet;
};

extern struct tTypeSpec *ConstantIntegerTypespec, *ConstantFloatTypespec,
  *ConstantBooleanTypespec, *ConstantAccessTypespec, *ConstantStringTypespec,
  *UnitTypespec, *SeparateBlockTypespec;
extern struct tType ConstantIntegerType, ConstantFloatType, ConstantBooleanType,
  ConstantAccessType, ConstantStringType, UnitType, SeparateBlockType;

  // Boolean, character and universal_index must be initialised to NULL because this is tested in the target definition
extern struct tTypeSpec *BooleanTypespec, *CharacterTypespec,
  *UniversalIndexTypespec, *CstringTypespec, *AddressTypespec;
extern struct tType *BooleanType, *CharacterType, *UniversalIndexType, *CstringType,
  *AddressType;
extern struct tUnitDef *CharacterUnit;
extern struct tVar *BooleanVar, *CharacterVar, *UniversalIndexVar;

void
AnalyseExpression (struct tNode *expr, enum tBaseType *exprtype, BIGINT * min, BIGINT * max,    // for integers and booleans (0 false 1 true)
                   int *digits, int *magnitude, // for floats
                   struct tUnitDef **unit,
                   bool * staticexpression, BIGINT * intval,
                   BIGFLOAT * floatval, bool * boolval,
                   struct tTypeSpec **accessedtype, bool * uncheckedaccess,
                   bool parameter, bool rangefor, bool verbose,
                   bool * virtualexpression, bool * accessconstant, bool * accesspersistent, bool * accessnew);
                   
void
TestIntegerExpression (struct tNode *expr, BIGINT * min, BIGINT * max,
                       struct tUnitDef **unit, bool parameter, bool rangefor,
                       bool * virtualexpression);
                       
void
TestFloatExpression (struct tNode *expr, struct tUnitDef **unit, int *digits,
                     int *magnitude, bool parameter, bool * virtualexpression);
                     
void
TestBooleanExpression (struct tNode *expr, bool parameter, BIGINT * min,
                       BIGINT * max, bool verbose, bool * virtualexpression);
                       
void
TestAccessExpression (struct tNode *expr, bool parameter, bool * couldbenull,
                      bool * isnull, struct tTypeSpec **accesstype,
                      bool * uncheckedaccess, bool * isaccessvalue,
                      BIGINT * min, BIGINT * max, bool * virtualexpression,
                      bool * accessconstant, bool * accesspersistent, bool * accessnew, bool * addressexpr);
                       
// Is a variable (not) part of a composite object?
bool
SimpleVar (struct tNode * n, struct tVar ** var, enum tAttribute * attr, bool allowunits);

struct tTargetType *GetTargetIntegerType (char *filename, int line,
                                          BIGINT min, BIGINT max,
                                          bool arithmetic, bool erroriffail);
                                          
struct tTargetType *GetTargetFloatType (char *filename, int line,
                                        BIGINT digits, BIGINT magnitude,
                                        bool arithmetic);

struct tTargetType *GetWordType (char *filename, int line,
                                 enum tTargetBaseType basetype,
                                 struct tNode *expr);

BIGINT
MinBigInt (BIGINT a, BIGINT b);

BIGINT
MaxBigInt (BIGINT a, BIGINT b);

int
FloatMagnitude (BIGFLOAT f);

int
IntMagnitude (BIGINT i);

int
IntDigits (BIGINT i);

unsigned
binlog (BIGINT v);

BIGINT
binexpmask (unsigned L);

// Evaluate a C-like character literal (e.g. 'a', '\n', '\x1a')
BIGINT
EvalCharacterLiteral (char *str);

void
EvalStaticExpression (struct tNode *expr, enum tBaseType *exprtype,
                      BIGINT * intval, BIGFLOAT * floatval, bool * boolval,
                      struct tTypeSpec **accessedtype, bool * uncheckedaccess,
                      struct tUnitDef **unit, bool * success);

// A common case is integers
BIGINT
EvalStaticInteger (struct tNode *expr, bool * success, bool reportfailure,
                   struct tUnitDef **unit);
                   
BIGINT
EvalStaticBoolean (struct tNode * expr, bool * success, bool reportfailure);

BIGFLOAT
EvalStaticFloat (struct tNode * expr, bool * success, bool reportfailure,
                 struct tUnitDef ** unit);

void
EvalStaticAccess (struct tNode *expr, bool * success,
                  struct tTypeSpec **accessedtype, bool reportfailure);

// Search for a relation between two variables, constructing one if necessary (and possible)
// Returns a pointer to a newly allocated structure that can be freed
struct tRelation *
FindRelationIndirect (struct tVar *lhs, enum tAttribute lhsattr,
                      struct tVar *rhs, enum tAttribute rhsattr);

void
TestBooleanLHSRHS (struct tVar *lvar, enum tAttribute lattr,
                   BIGINT loffsetmin, BIGINT loffsetmax, struct tVar *rvar,
                   enum tAttribute rattr, BIGINT roffsetmin,
                   BIGINT roffsetmax, enum tOp op, BIGINT * min, BIGINT * max);
                      
void
TestBooleanRelation (struct tNode *lhs, struct tNode *rhs, enum tOp op,
                     BIGINT * min, BIGINT * max);

BIGINT
MaxAbs (BIGINT a, BIGINT b);
                  
void AnalyseReference (char *filename, int line, struct tReference *reference,
                       enum tBaseType *basetype, struct tVar **basevar,
                       struct tTypeSpec **reftype, BIGINT * min, BIGINT * max,
                       BIGINT * initialmin, BIGINT * initialmax,
                       struct tUnitDef **unit,
                       struct tTypeSpec **accessedtype, bool * initialised,
                       enum tMode *mode, int *lock, bool parameter,
                       bool * wholeobject, bool donotsetaccessed,
                       bool * constantaccess, bool typeorattributesearch,
                       bool * subsystemboundary);
                       
// Trace through a chain of 'index_type 'element_type 'accessed_type 'type
// Update attr to any following attribute and reftype to point to a TYPE_TYPE
void
TraceTypeChain (char *filename, int line,
                struct tAttributeChainEntry **attribute,
                struct tTypeSpec **typespec);
                
bool
IsRelop (enum tOp op);

bool
IsIntegerOp (enum tOp op);

bool
IsFloatOp (enum tOp op);

bool
IsBooleanOp (enum tOp op);

bool
IsConstant (enum tOp op);

bool
NamedConstantType (struct tTypeSpec *t);

// Does v represent a variable (as opposed to a type, package, procedure, constant etc.)
bool
IsVariable (struct tVar * v);

bool
IsNamedConstant (struct tVar * v);

// Find the rightmost attribute in an expression
enum tAttribute
GetExprAttribute (struct tNode *expr);

#endif /* EXPRESSIONS_H */
