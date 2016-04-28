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
 
/* Parse tree support routines */

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

struct tTypeSpec *
MakeIntegerType (int line,
                 struct tNode *lowrange, struct tNode *highrange,
                 struct tUnitTerm *unit)
{
  struct tTypeSpec *t;

  t = NewTypespec (line);
  t->Line = line;
  t->BaseType = INTEGER_TYPE;
  t->LowExpr = lowrange;
  t->HighExpr = highrange;
  t->EnumList = NULL;
  t->ReqUnit = unit;
  t->StructureAccess = PRIVATE_ACCESS;
  return t;
}

struct tTypeSpec *
MakeFloatingType (int line,
                  BIGINT digits, BIGINT magnitude, struct tUnitTerm *unit)
{
  struct tTypeSpec *t;

  t = NewTypespec (line);
  t->Line = line;
  t->BaseType = FLOATING_TYPE;
  t->Digits = digits;
  t->Magnitude = magnitude;
  t->ReqUnit = unit;
  t->StructureAccess = PRIVATE_ACCESS;
  return t;
}

struct tArrayIndex *
MakeArrayIndex (enum tIndexSpecType indextype,
                struct tTypeIdentifier *indexname,
                struct tTypeSpec *indexspec)
{
  struct tArrayIndex *a = checked_malloc (sizeof *a);

  a->IndexType = indextype;
  a->IndexName = indexname;
  a->IndexTypeSpec = indexspec;
  return a;
}

struct tTypeSpec *
MakeArrayType (int line, struct tArrayIndex *indexspec, struct tTypeRefOrSpec *elementtype,     //struct tTypeIdentifier *elementtype,
               bool mindefinite, bool maxdefinite)
{
  struct tTypeSpec *t;

  t = NewTypespec (line);
  t->Line = line;
  t->BaseType = ARRAY_TYPE;
  t->IndexSpec = indexspec;
  //t->IndexType = indextype;
  t->ArrayElementType = elementtype;
  t->StructureAccess = PRIVATE_ACCESS;
  t->MinDefinite = mindefinite;
  t->MaxDefinite = maxdefinite;
  return t;
}

struct tTypeSpec *
SetTypeSpecAccess (struct tTypeSpec *typespec, enum tAccess access)
{
  typespec->StructureAccess = access;
  return typespec;
}

struct tTypeSpec *
SetControlled (struct tTypeSpec *typespec)
{
  typespec->Controlled = TRUE;
  return typespec;
}

struct tTypeSpec *
SetRepresentation (struct tTypeSpec *typespec,
                   struct tRepresentationClause *representation)
{
  typespec->Representation = representation;
  return typespec;
}

struct tRepresentationClause *
MakeRepresentationClause (char *name, struct tNode *expr)
{
  struct tRepresentationClause *r =
    checked_malloc (sizeof *r);

  r->Name = name;
  r->Value = expr;
  r->Next = NULL;
  return r;
}

struct tRepresentationClause *
PrefixRepresentationClause (struct tRepresentationClause *head,
                            struct tRepresentationClause *tail)
{
  head->Next = tail;
  return head;
}

// Attribute to name
const char *AttributeNames[ATTR_NULL] =
  { "first", "last", "length", "unit", "index_unit",
  "minimum", "maximum",         /*"first_no_unit", "last_no_unit", */
  "access", "persistent_access", "size", "initial", "type",
  "index_type", "element_type", "accessed_type", "return_type", "parameter_type", "field_type"
};

struct tSingleAttribute *
MakeSingleAttribute (char *attrname, char *auxname)
{
  struct tSingleAttribute *p =
    checked_malloc (sizeof *p);

  p->AttrName = attrname;
  p->AuxName = auxname;
  return p;
}

struct tAttributeChainEntry *
MakeAttributeChainEntry (char *filename, int line,
                         struct tSingleAttribute *attr)
{
  struct tAttributeChainEntry *p =
    checked_malloc (sizeof *p);
  enum tAttribute a;

