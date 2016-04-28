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

/* Types and routines used by the generated parser of Rihtan */

#ifndef YTYPES_H
#define YTYPES_H

#include <stdint.h>
#include <inttypes.h>

extern int yyparse (void);
extern int yylex (void);
extern FILE *yyin;
extern int yylineno;
void yyerror (const char *str);

typedef int64_t BIGINT;
#define MAX_BIGINT INT64_MAX
#define MIN_BIGINT INT64_MIN
#define PRINTBIG PRId64
#define PRINTBIGx PRIx64
#define SCANBIGd SCNd64
#define SCANBIGx SCNx64
typedef double BIGFLOAT;

typedef int bool;
#define FALSE 0
#define TRUE 1

/* ----- Parse tree structures ----- */

struct tUnitTerm
{
  struct tReference *Ref;
  BIGINT Power;
  bool GetUnit;
  struct tUnitTerm *Next;
};

enum tAccess
{ PRIVATE_ACCESS, PUBLIC_ACCESS, HIDDEN_ACCESS };
/* Parameter modes */
enum tMode
{ MODE_IN, MODE_OUT, MODE_IN_OUT, MODE_FINAL_IN_OUT, MODE_LOCAL, MODE_GLOBAL,
  MODE_SHARED, MODE_UNSPECIFIED
};

/*
   Vars, types and typespecs
   -------------------------

   For type definitions: Var->Type(a)->Typespec(TYPE_TYPE)->Type(b)->Typespec(TYPE_INTEGER, TYPE_ARRAY etc.)
   For variables: Var->Type(b)->Typespec(TYPE_INTEGER, TYPE_ARRAY etc.)
   For arrays: Typespec(TYPE_ARRAY).IndexType->Type(a)
   For arrays and access types: Typespec(TYPE_ARRAY or TYPE_ACCESS).ElementType->Type(a)
   For all types: Typespec(TYPE_INTEGER, TYPE_ARRAY etc.).ThisType->Type(a)
*/


// Generated during analysis

struct tTypeSpecChain
{
  struct tTypeSpec *TypeSpec;
  struct tTypeSpecChain *Next;
};

struct tType
{
  char *Name;
  struct tTypeSpec *TypeSpec;
  enum tAccess NameAccess;
  struct tType *Next;
  struct tTypeSpecChain *DeferredTypespecs;
};

// A unit (generated during analysis)
struct tUnitDef
{
  char *Name;
  BIGINT Power;
  struct tUnitDef *Next;
};

enum tNameFor
{ NAME_NONE, NAME_PACKAGE, NAME_SUBSYSTEM, NAME_PROCEDURE, NAME_FUNCTION,
  NAME_TYPE, NAME_UNIT, NAME_CONSTANT, NAME_VAR
};

enum tUsage
{ REQUIRED, LIBRARY, TARGET_LIBRARY };

struct tRangeValues
{
};

// Generated during analysis
struct tVar
{
  char *FileName;
  int Line;
  char *Name;
  struct tType *Type;
  struct tUnitDef *Unit;        // for unit declarations
  //
  // Range and initialisation values
  bool Initialised;             // initialised yet?
  bool Finalised;               // finalised?
  bool Accessed;                // Possibly read since the last time it was updated?
  bool EverAccessed;            // Read at any point?
  BIGINT Low, High;             // ranges for integer types
  BIGINT InitialLow, InitialHigh;       // initial ranges for analysis conditions and pragmas
  //
  bool InitialisedGlobally;     // initialised in package initialisation?
  bool EverInitialised;         // initialised at any point? (used to detect unused globals)
  BIGINT FirstLow, FirstHigh, LastLow, LastHigh;        // ranges for array 'first and 'last
  BIGFLOAT FloatVal;            // for named floats
  bool MinDefinite, MaxDefinite;        // for array parameters
  struct tRHS *ConstValue;      // or NULL
  enum tAccess Access;          // public, private
  enum tMode Mode;              // in, out, in out, local, global
  char *CName;                  // output name
  char *TargetAttribute;        // Attribute prefix
  char *TargetAttributeLine;    // Attribute line prefix
  int Lock;                     // lock against updates that could change the tag (for a union); a count because it can be nested
  struct tVar *Referent;        // In the case of a name for declaration, the base var that is aliased
  bool WholeObjectReference;    // True if a name for declaration names an entire object (rather than a field, component etc.)
  struct tUnitDef *ConstUnit;   // only for named constant declarations
  struct tReference *ConstAccess;       //
  struct tTypeSpec *ConstAccessType;
  bool AddressSpecified;        // address => ... given
  bool AccessVolatile;          // using access => volatile
  bool AccessMappedDevice;      // using access => mapped_device (also sets volatile)
  bool AccessSharedAtomic;      // using access => sared_atomic (also sets volatile)
  bool TestMappedDevice;        // mapped_device under unit test builds
  bool AccessImage;             // using access => image
  bool AccessVirtual;           // using access => virtual
  bool ExemptionAliasing;       // using exemption => alias
  struct tVar *Next;            // for export from packages
  struct tVar *PrivatePublicNext;       // chain of all private or public global variables
  struct tGlobalAccess *ShareList;
  struct tPackage *EnclosingPackage;
  enum tUsage Usage;            // using (usage => ...)
  bool EverUsed;                // for globals, including routines
  struct tVar *Actual;          // for generic formal parameters, refers to the actual parameter
  bool PublicOut;               // 'public out'
  bool ShareLock;               // procedure using (lock => var)
  bool Hidden;                  // pragma hide
  bool RequirePackageInitialisation;  // Require initialisation in a package initialisation section
  bool SubsystemBoundary;       // Did we traverse a subsystem bondary to reach this (most recently)?
  struct tElt *Statements;      // For separate blocks
};

