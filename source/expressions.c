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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <setjmp.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "ytypes.h"
#include "expressions.h"
#include "rihtan.h"

struct tTypeSpec *ConstantIntegerTypespec, *ConstantFloatTypespec,
  *ConstantBooleanTypespec, *ConstantAccessTypespec, *ConstantStringTypespec,
  *UnitTypespec, *SeparateBlockTypespec;
struct tType ConstantIntegerType, ConstantFloatType, ConstantBooleanType,
  ConstantAccessType, ConstantStringType, UnitType, SeparateBlockType;

  // Boolean, character and universal_index must be initialised to NULL because this is tested in the target definition
struct tTypeSpec *BooleanTypespec = NULL, *CharacterTypespec =
  NULL, *UniversalIndexTypespec = NULL, *CstringTypespec, *AddressTypespec;
struct tType *BooleanType, *CharacterType, *UniversalIndexType, *CstringType,
  *AddressType;
struct tUnitDef *CharacterUnit;
struct tVar *BooleanVar, *CharacterVar, *UniversalIndexVar;

/* Pretest and assigmment for n * mult + add - minus */
void
ptest (char *filename, int line, BIGINT n, BIGINT mult, BIGINT add,
       BIGINT minus, BIGINT * val)
{
  BIGINT v1, v2, v3, t;

  if (mult == 0)
    {
      t = n;
      v1 = 0;
    }
  else
    {
      v1 = n * mult;
      t = v1 / mult;
    }
  if (t == n)
    {
      v2 = v1 + add;
      t = v2 - add;
      if (t == v1)
        {
          v3 = v2 - minus;
          t = v3 + minus;
          if (t == v2)
            {
              /* OK */
              *val = v3;
              return;
            }
        }
    }
  err (filename, line,
       "Range error: %" PRINTBIG " * %" PRINTBIG " + %" PRINTBIG " - %"
       PRINTBIG "", n, mult, add, minus);
}

/* lmin, lmax straddles 0 */
void
straddle0 (char *filename, int line, BIGINT lmin, BIGINT lmax, BIGINT rmin,
           BIGINT rmax, BIGINT * min, BIGINT * max)
{
  if (rmin >= 0)
    {
      /* rhs all positive */
      ptest (filename, line, lmin, rmax, 0, 0, min);
      ptest (filename, line, lmax, rmax, 0, 0, max);
    }
  else if (rmax <= 0)
    {
      /* rhs all negative */
      ptest (filename, line, lmax, rmin, 0, 0, min);
      ptest (filename, line, lmin, rmin, 0, 0, max);
    }
  else
    {
      /* both straddle 0 */
      BIGINT n1, n2;

      ptest (filename, line, lmin, rmax, 0, 0, &n1);
      ptest (filename, line, rmin, lmax, 0, 0, &n2);
      if (n1 < n2)
        *min = n1;
      else
        *min = n2;
      ptest (filename, line, lmin, rmin, 0, 0, &n1);
      ptest (filename, line, lmax, rmax, 0, 0, &n2);
      if (n1 > n2)
        *max = n1;
      else
        *max = n2;
    }
}