  p->Next = NULL;
  p->AuxName = attr->AuxName;
  for (a = ATTR_FIRST; a < ATTR_NULL; a++)
    {
      if (!strcasecmp (attr->AttrName, AttributeNames[a]))
        {
          p->Name = a;
          break;
        }
    }
  if (a == ATTR_NULL)
    {
      err (filename, line, "Undefined attribute %s", attr->AttrName);
    }
  return p;
}

struct tAttributeChainEntry *
PrefixAttributeChainEntry (char *filename, int line,
                           struct tSingleAttribute *attr,
                           struct tAttributeChainEntry *chain)
{
  struct tAttributeChainEntry *p =
    MakeAttributeChainEntry (filename, line, attr);

  p->Next = chain;
  return p;
}

struct tTypeDeclaration *
AddType (char *name, struct tTypeSpec *typespec)
{
  struct tTypeDeclaration *t;

  t = checked_malloc (sizeof *t);
  t->Name = name;
  t->TypeSpec = typespec;
  return t;
}

struct tRepresentationSpec *
AddRepresentation (char *typename,
                   struct tRepresentationClause *representation)
{
  struct tRepresentationSpec *r;

  r = checked_malloc (sizeof *r);
  r->TypeName = typename;
  r->Representation = representation;
  return r;
}

struct tTypeIdentifier *
MakeTypeIdentifier (struct tReference *name,
                    struct tAttributeChainEntry *attribute)
{
  struct tTypeIdentifier *t;

  t = checked_malloc (sizeof *t);

  t->Name = name;
  t->Attribute = attribute;
  return t;
}

struct tVariableDeclaration *
DeclareVariable (
                  //char *filename, int line,
                  struct tIdentifierListEntry *varnames, struct tTypeIdentifier *typename, struct tTypeSpec *typespec,  // either typename or typespec
                  struct tRHS *constvalue, struct tReference *names,
                  enum tMode mode,
                  struct tRepresentationClause *representation,
                  enum tNameFor namefor)
{
  struct tVariableDeclaration *v;

  v = checked_malloc (sizeof *v);
//  v->FileName = filename;
//  v->Line = line;
  v->VarNames = varnames;
  v->TypeName = typename;
  v->TypeSpec = typespec;
  v->ConstValue = constvalue;
  v->Names = names;
  v->Mode = mode;
  v->Representation = representation;
  v->NameFor = namefor;
  v->PublicOut = FALSE;
  v->Vars = NULL;
  return v;
}

struct tElt *
SetMode (struct tElt *elt, enum tMode mode)
{
  struct tVariableDeclaration *v = elt->Ptr;

  v->Mode = mode;
  return elt;
}

struct tElt *
SetPublicOut (struct tElt *elt)
{
  struct tVariableDeclaration *v = elt->Ptr;

  v->PublicOut = TRUE;
  return elt;
}

struct tUnitDeclaration *
DeclareUnit (int line, char *unitname, struct tUnitTerm *unit)
{
  struct tUnitDeclaration *u;

  u = checked_malloc (sizeof *u);
  u->Line = line;
  u->UnitName = unitname;
  u->unit = unit;
  return u;
}

struct tAssignment *
Assignment (struct tReference *var, struct tRHS *rhs, bool makenew)
{
  struct tAssignment *a;

  a = checked_malloc (sizeof *a);
  a->Var = var;
  a->RHS = rhs;
  a->MakeNew = makenew;
  return a;
}

struct tRHS *
MakeRHS (enum tRHSType rhstype,
         struct tNode *expr,
         struct tArrayInitialisation *arrayexpr,
         struct tFieldInitialisation *recordexpr)
{
  struct tRHS *r;

  r = checked_malloc (sizeof *r);
  r->RHSType = rhstype;
  r->Expr = expr;
  r->ArrayExpr = arrayexpr;
  r->RecordExpr = recordexpr;
  return r;
}