struct tFormalParameter
{
  char *Name;
  struct tTypeSpec *TypeSpec;
  enum tMode Mode;
  struct tVar *Var;
  struct tFormalParameter *Next;
};

struct tGlobalAccess
{
  int Line;
  struct tVar *Var;
  bool Read;
  bool Write;
  bool RequireInitialisation;
  bool Initialised;
  bool LocalAccess;
  bool InheritedAccess;
  bool Lock;
  struct tGlobalAccess *Next;
};

struct tShareLockElt {
  struct tVar *Var;
  struct tShareLockElt *Next;
};

enum tBaseType
{ INTEGER_TYPE, FLOATING_TYPE, BOOLEAN_TYPE, ARRAY_TYPE,
  RECORD_TYPE, PROCEDURE_TYPE, PACKAGE_TYPE, TYPE_TYPE, CSTRING_TYPE,
  ACCESS_TYPE,
  UNIT_TYPE, ADDRESS_TYPE,
  SEPARATE_BLOCK_TYPE
};
#define MAX_BASE_TYPE ADDRESS_TYPE

enum tRecordType
{ REC_RECORD, REC_UNION, REC_UNCHECKED_UNION };

enum tPrefix
{ PREFIX_NONE, PREFIX_CLOSED, PREFIX_SHARED, PREFIX_MAIN,
  PREFIX_UNIT_TEST, PREFIX_NOT_UNIT_TEST
};

/* An anonymous type specification */
struct tTypeSpec
{
  int Line;
  char *Name;                   // Name for error messages
  enum tBaseType BaseType;
  BIGINT Low, High;             // for integer types, array indexes and access types (0 null, 1 non-null)
  BIGINT Digits, Magnitude;     // for floating point types
  struct tUnitDef *Unit;        // unit after analysis
  struct tUnitTerm *ReqUnit;    // unit as requested
  struct tArrayIndex *IndexSpec;        // for array type definitions
  struct tTypeIdentifier *IndexType;    //, *ElementType;   // for array types; element type also for access types
  struct tTypeRefOrSpec *ArrayElementType;      // For array types
  struct tTypeRefOrSpec *AccessedElementType;   // For access types
  struct tTypeSpec *IndexTypeSpec, *ElementTypeSpec;
  char *ElementName;            // For access types - used with forward declarations
  bool MinDefinite, MaxDefinite;
  struct tEnum *EnumList;       // for enum types
  struct tFieldSpec *FieldList; // for record types
  enum tRecordType RecordType;  // for record types
  struct tFormalParameter *Formals;     // for procedures
  struct tTypeSpec *ReturnType; // for functions, NULL for procedures
  bool NamedParametersRequired; // for procedures and functions
  struct tPackage *PackageSpec; // for packages
  struct tVar *PackageVars;     // for packages
  bool PackageClosed;           // for packages (has the end of the package been reached?)
  struct tGenericParameter *GenericParameters;  // for generics
  struct tVars *Scope;          // for generics: the environment in which the generic was defined
  struct tType *Type;           // for types
  enum tAccess StructureAccess;
  char *CName;
  char *TargetAttribute;
  char *TargetAttributeLine;
  char *TargetCast;             // Cast to C type for parameters (else NULL)
  struct tTargetType *CType;
  bool Controlled;
  bool PrefixShared;            // for procedures
  bool PrefixClosed;            // for procedures
  struct tShareClause *ShareClause;     // for procedures
  struct tNode *LowExpr, *HighExpr;     // for integer types before elaboration
  struct tRepresentationClause *Representation;
  bool StorageSet;              // 'storage' representation set
  bool CanBeNull;               // for access types
  bool AccessRoutinesCreated;   // for accessed types
  struct tVar *AccessFinalisation;      // for controlled accessed types
  char *AllocateFunction, *FreeFunction;        // for access types
  bool UnmanagedAccess;         // for access types
  bool ConstantAccess;          // access constant
  bool PersistentAccess;        // access persistent (must also set UnmanagedAccess)
  bool NewAccess;               // unchecked new access (with UnmanagedAccess)
  struct tGlobalAccess *GlobalsAccess;  // for procedures and functions
  BIGINT StackOverhead;         // for procedures and functions
  bool External;                // external reference
  struct tNode *Precondition, *Postcondition;   // for procedures and functions
  struct tCallEntry *CallList;  // for procedures and functions
  struct tType *ThisType;       // The result of adding the type, for use in 'index_type etc.
  unsigned FirstBranch, LastBranch;     // For coverage analysis of procedures
  bool Incomplete;              // An incomplete routine (can only used in analysis)
  bool Dispatches;              // Does a routine, or anything it calls, perform a dispatch?
  struct tShareLockElt *ShareLockList;    // For procedures, using (lock => var)
  bool NoLocks;                 // For procedures, using (no_locks)
  bool Restarts;                // For procedures - can it perform a restart (directly or indirectly)
};

