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

/* Main program of Rihtan */

#ifndef RIHTAN_H
#define RIHTAN_H

extern BIGINT MaximumObjectSize;
extern bool InFunction;
extern bool InUnitTestOnlyBlock;
extern bool RoutinePrefixShared, RoutinePrefixClosed;
extern bool InPackageInitialisation, InPackageFinalisation;
extern enum tPrefix RequestedRoutinePrefix;
extern struct tTargetType *NO_COMPUTATION;
extern bool VerboseBuild;
extern struct tTargetType TargetTypes[];
extern int NumTargetTypes;

#define MAX_VARS 4000
#define MAX_RELATIONS 300

// The bottom part of the Vars array (below NumVars) contains symbols that are in scope
struct tVars
{
  int NumVars;
  struct tVar *Vars[MAX_VARS];
  int NumRelations;
  struct tRelation Relations[MAX_RELATIONS];
};

extern struct tVars *CurrentVars;

struct tExemptions
{
  bool SideEffect;
  bool AmbiguousOrder;
  bool SubsystemAccess;
  bool SharedAtomicAccess;
  bool SimpleForLoopAnalysis;
  bool AccessConversion;
  bool UncheckedRange;
};

extern struct tExemptions CurrentExemptions;

extern struct tPackage *CurrentPackage;

void
RecordGlobalAccess (int line, struct tGlobalAccess **List, struct tVar *var,
                    bool read, bool write, bool requireinitialisation,
                    bool initialised);

struct tGlobalAccess **
GetGlobalsAccess (void);

struct tVar *TraceWholeReferents(struct tVar *var);

struct tVar *TraceAllReferents(struct tVar *var);

void
SetFinalised (struct tVar *v);

void
SetAccessed (struct tVar *v);

struct tVar *FindVariable (char *filename, int line, struct tReference **name,
                           int from, bool currentscopeonly,
                           bool errorifnotfound);

void TestUnitMatch (struct tUnitDef *u1, struct tUnitDef *u2,
                    struct tNode *expr, bool AllowMatchToNull);

void SetLRUnit(struct tUnitDef **unit, struct tUnitDef *lunit, struct tUnitDef *runit);

struct tUnitDef *MergeUnit (struct tUnitDef *u1, struct tUnitDef *u2,
                            int sense);

void AnalyseProcedureCall (char *filename, int line, struct tProcedureCall *p,
                           struct tTypeSpec **ReturnType,
                           bool * FinalisedThis, bool * Success,
                           bool functioncall, bool closedcall,
                           struct tShareClause **shareclause);

struct tTargetType *
AdjustComputationType (struct tTargetType *comptype, struct tNode *subexpr);

struct tDynamicValues *
StoreDynamicValues ();

// Copy the dynamic values to CurrentVars
void
RestoreDynamicValues (struct tDynamicValues *values);

// Free a set of dynamic values
void
FreeDynamicValues (struct tDynamicValues *values);

struct tTargetType *
GetTargetTypeByName (enum tTargetBaseType basetype, char *wordname);

// lhs and rhs are variable IDs
struct tRelation *
FindRelation (struct tVar *lhs, enum tAttribute lhsattr, struct tVar *rhs,
              enum tAttribute rhsattr);

struct tVar *
SearchForVariable (char *filename, int line, struct tReference **name, int from,
                   bool currentscopeonly, bool errorifnotfound, bool typeorattributesearch,
                   bool *subsystemboundary);

bool
InUnitTestCode (void);
       
// Return the ancestor package of two packages (including possibly those packages themselves)
// that is marked 'shared', or NULL if none
struct tPackage *
SharedAncestorPackage (struct tPackage *pack1, struct tPackage *pack2);
       
// Generate an error message

void
errstart (char *filename, int line);

void
errcont (char *fmt, ...);

void
errbool (BIGINT boolval);

void
errshowvar (struct tVar *v);

void
errdisplayvar (int depth, struct tVar *v);

void
errend_and_continue (void);

void
errend (void);

void
err (char *filename, int line, char *fmt, ...);

void
err_and_continue (char *filename, int line, char *fmt, ...);

void
errwritevariable (struct tReference *varref);

void
warning (char *filename, int line, char *fmt, ...);

void
note (char *str);

void
info (char *filename, int line, char *fmt, ...);

void errDumpReference (struct tReference *r);
void errDumpTypeSpec (struct tTypeSpec *t);
void errDumpMode (enum tMode mode);
void errDumpProcedureCall (struct tProcedureCall *p);
void errDumpExpr (struct tNode *p);
void
tracewritebigint (BIGINT v);

char *
basetypename (enum tBaseType b);

#endif /* RIHTAN_H */