struct tWhen *
When (struct tNode *cond, struct tElt *when_stmt, struct tElt *else_stmt, bool elsecanbecommentsonly)
{
  struct tWhen *w;

  w = (struct tWhen *) checked_malloc (sizeof *w);
  w->Cond = cond;
  w->When_stmt = when_stmt;
  w->Else_stmt = else_stmt;
  w->ElseCanBeCommentsOnly = elsecanbecommentsonly;
  return w;
}

struct tLoop *
Loop (struct tElt *body)
{
  struct tLoop *p;

  p = (struct tLoop *) checked_malloc (sizeof *p);
  p->Body = body;
  return p;
}

struct tWhile *
WhileStmt (struct tNode *cond, struct tElt *body)
{
  struct tWhile *p;

  p = (struct tWhile *) checked_malloc (sizeof *p);
  p->Cond = cond;
  p->Body = body;
  return p;
}

struct tDo *
DoStmt (struct tElt *body)
{
  struct tDo *p;

  p = (struct tDo *) checked_malloc (sizeof *p);
  p->Body = body;
  return p;
}

struct tFor *
ForStmt (struct tElt *controlvar, struct tNode *rangelow,
         struct tNode *rangehigh, struct tElt *body)
{
  struct tFor *p = checked_malloc (sizeof *p);

  p->ControlVar = controlvar;
  p->RangeLow = rangelow;
  p->RangeHigh = rangehigh;
  p->Body = body;
  return p;
}

struct tCase *
Case (struct tNode *caseexpr, struct tCaseOption *options)
{
  struct tCase *p = (struct tCase *) checked_malloc (sizeof *p);

  p->CaseExpr = caseexpr;
  p->Options = options;
  return p;
}

struct tElt *
AppendStmts (struct tElt *head, struct tElt *tail)
{
  struct tElt *p;

  if (head == NULL)
    return tail;
  else
    {
      p = head;
      while (p->Next != NULL)
        {
          p = p->Next;
        }
      p->Next = tail;
      return head;
    }
}

struct tElt *
Elt (char *filename, int line, int lastline, enum tEltType elttype, void *Ptr)
{
  struct tElt *e;

  e = (struct tElt *) checked_malloc (sizeof *e);
  e->FileName = filename;
  e->Line = line;
  e->LastLine = lastline;
  e->Type = elttype;
  e->Ptr = Ptr;
  e->Access = PRIVATE_ACCESS;
  e->Prefix = PREFIX_NONE;
  e->Next = NULL;
  return e;
}

struct tElt *
PrefixElt (struct tElt *first, struct tElt *rest)
{
  first->Next = rest;
  return first;
}

struct tRange *
MakeRange (struct tNode *minval, struct tNode *maxval)
{
  struct tRange *r =
    (struct tRange *) checked_malloc (sizeof *r);

  r->MinValue = minval;
  r->MaxValue = maxval;
  r->Used = FALSE;
  r->Next = NULL;
  return r;
}

struct tRange *
PrefixRange (struct tRange *head, struct tRange *tail)
{
  head->Next = tail;
  return head;
}

struct tCaseOption *
BuildCaseOption (struct tRange *ranges, struct tElt *stmts)
{
  struct tCaseOption *p =
    (struct tCaseOption *) checked_malloc (sizeof *p);

  p->Ranges = ranges;
  p->Stmts = stmts;
  p->Next = NULL;
  return p;
}

struct tCaseOption *
AppendCaseOption (struct tCaseOption *Left, struct tCaseOption *Right)
{
  struct tCaseOption *p;

  p = Left;
  if (p == NULL)
    return Right;
  else
    {
      while (p->Next != NULL)
        p = p->Next;
      p->Next = Right;
    }
  return Left;
}

struct tUnitTerm *
MakeUnitTerm (struct tReference *ref, int power, bool getunit)
{
  struct tUnitTerm *unit = checked_malloc (sizeof *unit);

  unit->Ref = ref;
  unit->Power = power;
  unit->Next = NULL;
  unit->GetUnit = getunit;
  return unit;
}

struct tUnitTerm *
PrefixUnitTerm (struct tUnitTerm *head, struct tUnitTerm *tail)
{
  head->Next = tail;
  return head;
}