// Call list entry
struct tCallEntry
{
  struct tVar *Routine;         // Routine called
  struct tCallEntry *Next;
};

struct tRepresentationClause
{
  char *Name;
  struct tNode *Value;
  struct tRepresentationClause *Next;
};

/* A named type declaration */
struct tTypeDeclaration
{
  char *Name;
  struct tTypeSpec *TypeSpec;
};

struct tIdentifierListEntry {
  char *SymbolName;
  struct tIdentifierListEntry *Next;
};

struct tVarListEntry {
  struct tVar *Var;
  struct tVarListEntry *Next;
};

struct tVariableDeclaration
{
  struct tIdentifierListEntry *VarNames;
  // Either the type name or the type spec is used (the other is NULL)
  struct tTypeIdentifier *TypeName;
  struct tTypeSpec *TypeSpec;
  struct tRHS *ConstValue;
  enum tMode Mode;
  struct tVarListEntry *Vars;
  struct tReference *Names;
  struct tRepresentationClause *Representation;
  enum tNameFor NameFor;
  bool PublicOut;               // 'public out'
};

struct tUnitDeclaration
{
  int Line;
  char *UnitName;
  struct tUnitTerm *unit;
};

/* Node in an expression */
enum tOp
{ _PLUS = 0, _MINUS, _TIMES, _DIVIDE,
  _MOD, _IAND, _IOR, _IXOR,
  _LT, _GT, _LEQ, _GEQ,
  _EQ, _NEQ, _NOT, _AND,
  _OR, _CONST_INT, _CONST_BOOL, _CONST_FLOAT,
  _CONST_STRING, _CONST_CHARACTER, _CONST_NULL, _VAR,
  _FN_CALL
};

// Attributes
// If this is changed, update AttributeNames (parser.c) accordingly
enum tAttribute
{ ATTR_FIRST =
    0, ATTR_LAST, ATTR_LENGTH, ATTR_UNIT, ATTR_INDEX_UNIT, ATTR_MINIMUM,
  ATTR_MAXIMUM,
  ATTR_ACCESS, ATTR_PERSISTENT_ACCESS, ATTR_SIZE, ATTR_INITIAL, ATTR_TYPE, ATTR_INDEX_TYPE,
  ATTR_ELEMENT_TYPE, ATTR_ACCESSED_TYPE,
  ATTR_RETURN_TYPE, ATTR_PARAMETER_TYPE, ATTR_FIELD_TYPE, ATTR_NULL
};                              // ATTR_NULL must be the last one - see AttributeNames
extern const char *AttributeNames[ATTR_NULL];

extern char *OpString[_OR + 1];

struct tSingleAttribute
{
  char *AttrName;
  char *AuxName;
};

struct tAttributeChainEntry
{
  enum tAttribute Name;
  char *AuxName;
  struct tAttributeChainEntry *Next;
};