// Analyse an expression as it appears in the source
// Calls to this routine must be followed by statements that fill in expr->Min and expr->Max
// The regular interface is AnalyseExpression
void
AnalyseExpressionAsWritten (struct tNode *expr, enum tBaseType *exprtype, BIGINT * min, BIGINT * max,   // for integers and booleans (0 false 1 true)
                            int *digits, int *magnitude,        // for floats
                            struct tUnitDef **unit,
                            bool * staticexpression, BIGINT * intval,
                            BIGFLOAT * floatval, bool * boolval,
                            struct tTypeSpec **accessedtype,
                            bool * uncheckedaccess, bool parameter,
                            bool rangefor, bool verbose,
                            bool * virtualexpression, bool * accessconstant,
                            bool * accesspersistent, bool * accessnew)
{
  enum tOp op;
  struct tVar *basevar;
  bool initialised;
  struct tTypeSpec *reftype;
  enum tMode mode;
  struct tReference *ref;
  struct tVar *v;
  int lock;
  struct tRelation *rel;
  // The next two are initialised to avoid a warning - actually OK because of overridemin/max test
  BIGINT overridelow = 0, overridehigh = 0;
  //
  bool overridemin, overridemax, wholeobject;
  struct tVar *leftvar, *rightvar, *exprvar;
  enum tAttribute leftattr, rightattr, exprattr;

  // Test for a simple integer variable and record it in the node
  if (SimpleVar (expr, &exprvar, &exprattr, FALSE))
    {
      if (exprvar->Type->TypeSpec->BaseType == INTEGER_TYPE
          && exprattr == ATTR_NULL)
        {
          expr->IntVar = exprvar;
        }
      else
        {
          expr->IntVar = NULL;  // Everything except simple integer variable
        }
    }
  else
    {
      expr->IntVar = NULL;      // Everything except simple integer variable
    }
  //
  op = expr->Op;
  // Test for correct parenthesisation
  if (op == _AND || op == _OR || op == _NOT)
    {
      enum tOp lop = expr->Left->Op;

      if ((lop != op) && (lop == _AND || lop == _OR || lop == _NOT)
          && (!expr->Left->EnclosedInParen))
        {
          err (expr->FileName, expr->Line,
               "(Sub)expressions that contain a mixture of 'and', 'or' and 'not' must be fully parenthesised with respect to those operators");
        }
    }
  else if (op == _IAND || op == _IOR || op == _IXOR)
    {
      enum tOp lop = expr->Left->Op;

      if ((lop != op) && (lop == _IAND || lop == _IOR || lop == _IXOR)
          && (!expr->Left->EnclosedInParen))
        {
          err (expr->FileName, expr->Line,
               "(Sub)expressions that contain a mixture of 'iand', 'ior' and 'ixor' must be fully parenthesised with respect to those operators");
        }
    }
  //
  overridemin = FALSE;
  overridemax = FALSE;
  if (op == _CONST_INT)
    {
      *min = expr->Value;
      *max = expr->Value;
      *unit = NULL;
      *exprtype = INTEGER_TYPE;
      *intval = expr->Value;
      *staticexpression = TRUE;
      expr->ComputationType =
        GetTargetIntegerType (expr->FileName, expr->Line, expr->Value,
                              expr->Value, TRUE, FALSE);
      return;
    }
  if (op == _CONST_FLOAT)
    {
      *magnitude = FloatMagnitude (expr->FloatValue);
      *digits = 1;
      *unit = NULL;
      *exprtype = FLOATING_TYPE;
      *floatval = expr->FloatValue;
      *staticexpression = TRUE;
      return;
    }
  if (op == _CONST_CHARACTER)
    {
      BIGINT charvalue;

      charvalue = EvalCharacterLiteral (expr->String);
      *min = charvalue;
      *max = charvalue;
      *unit = CharacterUnit;
      *exprtype = INTEGER_TYPE;
      *intval = charvalue;
      *staticexpression = TRUE;
      expr->ComputationType = CharacterTypespec->CType;
      return;
    }
  if (op == _CONST_BOOL)
    {
      *unit = NULL;
      *exprtype = BOOLEAN_TYPE;
      if (expr->Value == 0)
        {
          *boolval = FALSE;
          *min = 0;
          *max = 0;
        }
      else
        {
          *boolval = TRUE;
          *min = 1;
          *max = 1;
        }
      *staticexpression = TRUE;
      expr->ComputationType = BooleanTypespec->CType;
      return;
    }
  if (op == _CONST_STRING)
    {
      err (expr->FileName, expr->Line,
           "String constants are not allowed in expressions");
      *unit = NULL;
      *staticexpression = TRUE;
      return;
    }
  if (op == _CONST_NULL)
    {
      *unit = NULL;
      *exprtype = ACCESS_TYPE;  // The only possible static access value is null
      *accessedtype = NULL;
      *uncheckedaccess = TRUE;  // actually it could be either, but null will match both
      *accessconstant = FALSE;  // need to match both
      *accesspersistent = FALSE;
      *accessnew = FALSE;
      *min = 0;                 // 0 means null. That doesn't mean that the
      *max = 0;                 // machine representation needs to be 0.
      *intval = 0;
      *staticexpression = TRUE;
      return;
    }
  if (op == _MINUS)
    {
      // Test for v1 - v2 where v1 and v2 are integers (difference constraint test)
      if (SimpleVar (expr->Left, &leftvar, &leftattr, TRUE)
          && SimpleVar (expr->Right, &rightvar, &rightattr, TRUE))
        {
          enum tBaseType ltype = leftvar->Type->TypeSpec->BaseType,
            rtype = rightvar->Type->TypeSpec->BaseType;
          //enum tAttribute lattr = expr->Left->Attribute,
          //                        rattr = expr->Right->Attribute;
          if ((ltype == INTEGER_TYPE || ltype == ARRAY_TYPE)
              && (rtype == INTEGER_TYPE || rtype == ARRAY_TYPE))
            {
              if (leftvar == rightvar && leftattr == rightattr)
                {
                  // v1 - v1
                  overridelow = 0;
                  overridemin = TRUE;
                  overridehigh = 0;
                  overridemax = TRUE;
                }
              else
                {
                  rel =
                    FindRelationIndirect (leftvar, leftattr, rightvar,
                                          rightattr);
                  if (rel != NULL)
                    {
                      if (rel->LowSet)
                        {
                          overridelow = rel->Low;
                          overridemin = TRUE;
                        }
                      if (rel->HighSet)
                        {
                          overridehigh = rel->High;
                          overridemax = TRUE;
                        }
                      free (rel);
                      // And continue
                    }
                  // Now try a search path using the reverse order
                  rel =
                    FindRelationIndirect (rightvar, rightattr, leftvar,
                                          leftattr);
                  if (rel != NULL)
                    {
                      if (rel->HighSet)
                        {
                          if (overridemin)
                            {
                              if (-rel->High > overridelow)
                                {
                                  // improved (narrower) range
                                  overridelow = -rel->High;
                                }
                            }
                          else
                            {
                              overridelow = -rel->High;
                              overridemin = TRUE;
                            }
                        }
                      if (rel->LowSet)
                        {
                          if (overridemax)
                            {
                              if (-rel->Low < overridehigh)
                                {
                                  overridehigh = -rel->Low;
                                }
                            }
                          else
                            {
                              overridehigh = -rel->Low;
                              overridemax = TRUE;
                            }
                        }
                      free (rel);
                    }
                }
            }
        }
    }
  if (op == _VAR)
    {
      BIGINT initialmin, initialmax;
      bool constantaccess, subsystemboundary;
      struct tAttributeChainEntry *attr;
      attr = expr->Attribute;

      AnalyseReference (expr->FileName, expr->Line, expr->Var, exprtype,
                        &basevar, &reftype, min, max, &initialmin,
                        &initialmax, unit, accessedtype, &initialised, &mode,
                        &lock, parameter, &wholeobject, attr != NULL,
                        &constantaccess, (attr != NULL) || rangefor,
                        &subsystemboundary);
      //
      if (basevar->Type->TypeSpec->BaseType == TYPE_TYPE)
        {
          // This is simply the name of a type
          // Use the type itself rather than its definition
          reftype = basevar->Type->TypeSpec;
        }
      if (attr == NULL)
        {
          if (basevar->AccessVirtual)
            {
              *virtualexpression = TRUE;
            }
          //
          if (!rangefor)
            {
              RecordGlobalAccess (expr->Line, GetGlobalsAccess (), basevar,
                                  TRUE, FALSE, (!(TraceAllReferents(basevar))->Initialised), FALSE);
            }
        }
      if (attr != NULL)
        {
          // Handle chains of 'type 'element_type 'accessed_type 'index_type 'field_type
          TraceTypeChain (expr->FileName, expr->Line, &attr, &reftype);
        }
      enum tAttribute exprattr;
      if (attr == NULL)
        {
          exprattr = ATTR_NULL;
        }
      else
        {
          exprattr = attr->Name;
        }
      //
      if (exprattr == ATTR_ACCESS || exprattr == ATTR_PERSISTENT_ACCESS)
        {
          if (IsVariable (basevar))
            {
              if (basevar->AccessVirtual)
                {
                  err (expr->FileName, expr->Line,
                       "The '%s attribute cannot be applied to a virtual variable", AttributeNames[ATTR_ACCESS]);
                }
              *accessedtype = reftype;
              *uncheckedaccess = TRUE;
              if (IsNamedConstant(basevar) || basevar->ConstValue != NULL) {
            	  *accessconstant = TRUE;
              }
              else {
            	  *accessconstant = FALSE;
              }
              if (exprattr == ATTR_ACCESS) {
            	  *accesspersistent = FALSE;
                  *accessnew = FALSE;
              }
              else {
            	  *accesspersistent = TRUE;
                  *accessnew = FALSE;
              }
              *unit = NULL;
              *exprtype = ACCESS_TYPE;
              *min = 1;         // 0 means null. That doesn't mean that the
              *max = 1;         // machine representation needs to be 0.
              *intval = 1;
              // Accesses to globals are static. Accesses to locals are not.
              if (basevar->Mode == MODE_GLOBAL
                  || basevar->Mode == MODE_SHARED)
                {
                  *staticexpression = TRUE;
                }
              else
                {
            	  if (exprattr == ATTR_PERSISTENT_ACCESS) {
            		  err(expr->FileName, expr->Line,
                          "The '%s attribute cannot be applied to a local variable or a formal parameter", AttributeNames[ATTR_PERSISTENT_ACCESS]);
            	  }
                  *staticexpression = FALSE;
                }
              SetAccessed (basevar);
            }
          else
            {
              err (expr->FileName, expr->Line,
                   "Access attributes can only be applied to a variable");
            }
        }
      else if (exprattr == ATTR_SIZE)
        {
          if (basevar->Type->TypeSpec->BaseType == TYPE_TYPE || IsVariable (basevar))
            {
              if (MaximumObjectSize == 0)
                {
                  err (expr->FileName, expr->Line,
                       "Maximum object size has not been set (use pragma maximum_object_size)");
                }
              *unit = NULL;
              *exprtype = INTEGER_TYPE;
              *min = 1;
              *max = MaximumObjectSize;
              *staticexpression = FALSE;        // It is not known by the compiler
            }
          else
            {
              err (expr->FileName, expr->Line,
                   "The size attribute can only be applied to variables and types");
            }
        }
      else if (reftype->BaseType == ARRAY_TYPE && exprattr != ATTR_NULL)
        {
          ref = expr->Var;
          v = FindVariable (expr->FileName, expr->Line, &ref, 15, FALSE, TRUE);     //*****basevar?????
          if (v->Type->TypeSpec->BaseType == TYPE_TYPE)
            {
              // The name of an array type
              v = NULL;
            }
          else if (ref->Next == NULL)
            {
                // A direct variable reference, e.g. A'first where A is an array
                // In this case 'first and 'last could depend on slice bounds of A, for example
            }
          else
            {
              // Something like A[j]'first (where A[j] is itself an array)
              // In this case 'first and 'last depend only on the type of A[j]
              v = NULL;
            }
          if (exprattr == ATTR_FIRST || exprattr == ATTR_MINIMUM)
            {
              if (v == NULL || reftype->MinDefinite)
                {
                  *min = reftype->Low;
                  *max = reftype->Low;
                  *intval = reftype->Low;
                  *staticexpression = TRUE;
                }
              else
                {
                  *min = v->FirstLow;
                  *max = v->FirstHigh;
                  *staticexpression = FALSE;
                }
              *exprtype = INTEGER_TYPE;
              *unit = reftype->IndexTypeSpec->Unit;
            }
          else if (exprattr == ATTR_LAST || exprattr == ATTR_MAXIMUM)
            {
              if (v == NULL || reftype->MaxDefinite)
                {
                  *min = reftype->High;
                  *max = reftype->High;
                  *intval = reftype->High;
                  *staticexpression = TRUE;
                }
              else
                {
                  *min = v->LastLow;
                  *max = v->LastHigh;
                  *staticexpression = FALSE;
                }
              *exprtype = INTEGER_TYPE;
              *unit = reftype->IndexTypeSpec->Unit;
            }
          else if (exprattr == ATTR_LENGTH)
            {
              if (v == NULL || (reftype->MinDefinite && reftype->MaxDefinite))
                {
                  *min = reftype->High - reftype->Low + 1;
                  *max = *min;
                  *intval = *min;
                  *staticexpression = TRUE;
                }
              else
                {
                  *min = v->LastLow - v->FirstHigh + 1;
                  *max = v->LastHigh - v->FirstLow + 1;
                  *staticexpression = FALSE;
                }
              *exprtype = INTEGER_TYPE;
              *unit = reftype->IndexTypeSpec->Unit;
            }
          else if (exprattr == ATTR_INDEX_UNIT)
            {
              *min = 1;
              *max = 1;
              *intval = 1;
              *staticexpression = TRUE;
              *exprtype = INTEGER_TYPE;
              *unit = reftype->IndexTypeSpec->Unit;
            }
          else
            {
              err (expr->FileName, expr->Line,
                   "Invalid attribute for an array in an expression");
              *unit = NULL;
            }
        }
      else if (reftype->BaseType == TYPE_TYPE)
        {
          // We could get here from tracing a type chain 'index_type etc.
          if (exprattr == ATTR_NULL)
            {
              if (rangefor
                  && (reftype->Type->TypeSpec->BaseType == INTEGER_TYPE))
                {
                  // In 'type t is range for ...', integer type names can be used and correspond to their ranges
                  *min = reftype->Type->TypeSpec->Low;
                  *max = reftype->Type->TypeSpec->High;
                  *exprtype = INTEGER_TYPE;
                  *staticexpression = FALSE;
                  *unit = reftype->Type->TypeSpec->Unit;
                }
              else
                {
                  errstart (expr->FileName, expr->Line);
                  errcont ("Attribute expected for type ");
                  errDumpReference (expr->Var); //**********CHECK THIS ERROR MESSAGE
                  errcont (" (range for=%d)", rangefor);
                  errend ();
                  *exprtype = INTEGER_TYPE;
                  *unit = NULL;
                }
            }
          else
            {
              if (exprattr == ATTR_FIRST
                  || exprattr == ATTR_MINIMUM)
                {
                  if (reftype->Type->TypeSpec->BaseType == INTEGER_TYPE)
                    {
                      *min = reftype->Type->TypeSpec->Low;
                      *max = *min;
                      *exprtype = INTEGER_TYPE;
                      *intval = *min;
                      *staticexpression = TRUE;
                      *unit = reftype->Type->TypeSpec->Unit;
                    }
                  else if (reftype->Type->TypeSpec->BaseType == ARRAY_TYPE)
                    {
                      *min = reftype->Type->TypeSpec->IndexTypeSpec->Low;
                      *max = *min;
                      *exprtype = INTEGER_TYPE;
                      *intval = *min;
                      *staticexpression = TRUE;
                      *unit = reftype->Type->TypeSpec->IndexTypeSpec->Unit;
                    }
                  else
                    {
                      err (expr->FileName, expr->Line,
                           "Integer or array type expected for attribute 'first");
                    }
                }
              else if (exprattr == ATTR_LAST
                       || exprattr == ATTR_MAXIMUM)
                {
                  if (reftype->Type->TypeSpec->BaseType == INTEGER_TYPE)
                    {
                      *min = reftype->Type->TypeSpec->High;
                      *max = *min;
                      *exprtype = INTEGER_TYPE;
                      *intval = *min;
                      *staticexpression = TRUE;
                      *unit = reftype->Type->TypeSpec->Unit;
                    }
                  else if (reftype->Type->TypeSpec->BaseType == ARRAY_TYPE)
                    {
                      *min = reftype->Type->TypeSpec->IndexTypeSpec->High;
                      *max = *min;
                      *exprtype = INTEGER_TYPE;
                      *intval = *min;
                      *staticexpression = TRUE;
                      *unit = reftype->Type->TypeSpec->IndexTypeSpec->Unit;
                    }
                  else
                    {
                      err (expr->FileName, expr->Line,
                           "Integer or array type expected for attribute 'last");
                    }
                }
              else if (exprattr == ATTR_LENGTH)
                {
                  if (reftype->Type->TypeSpec->BaseType == ARRAY_TYPE)
                    {
                      *min =
                        reftype->Type->TypeSpec->IndexTypeSpec->High -
                        reftype->Type->TypeSpec->IndexTypeSpec->Low + 1;
                      *max = *min;
                      *exprtype = INTEGER_TYPE;
                      *intval = *min;
                      *staticexpression = TRUE;
                      *unit = reftype->Type->TypeSpec->IndexTypeSpec->Unit;
                    }
                  else
                    {
                      err (expr->FileName, expr->Line,
                           "Array type expected for attribute 'length");
                    }
                }
              else if (exprattr == ATTR_UNIT)
                {
                  if (reftype->Type->TypeSpec->BaseType == INTEGER_TYPE
                      || reftype->Type->TypeSpec->BaseType == FLOATING_TYPE)
                    {
                      *min = 1;
                      *max = 1;
                      *exprtype = INTEGER_TYPE;
                      *intval = 1;
                      *staticexpression = TRUE;
                      *unit = reftype->Type->TypeSpec->Unit;
                    }
                  else
                    {
                      err (expr->FileName, expr->Line,
                           "Wrong type for attribute 'unit");
                    }
                }
              else if (exprattr == ATTR_INDEX_UNIT)
                {
                  if (reftype->Type->TypeSpec->BaseType == ARRAY_TYPE)
                    {
                      *min = 1;
                      *max = 1;
                      *exprtype = INTEGER_TYPE;
                      *intval = 1;
                      *staticexpression = TRUE;
                      *unit = reftype->Type->TypeSpec->IndexTypeSpec->Unit;
                    }
                  else
                    {
                      err (expr->FileName, expr->Line,
                           "Wrong type for attribute 'index_unit (array type expected)");
                    }
                }
              else
                {
                  err (expr->FileName, expr->Line,
                       "Invalid attribute for a type in an expression");
                }
            }
        }
      else if (reftype->BaseType == INTEGER_TYPE && exprattr != ATTR_NULL)
        {
          if (exprattr == ATTR_FIRST)
            {
              *min = reftype->Low;
              *max = *min;
            }
          else if (exprattr == ATTR_LAST)
            {
              *min = reftype->High;
              *max = *min;
            }
          else if (exprattr == ATTR_UNIT)
            {
              *min = 1;
              *max = *min;
            }
          else if (exprattr == ATTR_MINIMUM)
            {
              ;                 // *min was set by AnalyseReference
              *max = *min;
            }
          else if (exprattr == ATTR_MAXIMUM)
            {
              *min = *max;
            }
          else if (exprattr == ATTR_INITIAL)
            {
              *min = initialmin;
              *max = initialmax;
            }
          else
            {
              err (expr->FileName, expr->Line,
                   "Invalid attribute for an integer: '%s'",
                   AttributeNames[exprattr]);
            }
          // After checking the attribute above, override the value if this is a named constant, apart
          // from in the case of 'unit which still = 1
          if (reftype == ConstantIntegerTypespec && exprattr != ATTR_UNIT)
            {
              *min = basevar->High;
              *max = *min;
            }
          *exprtype = INTEGER_TYPE;
          if (*max == *min)
            {
              *intval = *min;
              *staticexpression = TRUE;
            }
          else
            {
              *staticexpression = FALSE;  // 'initial possibly
            }
          *unit = reftype->Unit;
        }
      else if (reftype->BaseType == FLOATING_TYPE && exprattr != ATTR_NULL)
        {
          if (exprattr == ATTR_UNIT)
            {
              *min = 1;
              *max = *min;
              *exprtype = INTEGER_TYPE;
              *intval = *min;
              *staticexpression = TRUE;
              *unit = reftype->Unit;
            }
          else
            {
              err (expr->FileName, expr->Line,
                   "Wrong attribute for floating point type: '%s'",
                   AttributeNames[exprattr]);
            }
        }
      else if (reftype->BaseType == UNIT_TYPE)
        {
          *min = 1;
          *max = 1;
          *exprtype = INTEGER_TYPE;
          *intval = 1;
          *staticexpression = TRUE;
          // *unit was set by AnalyseReference
        }
      else if (reftype->BaseType == ADDRESS_TYPE)
        {
          *min = 0;
          *max = 1;
          *exprtype = ADDRESS_TYPE;
          *uncheckedaccess = TRUE;
          *accessconstant = FALSE;
          *accesspersistent = FALSE;
          *accessnew = FALSE;
          *accessedtype = NULL;
          *staticexpression = FALSE;
        }
      else
        {
          *exprtype = reftype->BaseType;
          if (reftype->BaseType == ACCESS_TYPE)
            {
              // accessedtype has been set by AnalyseReference, which takes account of constants
              // versus variables
              // All constants are unchecked, which is picked up below
              *uncheckedaccess = reftype->UnmanagedAccess;
              *accessconstant = reftype->ConstantAccess;
              *accesspersistent = reftype->PersistentAccess;
              *accessnew = reftype->NewAccess;
              // Don't set min and max because that will have been done by AnalyseReference
            }
          else if (reftype->BaseType == FLOATING_TYPE)
            {
              *magnitude = reftype->Magnitude;
              *digits = reftype->Digits;
              // Note: don't do this for integers because min and max will have been set by AnalyseReference above
            }
          if (NamedConstantType (reftype))
            {
              if (reftype == ConstantIntegerTypespec)
                {
                  *exprtype = INTEGER_TYPE;
                  *intval = basevar->High;
                  *min = *intval;
                  *max = *intval;
                }
              else if (reftype == ConstantFloatTypespec)
                {
                  *exprtype = FLOATING_TYPE;
                  *floatval = basevar->FloatVal;
                }
              else if (reftype == ConstantBooleanTypespec)
                {
                  *exprtype = BOOLEAN_TYPE;
                  *boolval = basevar->High;
                  if (*boolval)
                    {
                      *min = 1;
                      *max = 1;
                    }
                  else
                    {
                      *min = 0;
                      *max = 0;
                    }
                }
              else if (reftype == ConstantAccessTypespec)
                {
                  *exprtype = ACCESS_TYPE;
                  *uncheckedaccess = TRUE;
                  *accessconstant = FALSE;
                  *accesspersistent = FALSE;
                  *accessnew = FALSE;
                  ///*****CHECK THIS
                  if (basevar->ConstAccess == NULL)
                    {
                      *min = 0;
                    }
                  else
                    {
                      *min = 1;
                    }
                  *max = *min;
                  *intval = *min;
                }
              else if (reftype == ConstantStringTypespec)
                {
                  err (expr->FileName, expr->Line,
                       "String constants are not allowed in expressions");
                  *exprtype = CSTRING_TYPE;
                }
              *staticexpression = TRUE;
              // Unit and accessedtype will have been set by AnalyseReference
            }
          else
            {
              *staticexpression = FALSE;
              expr->ComputationType = reftype->CType;
              if (basevar->AccessVolatile)
                { // includes mapped device and shared atomic
                  // Assume that the value could change at any time
                  *min = reftype->Low;
                  *max = reftype->High;
                }
            }
          if (*exprtype == INTEGER_TYPE)
            {
              if (expr->ComputationType == NULL)
                {
                  expr->ComputationType =
                    GetTargetIntegerType (expr->FileName, expr->Line, *min,
                                          *max, TRUE, FALSE);
                }
            }
          if (!initialised)
            {
              if (basevar->Mode == MODE_GLOBAL
                  || basevar->Mode == MODE_SHARED)
                {
                  *min = reftype->Low;
                  *max = reftype->High;
                }
              else
                {
                  errstart (expr->FileName, expr->Line);
                  errDumpReference (expr->Var);
                  errcont (" might not be initialised (1)");
                  errend ();
                }
            }
          if (exprattr == ATTR_NULL)
            SetAccessed (basevar);
        }
      if (InFunction && RoutinePrefixClosed
          && (basevar->Mode == MODE_GLOBAL || basevar->Mode == MODE_SHARED)
          && (!(*staticexpression)))
        {
          err (expr->FileName, expr->Line,
               "Closed functions cannot read global variables");
        }
    }
  else if (op == _FN_CALL)
    {
      struct tProcedureCall *p = expr->Call->Ptr;
      struct tTypeSpec *returntype;
      bool tmpbool, success;
      struct tShareClause *shareclause;

      AnalyseProcedureCall (expr->FileName, expr->Line, p, &returntype,
                            &tmpbool, &success, TRUE, FALSE, &shareclause);
      if (returntype == NULL)
        {
          err (expr->FileName, expr->Line,
               "Call to a procedure in an expression");
        }
      if (returntype != NULL)
        {
          if (returntype->BaseType == INTEGER_TYPE)
            {
              *min = returntype->Low;
              *max = returntype->High;
              *exprtype = INTEGER_TYPE;
              *unit = returntype->Unit;
            }
          else if (returntype->BaseType == FLOATING_TYPE)
            {
              *digits = returntype->Digits;
              *magnitude = returntype->Magnitude;
              *exprtype = FLOATING_TYPE;
              *unit = returntype->Unit;
            }
          else if (returntype->BaseType == BOOLEAN_TYPE)
            {
              *exprtype = BOOLEAN_TYPE;
              *min = 0;
              *max = 1;
              *unit = NULL;
            }
          else if (returntype->BaseType == ACCESS_TYPE)
            {
              *accessedtype = returntype->ElementTypeSpec;
              *uncheckedaccess = returntype->UnmanagedAccess;
              *accessconstant = returntype->ConstantAccess;
              *accesspersistent = returntype->PersistentAccess;
              *accessnew = returntype->NewAccess;
              if (returntype->CanBeNull)
                {
                  *min = 0;
                  *max = 1;
                }
              else
                {
                  *min = 1;
                  *max = 1;
                }
              *exprtype = ACCESS_TYPE;
              *unit = NULL;
            }
          else if (returntype->BaseType == ADDRESS_TYPE)
            {
              *exprtype = ADDRESS_TYPE;
	      *uncheckedaccess = TRUE;
	      *accessconstant = FALSE;
	      *accesspersistent = FALSE;
	      *accessnew = FALSE;
	      *accessedtype = NULL;
              *unit = NULL;
            }
          else if (returntype->BaseType == CSTRING_TYPE)
            {
              *exprtype = CSTRING_TYPE;
              *unit = NULL;
            }
          expr->ComputationType = returntype->CType;
        }
      if (!success)
        {
          // Patch out the erroneous call so that we don't try to generate code for it later
          expr->Op = _CONST_INT;
          expr->Value = -1;
          *exprtype = INTEGER_TYPE;
          *unit = NULL;
        }
      *staticexpression = FALSE;
    }
  else
    if (!
        (IsIntegerOp (op) || IsFloatOp (op) || IsBooleanOp (op)
         || IsRelop (op)))
    {
      err (expr->FileName, expr->Line, "Expression expected");
      *exprtype = INTEGER_TYPE;
      *unit = NULL;
    }
  else
    {
      enum tBaseType ltype, rtype;
      BIGINT lmin, lmax, rmin, rmax, lintval, rintval;
      BIGFLOAT lfloatval, rfloatval;
      bool lboolval, rboolval;
      int ldigits, lmagnitude, rdigits, rmagnitude;
      struct tUnitDef *lunit, *runit;
      bool lstatic, rstatic, bothstatic;
      struct tTypeSpec *laccessed, *raccessed;
      bool luncheckedaccess, runcheckedaccess, laccessconstant,
        raccessconstant, laccesspersistent, raccesspersistent, laccessnew, raccessnew;

      AnalyseExpressionAsWritten (expr->Left, &ltype, &lmin, &lmax, &ldigits,
                                  &lmagnitude, &lunit, &lstatic, &lintval,
                                  &lfloatval, &lboolval, &laccessed,
                                  &luncheckedaccess,
                                  parameter /*****=====FALSE*/ , rangefor,
                                  verbose,
                                  virtualexpression, &laccessconstant, &laccesspersistent, &laccessnew);
      expr->Left->Min = lmin;
      expr->Left->Max = lmax;
      if (op != _NOT)
        {
          AnalyseExpressionAsWritten (expr->Right, &rtype, &rmin, &rmax,
                                      &rdigits, &rmagnitude, &runit, &rstatic,
                                      &rintval, &rfloatval, &rboolval,
                                      &raccessed, &runcheckedaccess,
                                      parameter /*****=====FALSE*/ , rangefor,
                                      verbose,
                                      virtualexpression, &raccessconstant, &raccesspersistent, &raccessnew);
          expr->Right->Min = rmin;
          expr->Right->Max = rmax;
          bothstatic = (lstatic && rstatic);
        }
      else
        {
          bothstatic = lstatic;
        }
      switch (op)
        {
        case _PLUS:
          if (ltype == INTEGER_TYPE && rtype == INTEGER_TYPE)
            {
              ptest (expr->FileName, expr->Line, lmin, 1, rmin, 0, min);
              ptest (expr->FileName, expr->Line, lmax, 1, rmax, 0, max);
              if (bothstatic)
                *intval = lintval + rintval;
            }
          else if (ltype == INTEGER_TYPE && rtype == FLOATING_TYPE)
            {
              if (bothstatic)
                *floatval = (BIGFLOAT) lintval + rfloatval;
            }
          else if (ltype == FLOATING_TYPE && rtype == INTEGER_TYPE)
            {
              if (bothstatic)
                *floatval = lfloatval + (BIGFLOAT) rintval;
            }
          else if (ltype == FLOATING_TYPE && rtype == FLOATING_TYPE)
            {
              if (bothstatic)
                *floatval = lfloatval + rfloatval;
            }
          else
            {
              err (expr->FileName, expr->Line,
                   "Numbers required for '+' (got %s + %s)",
                   basetypename (ltype), basetypename (rtype));
            }
          TestUnitMatch (lunit, runit, expr, TRUE);
          SetLRUnit(unit, lunit, runit);
          break;
        case _MINUS:
          if (ltype == INTEGER_TYPE && rtype == INTEGER_TYPE)
            {
              ptest (expr->FileName, expr->Line, lmin, 1, 0, rmax, min);
              ptest (expr->FileName, expr->Line, lmax, 1, 0, rmin, max);
              if (bothstatic)
                *intval = lintval - rintval;
            }
          else if (ltype == INTEGER_TYPE && rtype == FLOATING_TYPE)
            {
              if (bothstatic)
                *floatval = (BIGFLOAT) lintval - rfloatval;
            }
          else if (ltype == FLOATING_TYPE && rtype == INTEGER_TYPE)
            {
              if (bothstatic)
                *floatval = lfloatval - (BIGFLOAT) rintval;
            }
          else if (ltype == FLOATING_TYPE && rtype == FLOATING_TYPE)
            {
              if (bothstatic)
                *floatval = lfloatval - rfloatval;
            }
          else
            {
              err (expr->FileName, expr->Line, "Numbers required for '-'");
            }
          TestUnitMatch (lunit, runit, expr, TRUE);
          SetLRUnit(unit, lunit, runit);
          break;
        case _TIMES:
          if (ltype == INTEGER_TYPE && rtype == INTEGER_TYPE)
            {
              if (lmin >= 0 && rmin >= 0)
                {
                  /* all positive */
                  ptest (expr->FileName, expr->Line, lmin, rmin, 0, 0, min);
                  ptest (expr->FileName, expr->Line, lmax, rmax, 0, 0, max);
                }
              else if (lmax <= 0 && rmax <= 0)
                {
                  /* all negative */
                  ptest (expr->FileName, expr->Line, lmax, rmax, 0, 0, min);
                  ptest (expr->FileName, expr->Line, lmin, rmin, 0, 0, max);
                }
              else if (lmax <= 0 && rmin >= 0)
                {
                  /* lhs is all negative, rhs is all positive */
                  ptest (expr->FileName, expr->Line, lmin, rmax, 0, 0, min);
                  ptest (expr->FileName, expr->Line, lmax, rmin, 0, 0, max);
                }
              else if (rmax <= 0 && lmin >= 0)
                {
                  /* reverse of the previous case */
                  ptest (expr->FileName, expr->Line, rmin, lmax, 0, 0, min);
                  ptest (expr->FileName, expr->Line, rmax, lmin, 0, 0, max);
                }
              else if (lmin <= 0 && lmax >= 0)
                straddle0 (expr->FileName, expr->Line, lmin, lmax, rmin, rmax,
                           min, max);
              else
                straddle0 (expr->FileName, expr->Line, rmin, rmax, lmin, lmax,
                           min, max);
              if (bothstatic)
                *intval = lintval * rintval;
            }
          else if (ltype == INTEGER_TYPE && rtype == FLOATING_TYPE)
            {
              if (bothstatic)
                *floatval = (BIGFLOAT) lintval *rfloatval;
            }
          else if (ltype == FLOATING_TYPE && rtype == INTEGER_TYPE)
            {
              if (bothstatic)
                *floatval = lfloatval * (BIGFLOAT) rintval;
            }
          else if (ltype == FLOATING_TYPE && rtype == FLOATING_TYPE)
            {
              if (bothstatic)
                *floatval = lfloatval * rfloatval;
            }
          else
            {
              err (expr->FileName, expr->Line, "Numbers required for '*'");
            }
          *unit = MergeUnit (lunit, runit, 1);
          break;
        case _DIVIDE:
          if (ltype == INTEGER_TYPE && rtype == INTEGER_TYPE)
            {
              /* The divisor must be provably > 0 or < 0 in order to avoid a potential division by zero error.
               * This might require some extra tests, although it's usually just positive anyway.
               */
              if (rmin > 0 && rmax > 0)
                {
                  if (lmin >= 0)
                    {
                      *min = lmin / rmax;
                      *max = lmax / rmin;
                    }
                  else if (lmax >= 0)
                    {
                      *min = lmin / rmin;
                      *max = lmax / rmin;
                    }
                  else
                    {
                      *min = lmin / rmin;
                      *max = lmax / rmax;
                    }
                  if (bothstatic)
                    *intval = lintval / rintval;
                }
              else if (rmin < 0 && rmax < 0)
                {
                  if (lmax <= 0)
                    {
                      *min = lmax / rmin;
                      *max = lmin / rmax;
                    }
                  else if (lmin <= 0)
                    {
                      *min = lmax / rmax;
                      *max = lmin / rmax;
                    }
                  else
                    {
                      *min = lmax / rmax;
                      *max = lmin / rmin;
                    }
                  if (bothstatic)
                    *intval = lintval / rintval;
                }
              else
                {
                  err (expr->FileName, expr->Line,
                       "Divisor must be positive or negative (rhs has range %"PRINTBIG"..%"PRINTBIG")", rmin, rmax);
                }
            }
          else if (ltype == INTEGER_TYPE && rtype == FLOATING_TYPE)
            {
              if (bothstatic)
                *floatval = (BIGFLOAT) lintval / rfloatval;
            }
          else if (ltype == FLOATING_TYPE && rtype == INTEGER_TYPE)
            {
              if (rstatic && (rintval == 0))
                {
                  err (expr->FileName, expr->Line, "Division by zero");
                }
              else
                {
                  if (bothstatic)
                    *floatval = lfloatval / (BIGFLOAT) rintval;
                }
            }
          else if (ltype == FLOATING_TYPE && rtype == FLOATING_TYPE)
            {
              if (bothstatic)
                *floatval = lfloatval / rfloatval;
            }
          else
            {
              err (expr->FileName, expr->Line, "Numbers required for '/'");
            }
          *unit = MergeUnit (lunit, runit, -1);
          break;
        case _MOD:
          if (ltype == INTEGER_TYPE && rtype == INTEGER_TYPE)
            {
              /* LHS must be >= 0, RHS must be > 0.
               * We do not allow mod of or by negative numbers because it is more likely to be a mistake
               * than intentional. In the relatively rare cases that mod of or by a negative number is
               * required, it can be coded explicitly.
               */
              if (!(rmin > 0 && rmax > 0))
                {
                  err (expr->FileName, expr->Line,
                       "Potential mod by a negative number or zero (rhs has range %"
                       PRINTBIG "..%" PRINTBIG ")", rmin, rmax);
                }
              else if (lmin < 0)
                {
                  err (expr->FileName, expr->Line,
                       "Potential mod of a negative number (lhs has range %"
                       PRINTBIG "..%" PRINTBIG ")", lmin, lmax);
                }
              else
                {
                  if (lmax == 0)
                    {
                      *min = 0;
                      *max = 0;
                    }
                  else if (lmax < rmin)
                    {
                      *min = lmin;
                      *max = lmax;
                    }
                  else if (lmax < rmax)
                    {
                      *min = 0;
                      *max = lmax;
                    }
                  else
                    {
                      *min = 0;
                      *max = rmax - 1;
                    }
                  if (bothstatic)
                    *intval = lintval % rintval;
                }
            }
          else
            {
              err (expr->FileName, expr->Line,
                   "mod is only defined for integers");
            }
          *unit = MergeUnit (lunit, runit, -1);
          break;
        case _IAND:
          if (ltype == INTEGER_TYPE && rtype == INTEGER_TYPE)
            {
              if (lmin >= 0 && rmin >= 0)
                {
                  unsigned mag;

                  if (lmax < rmax)
                    mag = binlog (lmax);
                  else
                    mag = binlog (rmax);
                  *min = 0;
                  *max = binexpmask (mag);
                  TestUnitMatch (lunit, runit, expr, TRUE);
                  SetLRUnit(unit, lunit, runit);
                  if (bothstatic)
                    *intval = lintval & rintval;
                }
              else
                {
                  err (expr->FileName, expr->Line,
                       "iand is only defined for non-negative values (here lhs >= %"
                       PRINTBIG ", rhs >= %" PRINTBIG "", lmin, rmin);
                }
            }
          else
            {
              err (expr->FileName, expr->Line,
                   "iand is only defined for integers");
            }
          break;
        case _IOR:
        case _IXOR:
          if (ltype == INTEGER_TYPE && rtype == INTEGER_TYPE)
            {
              if (lmin >= 0 && rmin >= 0)
                {
                  unsigned mag;

                  if (lmax > rmax)
                    mag = binlog (lmax);
                  else
                    mag = binlog (rmax);
                  *min = 0;
                  *max = binexpmask (mag);
                  TestUnitMatch (lunit, runit, expr, TRUE);
                  SetLRUnit(unit, lunit, runit);
                  if (bothstatic)
                    {
                      if (op == _IOR)
                        {
                          *intval = lintval | rintval;
                        }
                      else
                        {
                          *intval = lintval ^ rintval;
                        }
                    }
                }
              else
                {
                  err (expr->FileName, expr->Line,
                       "ior and ixor are only defined for non-negative values (here lhs >= %"
                       PRINTBIG ", rhs >= %" PRINTBIG "", lmin, rmin);
                }
            }
          else
            {
              err (expr->FileName, expr->Line,
                   "ior and ixor are only defined for integers");
            }
          break;
        case _LT:
        case _GT:
        case _LEQ:
        case _GEQ:
        case _EQ:
        case _NEQ:
          if ((ltype == ACCESS_TYPE || ltype == ADDRESS_TYPE) || (rtype == ACCESS_TYPE || rtype == ADDRESS_TYPE))
            {
              if ((ltype == ACCESS_TYPE || ltype == ADDRESS_TYPE) && (rtype == ACCESS_TYPE || rtype == ADDRESS_TYPE))
                {
                  if (op == _EQ || op == _NEQ)
                    {
                      if (bothstatic && lmin == 0 && lmax == 0 && rmin == 0
                          && rmax == 0)
                        {
                          // Both are the constant null
                          *boolval = (op == _EQ);
                          if (*boolval)
                            {
                              *min = 1;
                              *max = 1;
                            }
                          else
                            {
                              *min = 0;
                              *max = 0;
                            }
                        }
                      else if (lstatic && lmin == 0 && lmax == 0)
                        {
                          // The LHS is the constant null
                          if (op == _EQ)
                            {
                              if (rmax == 0)
                                {
                                  *min = 1;
                                  *max = 1;
                                }
                              else if (rmin != 0)
                                {
                                  *min = 0;
                                  *max = 0;
                                }
                              else
                                {
                                  *min = 0;
                                  *max = 1;
                                }
                            }
                          else
                            {
                              if (rmax == 0)
                                {
                                  *min = 0;
                                  *max = 0;
                                }
                              else if (rmin != 0)
                                {
                                  *min = 1;
                                  *max = 1;
                                }
                              else
                                {
                                  *min = 0;
                                  *max = 1;
                                }
                            }
                        }
                      else if (rstatic && rmin == 0 && rmax == 0)
                        {
                          // The RHS is the constant null
                          if (op == _EQ)
                            {
                              if (lmax == 0)
                                {
                                  *min = 1;
                                  *max = 1;
                                }
                              else if (lmin != 0)
                                {
                                  *min = 0;
                                  *max = 0;
                                }
                              else
                                {
                                  *min = 0;
                                  *max = 1;
                                }
                            }
                          else
                            {
                              if (lmax == 0)
                                {
                                  *min = 0;
                                  *max = 0;
                                }
                              else if (lmin != 0)
                                {
                                  *min = 1;
                                  *max = 1;
                                }
                              else
                                {
                                  *min = 0;
                                  *max = 1;
                                }
                            }
                        }
                      else
                        {
                          // Neither is the constant null
                          if (ltype != ADDRESS_TYPE && rtype != ADDRESS_TYPE)
                            {
                              // Neither is address; check the types
                              if (laccessed->ElementTypeSpec !=
                                  raccessed->ElementTypeSpec)
                                {
                                  err (expr->FileName, expr->Line,
                                       "Access type mismatch");
                                }
                              else if ((!luncheckedaccess) != (!runcheckedaccess))
                                {
                                  err (expr->FileName, expr->Line,
                                       "Unchecked access types cannot be compared with checked access types");
                                }
                            }
                          *min = 0;
                          *max = 1;
                        }
                    }
                  else
                    {
                      err (expr->FileName, expr->Line,
                           "The only comparisons defined for access values are = and /=");
                    }
                }
              else
                {
                  err (expr->FileName, expr->Line,
                       "Access values can only be compared with access values");
                }
            }
          else if (ltype == BOOLEAN_TYPE || rtype == BOOLEAN_TYPE)
            {
              if (ltype == BOOLEAN_TYPE && rtype == BOOLEAN_TYPE)
                {
                  if (op == _EQ || op == _NEQ)
                    {
                      if (bothstatic)
                        {
                          if (op == _EQ)
                            *boolval = (lboolval == rboolval);
                          else
                            *boolval = (lboolval != rboolval);
                          if (*boolval)
                            {
                              *min = 1;
                              *max = 1;
                            }
                          else
                            {
                              *min = 0;
                              *max = 0;
                            }
                        }
                      else
                        {
                          // min = false, max = true
                          *min = 0;
                          *max = 1;
                          // try to refine this
                          if (op == _EQ)
                            {
                              if (lmin == lmax && rmin == rmax
                                  && lmin == rmin)
                                *min = 1;       // always true
                              else if (lmax < rmin || lmin > rmax)
                                {
                                  *min = 0;     // always false
                                }
                            }
                          else
                            {
                              if (lmax < rmin || lmin > rmax)
                                *min = 1;       // always true
                              else if (lmin == lmax && rmin == rmax
                                       && lmin == rmin)
                                {
                                  *min = 0;     // always false
                                }
                            }
                        }
                    }
                  else
                    {
                      err (expr->FileName, expr->Line,
                           "The only comparisons defined for boolean values are = and /=");
                    }
                }
              else
                {
                  err (expr->FileName, expr->Line,
                       "Boolean values can only be compared with boolean values");
                }
            }
          else if ((ltype == INTEGER_TYPE || ltype == FLOATING_TYPE)
                   && (rtype == INTEGER_TYPE || rtype == FLOATING_TYPE))
            {
              TestUnitMatch (lunit, runit, expr, TRUE);
              if (op == _EQ || op == _NEQ)
                {
                  if (ltype == FLOATING_TYPE || rtype == FLOATING_TYPE)
                    {
                      err (expr->FileName, expr->Line,
                           "Exact equality and inequality are not defined for floating types");
                    }
                }
              if (bothstatic)
                {
                  if (ltype == INTEGER_TYPE && rtype == INTEGER_TYPE)
                    {
                      switch (op)
                        {
                        case _LT:
                          *boolval = (lintval < rintval);
                          break;
                        case _GT:
                          *boolval = (lintval > rintval);
                          break;
                        case _LEQ:
                          *boolval = (lintval <= rintval);
                          break;
                        case _GEQ:
                          *boolval = (lintval >= rintval);
                          break;
                        case _EQ:
                          *boolval = (lintval == rintval);
                          break;
                        case _NEQ:
                          *boolval = (lintval != rintval);
                          break;
                        default:
                          err (expr->FileName, expr->Line,
                               "Internal error 1\n");
                        }
                    }
                  else
                    {
                      if (ltype == FLOATING_TYPE)
                        lfloatval = (BIGFLOAT) lintval;
                      if (rtype == FLOATING_TYPE)
                        rfloatval = (BIGFLOAT) rintval;
                      switch (op)
                        {
                        case _LT:
                          *boolval = (lfloatval < rfloatval);
                          break;
                        case _GT:
                          *boolval = (lfloatval > rfloatval);
                          break;
                        case _LEQ:
                          *boolval = (lfloatval <= rfloatval);
                          break;
                        case _GEQ:
                          *boolval = (lfloatval >= rfloatval);
                          break;
                        case _EQ:
                        case _NEQ:
                          err (expr->FileName, expr->Line,
                               "Exact equality and inequality are not defined for floating types");
                          break;
                        default:
                          err (expr->FileName, expr->Line,
                               "Internal error 2\n");
                        }
                    }
                  if (*boolval)
                    {
                      *min = 1;
                      *max = 1;
                    }
                  else
                    {
                      *min = 0;
                      *max = 0;
                    }
                }
              else
                {
                  // By default min = false, max = true
                  *min = 0;
                  *max = 1;
                  // Try to refine this for integers
                  if (ltype == INTEGER_TYPE && rtype == INTEGER_TYPE)
                    {
                      switch (op)
                        {
                        case _LT:
                          if (lmax < rmin)
                            *min = 1;   // always true
                          else if (lmin >= rmax)
                            *max = 0;   // always false
                          break;
                        case _GT:
                          if (lmin > rmax)
                            *min = 1;   // always true
                          else if (lmax <= rmin)
                            *max = 0;   // always false
                          break;
                        case _LEQ:
                          if (lmax <= rmin)
                            *min = 1;   // always true
                          else if (lmin > rmax)
                            *max = 0;   // always false
                          break;
                        case _GEQ:
                          if (lmin >= rmax)
                            *min = 1;   // always true
                          else if (lmax < rmin)
                            *max = 0;   // always false
                          break;
                        case _EQ:
                          if (lmin == lmax && rmin == rmax && lmin == rmin)
                            *min = 1;   // always true
                          else if (lmax < rmin || lmin > rmax)
                            *max = 0;   // always false
                          break;
                        case _NEQ:
                          if (lmax < rmin || lmin > rmax)
                            *min = 1;   // always true
                          else if (lmin == lmax && rmin == rmax
                                   && lmin == rmin)
                            *max = 0;   // always false
                          break;
                        default:
                          err (expr->FileName, expr->Line,
                               "Internal error 3\n");
                        }
                      TestBooleanRelation (expr->Left, expr->Right, op, min,
                                           max);
                    }
                }
            }
          else
            {
              err (expr->FileName, expr->Line,
                   "Numeric or access types required for relational operator");
            }
          *exprtype = BOOLEAN_TYPE;
          *unit = NULL;
          break;
        case _AND:
        case _OR:
          if (ltype != BOOLEAN_TYPE || rtype != BOOLEAN_TYPE)
            {
              err (expr->FileName, expr->Line,
                   "Booleans expected for boolean operator");
            }
          else
            {
              if (bothstatic)
                {
                  if (op == _AND)
                    *boolval = (lboolval && rboolval);
                  else
                    *boolval = (lboolval || rboolval);
                  if (*boolval)
                    {
                      *min = 1;
                      *max = 1;
                    }
                  else
                    {
                      *min = 0;
                      *max = 0;
                    }
                }
              else
                {
                  *min = 0;     // false
                  *max = 1;     // true
                  if (op == _AND)
                    {
                      if (lmin == 1 && rmin == 1)
                        *min = 1;       // both true
                      else if (lmax == 0 || rmax == 0)
                        *max = 0;       // at least one false
                    }
                  else
                    {
                      if (lmin == 1 || rmin == 1)
                        *min = 1;       // at least one true
                      else if (lmax == 0 && rmax == 0)
                        *max = 0;       // both false
                    }
                }
            }
          *exprtype = BOOLEAN_TYPE;
          *unit = NULL;
          break;
        case _NOT:
          if (ltype != BOOLEAN_TYPE)
            {
              err (expr->FileName, expr->Line,
                   "Boolean expected for not operator");
            }
          if (lstatic)
            {
              *boolval = !lboolval;
              if (*boolval)
                {
                  *min = 1;
                  *max = 1;
                }
              else
                {
                  *min = 0;
                  *max = 0;
                }
            }
          else
            {
              *min = 0;
              *max = 1;
              if (lmax == 0)
                *min = 1;       // not (false)
              else if (lmin == 1)
                *max = 0;       // not (true)
            }
          *exprtype = BOOLEAN_TYPE;
          *unit = NULL;
          break;
        default:
          err (expr->FileName, expr->Line, "Internal error 4\n");
        }
      //
      if (IsFloatOp (op)
          && (ltype == FLOATING_TYPE || rtype == FLOATING_TYPE))
        {
          // Get the type for the largest range
          if (ltype == INTEGER_TYPE)
            {
              BIGINT lr = MaxAbs (lmin, lmax);
              lmagnitude = IntMagnitude (lr);
              ldigits = IntDigits (lr);
            }
          if (rtype == INTEGER_TYPE)
            {
              BIGINT rr = MaxAbs (rmin, rmax);
              rmagnitude = IntMagnitude (rr);
              rdigits = IntDigits (rr);
            }
          if (lmagnitude > rmagnitude)
            *magnitude = lmagnitude;
          else
            *magnitude = rmagnitude;
          if (ldigits > rdigits)
            *digits = ldigits;
          else
            *digits = rdigits;
          *exprtype = FLOATING_TYPE;
          expr->ComputationType =
            GetTargetFloatType (expr->FileName, expr->Line, *digits,
                                *magnitude, TRUE);
        }
      else if (IsIntegerOp (op))
        { // after the float test
          // Possibly restrict the range further based on an 'A - B' expression processed earlier
          if (overridemin)
            {
              if (overridelow > *min)
                *min = overridelow;
            }
          if (overridemax)
            {
              if (overridehigh < *max)
                *max = overridehigh;
            }
          *exprtype = INTEGER_TYPE;
          // Get the type for the largest range
          if (expr->ComputationType != NO_COMPUTATION)
            {
              struct tTargetType *comptype;

              comptype = GetTargetIntegerType (expr->FileName, expr->Line,
                                               MinBigInt (MinBigInt
                                                          (lmin, rmin), *min),
                                               MaxBigInt (MaxBigInt
                                                          (lmax, rmax), *max),
                                               TRUE, TRUE);
              comptype = AdjustComputationType (comptype, expr->Left);
              comptype = AdjustComputationType (comptype, expr->Right);
              // Compare with the current computation type (if any), rather then just assigning
              // to expr->ComputationType unconditionally, in case this is a reevaluation with adjusted ranges
              if (expr->ComputationType == NULL)
                {
                  expr->ComputationType = comptype;
                }
              else
                {
                  if (comptype->Max > expr->ComputationType->Max || comptype->Min < expr->ComputationType->Min)
                    {
                      comptype = GetTargetIntegerType(expr->FileName, expr->Line,
                                                      MinBigInt(expr->ComputationType->Min, comptype->Min),
                                                      MaxBigInt(expr->ComputationType->Max, comptype->Max),
                                                      TRUE, TRUE);
                      expr->ComputationType = comptype;
                    }
                }
            }
        }
      *staticexpression = bothstatic;
    }
  //
  if (verbose)
    {
      errstart ("", -1);
      errcont ("  ");
      errDumpExpr (expr);
      errcont (" is a %s", basetypename (*exprtype));
      if (*exprtype == INTEGER_TYPE)
        errcont (" with range %" PRINTBIG "..%" PRINTBIG "", *min, *max);
      else if (*exprtype == BOOLEAN_TYPE)
        {
          errcont (" with range ");
          errbool (*min);
          errcont ("..");
          errbool (*max);
        }
      errend_and_continue ();
    }
}