struct tReference *
MakeReference (enum tReferenceType reftype,
               char *varname, struct tNode *index, struct tNode *index2)
{
  struct tReference *r = checked_malloc (sizeof *r);

  r->ReferenceType = reftype;
  r->VarName = varname;
  r->Index = index;
  r->Index2 = index2;
  r->ReferenceAbbreviation = NULL;
  r->ReferenceAbbreviationEnd = NULL;
  r->Next = NULL;
  r->BitField = FALSE;
  return r;
}

struct tReference *
PrefixReference (struct tReference *head, struct tReference *tail)
{
  head->Next = tail;
  return head;
}

struct tEnum *
MakeEnumSymbol (char *name, struct tRepresentationClause *representation)
{
  struct tEnum *e = (struct tEnum *) checked_malloc (sizeof *e);

  e->Name = name;
  e->Representation = representation;
  e->Next = NULL;
  e->ConstVar = NULL;
  return e;
}

struct tEnum *
PrefixEnumSymbol (struct tEnum *head, struct tEnum *tail)
{
  head->Next = tail;
  return head;
}

struct tTypeSpec *
MakeEnumType (int line, struct tEnum *e)
{
  struct tTypeSpec *t;

  t = NewTypespec (line);
  t->Line = line;
  t->BaseType = INTEGER_TYPE;
  t->EnumList = e;
  t->Unit = NULL;
  t->StructureAccess = PRIVATE_ACCESS;
  return t;
}

struct tArrayInitialisation *
MakeArrayInitialisation (char *varname,
                         struct tRHS *rhs,
                         struct tArrayInitialisationEntry *explicitinit)
{
  struct tArrayInitialisation *a =
    checked_malloc (sizeof *a);

  a->VarName = varname;
  a->RHS = rhs;
  a->ExplicitInit = explicitinit;
  return a;
}

struct tArrayInitialisationEntry *
MakeArrayInitialisationEntry (struct tRange *ranges, struct tRHS *rhs)
{
  struct tArrayInitialisationEntry *a =
    checked_malloc (sizeof *a);

  a->Ranges = ranges;
  a->RHS = rhs;
  a->Next = NULL;
  return a;
}

struct tArrayInitialisationEntry *
PrefixArrayInitialisationEntry (struct tArrayInitialisationEntry *head,
                                struct tArrayInitialisationEntry *tail)
{
  head->Next = tail;
  return head;
}

struct tFieldSpec *
MakeFieldSpec (char *name, struct tTypeRefOrSpec *type,
               struct tRepresentationClause *representation, char *code, char *comment)
{
  struct tFieldSpec *f = checked_malloc (sizeof *f);

  f->Name = name;
  f->Type = type;
  f->Typespec = NULL;
  f->Code = code;
  f->Next = NULL;
  f->Representation = representation;
  f->Comment = comment;
  return f;
}

struct tFieldSpec *
PrefixFieldSpec (struct tFieldSpec *head, struct tFieldSpec *tail)
{
  head->Next = tail;
  return head;
}

struct tTypeSpec *
MakeRecordType (int line,
                struct tFieldSpec *fieldlist, enum tRecordType recordtype)
{
  struct tTypeSpec *t;

  t = NewTypespec (line);
  t->Line = line;
  t->BaseType = RECORD_TYPE;
  t->FieldList = fieldlist;
  t->StructureAccess = PRIVATE_ACCESS;
  t->RecordType = recordtype;
  // NOTE: we use record ... end record rather than record ... end typename so that
  // somevar : record ... end record makes sense and works
  return t;
}

struct tFieldInitialisation *
MakeFieldInitialisation (char *name, struct tRHS *rhs, bool makenew)
{
  struct tFieldInitialisation *f =
    checked_malloc (sizeof *f);

  f->Name = name;
  f->RHS = rhs;
  f->MakeNew = makenew;
  f->Next = NULL;
  return f;
}