struct tNode
{
  char *FileName;
  int Line;
  enum tOp Op;
  struct tNode *Left, *Right;
  BIGINT Value;
  BIGFLOAT FloatValue;
  char *String;
  struct tReference *Var;
  struct tElt *Call;
  struct tTargetType *ComputationType;
  struct tAttributeChainEntry *Attribute;
  bool EnclosedInParen;
  BIGINT Min, Max;              // Filled in during expression analysis
  struct tVar *IntVar;          // ... simple integer variable or NULL
};

struct tAssignment
{
  struct tReference *Var;
  struct tRHS *RHS;
  bool MakeNew;
};

enum tRHSType
{ EXPR_RHS, ARRAY_RHS, RECORD_RHS };
struct tRHS
{
  enum tRHSType RHSType;
  struct tNode *Expr;
  struct tArrayInitialisation *ArrayExpr;
  struct tFieldInitialisation *RecordExpr;
};

struct tBooleanAssignment
{
  char *VarName;
  struct tNode *Expr;
};

struct tWhen
{
  struct tNode *Cond;
  struct tElt *When_stmt, *Else_stmt;
  bool ElseCanBeCommentsOnly;
  // ElseCanBeCommentsOnly is TRUE in the following circumstance:
  //
  // loop
  //   ...
  //   exit when cond;
  //   (possible comments)
  // end loop
  //
};

struct tLoop
{
  struct tElt *Body;
};

struct tWhile
{
  struct tNode *Cond;
  struct tElt *Body;
};

struct tDo
{
  struct tElt *Body;
};

struct tRange
{
  struct tNode *MinValue, *MaxValue;
  bool Used;
  struct tRange *Next;
};

struct tCaseOption
{
  struct tRange *Ranges;
  struct tElt *Stmts;
  struct tCaseOption *Next;
};

struct tCase
{
  struct tNode *CaseExpr;
  struct tCaseOption *Options;
};

struct tFor
{
  struct tElt *ControlVar;
  struct tNode *RangeLow;
  struct tNode *RangeHigh;
  struct tElt *Body;
};

enum tEltType
{ TYPE_DECLARATION_ELT, VAR_DECLARATION_ELT, UNIT_DECLARATION_ELT,
  ASSIGNMENT_ELT,
  WHEN_ELT, LOOP_ELT, WHILE_ELT, DO_ELT, EXIT_ELT, REPEAT_ELT, NULL_ELT,
  CASE_ELT, FOR_ELT,
  PROCEDURE_DECLARATION_ELT, PACKAGE_DECLARATION_ELT,
  PROCEDURE_CALL_ELT, CODE_ELT, HEADER_CODE_ELT, PRAGMA_ELT,
  GENERIC_DECLARATION_ELT,
  GENERIC_INSTANTIATION_ELT, CONSTANT_DECLARATION_ELT, DECLARE_BLOCK_ELT,
  SEPARATE_DECLARATION_ELT, APPEND_REPRESENTATION_ELT, FREE_ELT, RESTART_ELT,
  SEPARATE_BLOCK_ELT, SEPARATE_STMT_ELT, ASSERT_ELT, ADVISE_ELT,
  COMMENT_ELT
};

struct tElt
{
  char *FileName;
  int Line;
  int LastLine;
  enum tEltType Type;
  void *Ptr;
  enum tAccess Access;          // For declarations
  enum tPrefix Prefix;
  struct tElt *Next;
};

struct tEnum
{
  char *Name;
  struct tRepresentationClause *Representation;
  struct tEnum *Next;
  struct tVar *ConstVar;
};

/* Variable reference term */
enum tReferenceType
{ IDENTIFIER_REFERENCE, ARRAY_REFERENCE, ARRAY_SLICE_REFERENCE,
  ARRAY_SLICE_LENGTH_REFERENCE, FIELD_REFERENCE
};
struct tReference
{
  enum tReferenceType ReferenceType;
  char *VarName;                // for identifier or field reference
  struct tNode *Index, *Index2; // for array reference (index2 is for slices)
  char *ReferenceAbbreviation;  // for array initialisations
  struct tReference *ReferenceAbbreviationEnd;
  struct tReference *Next;
  bool BitField;                // Initially FALSE, filled in during analysis to help with output generation
};

enum tIndexSpecType
{ INDEX_NAME, INDEX_SPEC };

struct tArrayIndex
{
  enum tIndexSpecType IndexType;
  struct tTypeIdentifier *IndexName;
  struct tTypeSpec *IndexTypeSpec;
};

struct tArrayIndex *MakeArrayIndex (enum tIndexSpecType indextype,
                                    struct tTypeIdentifier *indexname,
                                    struct tTypeSpec *indexspec);