#define MAX_EXPR_INT_VARS 10

void
ScanExpression (struct tNode *expr, struct tVar **vartable,
                BIGINT * lowvalues, BIGINT * highvalues, int *nvars,
                bool * overflow)
{
  int j;
  bool found;

  if (*overflow)
    {
      return;
    }
  if (expr->IntVar != NULL)
    {
      found = FALSE;
      for (j = 0; j < *nvars; j++)
        {
          if (vartable[j] == expr->IntVar)
            {
              found = TRUE;
              break;
            }
        }
      if (!found)
        {
          if (*nvars < MAX_EXPR_INT_VARS)
            {
              vartable[*nvars] = expr->IntVar;
              lowvalues[*nvars] = expr->IntVar->Low;
              highvalues[*nvars] = expr->IntVar->High;
              (*nvars)++;
            }
          else
            {
              *overflow = TRUE;
            }
        }
    }
  else
    {
      if (expr->Left != NULL)
        {
          ScanExpression (expr->Left, vartable, lowvalues, highvalues, nvars,
                          overflow);
        }
      if (expr->Right != NULL)
        {
          ScanExpression (expr->Right, vartable, lowvalues, highvalues, nvars,
                          overflow);
        }
    }
}

void
ReevaluateExpression (struct tNode *expr, bool parameter, bool rangefor,
                      struct tVar **vartable, BIGINT * lowvalues,
                      BIGINT * highvalues, int varnum, int nvars,
                      BIGINT * minsofar, BIGINT * maxsofar)
{
  BIGINT v;
  enum tBaseType exprtype;
  BIGINT min, max;
  int digits, magnitude;
  bool staticexpression;
  BIGINT intval;
  BIGFLOAT floatval;
  bool boolval;
  struct tTypeSpec *accessed;
  bool uncheckedaccess, accessconstant, accesspersistent, accessnew, virtualexpression;
  struct tUnitDef *unit;
  struct tRelation *rel;

  if (varnum == nvars)
    {
      // All values have been set
      // Check x - y restrictions

      int dir;
      bool mismatch;
      BIGINT prevvalue[MAX_EXPR_INT_VARS], newvalue[MAX_EXPR_INT_VARS];

      dir = 0;
      do
        {
          int j1, j2, j, k;

          {
            int k;
            for (k = 0; k < nvars; k++)
              {
                prevvalue[k] = vartable[k]->Low;
                newvalue[k] = prevvalue[k];
              }
          }
          // In the case if mismatches against x - y conditions, perform adjustments to each variable of mismatching pairs
          // in turn (hence the do loop)
          mismatch = FALSE;
          for (j1 = 0; j1 < nvars - 1; j1++)
            {
              for (j2 = j1 + 1; j2 < nvars; j2++)
                {
                  if (dir == 0)
                    {
                      j = j1;
                      k = j2;
                    }
                  else
                    {
                      j = j2;
                      k = j1;
                    }
                  // search for j - k in [L, H]
                  rel =
                    FindRelationIndirect (vartable[j], ATTR_NULL, vartable[k],
                                          ATTR_NULL);
                  if (rel != NULL)
                    {
                      if (rel->LowSet)
                        {
                          if (vartable[j]->Low - vartable[k]->Low < rel->Low)
                            { // ->Low = ->High at this point
                              mismatch = TRUE;
                              // j - k >= L   =>   k <= j - L, j >= k + L
                              v = vartable[j]->Low - rel->Low;
                              if (v > highvalues[k])
                                v = highvalues[k];
                              //vartable[k]->High = v;
                              newvalue[k] = v;
                            }
                        }
                      if (rel->HighSet)
                        {
                          if (vartable[j]->Low - vartable[k]->Low > rel->High)
                            {
                              mismatch = TRUE;
                              // j - k <= H   =>   k >= j - H
                              v = vartable[j]->Low - rel->High;
                              if (v < lowvalues[k])
                                v = lowvalues[k];
                              newvalue[k] = v;
                            }
                        }
                      free (rel);
                    }
                }
            }
          if (mismatch)
            {
              int k;
              for (k = 0; k < nvars; k++)
                {
                  vartable[k]->Low = newvalue[k];
                  vartable[k]->High = newvalue[k];
                }
            }
          AnalyseExpressionAsWritten (expr, &exprtype, &min, &max,  // for integers and booleans (0 false 1 true)
                                      &digits, &magnitude,          // for floats
                                      &unit,
                                      &staticexpression, &intval, &floatval,
                                      &boolval, &accessed, &uncheckedaccess,
                                      parameter, rangefor, FALSE,
                                      &virtualexpression, &accessconstant, &accesspersistent, &accessnew);

          {
            int k;
            for (k = 0; k < nvars; k++)
              {
                vartable[k]->Low = prevvalue[k];
                vartable[k]->High = prevvalue[k];
              }
          }
          if (min < *minsofar)
            {
              *minsofar = min;
            }
          if (max > *maxsofar)
            {
              *maxsofar = max;
            }
          dir++;
        }
      while (dir == 1 && mismatch);
    }
  else
    {
      v = lowvalues[varnum];
      vartable[varnum]->Low = v;
      vartable[varnum]->High = v;
      ReevaluateExpression (expr, parameter, rangefor, vartable, lowvalues,
                            highvalues, varnum + 1, nvars, minsofar,
                            maxsofar);
      v = highvalues[varnum];
      vartable[varnum]->Low = v;
      vartable[varnum]->High = v;
      ReevaluateExpression (expr, parameter, rangefor, vartable, lowvalues,
                            highvalues, varnum + 1, nvars, minsofar,
                            maxsofar);
    }
}