struct tFieldInitialisation *
PrefixFieldInitialisation (struct tFieldInitialisation *head,
                           struct tFieldInitialisation *tail)
{
  head->Next = tail;
  return head;
}

struct tTypeSpec *
MakeAccessType (int line, struct tTypeRefOrSpec *type, bool canbenull,
                bool unchecked, bool accessconstant, bool accesspersistent, bool accessnew)
{
  struct tTypeSpec *t;

  t = NewTypespec (line);
  t->Line = line;
  t->BaseType = ACCESS_TYPE;
  t->AccessedElementType = type;
  t->CanBeNull = canbenull;
  t->UnmanagedAccess = unchecked;
  t->ConstantAccess = accessconstant;
  t->PersistentAccess = accesspersistent;
  t->NewAccess = accessnew;
  return t;
}

struct tElt *
SetVarName (struct tElt *varelt, char *name)
{
  struct tVariableDeclaration *v;

  v = varelt->Ptr;
  v->VarNames = MakeIdentifierListEntry(name);
  return varelt;
}

void CheckNameMatch(char *filename, int line, char *name, char *confirmname)
{
  if (strcmp (name, confirmname))
    {
      err (filename, line, "Names '%s' and '%s' do not match", name,
           confirmname);
    }
}

struct tProcedure *
MakeProcedure (char *filename, int line,
               char *name,
               struct tElt *formals,
               struct tElt *returntype,
               struct tElt *declarations,
               struct tElt *statements,
               char *confirmname,
               struct tShareClause *shareclause,
               struct tRepresentationClause *representation,
               struct tElt *unittest,
               int lastlineofdeclaration)
{
  struct tProcedure *p = checked_malloc (sizeof *p);

  CheckNameMatch(filename, line, name, confirmname);
  p->Name = name;
  p->Formals = formals;
  p->ReturnType = returntype;
  p->Declarations = declarations;
  p->Statements = statements;
  p->ShareClause = shareclause;
  p->Representation = representation;
  p->UnitTest = unittest;
  p->LastLineOfDeclaration = lastlineofdeclaration;
  return p;
}

struct tShareClause *
MakeShareClause (struct tNode *expr)
{
  struct tShareClause *s = checked_malloc (sizeof *s);

  s->Expr = expr;
  return s;
}

struct tElt *
SetPrefix (struct tElt *elt, enum tPrefix prefix, int adjustline)
{
  elt->Prefix = prefix;
  elt->Line = adjustline;
  return elt;
}

struct tPackage *
MakePackage (char *filename, int line,
             enum tPackageType packagetype,
             char *name,
             struct tElt *declarations,
             struct tElt *initialisation,
             struct tElt *finalisation,
             char *confirmname,
             struct tRepresentationClause *representation,
             int headerstart, int headerend, int trailerstart, int trailerend)
{
  struct tPackage *p = checked_malloc (sizeof *p);

  CheckNameMatch(filename, line, name, confirmname);
  p->PackageType = packagetype;
  p->Name = name;
  p->Access = PRIVATE_ACCESS;
  p->PathName = NULL;
  p->Declarations = declarations;
  p->Initialisation = initialisation;
  p->Finalisation = finalisation;
  p->Start = NULL;
  p->StackSize = 0;
  p->Representation = representation;
  p->Shared = FALSE;
  p->UnitTest = FALSE;
  p->NotUnitTest = FALSE;
  p->GenericFrom = NULL;
  p->GenericActuals = NULL;
  p->LockVar = NULL;
  p->EnclosingPackage = NULL;
  p->Dispatches = FALSE;
  p->HeaderStart = headerstart;
  p->HeaderEnd = headerend;
  p->TrailerStart = trailerstart;
  p->TrailerEnd = trailerend;
  return p;
}

struct tDeclareBlock *
MakeDeclareBlock (struct tElt *declarations, struct tElt *statements,
                  struct tRepresentationClause *representation, bool unittest,
                  bool notunittest)
{
  struct tDeclareBlock *d = checked_malloc (sizeof *d);

  d->Declarations = declarations;
  d->Statements = statements;
  d->Representation = representation;
  d->UnitTest = unittest;
  d->NotUnitTest = notunittest;
  return d;
}