/* Array initialisation expression */
struct tArrayInitialisation
{
  char *VarName;
  struct tRHS *RHS;
  struct tArrayInitialisationEntry *ExplicitInit;
};

// Array initialisation entry for explicit option
struct tArrayInitialisationEntry
{
  struct tRange *Ranges;
  struct tRHS *RHS;
  struct tArrayInitialisationEntry *Next;
};

/* Whole record initialisation */
struct tFieldInitialisation
{
  char *Name;
  struct tRHS *RHS;
  bool MakeNew;
  struct tFieldInitialisation *Next;
};

struct tFieldSpec
{
  char *Name;
  struct tTypeRefOrSpec *Type;
  struct tTypeSpec *Typespec;   // Might be needed when the type name is out of scope
  struct tFieldSpec *Next;
  char *Code;                   // Literal code if not NULL
  struct tRepresentationClause *Representation;
  // Generated representation information
  bool FieldSet;
  bool BitFieldSet;
  unsigned FieldOffset;
  struct tTargetType *FieldCType;
  unsigned BitOffset, NumBits;
  char *BitName;
  bool UseShortBitFieldForm;
  char *Comment;                // A full line comment (will be removed after possibly being output)
};

/* Procedures */

struct tProcedure
{
  char *Name;
  struct tElt *Formals;
  struct tElt *ReturnType;      // NULL for a procedure, non-NULL for a function
  struct tElt *Declarations;
  struct tElt *Statements;
  struct tShareClause *ShareClause;
  struct tRepresentationClause *Representation;
  struct tElt *UnitTest;
  int LastLineOfDeclaration;
};

struct tShareClause
{
  struct tNode *Expr;
};

/* Packages */

enum tPackageType
{ PACKAGE_PACKAGE, PACKAGE_SUBSYSTEM, PACKAGE_SYSTEM };

struct tPackage
{
  char *Name;
  enum tAccess Access;
  char *PathName;
  enum tPackageType PackageType;
  struct tElt *Declarations;
  struct tElt *Initialisation;
  struct tElt *Finalisation;
  struct tVar *Start;
  BIGINT StackSize;
  struct tRepresentationClause *Representation;
  bool Shared;
  bool UnitTest;
  bool NotUnitTest;
  // Instantiation details for packages that are derived from generics
  struct tTypeSpec *GenericFrom;
  struct tGenericActualParameter *GenericActuals;
  struct tVar *LockVar;
  struct tPackage *EnclosingPackage;
  bool Dispatches;              // Does any routine in the package directly or indirectly dispatch?
  int HeaderStart, HeaderEnd, TrailerStart, TrailerEnd;
};

/* Actual parameters */
struct tActualParameter
{
  char *Name;                   // NULL for unnamed
  struct tNode *Expr;
  struct tActualParameter *Next;
  struct tTargetType *AuxType, *ActualType;
  struct tTypeSpec *ActualTypeSpec;
  enum tMode Mode;
  bool IsThis;
  bool ExemptionAliasing;
  bool ExemptionRange;
  struct tVar *ActualVar;
  bool ResetRange;
};

/* Declare blocks */
struct tDeclareBlock
{
  struct tElt *Declarations;
  struct tElt *Statements;
  struct tRepresentationClause *Representation;
  bool UnitTest;
  bool NotUnitTest;
};

/* Separate blocks */
struct tSeparateBlock
{
  char *Name;
  struct tElt *Statements;
};

/* Separate statements */
struct tSeparateStmt
{
  struct tReference *Name;
};

/* Procedure calls */
struct tProcedureCall
{
  struct tReference *Name;
  struct tActualParameter *ActualList;
  // The following fields are filled in during analysis to assist in code generation
  int NumFormals;
  int *PositionNum;
  struct tFormalParameter *Formals;
};

enum tGenericParameterType
{ GENERIC_TYPE, GENERIC_RANGE_TYPE, GENERIC_DIGITS_TYPE,
  GENERIC_ARRAY_TYPE, GENERIC_ACCESS_TYPE,
  GENERIC_CONSTANT, GENERIC_RANGE_CONSTANT, GENERIC_DIGITS_CONSTANT,
  GENERIC_ACCESS_CONSTANT,
  GENERIC_BOOLEAN_CONSTANT, GENERIC_CSTRING_CONSTANT,
  GENERIC_PROCEDURE, GENERIC_FUNCTION,
  GENERIC_GENERIC_PACKAGE
};
struct tGenericParameter
{
  char *Name;
  enum tGenericParameterType Type;
  struct tReference *Identifier;
  struct tElt *Formals;
  struct tElt *ReturnType;
  struct tGenericActualParameter *GenericActuals;       // This is used for formals 'with p is new g ( x => y, ... )'
  struct tGenericParameter *Next;
};