void
AnalyseExpression (struct tNode *expr, enum tBaseType *exprtype, BIGINT * min, BIGINT * max,    // for integers and booleans (0 false 1 true)
                   int *digits, int *magnitude, // for floats
                   struct tUnitDef **unit,
                   bool * staticexpression, BIGINT * intval,
                   BIGFLOAT * floatval, bool * boolval,
                   struct tTypeSpec **accessedtype, bool * uncheckedaccess,
                   bool parameter, bool rangefor, bool verbose,
                   bool * virtualexpression, bool * accessconstant, bool * accesspersistent, bool * accessnew)
{
  *virtualexpression = FALSE;
  // First analyse the expression as it was written in order to determine the expression type,
  // whether it is static, and whether the expression can be evaluated on the target
  AnalyseExpressionAsWritten (expr, exprtype, min, max, // for integers and booleans (0 false 1 true)
                              digits, magnitude,        // for floats
                              unit,
                              staticexpression, intval, floatval, boolval,
                              accessedtype, uncheckedaccess,
                              parameter, rangefor, verbose,
                              virtualexpression, accessconstant, accesspersistent, accessnew);
  expr->Min = *min;
  expr->Max = *max;
  // If the expression is a non-static integer expression, then attempt to refine the
  // range min..max by substituting the full ranges (at this point in the program) of
  // individual integer variables in combinations and applying the relation constraints
  // to them in pairs, reevaluating the expression under each combination
  //
  // For example:
  //
  //    A [0..10]
  //    B [0..20]
  //
  //    if B < A then
  //      C := B + 100 - A;
  //    end if;
  //
  //    Cmax = Bmax + 100 - Amin = 20 + 100 - 0 = 120
  //
  //    however B < A => B - A < 0
  //
  //    therefore B + 100 - A < 100 for all A, B
  //
  if (*exprtype == INTEGER_TYPE && (!(*staticexpression)))
    {
      // Scan the expression and build up a list of all integer variables
      int nvars;
      struct tVar *intvars[MAX_EXPR_INT_VARS];
      BIGINT lowvalues[MAX_EXPR_INT_VARS], highvalues[MAX_EXPR_INT_VARS];
      bool overflow;

      nvars = 0;
      overflow = FALSE;
      ScanExpression (expr, intvars, lowvalues, highvalues, &nvars,
                      &overflow);
      if ((nvars != 0) && (!overflow))
        {
          struct tDynamicValues *savedvars;
          BIGINT accummin, accummax;

          // Save the current variable state
          savedvars = StoreDynamicValues ();
          //
          // Reevaluate the expression with full ranges of integer variables
          //
          accummin = MAX_BIGINT;
          accummax = MIN_BIGINT;
          ReevaluateExpression (expr, parameter, rangefor, intvars, lowvalues,
                                highvalues, 0, nvars, &accummin, &accummax);
          ReevaluateExpression (expr, parameter, rangefor, intvars, lowvalues,
                                highvalues, 0, nvars, &accummin, &accummax);
          //
          // Restore the current variable state
          RestoreDynamicValues (savedvars);
          FreeDynamicValues (savedvars);
          //
          if (accummin > *min)
            {
              if (VerboseBuild)
                {
                  printf ("%s:%d refined min from ", expr->FileName,
                          expr->Line);
                  tracewritebigint (*min);
                  printf (" to ");
                  tracewritebigint (accummin);
                  printf ("\n");
                }
              *min = accummin;
              expr->Min = accummin;
            }
          if (accummax < *max)
            {
              if (VerboseBuild)
                {
                  printf ("%s:%d refined max from ", expr->FileName,
                          expr->Line);
                  tracewritebigint (*max);
                  printf (" to ");
                  tracewritebigint (accummax);
                  printf ("\n");

                }
              *max = accummax;
              expr->Max = accummax;
            }
        }
    }
}