struct tSeparateBlock *
MakeSeparateBlock (char *filename, int line, char *name,
                   struct tElt *statements, char *confirmname)
{
  struct tSeparateBlock *s = checked_malloc(sizeof *s);

  CheckNameMatch(filename, line, name, confirmname);
  s->Name = name;
  s->Statements = statements;
  return s;
}

struct tSeparateStmt *
MakeSeparateStmt (struct tReference *name)
{
  struct tSeparateStmt *s = checked_malloc(sizeof *s);

  s->Name = name;
  return s;
}

struct tElt *
SetAccess (struct tElt *elt, enum tAccess access)
{
  elt->Access = access;
  return elt;
}

struct tActualParameter *
MakeActualParameter (char *name, struct tNode *expr, enum tMode mode)
{
  struct tActualParameter *a =
    checked_malloc (sizeof *a);

  a->Name = name;
  a->Expr = expr;
  a->AuxType = NULL;
  a->ActualType = NULL;
  a->ActualTypeSpec = NULL;
  a->IsThis = FALSE;
  a->Mode = mode;
  a->Next = NULL;
  a->ExemptionAliasing = FALSE;
  a->ActualVar = NULL;
  a->ResetRange = FALSE;
  return a;
}

struct tActualParameter *
AddRepresentationToActualParameter (char *filename, int line,
                                    struct tActualParameter *actual,
                                    struct tRepresentationClause
                                    *representation)
{
  struct tRepresentationClause *rep;

  rep = representation;
  while (rep != NULL)
    {
      if (!strcasecmp (rep->Name, "exemption"))
        {
          if (rep->Value->Op == _VAR)
            {
              if (!strcasecmp (rep->Value->Var->VarName, "unchecked_alias"))
                {
                  actual->ExemptionAliasing = TRUE;
                }
              else if (!strcasecmp (rep->Value->Var->VarName, "unchecked_range"))
                {
                  actual->ExemptionRange = TRUE;
                }
              else
                {
                  err (filename, line, "Unknown exemption: %s",
                       rep->Value->Var->VarName);
                }
            }
          else
            {
              err (filename, line, "Syntax error in 'exemption' clause");
            }
        }
      else
        {
          err (filename, line, "Unknown attribute: %s", rep->Name);
        }
      rep = rep->Next;
    }
  return actual;
}

struct tActualParameter *
PrefixActualParameter (struct tActualParameter *head,
                       struct tActualParameter *tail)
{
  head->Next = tail;
  return head;
}

struct tProcedureCall *
MakeProcedureCall (struct tReference *name,
                   struct tActualParameter *actuallist)
{
  struct tProcedureCall *a = checked_malloc (sizeof *a);

  a->Name = name;
  a->ActualList = actuallist;
  // For now
  a->NumFormals = 0;
  a->PositionNum = NULL;
  a->Formals = NULL;
  return a;
}

struct tElt *
MakePragma (struct tElt *proccallelt)
{
  proccallelt->Type = PRAGMA_ELT;
  return proccallelt;
}

struct tGenericParameter *
MakeGenericParameter (char *filename, int line,
                      char *name,
                      enum tGenericParameterType type,
                      char *identifiertype,
                      struct tReference *identifier,
                      struct tElt *formals, struct tElt *returntype,
                      struct tGenericActualParameter *genericactualparams)
{
  struct tGenericParameter *p =
    checked_malloc (sizeof *p);
  enum tGenericParameterType gtype;

  p->Name = name;
  if (type == GENERIC_CONSTANT)
    {
      if (identifiertype == NULL)
        gtype = type;
      else if (!strcmp (identifiertype, "boolean"))
        gtype = GENERIC_BOOLEAN_CONSTANT;
      else if (!strcmp (identifiertype, "cstring"))
        gtype = GENERIC_CSTRING_CONSTANT;
      else
        {
          err (filename, line,
               "Syntax error in generic formal constant declaration");
          gtype = type;         // avoid warning
        }
    }
  else
    {
      gtype = type;
    }
  p->Type = gtype;
  p->Identifier = identifier;
  p->Formals = formals;
  p->ReturnType = returntype;
  p->GenericActuals = genericactualparams;
  p->Next = NULL;
  return p;
}