struct tGeneric
{
  struct tGenericParameter *Parameters;
  struct tElt *Body;
};

struct tGenericActualParameter
{
  char *Name;
  struct tNode *Expression;
  struct tTypeSpec *FoundType;  // Filled in during instantiation
  struct tGenericActualParameter *Next;
};

struct tGenericInstantiation
{
  enum tPackageType PackageType;
  char *Name;
  struct tReference *Generic;
  struct tGenericActualParameter *Actuals;
  int FirstLine, LastLine;
};

struct tConstant
{
  struct tIdentifierListEntry *Names;
  struct tNode *Expression;
  struct tUnitTerm *Unit;
  struct tRepresentationClause *Representation;
};

struct tSeparate
{
  enum tPackageType PackageType;
  bool Generic;
  char *Name;
  char *FileName;
};

struct tRepresentationSpec
{
  char *TypeName;
  struct tRepresentationClause *Representation;
};

struct tTypeIdentifier
{
  struct tReference *Name;
  struct tAttributeChainEntry *Attribute;
};

struct tFileCommentBlock
{
  char *Comment;
  struct tFileCommentBlock *Next;
};

BIGINT StaticIntegerValue (struct tNode *expr);

struct tAttributeChainEntry *MakeAttibuteChainEntry (char *filename, int line,
                                                     char *name);

struct tAttributeChainEntry *PrefixAttibuteChainEntry (char *filename,
                                                       int line, char *name,
                                                       struct
                                                       tAttributeChainEntry
                                                       *chain);

struct tNode *NewNode (char *filename, int line,
                       enum tOp operator, struct tNode *lhs,
                       struct tNode *rhs, BIGINT literal,
                       BIGFLOAT floatliteral, struct tReference *var,
                       struct tElt *call,
                       struct tAttributeChainEntry *attribute, char *string);

struct tNode *SetEnclosedInParen (struct tNode *node);

struct tTypeSpec *MakeIntegerType (int line,
                                   struct tNode *lowrange,
                                   struct tNode *highrange,
                                   struct tUnitTerm *unit);

struct tTypeSpec *MakeFloatingType (int line,
                                    BIGINT digits, BIGINT magnitude,
                                    struct tUnitTerm *unit);

struct tTypeSpec *MakeArrayType (int line,
                                 struct tArrayIndex *indexspec,
                                 struct tTypeRefOrSpec *elementtype,
                                 bool mindefinite, bool maxdefinite);

struct tTypeSpec *SetTypeSpecAccess (struct tTypeSpec *typespec,
                                     enum tAccess access);

struct tTypeSpec *SetControlled (struct tTypeSpec *typespec);

struct tTypeSpec *SetRepresentation (struct tTypeSpec *typespec,
                                     struct tRepresentationClause
                                     *representation);

struct tRepresentationClause *MakeRepresentationClause (char *name,
                                                        struct tNode *expr);

struct tRepresentationClause *PrefixRepresentationClause (struct
                                                          tRepresentationClause
                                                          *head,
                                                          struct
                                                          tRepresentationClause
                                                          *tail);

struct tTypeDeclaration *AddType (char *name, struct tTypeSpec *typespec);

struct tRepresentationSpec *AddRepresentation (char *typename,
                                               struct tRepresentationClause
                                               *representation);

struct tTypeIdentifier *MakeTypeIdentifier (struct tReference *name,
                                            struct tAttributeChainEntry
                                            *attribute);

struct tVariableDeclaration *DeclareVariable (
                                               //char *filename, int line,
                                               struct tIdentifierListEntry *varnames, struct tTypeIdentifier *typename, struct tTypeSpec *typespec,     // either typename or typespec
                                               struct tRHS *constvalue,
                                               struct tReference *names,
                                               enum tMode mode,
                                               struct tRepresentationClause
                                               *representation,
                                               enum tNameFor namefor);

struct tUnitDeclaration *DeclareUnit (int line, char *unitname,
                                      struct tUnitTerm *unit);

struct tElt *SetMode (struct tElt *elt, enum tMode mode);

struct tElt *SetPublicOut (struct tElt *elt);

struct tAssignment *Assignment (struct tReference *var,
                                struct tRHS *rhs, bool new);