void
TestIntegerExpression (struct tNode *expr, BIGINT * min, BIGINT * max,
                       struct tUnitDef **unit, bool parameter, bool rangefor,
                       bool * virtualexpression)
{
  enum tBaseType exprtype;
  int digits, magnitude;
  bool staticexpression;
  BIGINT intval;
  BIGFLOAT floatval;
  bool boolval;
  struct tTypeSpec *accessed;
  bool uncheckedaccess, accessconstant, accesspersistent, accessnew;

  AnalyseExpression (expr,
                     &exprtype,
                     min, max,
                     &digits, &magnitude,
                     unit,
                     &staticexpression, &intval, &floatval, &boolval,
                     &accessed, &uncheckedaccess,
                     parameter, rangefor, FALSE, virtualexpression,
                     &accessconstant, &accesspersistent, &accessnew);
  if (exprtype != INTEGER_TYPE)
    {
      err (expr->FileName, expr->Line, "Integer expression expected");
    }
}

void
AnalyseIntegerToFloat (struct tTypeSpec *reftype, int *magnitude, int *digits,
                       struct tUnitDef **unit)
{
  BIGINT m;

  if (-reftype->Low > reftype->High)
    m = -reftype->Low;
  else
    m = reftype->High;
  *magnitude = IntMagnitude (m);
  *digits = IntDigits (m);
  *unit = reftype->Unit;
}

void
TestFloatExpression (struct tNode *expr, struct tUnitDef **unit, int *digits,
                     int *magnitude, bool parameter, bool * virtualexpression)
{
  enum tBaseType exprtype;
  BIGINT min, max;
  bool staticexpression;
  BIGINT intval;
  BIGFLOAT floatval;
  bool boolval;
  struct tTypeSpec *accessed;
  bool ucheckedaccess, accessconstant, accesspersistent, accessnew;

  AnalyseExpression (expr,
                     &exprtype,
                     &min, &max,
                     digits, magnitude,
                     unit,
                     &staticexpression, &intval, &floatval, &boolval,
                     &accessed, &ucheckedaccess,
                     parameter, FALSE, FALSE, virtualexpression,
                     &accessconstant, &accesspersistent, &accessnew);
  if (exprtype != FLOATING_TYPE)
    {
      err (expr->FileName, expr->Line, "Floating point expression expected");
    }
}

void
TestBooleanExpression (struct tNode *expr, bool parameter, BIGINT * min,
                       BIGINT * max, bool verbose, bool * virtualexpression)
{
  enum tBaseType exprtype;
  int digits, magnitude;
  bool staticexpression;
  BIGINT intval;
  BIGFLOAT floatval;
  bool boolval;
  struct tUnitDef *unit;
  struct tTypeSpec *accessed;
  bool ucheckedaccess, accessconstant, accesspersistent, accessnew;

  AnalyseExpression (expr,
                     &exprtype,
                     min, max,
                     &digits, &magnitude,
                     &unit,
                     &staticexpression, &intval, &floatval, &boolval,
                     &accessed, &ucheckedaccess,
                     parameter, FALSE, verbose, virtualexpression,
                     &accessconstant, &accesspersistent, &accessnew);
  if (exprtype != BOOLEAN_TYPE)
    {
      err (expr->FileName, expr->Line, "Boolean expression expected 1");
    }
}

void
TestAccessExpression (struct tNode *expr, bool parameter, bool * couldbenull,
                      bool * isnull, struct tTypeSpec **accesstype,
                      bool * uncheckedaccess, bool * isaccessvalue,
                      BIGINT * min, BIGINT * max, bool * virtualexpression,
                      bool * accessconstant, bool * accesspersistent, bool * accessnew, bool * addressexpr)
{
  enum tBaseType exprtype;
  int digits, magnitude;
  bool staticexpression;
  BIGINT intval;
  BIGFLOAT floatval;
  bool boolval;
  struct tUnitDef *unit;

  *couldbenull = TRUE;
  *accesstype = NULL;
  *isnull = FALSE;
  *isaccessvalue = FALSE;
  *accessconstant = FALSE;
  *accesspersistent = FALSE;
  *accessnew = FALSE;
  *addressexpr = FALSE;
  AnalyseExpression (expr,
                     &exprtype,
                     min, max,
                     &digits, &magnitude,
                     &unit,
                     &staticexpression, &intval, &floatval, &boolval,
                     accesstype, uncheckedaccess,
                     parameter, FALSE, FALSE, virtualexpression,
                     accessconstant, accesspersistent, accessnew);
  if (exprtype != ACCESS_TYPE && exprtype != ADDRESS_TYPE)
    {
      err (expr->FileName, expr->Line,
           "Access or address expression expected");
    }
  if (exprtype == ADDRESS_TYPE)
    {
      *couldbenull = TRUE;
      *isnull = FALSE;
      *addressexpr = TRUE;
    }
  if (*accesstype == NULL)
    {
      // null
      *couldbenull = TRUE;
      *isnull = TRUE;
    }
  else
    {
      *couldbenull = ((*accesstype)->CanBeNull && (*min == 0));
      *isnull = (*min == 0 && *max == 0);
    }
  *isaccessvalue = TRUE;
}