struct tGenericParameter *
PrefixGenericParameter (struct tGenericParameter *head,
                        struct tGenericParameter *tail)
{
  head->Next = tail;
  return head;
}

struct tGenericActualParameter *
MakeGenericActualParameter (char *name, struct tNode *expression)
{
  struct tGenericActualParameter *p =
    checked_malloc (sizeof *p);

  p->Name = name;
  p->Expression = expression;
  p->FoundType = NULL;
  p->Next = NULL;
  return p;
}

struct tGenericActualParameter *
PrefixGenericActualParameter (struct tGenericActualParameter *head,
                              struct tGenericActualParameter *tail)
{
  head->Next = tail;
  return head;
}

struct tGeneric *
MakeGeneric (struct tGenericParameter *params, struct tElt *body)
{
  struct tGeneric *g = checked_malloc (sizeof *g);
  bool match;

  g->Parameters = params;
  g->Body = body;
  match = FALSE;
  if (body->Type == PACKAGE_DECLARATION_ELT)
    {
      enum tPackageType ptype = ((struct tPackage *) body->Ptr)->PackageType;
      if (ptype == PACKAGE_PACKAGE || ptype == PACKAGE_SUBSYSTEM)
        {
          match = TRUE;
        }
    }
  if (!match)
    {
      err (body->FileName, body->Line,
           "Ordinary package or subsystem required for generic body");
    }
  return g;
}

struct tGenericInstantiation *
MakeGenericInstantiation (enum tPackageType packagetype, char *name,
                          struct tReference *generic,
                          struct tGenericActualParameter *actuallist,
                          int firstline, int lastline)
{
  struct tGenericInstantiation *g =
    checked_malloc (sizeof *g);

  g->PackageType = packagetype;
  g->Name = name;
  g->Generic = generic;
  g->Actuals = actuallist;
  g->FirstLine = firstline;
  g->LastLine = lastline;
  return g;
}

struct tConstant *
MakeConstant (struct tIdentifierListEntry *names, struct tNode *expression, struct tUnitTerm *unit,
              struct tRepresentationClause *representation)
{
  struct tConstant *c = checked_malloc (sizeof *c);

  c->Names = names;
  c->Expression = expression;
  c->Unit = unit;
  c->Representation = representation;
  return c;
}

struct tSeparate *
MakeSeparate (enum tPackageType packagetype, bool generic, char *name, char *filename)
{
  struct tSeparate *s = checked_malloc (sizeof *s);

  s->PackageType = packagetype;
  s->Generic = generic;
  s->Name = name;
  s->FileName = filename;
  return s;
}

struct tTypeRefOrSpec *
MakeTypeRefOrSpec (struct tTypeIdentifier *typeidentifier,
                   struct tTypeSpec *typespec)
{
  struct tTypeRefOrSpec *t = checked_malloc (sizeof *t);

  t->TypeIdentifier = typeidentifier;
  t->TypeSpec = typespec;
  return t;
}

struct tIdentifierListEntry *MakeIdentifierListEntry(char *str)
{
  struct tIdentifierListEntry *p = checked_malloc(sizeof *p);

  p->SymbolName = str;
  p->Next = NULL;
  return p;
}

struct tIdentifierListEntry *PrefixIdentifierListEntry(struct tIdentifierListEntry *head, struct tIdentifierListEntry *tail)
{
  head->Next = tail;
  return head;
}

struct tFileCommentBlock *AppendCommentLine(char *comment, struct tFileCommentBlock *restofblock)
{
  struct tFileCommentBlock *p = malloc(sizeof(*p));

  p->Comment= comment;
  p->Next = restofblock;
  return p;
}