struct tRHS *MakeRHS (enum tRHSType rhstype,
                      struct tNode *expr,
                      struct tArrayInitialisation *arrayexpr,
                      struct tFieldInitialisation *recordexpr);

struct tWhen *When (struct tNode *cond,
                    struct tElt *when_stmt, struct tElt *else_stmt,
                    bool elsecanbecommentsonly);

struct tLoop *Loop (struct tElt *body);

struct tWhile *WhileStmt (struct tNode *cond, struct tElt *body);

struct tDo *DoStmt (struct tElt *body);

struct tCase *Case (struct tNode *caseexpr, struct tCaseOption *options);

struct tElt *AppendStmts (struct tElt *head, struct tElt *tail);

struct tElt *Elt (char *filename, int line, int lastline, enum tEltType elttype, void *Ptr);

struct tElt *PrefixElt (struct tElt *first, struct tElt *rest);

struct tRange *MakeRange (struct tNode *minval, struct tNode *maxval);

struct tRange *PrefixRange (struct tRange *head, struct tRange *tail);

struct tCaseOption *BuildCaseOption (struct tRange *ranges,
                                     struct tElt *stmts);

struct tCaseOption *AppendCaseOption (struct tCaseOption *Left,
                                      struct tCaseOption *Right);

char *MakeUniqueName (char *basename);

struct tUnitTerm *MakeUnitTerm (struct tReference *ref, int power,
                                bool getunit);

struct tUnitTerm *PrefixUnitTerm (struct tUnitTerm *head,
                                  struct tUnitTerm *tail);

struct tReference *MakeReference (enum tReferenceType reftype,
                                  char *varname,
                                  struct tNode *index, struct tNode *index2);

struct tReference *PrefixReference (struct tReference *head,
                                    struct tReference *tail);

struct tEnum *MakeEnumSymbol (char *name,
                              struct tRepresentationClause *representation);

struct tEnum *PrefixEnumSymbol (struct tEnum *head, struct tEnum *tail);

struct tTypeSpec *MakeEnumType (int line, struct tEnum *e);

struct tArrayInitialisation *MakeArrayInitialisation (char *varname,
                                                      struct tRHS *rhs,
                                                      struct
                                                      tArrayInitialisationEntry
                                                      *explicitinit);

struct tArrayInitialisationEntry *MakeArrayInitialisationEntry (struct tRange
                                                                *ranges,
                                                                struct tRHS
                                                                *rhs);

struct tArrayInitialisationEntry *PrefixArrayInitialisationEntry (struct
                                                                  tArrayInitialisationEntry
                                                                  *head,
                                                                  struct
                                                                  tArrayInitialisationEntry
                                                                  *tail);

struct tFieldSpec *MakeFieldSpec (char *name, struct tTypeRefOrSpec *type,
                                  struct tRepresentationClause
                                  *representation, char *code, char *comment);

struct tFieldSpec *PrefixFieldSpec (struct tFieldSpec *head,
                                    struct tFieldSpec *tail);

struct tTypeSpec *MakeRecordType (int line,
                                  struct tFieldSpec *fieldlist,
                                  enum tRecordType recordtype);

struct tFieldInitialisation *MakeFieldInitialisation (char *name,
                                                      struct tRHS *rhs,
                                                      bool makenew);

struct tFieldInitialisation *PrefixFieldInitialisation (struct
                                                        tFieldInitialisation
                                                        *head,
                                                        struct
                                                        tFieldInitialisation
                                                        *tail);

struct tTypeSpec *MakeAccessType (int line, struct tTypeRefOrSpec *type,
                                  bool canbenull, bool unchecked,
                                  bool accessconstant, bool accesspersistent, bool accessnew);

struct tElt *SetVarName (struct tElt *varelt, char *name);

struct tProcedure *MakeProcedure (char *filename, int line,
                                  char *name,
                                  struct tElt *formals,
                                  struct tElt *returntype,
                                  struct tElt *declarations,
                                  struct tElt *statements,
                                  char *confirmname,
                                  struct tShareClause *shareclause,
                                  struct tRepresentationClause
                                  *representation, struct tElt *unittest,
                                  int lastlineofdeclaration);

struct tShareClause *MakeShareClause ( /*bool withshared, */ struct tNode
                                      *expr);

struct tElt *SetPrefix (struct tElt *elt, enum tPrefix prefix, int adjustline);

struct tPackage *MakePackage (char *filename, int line,
                              enum tPackageType packagetype,
                              char *name,
                              struct tElt *declarations,
                              struct tElt *initialisation,
                              struct tElt *finalisation,
                              char *confirmname,
                              struct tRepresentationClause *representation,
                              int headerstart, int headerend, int trailerstart, int trailerend);