// Is a variable (not) part of a composite object?
bool
SimpleVar (struct tNode * n, struct tVar ** var, enum tAttribute * attr, bool allowunits)
{
  struct tVar *v;
  struct tReference *r;

  if (n->Op == _VAR)
    {
      r = n->Var;
      v = FindVariable (n->FileName, n->Line, &r, 4, FALSE, TRUE);
      *var = v;
      *attr = GetExprAttribute (n);
      return (r->Next == NULL); // Simple if there are no array, access or record references
    }
  else if (allowunits && (n->Op == _TIMES || n->Op == _DIVIDE))
    {
      // Test for multiplication or division by a unit. This needs to be handled so that units can be
      // cancelled out in expressions that involve differences X / X'unit - Y / Y'unit etc.
      if (n->Left->Op == _VAR && n->Right->Op == _VAR)
        {
          struct tReference *rleft, *rright;
          struct tVar *vleft, *vright;
          enum tAttribute aleft, aright;
          bool unitleft, unitright;

          rleft = n->Left->Var;
          rright = n->Right->Var;
          vleft = FindVariable(n->Left->FileName, n->Left->Line, &rleft, 4, FALSE, TRUE);
          aleft = GetExprAttribute(n->Left);
          vright = FindVariable(n->Right->FileName, n->Right->Line, &rright, 4, FALSE, TRUE);
          aright = GetExprAttribute(n->Right);
          unitleft = FALSE;
          if (vleft->Type->TypeSpec->BaseType == UNIT_TYPE)
            {
              unitleft = TRUE;
            }
          else
            {
              if (aleft == ATTR_UNIT)
                {
                  unitleft = TRUE;
                }
            }
          unitright = FALSE;
          if (vright->Type->TypeSpec->BaseType == UNIT_TYPE)
            {
              unitright = TRUE;
            }
          else
            {
              if (aright == ATTR_UNIT)
                {
                  unitright = TRUE;
                }
            }
          if (unitleft)
            {
              *var = vright;
              *attr = aright;
              return (rright->Next == NULL);
            }
          else if (unitright)
            {
              *var = vleft;
              *attr = aleft;
              return (rleft->Next == NULL);
            }
        }
    }
  // None of the above
  return FALSE;
}

/* Test a range against target types; if none found and not erroriffail then return NULL */
struct tTargetType *
GetTargetIntegerType (char *filename, int line, BIGINT min, BIGINT max,
                      bool arithmetic, bool erroriffail)
{
  int j;

  for (j = 0; j < NumTargetTypes; j++)
    {
      if (TargetTypes[j].BaseType == TARGET_INTEGER)
        {
          if (TargetTypes[j].Min <= min && TargetTypes[j].Max >= max)
            {
              if ((!arithmetic) || TargetTypes[j].UseForArithmetic)
                {
                  /* Found one */
                  return &TargetTypes[j];
                }
            }
        }
    }
  if (erroriffail)
    {
      err (filename, line,
           "Range %" PRINTBIG "..%" PRINTBIG
           " is not realisable on the target", min, max);
    }
  return NULL;
}

/* Test a precision and magnitude against target types */
struct tTargetType *
GetTargetFloatType (char *filename, int line, BIGINT digits, BIGINT magnitude,
                    bool arithmetic)
{
  int j;

  for (j = 0; j < NumTargetTypes; j++)
    {
      if (TargetTypes[j].BaseType == TARGET_FLOAT)
        {
          if (TargetTypes[j].Digits >= digits
              && TargetTypes[j].Magnitude >= magnitude)
            {
              if ((!arithmetic) || TargetTypes[j].UseForArithmetic)
                {
                  /* Found one */
                  return &TargetTypes[j];
                }
            }
        }
    }
  err (filename, line,
       "digits %" PRINTBIG " magnitude %" PRINTBIG
       " is not realisable on the target", digits, magnitude);
  return NULL;                  // For the sake of -Wall
}

struct tTargetType *
GetWordType (char *filename, int line, enum tTargetBaseType basetype,
             struct tNode *expr)
{
  char *wordname;
  struct tTargetType *target;

  if (expr->Op == _VAR)
    {
      if (expr->Var->Next == NULL)
        {
          wordname = expr->Var->VarName;
          target = GetTargetTypeByName (basetype, wordname);
          if (target == NULL)
            {
              err (filename, line, "Unknown word type '%s'", wordname);
              return NULL;
            }
          else
            {
              return target;
            }
        }
    }
  err (filename, line,
       "Syntax error in word clause: expect simple iname from target types");
  return NULL;
}

BIGINT
MinBigInt (BIGINT a, BIGINT b)
{
  if (a < b)
    return a;
  else
    return b;
}

BIGINT
MaxBigInt (BIGINT a, BIGINT b)
{
  if (a > b)
    return a;
  else
    return b;
}

int
FloatMagnitude (BIGFLOAT f)
{
  BIGFLOAT v, n;
  int count;

  if (f >= 0.0)
    v = f;
  else
    v = -f;
  n = 1.0;
  count = 0;
  while (n < v)
    {
      n = n * 10.0;
      count++;
    }
  return count;
}

int
IntMagnitude (BIGINT i)
{
  BIGINT v, n;
  int count;

  if (i >= 0)
    v = i;
  else
    v = -i;
  n = 1;
  count = 0;
  while (n < v)
    {
      n = n * 10;
      count++;
    }
  return count;
}

int
IntDigits (BIGINT i)
{
  return IntMagnitude (i);
}

unsigned
binlog (BIGINT v)
{
  unsigned L;

  L = 0;
  while (v != 0)
    {
      L++;
      v /= 2;
    }
  return L;
}

BIGINT
binexpmask (unsigned L)
{
  BIGINT v;

  v = 1;
  while (L != 0)
    {
      v *= 2;
      L--;
    }
  return v - 1;
}

// Evaluate a C-like character literal (e.g. 'a', '\n', '\x1a')
BIGINT
EvalCharacterLiteral (char *str)
{
  unsigned char chr, digit;

  chr = str[2];
  if (chr == '\'')
    {
      if (str[3] == '\'')
        chr = '\'';
      else
        chr = str[1];
    }
  else if (chr == '\\')
    {
      chr = '\\';
    }
  else if (chr == 'n')
    {
      chr = '\n';
    }
  else if (chr == 'r')
    {
      chr = '\r';
    }
  else if (chr == 'b')
    {
      chr = '\b';
    }
  else if (chr == 't')
    {
      chr = '\t';
    }
  else if (chr == 'f')
    {
      chr = '\f';
    }
  else if (chr == 'a')
    {
      chr = '\a';
    }
  else if (chr == 'v')
    {
      chr = '\v';
    }
  else if (chr == 'x')
    {
      digit = str[3];
      if (digit >= '0' && digit <= '9')
        chr = digit - '0';
      else
        chr = digit - 'a' + 10;
      if (str[4] != '\'')
        {
          digit = str[4];
          if (digit >= '0' && digit <= '9')
            digit = digit - '0';
          else
            digit = digit - 'a' + 10;
          chr = chr * 16 + digit;
        }
    }
  else if (chr >= '0' && chr <= '7')
    {
      chr = str[2] - '0';
      if (str[3] != '\'')
        {
          chr = chr * 8 + str[3] - '0';
          if (str[4] != '\'')
            chr = chr * 8 + str[4] - '0';
        }
    }
  // else use chr as it is (not really standard)
  return (BIGINT) chr;
}

void
EvalStaticExpression (struct tNode *expr, enum tBaseType *exprtype,
                      BIGINT * intval, BIGFLOAT * floatval, bool * boolval,
                      struct tTypeSpec **accessedtype, bool * uncheckedaccess,
                      struct tUnitDef **unit, bool * success)
{
  BIGINT min, max;
  int digits, magnitude;
  bool staticexpression, virtualexpr, accessconstant, accesspersistent, accessnew;

  AnalyseExpression (expr,
                     exprtype,
                     &min, &max,
                     &digits, &magnitude,
                     unit,
                     &staticexpression, intval, floatval, boolval,
                     accessedtype, uncheckedaccess,
                     FALSE, FALSE, FALSE, &virtualexpr, &accessconstant, &accesspersistent, &accessnew);

  *success = staticexpression;
}

// A common case is integers
BIGINT
EvalStaticInteger (struct tNode *expr, bool * success, bool reportfailure,
                   struct tUnitDef **unit)
{
  enum tBaseType exprtype;
  BIGINT intval;
  BIGFLOAT floatval;
  bool boolval;
  struct tTypeSpec *accessedtype;
  bool uncheckedaccess;

  EvalStaticExpression (expr, &exprtype, &intval, &floatval, &boolval,
                        &accessedtype, &uncheckedaccess, unit, success);
  if (exprtype != INTEGER_TYPE)
    *success = FALSE;
  if ((!*success) && reportfailure)
    {
      err (expr->FileName, expr->Line, "Could not evaluate static integer expression static=%d exprtype=%d", *success, exprtype);       // DumpExpr(expr); printf("\n");
    }
  return intval;
}

BIGINT
EvalStaticBoolean (struct tNode * expr, bool * success, bool reportfailure)
{
  enum tBaseType exprtype;
  BIGINT intval;
  BIGFLOAT floatval;
  bool boolval;
  struct tTypeSpec *accessedtype;
  bool uncheckedaccess;
  struct tUnitDef *unit;

  EvalStaticExpression (expr, &exprtype, &intval, &floatval, &boolval,
                        &accessedtype, &uncheckedaccess, &unit, success);
  if (exprtype != BOOLEAN_TYPE)
    *success = FALSE;
  if ((!*success) && reportfailure)
    {
      err (expr->FileName, expr->Line, "Could not evaluate static boolean expression static=%d exprtype=%d", *success, exprtype);       // DumpExpr(expr); printf("\n");
    }
  return boolval;
}

BIGFLOAT
EvalStaticFloat (struct tNode * expr, bool * success, bool reportfailure,
                 struct tUnitDef ** unit)
{
  enum tBaseType exprtype;
  BIGINT intval;
  BIGFLOAT floatval;
  bool boolval;
  struct tTypeSpec *accessedtype;
  bool uncheckedaccess;

  EvalStaticExpression (expr, &exprtype, &intval, &floatval, &boolval,
                        &accessedtype, &uncheckedaccess, unit, success);
  if (exprtype != FLOATING_TYPE && exprtype != INTEGER_TYPE)
    *success = FALSE;
  if ((!*success) && reportfailure)
    {
      err (expr->FileName, expr->Line, "Could not evaluate static float expression");   // DumpExpr(expr); printf("\n");
    }
  else if (exprtype == INTEGER_TYPE)
    {
      floatval = intval;
    }
  return floatval;
}

void
EvalStaticAccess (struct tNode *expr, bool * success,
                  struct tTypeSpec **accessedtype, bool reportfailure)
{
  enum tBaseType exprtype;
  BIGINT intval;
  BIGFLOAT floatval;
  bool boolval;
  struct tUnitDef *unit;
  bool uncheckedaccess;

  EvalStaticExpression (expr, &exprtype, &intval, &floatval, &boolval,
                        accessedtype, &uncheckedaccess, &unit, success);
  if ((exprtype != ACCESS_TYPE) && (exprtype != ADDRESS_TYPE))
    *success = FALSE;
  if ((!*success) && reportfailure)
    {
      err (expr->FileName, expr->Line, "Could not evaluate static access expression static=%d exprtype=%d", *success, exprtype);        // DumpExpr(expr); printf("\n");
    }
}

// Search for a relation between two variables, constructing one if necessary (and possible)
// Returns a pointer to a newly allocated structure that can be freed
struct tRelation *
FindRelationIndirect (struct tVar *lhs, enum tAttribute lhsattr,
                      struct tVar *rhs, enum tAttribute rhsattr)
{
  struct tRelation *r, *r1, *rel;
  unsigned n;
  BIGINT tmp;

  if (CurrentVars->NumRelations == 0)
    return NULL;
  rel = NULL;
  // Search for lhs - rhs in [L, H]
  r = FindRelation (lhs, lhsattr, rhs, rhsattr);
  if (r != NULL)
    {
      rel = checked_malloc (sizeof *rel);
      *rel = *r;
    }
  else
    {
      // Try rhs - lhs in [L, H]
      r = FindRelation (rhs, rhsattr, lhs, lhsattr);
      if (r != NULL)
        {
          // rhs - lhs >= L
          // lhs - rhs <= -L
          // rhs - lhs <= H
          // lhs - rhs >= -H
          // lhs - rhs in [-H, -L]
          rel = checked_malloc (sizeof *rel);
          rel->LHS = lhs;
          rel->LHSAttr = lhsattr;
          rel->RHS = rhs;
          rel->RHSAttr = rhsattr;
          rel->LowSet = r->HighSet;
          if (rel->LowSet)
            {
              rel->Low = -r->High;
            }
          rel->HighSet = r->LowSet;
          if (rel->HighSet)
            {
              rel->High = -r->Low;
            }
        }
      else
        {
          // Now look for lhs something, something rhs
          r = &CurrentVars->Relations[CurrentVars->NumRelations];
          for (n = 0; n < CurrentVars->NumRelations; n++)
            {
              r--;
              if ((r->LowSet || r->HighSet) && r->LHS == lhs
                  && r->LHSAttr == lhsattr)
                {
                  r1 =
                    FindRelationIndirect (r->RHS, r->RHSAttr, rhs, rhsattr);
                  if (r1 != NULL)
                    {
                      // lhs - r->RHS in [r->Low, r->High] and r->RHS - rhs in [r1->Low, r1->High]
                      // lhs >= r->RHS + r->Low, r->RHS >= rhs + r1->Low
                      //                         rhs <= r->RHS - r1->Low
                      //                         -rhs >= r1->Low - r->RHS
                      // lhs - rhs >= r->Low + r1->Low
                      // lhs <= r->RHS + r->High, r->RHS <= rhs + r1->High
                      //                         rhs >= r->RHS - r1->High
                      //                         -rhs <= r1->High - r->RHS
                      // lhs - rhs <= r->High + r1->High
                      // Allocate a new record the first time, but keep searching in an attempt to
                      // find low and high values
                      if (rel == NULL)
                        {
                          rel = checked_malloc (sizeof *rel);     //*****free later?
                          rel->LHS = lhs;
                          rel->LHSAttr = lhsattr;
                          rel->RHS = rhs;
                          rel->RHSAttr = rhsattr;
                          rel->LowSet = FALSE;
                          rel->HighSet = FALSE;
                        }
                      if (r->LowSet && r1->LowSet)
                        {
                          tmp = r->Low + r1->Low;
                          if (rel->LowSet)
                            {
                              if (tmp > rel->Low)
                                rel->Low = tmp; // Tighter range
                            }
                          else
                            {
                              rel->Low = tmp;
                              rel->LowSet = TRUE;
                            }
                        }
                      if (r->HighSet && r1->HighSet)
                        {
                          tmp = r->High + r1->High;
                          if (rel->HighSet)
                            {
                              if (tmp < rel->High)
                                rel->High = tmp;
                            }
                          else
                            {
                              rel->High = tmp;
                              rel->HighSet = TRUE;
                            }
                        }
                      free (r1);
                    }
                }
            }
        }
    }
  return rel;
}