struct tDeclareBlock *MakeDeclareBlock (struct tElt *declarations,
                                        struct tElt *statements,
                                        struct tRepresentationClause
                                        *representation, bool unittest,
                                        bool notunittest);

struct tSeparateBlock *MakeSeparateBlock (
          char *filename, int line, char *name,
          struct tElt *statements, char *confirmname);

struct tSeparateStmt *MakeSeparateStmt (struct tReference *name);

struct tElt *SetAccess (struct tElt *elt, enum tAccess access);

struct tProcedureCall *MakeProcedureCall (struct tReference *name,
                                          struct tActualParameter
                                          *actuallist);

struct tActualParameter *MakeActualParameter (char *name, struct tNode *expr,
                                              enum tMode mode);

struct tActualParameter *AddRepresentationToActualParameter (char *filename,
                                                             int line,
                                                             struct
                                                             tActualParameter
                                                             *actual,
                                                             struct
                                                             tRepresentationClause
                                                             *representation);

struct tActualParameter *PrefixActualParameter (struct tActualParameter *head,
                                                struct tActualParameter
                                                *tail);

struct tElt *MakePragma (struct tElt *proccallelt);

struct tGenericParameter *MakeGenericParameter (char *filename, int line,
                                                char *name,
                                                enum tGenericParameterType
                                                type, char *identifiertype,
                                                struct tReference *identifier,
                                                struct tElt *formals,
                                                struct tElt *returntype,
                                                struct tGenericActualParameter
                                                *genericactualparams);

struct tGenericParameter *PrefixGenericParameter (struct tGenericParameter
                                                  *head,
                                                  struct tGenericParameter
                                                  *tail);

struct tGeneric *MakeGeneric (struct tGenericParameter *params,
                              struct tElt *body);

struct tGenericActualParameter *MakeGenericActualParameter (char *name,
                                                            struct tNode
                                                            *expression);

struct tGenericActualParameter *PrefixGenericActualParameter (struct
                                                              tGenericActualParameter
                                                              *head,
                                                              struct
                                                              tGenericActualParameter
                                                              *tail);

struct tGenericInstantiation *MakeGenericInstantiation (enum tPackageType
                                                        packagetype,
                                                        char *name,
                                                        struct tReference
                                                        *generic,
                                                        struct
                                                        tGenericActualParameter
                                                        *actuallist,
                                                        int firstline, int lastline);

struct tConstant *MakeConstant (struct tIdentifierListEntry *names, struct tNode *expression,
                                struct tUnitTerm *unit,
                                struct tRepresentationClause *representation);

struct tSeparate *MakeSeparate (enum tPackageType packagetype, bool generic,
	                        char *name, char *filename);

struct tFor *ForStmt (struct tElt *controlvar, struct tNode *rangelow,
                      struct tNode *rangehigh, struct tElt *body);

struct tTypeRefOrSpec
{
  struct tTypeIdentifier *TypeIdentifier;
  struct tTypeSpec *TypeSpec;
};

struct tTypeRefOrSpec *MakeTypeRefOrSpec (struct tTypeIdentifier
                                          *typeidentifier,
                                          struct tTypeSpec *typespec);

struct tSingleAttribute *MakeSingleAttribute (char *attrname, char *auxname);

struct tAttributeChainEntry *MakeAttributeChainEntry (char *filename,
                                                      int line,
                                                      struct tSingleAttribute
                                                      *attr);

struct tAttributeChainEntry *PrefixAttributeChainEntry (char *filename,
                                                        int line,
                                                        struct
                                                        tSingleAttribute
                                                        *attr,
                                                        struct
                                                        tAttributeChainEntry
                                                        *chain);

struct tIdentifierListEntry *MakeIdentifierListEntry(char *str);

struct tIdentifierListEntry *PrefixIdentifierListEntry(struct tIdentifierListEntry *head, struct tIdentifierListEntry *tail);

struct tFileCommentBlock *AppendCommentLine(char *comment, struct tFileCommentBlock *restofblock);

void DumpStmt (struct tElt *elt, int Indent);
void AnalyseProgram (struct tElt *elts, bool outer, struct tFileCommentBlock *packagecomment);

struct tTypeSpec *NewTypespec (int line);

// Memory allocation with exit on failure
void *checked_malloc (size_t size);

void err (char *filename, int line, char *fmt, ...);

#endif /* YTYPES_H */