void
TestBooleanLHSRHS (struct tVar *lvar, enum tAttribute lattr,
                   BIGINT loffsetmin, BIGINT loffsetmax, struct tVar *rvar,
                   enum tAttribute rattr, BIGINT roffsetmin,
                   BIGINT roffsetmax, enum tOp op, BIGINT * min, BIGINT * max)
{
  struct tRelation *rel;

  // Search for lhs - rhs
  rel = FindRelationIndirect (lvar, lattr, rvar, rattr);
  if (rel != NULL)
    {
      switch (op)
        {
        case _LT:
          //    L + x < R + y
          // => L + x - R - y < 0
          // L - R <= t
          // t(max) + x(max) - y(min) < 0    => true
          // t(min) + x(min) - y(max) >= 0   => false
          if (rel->HighSet && rel->High + loffsetmax - roffsetmin < 0)
            *min = 1;
          else if (rel->LowSet && rel->Low + loffsetmin - roffsetmax >= 0)
            *max = 0;
          break;
        case _GT:
          //    L + x > R + y
          // => L + x - R - y > 0
          // L - R <= t
          // t(min) + x(min) - y(max) > 0    => true
          // t(max) + x(max) - y(min) <= 0   => false
          if (rel->LowSet && rel->Low + loffsetmin - roffsetmax > 0)
            *min = 1;
          else if (rel->HighSet && rel->High + loffsetmax - roffsetmin <= 0)
            *max = 0;
          break;
        case _LEQ:
          //    L + x <= R + y
          // => L + x - R - y <= 0
          // L - R <= t
          // t(max) + x(max) - y(min) <= 0   => true
          // t(min) + x(min) - y(max) > 0    => false
          if (rel->HighSet && rel->High + loffsetmax - roffsetmin <= 0)
            *min = 1;
          else if (rel->LowSet && rel->Low + loffsetmin - roffsetmax > 0)
            *max = 0;
          break;
        case _GEQ:
          //    L + x > R + y
          // => L + x - R - y >= 0
          // L - R <= t
          // t(min) + x(min) - y(max) >= 0   => true
          // t(max) + x(max) - y(min) < 0    => false
          if (rel->LowSet && rel->Low + loffsetmin - roffsetmax >= 0)
            *min = 1;
          else if (rel->HighSet && rel->High + loffsetmax - roffsetmin < 0)
            *max = 0;
          break;
        case _EQ:
          // L - R <= t
          // if L - R = t
          //    L + x = R + y if t = y - x
          if (rel->LowSet && rel->HighSet && rel->Low == rel->High
              && loffsetmin == loffsetmax && roffsetmin == roffsetmax)
            {
              if (rel->Low == roffsetmin - loffsetmin)
                *min = 1;
              else
                *max = 0;
            }
          break;
        case _NEQ:
          // L - R <= t
          // if L - R = t
          //    L + x != R + y if t != y - x
          if (rel->LowSet && rel->HighSet && rel->Low == rel->High
              && loffsetmin == loffsetmax && roffsetmin == roffsetmax)
            {
              if (rel->Low != roffsetmin - loffsetmin)
                *min = 1;
              else
                *max = 0;
            }
          break;
        default:
          err ("", 0, "Internal error 20");
        }
    }
}

void
TestBooleanRelation (struct tNode *lhs, struct tNode *rhs, enum tOp op,
                     BIGINT * min, BIGINT * max)
{
  struct tNode *lvarexpr, *rvarexpr;
  struct tVar *leftvar, *rightvar;
  enum tAttribute leftattr, rightattr;
  BIGINT loffsetmin, loffsetmax, roffsetmin, roffsetmax, tmp;
  struct tUnitDef *unit;
  enum tOp reverseop;
  bool virtualexpr;

  if (lhs->Op == _VAR)
    {
      lvarexpr = lhs;
      loffsetmin = 0;
      loffsetmax = 0;
    }
  else if (lhs->Op == _PLUS || lhs->Op == _MINUS)
    {
      lvarexpr = lhs->Left;
      TestIntegerExpression (lhs->Right, &loffsetmin, &loffsetmax, &unit,
                             FALSE, FALSE, &virtualexpr);
      if (lhs->Op == _MINUS)
        {
          tmp = -loffsetmax;
          loffsetmax = -loffsetmin;
          loffsetmin = tmp;
        }
    }
  else
    return;
  if (rhs->Op == _VAR)
    {
      rvarexpr = rhs;
      roffsetmin = 0;
      roffsetmax = 0;
    }
  else if (rhs->Op == _PLUS || rhs->Op == _MINUS)
    {
      rvarexpr = rhs->Left;
      TestIntegerExpression (rhs->Right, &roffsetmin, &roffsetmax, &unit,
                             FALSE, FALSE, &virtualexpr);
      if (rhs->Op == _MINUS)
        {
          tmp = -roffsetmax;
          roffsetmax = -roffsetmin;
          roffsetmin = tmp;
        }
    }
  else
    return;
  if (SimpleVar (lvarexpr, &leftvar, &leftattr, FALSE)
      && SimpleVar (rvarexpr, &rightvar, &rightattr, FALSE))
    {
      TestBooleanLHSRHS (leftvar, leftattr, loffsetmin, loffsetmax, rightvar,
                         rightattr, roffsetmin, roffsetmax, op, min, max);
      if (*min == 1 || *max == 0)
        return;                 // proved
      // Try the reverse order
      switch (op)
        {
        case _LT:
          reverseop = _GT;
          break;
        case _GT:
          reverseop = _LT;
          break;
        case _LEQ:
          reverseop = _GEQ;
          break;
        case _GEQ:
          reverseop = _LEQ;
          break;
        default:
          reverseop = op;
          break;                // _EQ, _NEQ We are not inverting the meaning of the test, only the order of the operands
        }
      TestBooleanLHSRHS (rightvar, rightattr, roffsetmin, roffsetmax, leftvar,
                         leftattr, loffsetmin, loffsetmax, reverseop, min,
                         max);
    }
}

BIGINT
MaxAbs (BIGINT a, BIGINT b)
{
  BIGINT absa, absb;

  if (a >= 0)
    absa = a;
  else
    absa = -a;
  if (b >= 0)
    absb = b;
  else
    absb = -b;
  if (absa >= absb)
    return absa;
  else
    return absb;
}

bool
TestIndex (struct tNode * index, struct tVar * arrayvar,
           enum tAttribute arrayattr, enum tOp op)
{
  struct tReference *varref =
    MakeReference (IDENTIFIER_REFERENCE, arrayvar->Name, NULL, NULL);
  struct tAttributeChainEntry *attr =
    checked_malloc (sizeof *attr);
  attr->Name = arrayattr;
  attr->Next = NULL;
  struct tNode *nrhs =
    NewNode (index->FileName, index->Line, _VAR, NULL, NULL, 0, 0.0, varref,
             NULL, attr, NULL);
  struct tNode *nx =
    NewNode (index->FileName, index->Line, _MINUS, index, nrhs, 0, 0.0, NULL,
             NULL, NULL, NULL);
  BIGINT minx, maxx;
  struct tUnitDef *unitx;
  bool indexerror, virtualexpr;

  indexerror = FALSE;
  nx->ComputationType = NO_COMPUTATION;
  TestIntegerExpression (nx, &minx, &maxx, &unitx, FALSE, FALSE,
                         &virtualexpr);
  if (op == _GEQ)
    {
      if (!(minx >= 0))
        {
          indexerror = TRUE;
        }
    }
  else if (op == _LEQ)
    {
      if (!(maxx <= 0))
        {
          indexerror = TRUE;
        }
    }
  free (nrhs);
  free (nx);
  free (varref);
  return !indexerror;
}

void
AnalyseReference (char *filename, int line,
                  struct tReference *reference,
                  enum tBaseType *basetype,
                  struct tVar **basevar,
                  struct tTypeSpec **reftype,
                  BIGINT * min,
                  BIGINT * max,
                  BIGINT * initialmin,
                  BIGINT * initialmax,
                  struct tUnitDef **unit,
                  struct tTypeSpec **accessedtype,
                  bool * initialised,
                  enum tMode *mode,
                  int *lock,
                  bool parameter,
                  bool * wholeobject,
                  bool donotsetaccessed,
                  bool * constantaccess, bool typeorattributesearch,
                  bool * subsystemboundary)
{
  struct tVar *v;
  struct tReference *ref;
  struct tReference *r;
  struct tTypeSpec *basetypespec, *t, *indextype;
  BIGINT minvalue, maxvalue, minindex, maxindex, minhigh, maxhigh;
  struct tUnitDef *indexunit, *highunit;
  bool firstlevel, nextr, virtualexpr, globalaccesserror;

  ref = reference;
  v = SearchForVariable (filename, line, &ref, 1, FALSE, TRUE, typeorattributesearch, subsystemboundary);
  if (!donotsetaccessed)
    {
      SetAccessed (v);
    }
  *mode = v->Mode;
  *lock = v->Lock;
  *basevar = v;
  *constantaccess = FALSE;
  globalaccesserror = FALSE;
  if ((v->Mode == MODE_SHARED) && (!parameter)
      && (!(InPackageInitialisation || InPackageFinalisation))
      && (!CurrentExemptions.SubsystemAccess)
      && (!typeorattributesearch)
      && (!v->ShareLock)
      && (!InUnitTestCode()))
    {
      err (filename, line,
           "Direct access of shared variable outside of package/(sub)system initialisation/finalisation without lock (%s)",
           v->Name);
    }
  if (v->Mode == MODE_GLOBAL
      && IsVariable (v) && (!typeorattributesearch)
      && v->ConstValue == NULL
      && (!v->AccessSharedAtomic)
      && RoutinePrefixShared
      && (!CurrentExemptions.SubsystemAccess))
    {
      // This does not apply within a shared package
      if (SharedAncestorPackage (CurrentPackage, v->EnclosingPackage) == NULL)
        {
          globalaccesserror = TRUE;     // checked below
        }
    }
  basetypespec = v->Type->TypeSpec;
  *reftype = basetypespec;
  *basetype = basetypespec->BaseType;
  if ((v->Mode == MODE_GLOBAL || v->Mode == MODE_SHARED)
      && (!InPackageInitialisation))
    *initialised = TRUE;
  else
    *initialised = v->Initialised;
  *unit = NULL;
  *accessedtype = NULL;
  r = ref;
  t = basetypespec;

  if (t->BaseType == TYPE_TYPE)
    {
      if (typeorattributesearch)
        {
          t = t->Type->TypeSpec;
          if (t == NULL)
            {
              // The type definition is incomplete e.g. type t;
              return;
            }
        }
      else
        {
          err (filename, line, "A type is not allowed here");
        }
    }
  firstlevel = TRUE;
  *wholeobject = TRUE;
  while (TRUE)
    {
      if (t == NULL)
        {
          if (r != NULL)
            err (filename, line, "References are too deep, or incomplete type");
          break;
        }
      if (t->StructureAccess == HIDDEN_ACCESS)
        {
          bool lastref;
          if (r == NULL)
            lastref = TRUE;
          else if (r->Next == NULL)
            lastref = TRUE;
          else
            lastref = FALSE;
          if (!((parameter || typeorattributesearch) && lastref))
            {
              errstart (filename, line);
              errcont ("The structure of ");
              errDumpTypeSpec (t);
              errcont (" is hidden");
              errend ();
            }
        }
      if (r == NULL)
        break;
      nextr = TRUE;
      if (r->Next == NULL)
        {
          *reftype = t;
          if (NamedConstantType (t))
            {
              *unit = v->ConstUnit;
              *accessedtype = v->ConstAccessType;
            }
          else if (*basetype == UNIT_TYPE)
            {
              *unit = v->Unit;
              *accessedtype = NULL;
            }
          else
            {
              *unit = t->Unit;
              *accessedtype = t->ElementTypeSpec;       // only meaningful for access types
            }
          if (r == ref)
            {
              // Simple variable; get the calculated values
              *min = v->Low;
              *max = v->High;
              *initialmin = v->InitialLow;
              *initialmax = v->InitialHigh;
            }
          else
            {
              // Indirect (array element or record field); get the type values
              *min = t->Low;
              *max = t->High;
              *initialmin = t->Low;
              *initialmax = t->High;
            }
          //}
        }
      else if (t->BaseType == ARRAY_TYPE)
        {
          //
          if (r->Next->ReferenceType != ARRAY_REFERENCE
              && r->Next->ReferenceType != ARRAY_SLICE_REFERENCE
              && r->Next->ReferenceType != ARRAY_SLICE_LENGTH_REFERENCE)
            {
              err (filename, line, "Array index expression expected");
            }
          // Get the index type. Call GetTypeSpec because the name might
          // still be in scope, and therefore a lookup might still be required
          indextype = t->IndexTypeSpec;
          if (indextype->StructureAccess == HIDDEN_ACCESS)
            {
              errstart (filename, line);
              errcont ("The structure of index ");
              errDumpTypeSpec (indextype);
              errcont (" is hidden");
              errend ();
            }
          // Compute the range of the index expression, and check the unit
          TestIntegerExpression (r->Next->Index, &minvalue, &maxvalue,
                                 &indexunit, FALSE, FALSE, &virtualexpr);
          TestUnitMatch (indextype->Unit, indexunit, r->Next->Index, TRUE);
          //
          if (r->Next->ReferenceType == ARRAY_SLICE_REFERENCE
              || r->Next->ReferenceType == ARRAY_SLICE_LENGTH_REFERENCE)
            {
              // A slice
              if (!(t->MinDefinite && t->MaxDefinite))
                {
                  err (filename, line,
                       "A slice can only be taken from a closed array");
                }
              if (r->Next->Next != NULL)
                {
                  err (filename, line,
                       "An array slice must be the last element of a reference");
                }
              if (firstlevel)
                {
                  minindex = v->Low;
                  maxindex = v->High;
                }
              else
                {
                  minindex = t->Low;
                  maxindex = t->High;
                }
              if (r->Next->Index2 == NULL)
                {
                  // a[x..]
                  maxvalue = maxindex;
                }
              else
                {
                  TestIntegerExpression (r->Next->Index2, &minhigh, &maxhigh,
                                         &highunit, FALSE, FALSE,
                                         &virtualexpr);
                  if (r->Next->ReferenceType == ARRAY_SLICE_REFERENCE)
                    {
                      // a[x..y]
                      TestUnitMatch (indextype->Unit, highunit,
                                     r->Next->Index2, TRUE);
                      if (minhigh < maxvalue)
                        {
                          err (filename, line,
                               "Potentially zero or negative length for array slice '%s': (%"
                               PRINTBIG "..%" PRINTBIG ")..(%" PRINTBIG "..%"
                               PRINTBIG ")", v->Name, minvalue, maxvalue,
                               minhigh, maxhigh);
                        }
                      maxvalue = maxhigh;
                    }
                  else
                    {
                      // a[x for n]
                      TestUnitMatch (NULL, highunit, r->Next->Index2, TRUE);
                      if (minhigh <= 0)
                        {
                          err (filename, line,
                               "Potentially zero or negative length (%"
                               PRINTBIG "..%" PRINTBIG
                               ") for array slice '%s'", minhigh, maxhigh,
                               v->Name);
                        }
                      maxvalue = maxvalue + maxhigh - 1;
                    }
                }
              if (minvalue < minindex || maxvalue > maxindex)
                {
                  errstart (filename, line);
                  errcont ("Potential index out of range error: '");
                  errDumpTypeSpec (t);
                  errcont ("' has range %" PRINTBIG "..%" PRINTBIG
                           ", expression has range %" PRINTBIG "..%" PRINTBIG
                           "", minindex, maxindex, minvalue, maxvalue);
                  errend ();
                }
            }
          else
            {
              // a[x]
              bool mininrange, maxinrange;
              if (firstlevel && (!t->MinDefinite))
                {
                  // Require x - a'first >= 0
                  mininrange =
                    TestIndex (r->Next->Index, v, ATTR_FIRST, _GEQ);
                }
              else
                {
                  // Require minvalue >= the min index of the type
                  mininrange = (minvalue >= t->Low);
                }
              if (firstlevel && (!t->MaxDefinite))
                {
                  // Require x - a'last <= 0
                  maxinrange = TestIndex (r->Next->Index, v, ATTR_LAST, _LEQ);
                }
              else
                {
                  // Require maxvalue <= the max index of the type
                  maxinrange = (maxvalue <= t->High);
                }
              if (!(mininrange && maxinrange))
                {
                  err (filename, line,
                       "Index is potentially out of range %d %d", mininrange,
                       maxinrange);
                }
              t = t->ElementTypeSpec;
            }
        }
      else if (t->BaseType == RECORD_TYPE || t->BaseType == ACCESS_TYPE)
        {
          struct tFieldSpec *f;
          unsigned fieldnum;
          struct tTypeSpec *rectype;

          if (t->BaseType == ACCESS_TYPE)
            {
              rectype = t->ElementTypeSpec;
              if (!(r->Next->ReferenceType == FIELD_REFERENCE
                    || (rectype->BaseType == ARRAY_TYPE
                        && r->Next->ReferenceType == ARRAY_REFERENCE)))
                {
                  if (rectype->BaseType == ARRAY_TYPE)
                    {
                      err (filename, line,
                           "Incorrect access reference - expect a field or array index");
                    }
                  else
                    {
                      err (filename, line,
                           "Incorrect access reference - expect a field");
                    }
                }
              if (t->CanBeNull)
                {
                  if ((!firstlevel) || (v->Low != v->High) || (v->Low == 0))
                    {
                      err (filename, line,
                           "Unchecked access through potentially null pointer");
                    }
                }
              if (t->ConstantAccess)
                {
                  *constantaccess = TRUE;
                }
              if (firstlevel && (!typeorattributesearch))
                {
                  // The first element of the expression was an access variable; mark it
                  // as being accessed (since it was used)
                  SetAccessed (*basevar);
                  RecordGlobalAccess (line, GetGlobalsAccess (), *basevar,
                                      TRUE, FALSE, !(*basevar)->Initialised,
                                      FALSE);
                }
            }
          else
            {
              // RECORD_TYPE
              if (r->Next->ReferenceType != FIELD_REFERENCE)
                {
                  err (filename, line, "Field expected");
                }
              rectype = t;
            }
          if (r->Next->ReferenceType != FIELD_REFERENCE)
            {
              // This is Ptr[n] as an abbreviation for Ptr.all[n]
              // In this case don't advance the reference pointer
              t = rectype;
              nextr = FALSE;
            }
          else
            {
              if (!strcasecmp (r->Next->VarName, "all"))
                {
                  t = rectype;
                }
              else
                {
                  f = rectype->FieldList;
                  fieldnum = 0;
                  while (f != NULL)
                    {
                      if (!strcmp (r->Next->VarName, f->Name))
                        {
                          break;
                        }
                      f = f->Next;
                      fieldnum++;
                    }
                  if (f == NULL)
                    {
                      // Field not found
                      err (filename, line, "Field '%s' not found",
                           r->Next->VarName);
                    }
                  else
                    {
                      if (rectype->RecordType == REC_UNION)
                        {
                          if ((!firstlevel) || (v->Low != v->High))
                            {
                              err (filename, line,
                                   "Unchecked access to union field %s",
                                   r->Next->VarName);
                            }
                          else if (fieldnum != v->Low)
                            {
                              err (filename, line,
                                   "Access to wrong field %s of union",
                                   r->Next->VarName);
                            }
                        }
                      t = f->Typespec;  //GetTypeSpec(line, f->Type, &access); //Spec;
                      if (f->BitFieldSet)
                        {
                          reference->BitField = TRUE;
                        }
                    }
                }
            }
        }
      else
        {
          errstart (filename, line);
          errcont ("Illegal reference: ");
          errDumpReference (ref);
          errend ();
        }
      firstlevel = FALSE;
      if (nextr)
        {
          r = r->Next;
        }
      if (r != NULL)
        {
          *wholeobject = FALSE;
        }
    }
  t = *reftype;
  if (globalaccesserror
      && (!(CurrentExemptions.SharedAtomicAccess
            && (t->BaseType == INTEGER_TYPE || t->BaseType == BOOLEAN_TYPE)
            && t->CType->AtomicType))
      && (!v->AccessVirtual))
    {
      err (filename, line,
           "Shared routines cannot access non-shared globals (%s)", v->Name);
    }
}

// Trace through a chain of 'index_type 'element_type 'accessed_type 'type
// Update attr to any following attribute and reftype to point to a TYPE_TYPE
void
TraceTypeChain (char *filename, int line,
                struct tAttributeChainEntry **attribute,
                struct tTypeSpec **typespec)
{
  struct tAttributeChainEntry *attr = *attribute;
  struct tTypeSpec *reftype = *typespec;

  while (attr != NULL)
    {
      if (attr->Name == ATTR_TYPE)
        {
          // return a type
          if (reftype->BaseType != TYPE_TYPE)
            {
              reftype = reftype->ThisType->TypeSpec;
            }
          attr = attr->Next;
        }
      else if (attr->Name == ATTR_INDEX_TYPE)
        {
          if (reftype->BaseType == ARRAY_TYPE)
            {
              reftype = reftype->IndexTypeSpec->ThisType->TypeSpec;
              attr = attr->Next;
            }
          else if (reftype->BaseType == TYPE_TYPE
                   && reftype->Type->TypeSpec->BaseType == ARRAY_TYPE)
            {
              reftype =
                reftype->Type->TypeSpec->IndexTypeSpec->ThisType->TypeSpec;
              attr = attr->Next;
            }
          else
            {
              err (filename, line, "'%s can only be applied to an array",
                   AttributeNames[ATTR_INDEX_TYPE]);
            }
        }
      else if (attr->Name == ATTR_ELEMENT_TYPE)
        {
          if (reftype->BaseType == ARRAY_TYPE)
            {
              reftype = reftype->ElementTypeSpec->ThisType->TypeSpec;
              attr = attr->Next;
            }
          else if (reftype->BaseType == TYPE_TYPE
                   && reftype->Type->TypeSpec->BaseType == ARRAY_TYPE)
            {
              reftype =
                reftype->Type->TypeSpec->ElementTypeSpec->ThisType->TypeSpec;
              attr = attr->Next;
            }
          else
            {
              err (filename, line,
                   "'%s can only be applied to an array",
                   AttributeNames[ATTR_ELEMENT_TYPE]);
            }
        }
      else if (attr->Name == ATTR_ACCESSED_TYPE)
        {
          if (reftype->BaseType == ACCESS_TYPE)
            {
              reftype = reftype->ElementTypeSpec->ThisType->TypeSpec;
              attr = attr->Next;
            }
          else if (reftype->BaseType == TYPE_TYPE
                   && reftype->Type->TypeSpec->BaseType == ACCESS_TYPE)
            {
              reftype =
                reftype->Type->TypeSpec->ElementTypeSpec->ThisType->TypeSpec;
              attr = attr->Next;
            }
          else
            {
              err (filename, line,
                   "'%s can only be applied to an access",
                   AttributeNames[ATTR_ACCESSED_TYPE]);
            }
        }
      else if (attr->Name == ATTR_RETURN_TYPE)
        {
          if (reftype->BaseType == PROCEDURE_TYPE
              && reftype->ReturnType != NULL)
            {
              reftype = reftype->ReturnType->ThisType->TypeSpec;
              attr = attr->Next;
            }
          else
            {
              err (filename, line,
                   "'%s can only be applied to functions (within the body of a function f, f refers to the return variable so use f'unit)",
                   AttributeNames[ATTR_RETURN_TYPE]);
            }
        }
      else if (attr->Name == ATTR_PARAMETER_TYPE)
        {
          if (reftype->BaseType == PROCEDURE_TYPE)
            {
              struct tFormalParameter *formal;

              formal = reftype->Formals;
              while (formal != NULL)
                {
                  if (!strcmp (attr->AuxName, formal->Name))
                    {
                      reftype = formal->TypeSpec->ThisType->TypeSpec;
                      attr = attr->Next;
                      break;
                    }
                  formal = formal->Next;
                }
              if (formal == NULL)
                {
                  err (filename, line, "In '%s(%s), the parameter was not found",
                       AttributeNames[ATTR_PARAMETER_TYPE], attr->AuxName);
                }
            }
          else
            {
              err (filename, line,
                   "'%s can only be applied to procedures and functions",
                   AttributeNames[ATTR_PARAMETER_TYPE]);
            }
        }
      else if (attr->Name == ATTR_FIELD_TYPE)
        {
          struct tTypeSpec *ref;
          struct tFieldSpec *f;

          if (reftype->BaseType == RECORD_TYPE)
            {
              ref = reftype;
            }
          else if (reftype->BaseType == TYPE_TYPE)
            {
              ref = reftype->Type->TypeSpec;
            }
          else
            {
              err (filename, line,
                   "'%s can only be applied to a record or union",
                   AttributeNames[ATTR_FIELD_TYPE]);
              ref = NULL;
            }
          f = NULL;
          if (ref != NULL)
            {
              f = ref->FieldList;
              while (f != NULL)
                {
                  if (!strcmp (attr->AuxName, f->Name))
                    {
                      reftype = f->Typespec->ThisType->TypeSpec;
                      attr = attr->Next;
                      break;
                    }
                  f = f->Next;
                }
            }
          if (f == NULL)
            {
              err (filename, line, "In '%s(%s), the field was not found",
                   AttributeNames[ATTR_FIELD_TYPE], attr->AuxName);
            }
        }
      else
        {
          break;
        }
    }
  if (attr != NULL)
    {
      if (attr->Name == ATTR_ACCESS || attr->Name == ATTR_PERSISTENT_ACCESS)
        {
          if (attr != *attribute)
            {
              err (filename, line,
                   "Attributes '%s and '%s cannot appear to the right of other attributes",
                   AttributeNames[ATTR_ACCESS], AttributeNames[ATTR_PERSISTENT_ACCESS]);
            }
          else if (attr->Next != NULL)
            {
              err (filename, line,
                   "Attributes '%s and '%s cannot appear to the left of other attributes",
                   AttributeNames[ATTR_ACCESS], AttributeNames[ATTR_PERSISTENT_ACCESS]);
            }
        }
    }
  *attribute = attr;
  *typespec = reftype;
}

bool
IsRelop (enum tOp op)
{
  if (op == _LT || op == _GT || op == _LEQ || op == _GEQ
      || op == _EQ || op == _NEQ)
    return TRUE;
  else
    return FALSE;
}

bool
IsIntegerOp (enum tOp op)
{
  if (op == _PLUS || op == _MINUS || op == _TIMES || op == _DIVIDE
      || op == _MOD || op == _IAND || op == _IOR || op == _IXOR)
    return TRUE;
  else
    return FALSE;
}

bool
IsFloatOp (enum tOp op)
{
  if (op == _PLUS || op == _MINUS || op == _TIMES || op == _DIVIDE)
    return TRUE;
  else
    return FALSE;
}

bool
IsBooleanOp (enum tOp op)
{
  if (op == _AND || op == _OR || op == _NOT)
    return TRUE;
  else
    return FALSE;
}

bool
IsConstant (enum tOp op)
{
  if (op == _CONST_INT || op == _CONST_BOOL || op == _CONST_FLOAT
      || op == _CONST_STRING || op == _CONST_CHARACTER || op == _CONST_NULL)
    return TRUE;
  else
    return FALSE;
}


bool
NamedConstantType (struct tTypeSpec *t)
{
  return (t == ConstantIntegerTypespec || t == ConstantFloatTypespec
          || t == ConstantBooleanTypespec || t == ConstantAccessTypespec
          || t == ConstantStringTypespec);
}

// Does v represent a variable (as opposed to a type, package, procedure, constant etc.)
bool
IsVariable (struct tVar * v)
{
  enum tBaseType basetype;

  basetype = v->Type->TypeSpec->BaseType;
  if ((basetype == INTEGER_TYPE || basetype == FLOATING_TYPE
       || basetype == BOOLEAN_TYPE || basetype == ARRAY_TYPE
       || basetype == RECORD_TYPE || basetype == CSTRING_TYPE
       || basetype == ACCESS_TYPE || basetype == ADDRESS_TYPE)
      && (!NamedConstantType (v->Type->TypeSpec)))
    return TRUE;
  else
    return FALSE;
}

bool
IsNamedConstant (struct tVar * v)
{
  return NamedConstantType (v->Type->TypeSpec);
}

// Find the rightmost attribute in an expression
enum tAttribute
GetExprAttribute (struct tNode *expr)
{
  struct tAttributeChainEntry *p;

  p = expr->Attribute;
  if (p == NULL)
    {
      return ATTR_NULL;
    }
  else
    {
      while (p->Next != NULL)
        {
          p = p->Next;
        }
      return p->Name;
    }
}
