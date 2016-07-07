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

#ifdef TRACE
int tracepoint = 0;
#endif

bool CompilerTest = FALSE;
bool UnitTest = FALSE;
bool AllowCircularAccess = FALSE;       // Circular access structures allowed?
char *RangeCheckUnsignedCType = NULL, *RangeCheckSignedCType =
  NULL, *RangeCheckUnsignedFormat, *RangeCheckSignedFormat;
bool ExtendedErrorInformation = FALSE;
bool Annotate = FALSE;
bool IncludeSeparateCFiles = FALSE;
char **CommandArgv;
int CommandArgc;

char *GlobalOutputPrefix = "";

unsigned ErrorCount = 0;
bool VerboseAssertions = FALSE;
enum tTaskImplementation
{ NO_TASKING, COROUTINE, PTHREAD, CYCLIC, CONTIKI } TaskImplementation = NO_TASKING;
bool FixedProcedureOverheadSet = FALSE, InterruptOverheadSet = FALSE;
unsigned FixedProcedureOverhead, InterruptOverhead;
unsigned GlobalStackOverhead = 0;
bool GlobalStackOverheadSet = FALSE;
char *InlinePrefix;
bool InlinePrefixLine;
bool InlinePrefixSet = FALSE;
char *CoroutinePreDispatch = NULL, *CoroutinePostDispatch = NULL;
bool NoSystemMain = FALSE;

bool NumbersLeft = FALSE, NumbersInline = FALSE;
int OuterDepth = 0;

bool WithResume = FALSE;
unsigned NumResumePoints;
struct tVar *WithResumeThisSubsystem = NULL;

unsigned IgnoredDispatches = 0;

unsigned NumPragmaNotes = 0;

bool UnitTestWithResume = FALSE, UnitTestResumePointSet = FALSE;

// Output files - initialise to NULL so that they have sensible values if GenerateOutput is false
FILE *SourceFile = NULL, *HeaderFile = NULL, *AuxHeaderFile =
  NULL, *AppHeaderFile = NULL, *CondHeaderFile =
  NULL, *OutFile = NULL;
char *SourceFileName, *HeaderFileName, *AuxHeaderFileName, *AppHeaderFileName,
  *CondHeaderFileName;
bool GenerateOutput = TRUE;
bool InlineToHeader = FALSE;
bool VerboseBuild = FALSE;
bool WriteIndexFile = FALSE;
FILE *IndexFile;
// Used to suppress 'ever used' type tests on globals
// and 'ever called' tests on routines
bool SuppressGlobalUsageChecks = FALSE;
unsigned GlobalUsageErrors = 0;

char *SystemPrefix = NULL;

int CurrentLine, PreviousLine;
bool PrintLineNumbers = TRUE;

bool LoopScan = FALSE;

// Global type/variable/procedure name prefix and package level prefix
#define SYM_PREFIX_LENGTH 500
char globalprefix[SYM_PREFIX_LENGTH];

#define MAX_TARGET_TYPES 50
struct tTargetType TargetTypes[MAX_TARGET_TYPES];
int NumTargetTypes = 0;

char *TargetStackUnit = "unsigned";     //*****no default?
bool TargetStackUnitSet = FALSE;
bool ClearTargetStack = FALSE;

// The default type for C array indexes
char *DefaultArrayIndex = NULL;
BIGINT UniversalIndexMin = 1, UniversalIndexMax = -1;
bool UniversalIndexSet = FALSE;

bool InPackageInitialisation = FALSE, InPackageFinalisation = FALSE;

BIGINT MaximumObjectSize = 0;

char *StringToArrayCopy = "stringtoarray";
bool StringToArrayReqd = FALSE;

char *ArrayToArrayCopy = "memmove";
bool ArrayToArrayCopySet = FALSE;

// This variable is used to pass a parse tree back from a 'separate' declaration
struct tElt *ProgramElts = NULL;

struct tPackage *CurrentPackage = NULL;

struct tStartEntry
{
  struct tVar *Procedure;
  char *ProcedureName;
  BIGINT StackSize;
  struct tStartEntry *Next;
};

struct tStartEntry *StartList = NULL;
bool HaveSharedVariables = FALSE;
bool HaveRestarts = FALSE;

// Base type names for error messages
const char *BaseTypeString[MAX_BASE_TYPE + 1] = {
  "INTEGER", "FLOATING", "BOOLEAN", "ARRAY",
  "RECORD", "PROCEDURE", "PACKAGE", "TYPE", "CSTRING",
  "ACCESS",
  "UNIT", "ADDRESS"
};

struct tVar DummyVar;

struct InterruptHandlerRecord
{
  char *Name;
  bool WithInterruptsEnabled;
  struct InterruptHandlerRecord *Next;
};

struct InterruptHandlerRecord *InterruptHandlerList = NULL;

char *GATE_PREFIX = "__gate_";

struct tUnitTest
{
  struct tTypeSpec *ProcType;
  char *FileName;
  int Line;
  struct tUnitTest *Next;
};

struct tUnitTest *UnitTestHead = NULL, *UnitTestTail = NULL;
bool BuildingUnitTest = FALSE;
unsigned BranchNumber = 0;
bool CoverageOverflow = FALSE;

struct tCoverage
{
  char *FileName;
  int Line;
} *CoverageMap = NULL;
unsigned CurrentCoverageSize = 0;
#define COVERAGE_STEP 10000

char *UNIT_TEST_SUFFIX = "__unit_test";

// Test and merge units
void errDumpUnit (struct tUnitDef *u);
void EmitUnit (struct tUnitDef *u);

void UpdateRanges (struct tNode *cond, bool InvertSense, bool * success,
                   bool allowpartialsuccess, bool testingpostcondition);
struct tUnitDef *AnalyseAddUnit (int depth, char *filename, int line,
                                 struct tUnitDeclaration *unitdecl,
                                 enum tAccess nameaccess);

bool OuterDefinition;

char *DefaultAllocateFunction = "malloc";
char *DefaultFreeFunction = "free";

enum tUsage DefaultUsage = REQUIRED;

// List to remember which basic access routines have been defined

struct tAccessDecl
{
  char *Name;
  struct tAccessDecl *Next;
};

struct tAccessDecl *AccessDeclarations = NULL;

// Contiki autostart list for external C functions
// Initialise like this to make the code simpler
char *autostartlist;
bool haveautostarts = FALSE;

struct tVars *CurrentVars;

int FirstLocalVar = 0;

struct tExemptions CurrentExemptions =
  { FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE };

bool InFunction = FALSE;
bool InUnitTestOnlyBlock = FALSE;
bool RoutinePrefixShared = FALSE, RoutinePrefixClosed = FALSE;
enum tPrefix RequestedRoutinePrefix = PREFIX_NONE;
struct tTypeSpec *CurrentProc = NULL;
bool InRestartableRoutine = FALSE;
bool MainRestartDefined = FALSE;
//
long TotalVarMallocs = 0, TotalVarFrees = 0;

// Comments
bool RecordComments = FALSE;
int LastLine = -1;

//
// List of shared variables
struct tLockVarListEntry
{
  char *Name;
  struct tLockVarListEntry *Next;
};
struct tLockVarListEntry *AllLockVars = NULL;

const char *LockPrefix = "__lock_";

//
// Notes
#define MAX_NOTES 100
char *NoteList[MAX_NOTES];
unsigned NumNotes = 0;
unsigned CurrentNote = 0;

//
struct tTargetType *NO_COMPUTATION;
//

void AnalyseExpression (struct tNode *expr, enum tBaseType *exprtype, BIGINT * min, BIGINT * max,       // for integers
                        int *digits, int *magnitude,    // for floats
                        struct tUnitDef **unit,
                        bool * staticexpression, BIGINT * intval,
                        BIGFLOAT * floatval, bool * boolval,
                        struct tTypeSpec **accessedtype,
                        bool * uncheckedaccess, bool parameter, bool rangefor,
                        bool verbose, bool * virtualexpression,
                        bool * accessconstant, bool * accesspersistent, bool * accessnew);


// Error recovery
jmp_buf error_recovery;

// List of all private variables
struct tVar *PrivateVarList = NULL;
// List of all public variables
struct tVar *PublicVarList = NULL;
// Initialisation variable list
struct tGlobalAccess *InitialisationGlobals = NULL, *AllPackageAccesses =
  NULL, *UnitTestSectionGlobals = NULL;
// Initialisation call list
struct tCallEntry *InitialisationCallList = NULL;
// Target library routines (always present)
struct tCallEntry *TargetLibraryList = NULL;

// Variable pair list for unchecked deadlock pragmas
struct tVarPair
{
  struct tVar *Var1, *Var2;
  struct tVarPair *Next;
  bool Notified;
};
struct tVarPair *UncheckedDeadlockList = NULL;

int ErrorLine = 0, PreviousErrorLine = -1;
// Name of active generic instantiation if any, else NULL
char *ActiveGeneric = NULL;

#define BIT_MASK_LENGTH 64

char *BitMask[BIT_MASK_LENGTH] = {
  "0x1",
  "0x3",
  "0x7",
  "0x0f",
  "0x1f",
  "0x3f",
  "0x7f",
  "0x0ff",
  "0x1ff",
  "0x3ff",
  "0x7ff",
  "0x0fff",
  "0x1fff",
  "0x3fff",
  "0x7fff",
  "0x0ffff",
  "0x1ffff",
  "0x3ffff",
  "0x7ffff",
  "0x0fffff",
  "0x1fffff",
  "0x3fffff",
  "0x7fffff",
  "0x0ffffff",
  "0x1ffffff",
  "0x3ffffff",
  "0x7ffffff",
  "0x0fffffff",
  "0x1fffffff",
  "0x3fffffff",
  "0x7fffffff",
  "0x0ffffffff",
  "0x1ffffffff",
  "0x3ffffffff",
  "0x7ffffffff",
  "0x0fffffffff",
  "0x1fffffffff",
  "0x3fffffffff",
  "0x7fffffffff",
  "0x0ffffffffff",
  "0x1ffffffffff",
  "0x3ffffffffff",
  "0x7ffffffffff",
  "0x0fffffffffff",
  "0x1fffffffffff",
  "0x3fffffffffff",
  "0x7fffffffffff",
  "0x0ffffffffffff",
  "0x1ffffffffffff",
  "0x3ffffffffffff",
  "0x7ffffffffffff",
  "0x0fffffffffffff",
  "0x1fffffffffffff",
  "0x3fffffffffffff",
  "0x7fffffffffffff",
  "0x0ffffffffffffff",
  "0x1ffffffffffffff",
  "0x3ffffffffffffff",
  "0x7ffffffffffffff",
  "0x0fffffffffffffff",
  "0x1fffffffffffffff",
  "0x3fffffffffffffff",
  "0x7fffffffffffffff",
  "0x0ffffffffffffffff"
};

char *SingleBitMask[BIT_MASK_LENGTH] = {
  "0x1",
  "0x2",
  "0x4",
  "0x8",
  "0x10",
  "0x20",
  "0x40",
  "0x80",
  "0x100",
  "0x200",
  "0x400",
  "0x800",
  "0x1000",
  "0x2000",
  "0x4000",
  "0x8000",
  "0x10000",
  "0x20000",
  "0x40000",
  "0x80000",
  "0x100000",
  "0x200000",
  "0x400000",
  "0x800000",
  "0x1000000",
  "0x2000000",
  "0x4000000",
  "0x8000000",
  "0x10000000",
  "0x20000000",
  "0x40000000",
  "0x80000000",
  "0x100000000",
  "0x200000000",
  "0x400000000",
  "0x800000000",
  "0x1000000000",
  "0x2000000000",
  "0x4000000000",
  "0x8000000000",
  "0x10000000000",
  "0x20000000000",
  "0x40000000000",
  "0x80000000000",
  "0x100000000000",
  "0x200000000000",
  "0x400000000000",
  "0x800000000000",
  "0x1000000000000",
  "0x2000000000000",
  "0x4000000000000",
  "0x8000000000000",
  "0x10000000000000",
  "0x20000000000000",
  "0x40000000000000",
  "0x80000000000000",
  "0x100000000000000",
  "0x200000000000000",
  "0x400000000000000",
  "0x800000000000000",
  "0x1000000000000000",
  "0x2000000000000000",
  "0x4000000000000000",
  "0x8000000000000000"
};

// Memory allocation with exit on failure
void *
checked_malloc (size_t size)
{
  void *p;

  p = malloc (size);
  if (p == NULL)
    {
      printf ("***** OUT OF MEMORY *****\n");
      exit (2);
    }
  memset (p, 0, size);
  return p;
}

/* ----- Source tagging ----- */

const char FILE_INDEX_UTEST = 'U';
const char FILE_INDEX_PUBLIC = 'P';
const char FILE_INDEX_PSTART = 'H';
const char FILE_INDEX_PEND = 'E';
const char FILE_INDEX_GENERIC_INST = 'I';
const char FILE_INDEX_GENERIC_DEF = 'G';
const char FILE_INDEX_SEPARATE = 'S';
const char FILE_INDEX_COMMENT = 'C';

int PreviousFirstLine = -1, PreviousLastLine = -1;

void WriteToIndex(char prefix, const char *filename, int firstline, int lastline)
{
  if (WriteIndexFile && (ActiveGeneric == NULL) && (firstline > PreviousLastLine || firstline < PreviousFirstLine))
    {
      fprintf(IndexFile, "%c %s:%d-%d\n", prefix, filename, firstline, lastline);
      PreviousFirstLine = firstline;
      PreviousLastLine = lastline;
    }
}

/* ----- Output ----- */

void
OpenOutput (char *name, char *suffix, bool headerfile, FILE ** f,
            char **filename, bool alwaysopen, bool temporaryfile)
{
  char *tempname;

  if (!GenerateOutput)
    {
      *f = NULL;
      return;
    }
  *filename = checked_malloc (strlen (name) + strlen (suffix) + 1);
  strcpy (*filename, name);
  strcat (*filename, suffix);
  tempname = checked_malloc(strlen(GlobalOutputPrefix) + strlen(*filename) + 1);
  strcpy (tempname, GlobalOutputPrefix);
  strcat (tempname, *filename);
  if (VerboseBuild)
    {
      printf ("Creating %s\n", tempname);
    }
  *f = fopen (tempname, "w");
  if (*f == NULL)
    {
      printf ("Cannot write to %s\n", tempname);
      exit (2);
    }
  if (headerfile)
    {
      // Generate the usual C #ifndef ... #define ... (#endif below) for the header file
      char *h = strdup (*filename);
      char *p = h;
      while (*p)
        {
          if (*p == '.')
            *p = '_';
          p++;
        }
      fprintf (*f, "#ifndef %s\n", h);
      fprintf (*f, "#define %s\n\n", h);
      free (h);
    }
  free (tempname);
}

void
CloseOutput (FILE * f, bool headerfile)
{
  if (!GenerateOutput)
    {
      return;
    }
  if (headerfile && (f != NULL))
    {
      fputs ("#endif\n", f);
    }
  if (f != NULL)
    fclose (f);
}

void
SwitchToHeader (void)
{
  OutFile = HeaderFile;
}

void
SwitchToSource (void)
{
  if (InlineToHeader)
    {
      SwitchToHeader ();
    }
  else
    {
      OutFile = SourceFile;
    }
}

void
SwitchToAuxHeader (void)
{
  OutFile = AuxHeaderFile;
}

void
SwitchToAppHeader (void)
{
  OutFile = AppHeaderFile;
}

void
SwitchToCondHeader (void)
{
  OutFile = CondHeaderFile;
}

FILE *
GetOutput (void)
{
  return OutFile;
}

void
SwitchToOutput (FILE * f)
{
  OutFile = f;
}

void
SwitchToTrace (void)
{
  OutFile = stdout;
}

FILE *OutputBeforeTrace = NULL;

// Generate output code to the source file
// depth is for indentation; 0 not to prefix with indent
void
emit (int depth, char *fmt, ...)
{
  va_list ap;
  int j, emitdepth;

  if (GenerateOutput && (OutFile != NULL))
    {                           /* Output requested */
      // For left numbering, the outer depth is 1 rather than 0
      // We don't want this extra indent in a header file
      if (depth != 0 && NumbersLeft && (OutFile != SourceFile))
        {
          emitdepth = depth - 1;
        }
      else
        {
          emitdepth = depth;
        }
      if ((depth != 0) && NumbersLeft && (OutFile == SourceFile))
        {
          if ((CurrentLine != PreviousLine) && PrintLineNumbers)
            {
              fprintf (OutFile, "/*%05d*/", CurrentLine);
            }
          else
            {
              fputs ("         ", OutFile);
            }
          PreviousLine = CurrentLine;
        }
      for (j = emitdepth; j > 0; j--)
        {
          fprintf (OutFile, "  ");
        }
      va_start (ap, fmt);
      vfprintf (OutFile, fmt, ap);
      va_end (ap);
    }
}

// Write a string to the source file
void
emitstr (char *str)
{
  if (GenerateOutput && (OutFile != NULL))
    {
      fputs (str, OutFile);
    }
}

char *
SubstituteCode (char *filename, int line, char *csource);

// Generate an error message

void
errstart (char *filename, int line)
{
  ErrorLine = line;
  if (!LoopScan)
    {
      if (ErrorLine == 0)
        {
          printf ("ERROR: ");
        }
      else if (ErrorLine > 0 && ErrorLine != PreviousErrorLine)
        {
          if (ActiveGeneric != NULL)
            {
              printf ("[In %s:line %d of generic %s] ERROR: ", filename, line,
                      ActiveGeneric);
            }
          else
            {
              printf ("[%s:%d] ERROR: ", filename, line);
            }
        }
    }
}

void
errcont (char *fmt, ...)
{
  va_list ap;

  if (!LoopScan)
    {
      va_start (ap, fmt);
      if (ErrorLine != PreviousErrorLine || ErrorLine <= 0)
        vprintf (fmt, ap);
      va_end (ap);
    }
}

void
errbool (BIGINT boolval)
{
  if (boolval == 0)
    errcont ("FALSE");
  else
    errcont ("TRUE");
}

char *
procedureorfunction (struct tTypeSpec *t)
{
  if (t->ReturnType == NULL)
    {
      return "procedure";
    }
  else
    {
      return "function";
    }
}

char *
packageorsubsystem (struct tTypeSpec *t)
{
  switch (t->PackageSpec->PackageType)
    {
    case PACKAGE_PACKAGE:
      return "package";
    case PACKAGE_SUBSYSTEM:
      return "subsystem";
    case PACKAGE_SYSTEM:
      return "system";
    default:
      return "package?";
    }
}

void
errshowvar (struct tVar *v)
{
  char *acc, *typename;
  enum tBaseType basetype;

  printf ("%s", v->Name);
  if (v->CName != NULL)
    printf ("(%s)", v->CName);
  switch (v->Access)
    {
    case PRIVATE_ACCESS:
      acc = "priv";
      break;
    case PUBLIC_ACCESS:
      acc = "pub";
      break;
    case HIDDEN_ACCESS:
      acc = "hidden";
      break;
    default:
      acc = "?";
      break;
    }
  if (v->Type != NULL)
    {
      basetype = v->Type->TypeSpec->BaseType;
      if (basetype == TYPE_TYPE)
        {
          switch (v->Type->TypeSpec->Type->TypeSpec->BaseType)
            {
            case INTEGER_TYPE:
              typename = "integer type";
              break;
            case FLOATING_TYPE:
              typename = "float type";
              break;
            case BOOLEAN_TYPE:
              typename = "boolean type";
              break;
            case ARRAY_TYPE:
              typename = "array type";
              break;
            case RECORD_TYPE:
              typename = "record type";
            case PROCEDURE_TYPE:
              typename =
                procedureorfunction (v->Type->TypeSpec->Type->TypeSpec);
              break;
            case PACKAGE_TYPE:
              typename =
                packageorsubsystem (v->Type->TypeSpec->Type->TypeSpec);
            case TYPE_TYPE:
              typename = "type";
              break;
            case CSTRING_TYPE:
              typename = "string type";
              break;
            case ACCESS_TYPE:
              typename = "access type";
              break;
            case UNIT_TYPE:
              typename = "unit";
              break;
            case ADDRESS_TYPE:
              typename = "address";
              break;
            default:
              typename = "?";
              break;
            }
        }
      else if (basetype == PROCEDURE_TYPE)
        {
          typename = procedureorfunction (v->Type->TypeSpec);
        }
      else if (basetype == PACKAGE_TYPE)
        {
          typename = packageorsubsystem (v->Type->TypeSpec);
        }
      else
        {
          typename = v->Type->Name;
        }
      if (typename == NULL)
        typename = "?";
    }
  else
    typename = "NO_TYPE";
  printf ("[%s,IFA=%d%d%d,%s)", acc, v->Initialised, v->Finalised,
          v->Accessed, typename);
}

void
errdisplayvar (int depth, struct tVar *v)
{
  int j;

  for (j = 0; j < depth; j++)
    printf ("    ");
  errshowvar (v);
  printf ("\n");
  if (v->Type->TypeSpec->BaseType == PACKAGE_TYPE)
    {
      v = v->Type->TypeSpec->PackageVars;
      while (v != NULL)
        {
          printf (" ");
          errdisplayvar (depth + 1, v);
          v = v->Next;
        }
    }
}

void
errend_and_continue (void)
{
  if (!LoopScan)
    {
      if (CurrentNote != 0)
        {
          printf (" [note %u]", CurrentNote);
          CurrentNote = 0;
        }
      if (ErrorLine != PreviousErrorLine || ErrorLine <= 0)
        {
          printf ("\n");
          ErrorCount++;
        }
      PreviousErrorLine = ErrorLine;
      //
      if (ExtendedErrorInformation)
        {
          int j;
          struct tVar *v;

          printf ("--------------------\n");
          j = 0;
          while (j < CurrentVars->NumVars)
            {
              v = CurrentVars->Vars[j];
              errdisplayvar (0, v);
              j++;
            }
          printf ("====================\n");
        }
    }
}

void
errend (void)
{
  errend_and_continue ();
  longjmp (error_recovery, 1);
}

void
err (char *filename, int line, char *fmt, ...)
{
  va_list ap;

  errstart (filename, line);
  if (!LoopScan)
    {
      va_start (ap, fmt);
      if (ErrorLine != PreviousErrorLine || ErrorLine <= 0)
        {
          vprintf (fmt, ap);
        }
      va_end (ap);
    }
  errend ();
}

void
err_and_continue (char *filename, int line, char *fmt, ...)
{
  va_list ap;

  errstart (filename, line);
  if (!LoopScan)
    {
      va_start (ap, fmt);
      if (ErrorLine != PreviousErrorLine || ErrorLine <= 0)
        {
          vprintf (fmt, ap);
        }
      va_end (ap);
    }
  errend_and_continue ();
}

void
errwritevariable (struct tReference *varref)
{
  while (TRUE)
    {
      errcont ("%s", varref->VarName);
      varref = varref->Next;
      if (varref == NULL)
        break;
      if (varref->ReferenceType != FIELD_REFERENCE)
        break;
      errcont (".");
    }
}

bool warnon = TRUE;

void
warning (char *filename, int line, char *fmt, ...)
{
  va_list ap;

  if (warnon)
    {
      printf ("[%s:%d] WARNING: ", filename, line);
      va_start (ap, fmt);
      vprintf (fmt, ap);
      va_end (ap);
      printf ("\n");
    }
}

void
note (char *str)
{
  unsigned j, notenum;
  bool found;

  found = FALSE;
  for (j = 0; j < NumNotes; j++)
    {
      if (NoteList[j] == str)
        {
          found = TRUE;
          notenum = j + 1;
          break;
        }
    }
  if (!found)
    {
      if (NumNotes < MAX_NOTES)
        {
          NoteList[NumNotes] = str;
          NumNotes++;
          notenum = NumNotes;
          found = TRUE;
        }
    }
  if (found)
    {
      CurrentNote = notenum;
    }
}

void
PrintNotes (void)
{
  unsigned j;

  for (j = 0; j < NumNotes; j++)
    {
      printf ("Note %u:\n%s\n\n", j + 1, NoteList[j]);
    }
}

void
PrintErrorCount (void)
{
  if (ErrorCount == 0)
    printf ("No errors\n");
  else if (ErrorCount == 1)
    printf ("\n1 error\n");
  else
    printf ("\n%u errors\n", ErrorCount);
}

bool infoon = TRUE;

void
info (char *filename, int line, char *fmt, ...)
{
  va_list ap;

  if (infoon)
    {
      printf ("[%s:%d] (", filename, line);
      va_start (ap, fmt);
      vprintf (fmt, ap);
      va_end (ap);
      printf (")\n");
    }
}

//
void
CheckTaskImplementation (char *filename, int line)
{
  if (TaskImplementation == NO_TASKING)
    {
      err (filename, line,
           "Task implementation is required but has not been set: apply representation clause task_implementation to the system");
    }
}

void CheckUniversalIndexSet(char *filename, int line)
{
  if (!UniversalIndexSet)
    {
      err(filename, line, "Type universal_index has not been specified (use pragma universal_index_type)");
    }
}

void
EmitPreDispatch (int depth)
{
  if (CoroutinePreDispatch != NULL)
    emit (depth, "%s\n", CoroutinePreDispatch);
}

void
EmitPostDispatch (int depth)
{
  if (CoroutinePostDispatch != NULL)
    emit (depth, "%s\n", CoroutinePostDispatch);
}

//

// Remove surrounding quotes and embedded backslash escapes
// It does not perform full embedded backslash translation because it is not needed for
// that purpose - it just removes the initial \ of a pair of characters
char *
DeQuote (char *s)
{
  char *t, *u;
  int len;

  // Remove the surrounding ""
  len = strlen (s);
  t = s;
  if (len > 1)
    {
      if (s[0] == '"' && s[len - 1] == '"')
        {
          t = checked_malloc (strlen (s) - 1);
          s++;
          u = t;
          while (len > 2)
            {
              if (*s == '\\')
                {
                  s++;
                  len--;
                  if (len == 2)
                    break;
                }
              *u++ = *s++;
              len--;
            }
          *u = 0;
        }
    }
  return t;
}

struct tReference *
MakeNewReference (void)
{
  struct tReference *p;

  p = checked_malloc (sizeof (struct tReference));
  p->ReferenceType = IDENTIFIER_REFERENCE;
  p->VarName = NULL;
  p->Index = NULL;
  p->Index2 = NULL;
  p->ReferenceAbbreviation = NULL;
  p->ReferenceAbbreviationEnd = NULL;
  p->Next = NULL;
  p->BitField = FALSE;
  return p;
}

char *
MakeUniqueName (char *basename)
{
  static unsigned UniqueSuffix = 0;
  char *s = checked_malloc (2 + strlen (basename) + 1 + 6 + 1);

  UniqueSuffix++;
  sprintf (s, "_%s_%u", basename, UniqueSuffix);
  return s;
}

char *
MakeFieldName (char *basename, char *fieldname)
{
  char *s = checked_malloc (strlen (basename) + 1 + strlen (fieldname) + 1);

  sprintf (s, "%s_%s", basename, fieldname);
  return s;
}

char *
MakeEndName (char *basename)
{
  char *s = checked_malloc (5 + strlen (basename) + 1); // _end_basename

  sprintf (s, "_end_%s", basename);
  return s;
}

/* Operators */

char *OpString[_OR + 1] = {
  "+", "-", "*", "/", "mod", "iand", "ior", "ixor",
  "<", ">", "<=", ">=", "=", "/=",
  "not", "and", "or"
};

char *
COpString (enum tOp op)
{
  char *s;

  switch (op)
    {
    case _PLUS:
      s = "+";
      break;
    case _MINUS:
      s = "-";
      break;
    case _TIMES:
      s = "*";
      break;
    case _DIVIDE:
      s = "/";
      break;
    case _MOD:
      s = "%";
      break;
    case _IAND:
      s = "&";
      break;
    case _IOR:
      s = "|";
      break;
    case _IXOR:
      s = "^";
      break;
    case _LT:
      s = "<";
      break;
    case _GT:
      s = ">";
      break;
    case _LEQ:
      s = "<=";
      break;
    case _GEQ:
      s = ">=";
      break;
    case _EQ:
      s = "==";
      break;
    case _NEQ:
      s = "!=";
      break;
    case _NOT:
      s = "!";
      break;
    case _AND:
      s = "&&";
      break;
    case _OR:
      s = "||";
      break;
    default:
      err ("", 0, "Internal error: tried to translate op %d", op);
      s = "";
    }
  return s;
}

char *
basetypename (enum tBaseType b)
{
  switch (b)
    {
    case INTEGER_TYPE:
      return "INTEGER";
    case FLOATING_TYPE:
      return "FLOAT";
    case BOOLEAN_TYPE:
      return "BOOLEAN";
    case ARRAY_TYPE:
      return "ARRAY";
    case RECORD_TYPE:
      return "RECORD";
    case PROCEDURE_TYPE:
      return "PROCEDURE";
    case PACKAGE_TYPE:
      return "PACKAGE";
    case TYPE_TYPE:
      return "TYPE";
    case CSTRING_TYPE:
      return "STRING";
    case ACCESS_TYPE:
      return "ACCESS";
    case ADDRESS_TYPE:
      return "ADDRESS";
    default:
      return "???";
    }
}

struct tNode *
NewNode (char *filename, int line,
         enum tOp operator, struct tNode *lhs, struct tNode *rhs,
         BIGINT literal, BIGFLOAT floatliteral, struct tReference *var,
         struct tElt *call, struct tAttributeChainEntry *attribute,
         char *string)
{
  struct tNode *p = (struct tNode *) checked_malloc (sizeof (struct tNode));

  p->FileName = filename;
  p->Line = line;
  p->Op = operator;
  p->Left = lhs;
  p->Right = rhs;
  p->Value = literal;
  p->FloatValue = floatliteral;
  p->String = string;
  p->Var = var;
  p->Call = call;
  p->ComputationType = NULL;
  p->EnclosedInParen = FALSE;
  p->Attribute = attribute;
  return p;
}

struct tNode *
SetEnclosedInParen (struct tNode *node)
{
  node->EnclosedInParen = TRUE;
  return node;
}

struct tTypeSpec *
NewTypespec (int line)
{
  struct tTypeSpec *t;

  t = checked_malloc (sizeof (struct tTypeSpec));
  t->Name = "";
  t->CName = NULL;
  t->TargetAttribute = NULL;
  t->TargetAttributeLine = NULL;
  t->TargetCast = NULL;
  t->Line = line;
  t->CType = NULL;
  t->MinDefinite = TRUE;
  t->MaxDefinite = TRUE;
  t->Controlled = FALSE;
  t->PrefixShared = FALSE;
  t->PrefixClosed = FALSE;
  t->ShareClause = NULL;
  t->Low = 0;
  t->High = 0;
  t->LowExpr = NULL;
  t->HighExpr = NULL;
  t->Representation = NULL;
  t->AccessRoutinesCreated = FALSE;
  t->AccessFinalisation = NULL;
  t->FieldList = NULL;
  t->PackageVars = NULL;
  t->PackageClosed = FALSE;
  t->IndexSpec = NULL;
  t->External = FALSE;
  t->UnmanagedAccess = FALSE;
  t->ConstantAccess = FALSE;
  t->PersistentAccess = FALSE;
  t->NewAccess = FALSE;
  t->Precondition = NULL;
  t->CallList = NULL;
  t->ThisType = NULL;
  t->FirstBranch = 0;
  t->LastBranch = 0;
  t->Incomplete = FALSE;
  t->Dispatches = FALSE;
  t->ShareLockList = NULL;
  t->NoLocks = FALSE;
  t->Restarts = FALSE;
  return t;
}

char *
MakeCName (char *name)
{
  char *s;

  s = checked_malloc (strlen (globalprefix) + strlen (name) + 1);
  strcpy (s, globalprefix);
  strcat (s, name);
  return s;
}

/* ----- Semantic analysis ----- */

void Analyse (int depth, struct tElt *elt, bool statementlist, bool commentsonlylistallowed);
void Generate (struct tElt *elt);
void MatchActualsToFormals (char *filename, int line,
                            struct tProcedureCall *p,
                            struct tTypeSpec *reftype, char *procname,
                            bool functioncall, bool * FinalisedThis,
                            bool * mismatch, bool * sharedcall,
                            bool calltosharedpackage,
                            struct tVar *packagelockvar);
void ResetOutputParameterRanges (int line, struct tProcedureCall *p);
struct tTypeSpec *GetTypeSpec (char *filename, int line,
                               struct tTypeIdentifier *t,
                               enum tAccess *structureaccess);
void TestEnumExpression (struct tTypeSpec *lhstype, struct tNode *expr,
                         BIGINT * value, bool parameter);
void CheckRanges (char *filename, int line, struct tCaseOption *caseopts,
                  struct tArrayInitialisationEntry *arr, BIGINT lowvalue,
                  BIGINT highvalue, struct tTypeSpec *rangetype);
BIGINT EvalCaseValue (char *filename, int line, struct tNode *expr,
                      struct tTypeSpec *switchtype);
struct tUnitDef *AnalyseUnitOrUnique (char *filename, int line,
                                      struct tUnitTerm *unit);
void AnalysePragma (int depth, char *filename, int line,
                    struct tProcedureCall *p);
bool TestForCString (struct tNode *expr, bool parameter, bool constantonly);

//----------

// Allocate a single tVar structure
struct tVar *
MallocVar (char *filename, int line)
{
  struct tVar *p;

  p = checked_malloc (sizeof *p);
  TotalVarMallocs++;
  p->CName = NULL;
  p->TargetAttribute = NULL;
  p->TargetAttributeLine = NULL;
  p->Name = NULL;
  p->ConstUnit = NULL;
  p->ConstAccess = NULL;
  p->ConstAccessType = NULL;
  p->ConstValue = NULL;
  p->Next = NULL;
  p->Referent = NULL;
  p->ShareList = NULL;
  p->Type = NULL;
  p->Unit = NULL;
  p->WholeObjectReference = FALSE;
  p->Initialised = FALSE;
  p->InitialisedGlobally = FALSE;
  p->EverInitialised = FALSE;
  p->Finalised = FALSE;
  p->Accessed = FALSE;
  p->EverAccessed = FALSE;
  p->AccessImage = FALSE;
  p->AccessVolatile = FALSE;
  p->AccessMappedDevice = FALSE;
  p->AccessSharedAtomic = FALSE;
  p->TestMappedDevice = FALSE;
  p->AddressSpecified = FALSE;
  p->AccessVirtual = FALSE;
  p->ExemptionAliasing = FALSE;
  p->Lock = FALSE;
  p->EnclosingPackage = CurrentPackage;
  p->Usage = REQUIRED;
  p->EverUsed = FALSE;
  p->Actual = NULL;
  p->PrivatePublicNext = NULL;  // Initialise this to NULL - not yet exported
  p->PublicOut = FALSE;
  p->FileName = filename;
  p->Line = line;
  p->ShareLock = FALSE;
  p->Hidden = FALSE;
  p->RequirePackageInitialisation = FALSE;
  p->SubsystemBoundary = FALSE;
  p->Statements = NULL;
  return p;
}

// Allocate an initialise a tVars structure
struct tVars *
MallocVarSet (void)
{
  struct tVars *p;
  unsigned j;

  p = checked_malloc (sizeof *p);
  for (j = 0; j < MAX_VARS; j++)
    {
      p->Vars[j] = NULL;
    }
  p->NumVars = 0;
  p->NumRelations = 0;
  return p;
}

void
FreeVarSet (struct tVars *p)
{
  free (p);
}

char *
MinMaxName (char *prefix, char *formalname)
{
  char *s;

  s = checked_malloc (strlen (prefix) + strlen (formalname) + 1);
  strcpy (s, prefix);
  strcat (s, formalname);
  return s;
}

char *
MinName (char *formalname)
{
  return MinMaxName ("first__", formalname);
}

char *
MaxName (char *formalname)
{
  return MinMaxName ("last__", formalname);
}

char *
MakeAccessName (char *name)
{
  char *s;

  s = checked_malloc (7 + strlen (name) + 6);   // "struct " + name +  "__a *" + null byte
  strcpy (s, "struct ");
  strcat (s, name);
  strcat (s, "__a *");
  return s;
}

char *
MakeUnmanagedAccessName (char *name, bool constantaccess, bool structprefix)
{
  int len;
  char *s;

  len = strlen (name) + 3;      // name + " *" + null byte
  if (constantaccess)
    {
      len += 6;                 // "const "
    }
  if (structprefix)
    {
      len += 8;
    }
  s = checked_malloc (len);
  if (constantaccess)
    {
      strcpy (s, "const ");
    }
  else
    {
      strcpy (s, "");
    }
  if (structprefix)
    {
      strcat (s, "struct _");
    }
  strcat (s, name);
  strcat (s, " *");
  return s;
}

char *
MakeAName (char *cname)
{
  char *s, *t;

  s = strdup (cname);           // Must strdup because it might be freed
  t = s;
  while (*t)
    {
      if (*t == ' ')
        *t = '_';
      else if (*t == '*')
        *t = 'P';
      t++;
    }
  return s;
}

// Is formal of a type and mode that would normally be passed by reference?
bool
ByReference (struct tFormalParameter * formal)
{
  // Arrays are passed as pointers, which are values. Cstrings and addressed are pointers
  if (formal->TypeSpec->BaseType == ARRAY_TYPE)
    return FALSE;
  else if (formal->Mode == MODE_OUT || formal->Mode == MODE_IN_OUT
           || formal->Mode == MODE_FINAL_IN_OUT || (formal->Mode == MODE_IN
                                                    && formal->
                                                    TypeSpec->BaseType ==
                                                    RECORD_TYPE))
    return TRUE;
  else
    return FALSE;
}

// Is var of a type and mode that would normally be passed by reference?
bool
PassedByReference (struct tVar * var)
{
  // Arrays are passed by value (i.e. as a pointer to their first element in the usual C manner)
  // EXCEPT in the special case where they are a renames of an object of an array type, in which
  // case they are of type *a, where a is of type typedef t a[n], in which case they must be
  // dereferenced (*a)
  if (var->Type->TypeSpec->BaseType == ARRAY_TYPE) {
    if (var->Referent != NULL)
      return TRUE;
    else
      return FALSE;
  }
  else if (var->Mode == MODE_OUT || var->Mode == MODE_IN_OUT
           || var->Mode == MODE_FINAL_IN_OUT || (var->Mode == MODE_IN
                                                 && var->Type->
                                                 TypeSpec->BaseType ==
                                                 RECORD_TYPE))
    return TRUE;
  else
    return FALSE;
}

void printexpr (struct tNode *p, bool * bitfieldencountered, bool outer);

char *
gettypename (struct tTypeSpec *type)
{
  if (type->CName != NULL)
    return type->CName;
  else
    return "?????";
}

void
printtypename (struct tTypeSpec *type)
{
  emitstr (gettypename (type));
}

char *
getprocname (struct tTypeSpec *type)
{
  if (type->CName != NULL)
    return type->CName;
  else
    return "?????";
}

void
printprocname (struct tTypeSpec *type)
{
  emitstr (getprocname (type));
}

char *
getvarname (struct tVar *var)
{
  if (var->CName != NULL)
    {
      return var->CName;
    }
  else
    {
      return var->Name;         // might be called in pragmas for types etc. that don't have cnames
    }
}

void
printvarname (struct tVar *var)
{
  emitstr (getvarname (var));
}

//

bool CyclicTaskModel(void)
{
  return TaskImplementation == CYCLIC || TaskImplementation == CONTIKI;
}

//
// Buffers for building up structures for stack calculations
//

#define INITIAL_LOCALS_SIZE 10000
unsigned CurrentLocalsSize = INITIAL_LOCALS_SIZE, CurrentCallsSize = INITIAL_LOCALS_SIZE;
char *localsbuff, *localsbuffptr, *callsbuff, *callsbuffptr;
unsigned LocalsBuffNum, CallsBuffNum;

void
ResetLocalsCallsBuffs (void)
{
  localsbuffptr = localsbuff;
  *localsbuffptr = 0;
  LocalsBuffNum = 0;
  callsbuffptr = callsbuff;
  *callsbuffptr = 0;
  CallsBuffNum = 0;
}

// In a routine or a pckage initialisation or finalisation section?
bool InLocalSection(void)
{
  return CurrentProc != NULL || InPackageInitialisation || InPackageFinalisation;
}

void
AddToLocals (char *Modifier, char *TypeName)
{
  if ((!CyclicTaskModel()) && InLocalSection())
    {
      int nreq = strlen(TypeName) + 1 + strlen(Modifier) + 4 + 14 + 10;
      int nnow = localsbuffptr - localsbuff;
      if (nnow + nreq > CurrentLocalsSize)
        {
          CurrentLocalsSize += nreq + INITIAL_LOCALS_SIZE / 2;
          localsbuff = realloc(localsbuff, CurrentLocalsSize);
          if (localsbuff == NULL)
            {
              printf
                ("Cannot extend locals buffer (current size = %u)\n",
                 CurrentLocalsSize);
              exit(3);
            }
          localsbuffptr = localsbuff + nnow;
        }
      LocalsBuffNum++;
      sprintf (localsbuffptr, "%s %svar_%u;\n", TypeName, Modifier,
               LocalsBuffNum);
      while (*localsbuffptr)
        localsbuffptr++;
    }
}

void
AddToCalls (char *Prefix, char *ProcName)
{
  if ((!CyclicTaskModel()) && InLocalSection())
    {
      int nreq = 14 + strlen(Prefix) + strlen(ProcName) + 6 + 14 + 10;
      int nnow = callsbuffptr - callsbuff;
      if (nnow + nreq > CurrentCallsSize)
        {
          CurrentCallsSize += nreq + INITIAL_LOCALS_SIZE / 2;
          callsbuff = realloc(callsbuff, CurrentCallsSize);
          if (callsbuff == NULL)
            {
              printf
                ("Cannot extend calls buffer (current size = %u)\n",
                 CurrentCallsSize);
              exit(3);
            }
          callsbuffptr = callsbuff + nnow;
        }
      CallsBuffNum++;
      sprintf (callsbuffptr, "struct __stack%s%s call_%u;\n", Prefix,
               ProcName, CallsBuffNum);
      while (*callsbuffptr)
        callsbuffptr++;
    }
}

//
//
//

// The actual object is a reference, it is a formal pass-by-reference parameter, it is a whole object
void
printmodeprefix (bool actualrefparam, bool formalrefparam, bool wholeref,
                 bool * paren)
{
  *paren = FALSE;
  if (formalrefparam)
    {
      if ((!actualrefparam) || (!wholeref))
        {
          emitstr ("&");
        }
    }
  if (actualrefparam && ((!formalrefparam) || (!wholeref)))
    {
      if (wholeref)
        emitstr ("*");
      else
        {
          emit (0, "(*");
          *paren = TRUE;
        }
    }

}

void
printparensuffix (bool paren)
{
  if (paren)
    emit (0, ")");
}

void
printidentifier (struct tVar *v, bool actualrefparam, bool formalrefparam,
                 bool wholeref)
{
  bool paren;

  printmodeprefix (actualrefparam, formalrefparam, wholeref, &paren);
  if (v->Type->TypeSpec->BaseType == PROCEDURE_TYPE)
    printprocname (v->Type->TypeSpec);
  else
    printvarname (v);
  printparensuffix (paren);
}

void
printbigint (BIGINT v)
{
  if (v > 127)
    {
      emit (0, "%" PRINTBIG "u", v);
    }
  else
    {
      emit (0, "%" PRINTBIG "", v);
    }
}

void
tracewritebigint (BIGINT v)
{
  if (v > 127)
    {
      printf ("%" PRINTBIG "u", v);
    }
  else
    {
      printf ("%" PRINTBIG "", v);
    }
}

void
printintegerconstant (BIGINT v, char *cname, char *name, char *attr)
{
  if (cname != NULL)
    {
      emit (0, "%s", cname);
    }
  else
    {
      printbigint (v);
    }
}

void
TestNotVirtual (char *filename, int line, struct tVar *v)
{
  if (v->AccessVirtual)
    {
      err (filename, line,
           "Variables with access_mode virtual cannot be used in contexts where real values are required");
    }
}

bool
InUnitTestCode (void)
{
  return BuildingUnitTest || InUnitTestOnlyBlock
    || (RequestedRoutinePrefix == PREFIX_UNIT_TEST);
}

void
printLHSorRHSreference (char *filename, int line,
                        struct tReference *reference,
                        struct tAttributeChainEntry *attribute,
                        bool * bitfieldencountered, bool formalrefparam,
                        bool LHS, struct tFieldSpec **bitfield)
{
  struct tVar *v;
  struct tReference *r;
  struct tReference *name;
  bool ref;
  BIGINT minindex = 0;                  // Avoid a warning - actually will be initialised before use
  struct tTypeSpec *rectype = NULL;     // Avoid a warning - actually will be initialised before use
  struct tTypeSpec *indextype = NULL;   // Avoid a warning - actually will be initialised before use
  struct tFieldSpec *f;
  bool definiteindex = FALSE;           // Avoid a warning - actually will be initialised before use
  bool fieldfound;
  char *minindexname = NULL;            // Avoid a warning - actually will be initialised before use
  struct tTypeSpec *t;
  bool accessref, unmanagedaccess;
  char *linkstr;
  bool bf;
  bool abbreviated;             //, named;
  enum tReferenceType reftype;
  int formalref;

  // In general, X.Y[n].Z or X.Y[n].Z'attr1'attr2

  *bitfield = NULL;
  formalref = formalrefparam;
  r = reference;
  // Test for an attribute
  if (r->ReferenceType == IDENTIFIER_REFERENCE && attribute != NULL)
    {
      BIGINT minvalue, maxvalue, initialmin, initialmax;
      enum tBaseType basetype;
      struct tVar *basevar;
      bool initialised;
      struct tUnitDef *unit;
      struct tTypeSpec *accessedtype;
      struct tTypeSpec *refZ;
      enum tMode mode;
      int lock;
      bool wholeobject, constantaccess, subsystemboundary;

      // find 'Z'
      AnalyseReference (filename, line, r, &basetype, &basevar, &refZ,
                        &minvalue, &maxvalue, &initialmin, &initialmax, &unit,
                        &accessedtype, &initialised, &mode, &lock, FALSE,
                        &wholeobject, TRUE, &constantaccess, TRUE, &subsystemboundary);

      struct tAttributeChainEntry *attr = attribute;
      struct tTypeSpec *reftype = refZ;
      struct tVar *v = basevar;

      // Find attr2 and update the addressed type (e.g. to the index type for 'index_type'first)
      TraceTypeChain (filename, line, &attr, &reftype);
      if (attr == NULL)
        {
          err (filename, line,
               "A type cannot be used without an attribute in an expression");
        }
      if (attr->Name == ATTR_ACCESS || attr->Name == ATTR_PERSISTENT_ACCESS)
        {
          if (reftype->BaseType == TYPE_TYPE)
            {
              err (filename, line, "Cannot create an access to a type");
            }
          name = r;
          v = FindVariable (filename, line, &name, 12, FALSE, TRUE);
          emitstr ("&(");
          printLHSorRHSreference (filename, line, reference, NULL,
                                  bitfieldencountered, FALSE, LHS, bitfield);
          emitstr (")");
          if (*bitfieldencountered)
            {
              err (filename, line, "Cannot take access to bit fields");
            }
        }
      else if (attr->Name == ATTR_SIZE)
        {
          if (reftype->BaseType == TYPE_TYPE)
            {
              // From TraceTypeChain
              reftype = reftype->Type->TypeSpec;
            }
          if (!
              (reftype->MinDefinite
               && reftype->MaxDefinite))
            {
              err (filename, line, "'size cannot be applied to an open type");
            }
          CheckUniversalIndexSet(filename, line);
          emit (0, "((%s)sizeof(%s))", DefaultArrayIndex, reftype->CName);
        }
      else if (attr->Name == ATTR_FIRST || attr->Name == ATTR_MINIMUM)
        {
          if (reftype->BaseType == ARRAY_TYPE)
            {
              if (reftype->MinDefinite)
                {
                  printintegerconstant (reftype->Low, NULL, v->Name, "first");
                }
              else
                {
                  TestNotVirtual (filename, line, v);
                  emit (0, "%s", MinName (r->VarName));
                }
            }
          else if (reftype->BaseType == TYPE_TYPE)
            {
              printintegerconstant (reftype->Type->TypeSpec->Low, NULL,
                                    v->Name, "first");
            }
          else if (reftype->BaseType == INTEGER_TYPE)
            {
              if (attr->Name == ATTR_FIRST)
                printintegerconstant (reftype->Low, NULL, v->Name, "first");
              else
                printintegerconstant (v->Low, NULL, v->Name, "minimum");
            }
          else
            {
              emit (0, "???");  //***** it should have been checked by now
            }
        }
      else if (attr->Name == ATTR_LAST || attr->Name == ATTR_MAXIMUM)
        {
          if (reftype->BaseType == ARRAY_TYPE)
            {
              if (reftype->MaxDefinite)
                {
                  printintegerconstant (reftype->High, NULL, v->Name, "last");
                }
              else
                {
                  TestNotVirtual (filename, line, v);
                  emit (0, "%s", MaxName (r->VarName));
                }
            }
          else if (reftype->BaseType == TYPE_TYPE)
            {
              printintegerconstant (reftype->Type->TypeSpec->High, NULL,
                                    v->Name, "last");
            }
          else if (reftype->BaseType == INTEGER_TYPE)
            {
              if (attr->Name == ATTR_LAST)
                printintegerconstant (reftype->High, NULL, v->Name, "last");
              else
                printintegerconstant (v->High, NULL, v->Name, "maximum");
            }
          else
            emit (0, "???");    // it should have been checked by now
        }
      else if (attr->Name == ATTR_LENGTH)
        {
          if (reftype->BaseType == ARRAY_TYPE)
            {
              if (reftype->MaxDefinite && reftype->MinDefinite)
                {
                  printintegerconstant (reftype->High - reftype->Low + 1,
                                        NULL, v->Name, "length");
                }
              else
                {
                  TestNotVirtual (filename, line, v);
                  if (reftype->MaxDefinite && (!reftype->MinDefinite))
                    {
                      emit (0, "(");
                      printbigint (reftype->High);
                      emit (0, " - ");
                      emit (0, "%s", MinName (r->VarName));
                      emit (0, " + 1)");
                    }
                  else if ((!reftype->MaxDefinite) && reftype->MinDefinite)
                    {
                      emit (0, "(");
                      emit (0, "%s", MaxName (r->VarName));
                      emit (0, " - ");
                      printbigint (reftype->Low);
                      emit (0, " + 1)");
                    }
                  else
                    {
                      emit (0, "(%s - %s + 1)", MaxName (r->VarName),
                            MinName (r->VarName));
                    }
                }
            }
          else if (reftype->BaseType == TYPE_TYPE
                   && reftype->Type->TypeSpec->BaseType == ARRAY_TYPE)
            {
              printintegerconstant (reftype->Type->TypeSpec->High -
                                    reftype->Type->TypeSpec->Low + 1, NULL,
                                    v->Name, "length");
            }
          else
            {
              emit (0, "???");  // it should have been checked by now
            }
        }
      else if (attr->Name == ATTR_UNIT || attr->Name == ATTR_INDEX_UNIT)
        {
          emit (0, "1");
        }
      else if (attr->Name == ATTR_INITIAL)
        {
          err (filename, line,
               "Attribute 'initial cannot be used in contexts where code is generated");
        }
      else
        {
          emit (0, "?????%d", attr->Name);      //
        }
      return;
    }

  //
  // Not an attribute
  //

  // For bit fields enclose the generated code in parentheses because it will contain >> and & in general
  bool enclosebitref = (reference->BitField && (!LHS));

  if (enclosebitref)
    {
      emit (0, "(");
    }

  abbreviated = (r->ReferenceAbbreviation != NULL);
  if (abbreviated)
    {
      if (r->ReferenceType == IDENTIFIER_REFERENCE)
        {
          formalref = FALSE;
        }
      emit (0, "%s", r->ReferenceAbbreviation);
    }
  accessref = FALSE;
  unmanagedaccess = FALSE;
  //
  while (r != NULL)
    {
      reftype = r->ReferenceType;
      switch (reftype)
        {
        case IDENTIFIER_REFERENCE:
          name = r;
          v = FindVariable (filename, line, &name, 12, FALSE, TRUE);
          //
          if (v->Type == &ConstantIntegerType)
            {
              if (!abbreviated)
                {
                  printintegerconstant (v->High, v->CName, v->Name, NULL);
                }
            }
          else if (v->Type == &ConstantFloatType)
            {
              if (!abbreviated)
                {
                  if (v->CName != NULL)
                    {
                      emit (0, "%s", v->CName);
                    }
                  else
                    {
                      emit (0, "%lf", v->FloatVal);
                    }
                }
            }
          else if (v->Type == &ConstantBooleanType)
            {
              if (!abbreviated)
                {
                  if (v->CName != NULL)
                    {
                      emit (0, "%s", v->CName);
                    }
                  else
                    {
                      printbigint (v->High);
                    }
                  if (Annotate)
                    {
                      if (v->High == 0)
                        emitstr (" /*false*/");
                      else
                        emitstr (" /*true*/");
                    }
                }
            }
          else if (v->Type == &ConstantAccessType)
            {
              if (!abbreviated)
                {
                  if (v->ConstAccess == NULL)
                    {
                      emit (0, "NULL");
                    }
                  else
                    {
                      emitstr ("&(");
                      printLHSorRHSreference (filename, line, v->ConstAccess,
                                              NULL, bitfieldencountered,
                                              FALSE, LHS, bitfield);
                      emitstr (")");
                    }
                }
            }
          else if (v->Type == &ConstantStringType)
            {
              if (!abbreviated)
                emit (0, "%s", v->CName);
            }
          else if (v->Type->TypeSpec->BaseType == UNIT_TYPE)
            {
              if (!abbreviated)
                emit (0, "1");
            }
          else
            {
              ref = PassedByReference (v);
              //
              // Special case: if acc is a variable of an access type then
              //               acc[n] is an abbreviation for acc.all[n], and
              //               acc must refer to an object of an array type (not
              //               the type of the array element), so
              //               acc must always be dereferenced (this is similar to
              //               the exception in PassedByRference)
              if (v->Type->TypeSpec->BaseType == ACCESS_TYPE && r->Next != NULL)
                {
                  if (   r->Next->ReferenceType == ARRAY_REFERENCE
                      || r->Next->ReferenceType == ARRAY_SLICE_REFERENCE
                      || r->Next->ReferenceType == ARRAY_SLICE_LENGTH_REFERENCE)
                    {
                      ref = TRUE;
                    }
                }
              {
                bool star = FALSE, paren = FALSE, derefcheck = FALSE;

                TestNotVirtual (filename, line, v);
                if (!abbreviated)
                  {
                    if (v->Type->TypeSpec->BaseType == ACCESS_TYPE
                        && v->Type->TypeSpec->UnmanagedAccess)
                      {
                        if (name->Next != NULL)
                          {
                            if (name->Next->ReferenceType == FIELD_REFERENCE)
                              {
                                if (!strcasecmp (name->Next->VarName, "all"))
                                  {
                                    emitstr ("(*");
                                    star = TRUE;
                                  }
                              }
                            else
                              {
                                if (v->Type->TypeSpec->
                                    ElementTypeSpec->BaseType != ARRAY_TYPE)
                                  {
                                    emitstr ("(*");
                                    star = TRUE;
                                  }
                              }
                          }
                      }
                  }
                if (CompilerTest && (!abbreviated))
                  {
                    if (v->Type->TypeSpec->BaseType == ACCESS_TYPE
                        && name->Next != NULL)
                      {
                        // We will calling deref_check with the dereferenced access value; it will
                        // return this value, so we don't want to rereference it again. Hence
                        // the first parameter to printmodeprefix is FALSE here
                        printmodeprefix(FALSE, formalref, r->Next == NULL, &paren);
                        if (v->Type->TypeSpec->UnmanagedAccess)
                          {
                            struct tTypeSpec *etype = v->Type->TypeSpec->ElementTypeSpec;

                            emit (0, "(*(%s *)__deref_check(",
                                  etype->CName);
                          }
                        else
                          {
                            char *aname =
                              MakeAName (v->Type->TypeSpec->
                                         ElementTypeSpec->CName);
                            emit (0, "((struct %s__a *)__deref_check(",
                                  aname);
                            free (aname);
                          }
                        derefcheck = TRUE;
                      }
                  }
                if (!abbreviated)
                  {
                    printidentifier (v, ref, formalref && (!derefcheck), r->Next == NULL);
                  }
                if (v->Type->TypeSpec->BaseType == ARRAY_TYPE)
                  {
                    if (v->Type->TypeSpec->MinDefinite)
                      {
                        minindex = v->Type->TypeSpec->Low;
                        definiteindex = TRUE;
                      }
                    else
                      {
                        minindexname = MinName (name->VarName);
                        definiteindex = FALSE;
                      }
                    rectype = v->Type->TypeSpec->ElementTypeSpec;
                    indextype = v->Type->TypeSpec->IndexTypeSpec;
                  }
                else if (v->Type->TypeSpec->BaseType == RECORD_TYPE)
                  {
                    rectype = v->Type->TypeSpec;
                  }
                else if (v->Type->TypeSpec->BaseType == ACCESS_TYPE)
                  {
                    if (name->Next != NULL)
                      {
                        accessref = TRUE;
                        unmanagedaccess =
                          (v->Type->TypeSpec->UnmanagedAccess);

                        if (derefcheck)
                          {
                            emitstr (", ");
                            if (v->Type->TypeSpec->UnmanagedAccess)
                              {
                                emitstr ("1");
                              }
                            else
                              {
                                emitstr ("(");
                                printidentifier (v, ref, FALSE,
                                                 r->Next == NULL);
                                emitstr (" == NULL) ? 0 : ");
                                printidentifier (v, ref, FALSE,
                                                 r->Next == NULL);
                                emit (0, "->count");
                              }
                            emit (0, ", %d", line);
                            emit (0, "))");
                            if (paren) emit(0, ")");
                          }

                      }
                    rectype = v->Type->TypeSpec->ElementTypeSpec;
                    if (rectype->BaseType == ARRAY_TYPE)
                      {
                        minindex = rectype->Low;
                        definiteindex = TRUE;
                        indextype = rectype->IndexTypeSpec;
                      }
                  }
                if (star)
                  {
                    emitstr (")");
                  }
              }
            }
          r = name;
          break;
        case ARRAY_REFERENCE:
        case ARRAY_SLICE_REFERENCE:
        case ARRAY_SLICE_LENGTH_REFERENCE:
          if (accessref)
            {
              if ((!abbreviated) && (!unmanagedaccess))
                emit (0, "->all");
              accessref = FALSE;
            }
          if (!abbreviated)
            {
              if (reftype == ARRAY_REFERENCE)
                emit (0, "[");
              else
                emit (0, " + (");
              if (CompilerTest)
                {
                  emit (0, "__index_check(%d, (%s)(", line,
                        DefaultArrayIndex);
                }
              if (definiteindex)
                {
                  bf = FALSE;
                  printexpr (r->Index, &bf, TRUE);
                  if (minindex > 0)
                    {
                      emit (0, " - ");
                      printbigint (minindex);
                    }
                  else if (minindex < 0)
                    {
                      emit (0, " + ");
                      printbigint (-minindex);
                    }
                }
              else
                {
                  bf = FALSE;
                  printexpr (r->Index, &bf, TRUE);
                  emit (0, " - %s", minindexname);
                }
              if (CompilerTest)
                {
                  emit (0, "), ");
                  printbigint (indextype->Low);
                  emit (0, ", ");
                  printbigint (indextype->High);
                  emit (0, ")");
                }
              if (reftype == ARRAY_REFERENCE)
                emit (0, "]");
              else
                emit (0, ")");
            }
          if (rectype->BaseType == ARRAY_TYPE)
            {
              minindex = rectype->Low;
              definiteindex = TRUE;
              indextype = rectype->IndexTypeSpec;
            }
          break;
        case FIELD_REFERENCE:
          if (accessref && (!strcasecmp (r->VarName, "all")))
            {
              if ((!abbreviated) && (!unmanagedaccess))
                emit (0, "->all");
              t = rectype->ElementTypeSpec;
            }
          else
            {
              if (accessref)
                {
                  if (unmanagedaccess)
                    {
                      linkstr = "->";
                    }
                  else
                    {
                      linkstr = "->all.";
                    }
                }
              else
                {
                  linkstr = ".";
                }
              f = rectype->FieldList;
              fieldfound = FALSE;
              while (f != NULL)
                {
                  if (!strcmp (f->Name, r->VarName))
                    {
                      if (f->FieldSet)
                        {
                          if (f->BitFieldSet)
                            {
                              if (!abbreviated)
                                {
                                  if (LHS)
                                    {
                                      if (!f->UseShortBitFieldForm)
                                        {
                                          emit (0, "%s%s[%u]", linkstr,
                                                f->Name, f->FieldOffset);
                                        }
                                    }
                                  else
                                    {
                                      if (f->UseShortBitFieldForm)
                                        {
                                          if (f->BitOffset == 0)
                                            {
                                              emit (0, " & %s",
                                                    BitMask[f->NumBits - 1]);
                                            }
                                          else
                                            {
                                              emit (0, " >> %u & %s",
                                                    f->BitOffset,
                                                    BitMask[f->NumBits - 1]);
                                            }
                                        }
                                      else
                                        {
                                          if (f->BitOffset == 0)
                                            {
                                              emit (0, "%s%s[%u] & %s",
                                                    linkstr, f->Name,
                                                    f->FieldOffset,
                                                    BitMask[f->NumBits - 1]);
                                            }
                                          else
                                            {
                                              emit (0, "%s%s[%u] >> %u & %s",
                                                    linkstr, f->Name,
                                                    f->FieldOffset,
                                                    f->BitOffset,
                                                    BitMask[f->NumBits - 1]);
                                            }
                                        }
                                    }
                                }
                              *bitfield = f;
                              *bitfieldencountered = TRUE;
                            }
                          else
                            {
                              if (!abbreviated)
                                emit (0, "%s%s", linkstr, f->Name);
                            }
                          fieldfound = TRUE;
                          break;
                        }
                    }
                  f = f->Next;
                }
              if (!fieldfound)
                {
                  if (rectype->StorageSet)
                    {
                      if (!abbreviated)
                        emit (0, "%s_v", linkstr);
                      linkstr = ".";
                    }
                  if (rectype->RecordType == REC_UNION)
                    {
                      if (!abbreviated)
                        emit (0, "%s_u", linkstr);
                      linkstr = ".";
                    }
                  if (!abbreviated)
                    emit (0, "%s%s", linkstr, r->VarName);
                }
              f = rectype->FieldList;
              t = NULL;
              while (f != NULL)
                {
                  if (!strcmp (f->Name, r->VarName))
                    {
                      t = f->Typespec;
                      break;
                    }
                  f = f->Next;
                }
            }
          if (t != NULL)
            {
              if (t->BaseType == ARRAY_TYPE)
                {
                  minindex = t->Low;
                  definiteindex = TRUE;
                  rectype = t->ElementTypeSpec;
                  indextype = t->IndexTypeSpec;
                }
              else if (t->BaseType == RECORD_TYPE)
                {
                  rectype = t;
                  accessref = FALSE;
                }
              else if (t->BaseType == ACCESS_TYPE)
                {
                  rectype = t;
                  unmanagedaccess = t->UnmanagedAccess;
                  accessref = TRUE;
                  if (t->ElementTypeSpec->BaseType == ARRAY_TYPE)
                    {
                      definiteindex = TRUE; // in case this is x.ptr[n]
                      minindex = t->ElementTypeSpec->Low;
                      indextype = t->ElementTypeSpec->IndexTypeSpec;
                    }
                }
            }
          break;
        }

      if (abbreviated && r == reference->ReferenceAbbreviationEnd)
        abbreviated = FALSE;
      formalref = FALSE;
      r = r->Next;
    }
  //
  if (enclosebitref)
    {
      emit (0, ")");            // See comment above
    }
}

void
printreference (char *filename, int line, struct tReference *reference,
                struct tAttributeChainEntry *attribute,
                bool * bitfieldencountered, bool formalrefparam)
{
  struct tFieldSpec *bitfield;

  printLHSorRHSreference (filename, line, reference, attribute,
                          bitfieldencountered, formalrefparam, FALSE,
                          &bitfield);
}

void
CheckForSlice (struct tReference *ref, bool * slice, struct tNode **lowexpr,
               struct tNode **highexpr, enum tReferenceType *slicetype)
{
  struct tReference *r;

  r = ref;
  while (r->Next != NULL)
    {
      r = r->Next;
    }
  if (r->ReferenceType == ARRAY_SLICE_REFERENCE
      || r->ReferenceType == ARRAY_SLICE_LENGTH_REFERENCE)
    {
      *lowexpr = r->Index;
      *highexpr = r->Index2;
      *slicetype = r->ReferenceType;
      *slice = TRUE;
    }
  else
    *slice = FALSE;
}

bool
IsSlice (struct tReference *ref)
{
  bool slice;
  struct tNode *lowexpr, *highexpr;
  enum tReferenceType slicetype;

  CheckForSlice (ref, &slice, &lowexpr, &highexpr, &slicetype);
  return slice;
}

void
StartRangeCheckCall (int depth, char *filename, int line, bool unsignedtype)
{
  BIGINT maxu = 0, mins = 0, maxs = 0;  // Intialisations are to avoid warnings - actually OK
  int j, maxindex, widestindex;

  // If this is the first range check then find the appropriate C types to use for the
  // parameters to the range check functions
  if (RangeCheckUnsignedCType == NULL || RangeCheckSignedCType == NULL)
    {
      maxindex = -1;
      widestindex = -1;
      for (j = 0; j < NumTargetTypes; j++)
        {
          if (TargetTypes[j].BaseType == TARGET_INTEGER)
            {
              if (maxindex == -1 || TargetTypes[j].Max > maxu)
                {               // short-circuit
                  maxu = TargetTypes[j].Max;
                  maxindex = j;
                }
              if (widestindex == -1
                  || (TargetTypes[j].Min < mins && TargetTypes[j].Max > maxs))
                {
                  mins = TargetTypes[j].Min;
                  maxs = TargetTypes[j].Max;
                  widestindex = j;
                }
            }
        }
      if (maxindex == -1)
        {
          err (filename, line,
               "Target integer types have not been specified");
        }
      else
        {
          RangeCheckUnsignedCType = TargetTypes[maxindex].CName;
          RangeCheckUnsignedFormat = TargetTypes[maxindex].PrintfFormat;
          RangeCheckSignedCType = TargetTypes[widestindex].CName;
          RangeCheckSignedFormat = TargetTypes[widestindex].PrintfFormat;
        }
    }
  if (unsignedtype)
    emit (depth, "__range_check_unsigned(%d, ", line);
  else
    emit (depth, "__range_check_signed(%d, ", line);
}

// Return the ancestor package of two packages (including possibly those packages themselves)
// that is marked 'shared', or NULL if none
struct tPackage *
SharedAncestorPackage (struct tPackage *pack1, struct tPackage *pack2)
{
  struct tPackage *p1, *p2;

  p1 = pack1;
  while (p1 != NULL && (!p1->Shared))
    {
      p1 = p1->EnclosingPackage;
    }
  p2 = pack2;
  while (p2 != NULL && (!p2->Shared))
    {
      p2 = p2->EnclosingPackage;
    }
  if (p1 != NULL && p2 == p1)
    return p1;
  else
    return NULL;
}

// Scan for variables in an expression
#define MAX_VARS_IN_EXPR 100      // Use this value + 1 for the actual variable in order to allow for trailing NULL

void DoScanForVars(char *filename, int line, struct tNode *expr, struct tVar **vp, int *nvars)
{
  enum tOp op = expr->Op;
  int j;
  bool found;

  if (op == _VAR)
    {
      struct tReference *ref = expr->Var;
      struct tVar *var;
      var = FindVariable (filename, line, &ref, 35, FALSE, TRUE);
      // Check if already there
      found = FALSE;
      for (j = 0; j < *nvars; j++)
        {
          if (vp[j] == var)
            {
              found = TRUE;
              break;
            }
        }
      if (!found)
        {
          if (*nvars == MAX_VARS_IN_EXPR)
            {
              err(filename, line, "Too many variables in expression");
            }
          vp[*nvars] = var;
          (*nvars)++;
        }
    }
  else if (IsIntegerOp (op) || IsFloatOp (op) || IsBooleanOp (op) || IsRelop (op))
    {
      DoScanForVars(filename, line, expr->Left, vp, nvars);
      if (op != _NOT)
        {
          DoScanForVars(filename, line, expr->Right, vp, nvars);
        }
    }
  else if (op == _FN_CALL)
    {
      struct tProcedureCall *p = expr->Call->Ptr;
      struct tActualParameter *ap = p->ActualList;
      while (ap != NULL)
        {
          DoScanForVars(filename, line, ap->Expr, vp, nvars);
          ap = ap->Next;
        }
    }
}

void ScanForVars(char *filename, int line, struct tNode *expr, struct tVar **varlist)
{
  int nvars;

  nvars = 0;
  DoScanForVars(filename, line, expr, varlist, &nvars);
  varlist[nvars] = NULL;
}

void
printactualparam (char *filename, int line, struct tActualParameter *a,
                  struct tFormalParameter *f, bool testcall,
                  bool externalcall)
{
  bool ref;
  struct tNode *lowexpr, *highexpr;
  bool slice, performcast;
  enum tReferenceType slicetype;

  if (testcall)
    ref = FALSE;                // this is a range check call - need the value
  else
    ref = ByReference (f);
  slice = FALSE;
  if (a->Expr->Op == _VAR)
    {
      CheckForSlice (a->Expr->Var, &slice, &lowexpr, &highexpr, &slicetype);
    }
  performcast = (externalcall && (f->TypeSpec->TargetCast != NULL));
  if (ref)
    {
      if (performcast)
        {
          emit (0, "(%s *)(", f->TypeSpec->TargetCast);
        }
    }
  else
    {
      if (performcast)
        {
          emit (0, "(%s)(", f->TypeSpec->TargetCast);
        }
    }
  if (a->AuxType != NULL)
    emit (0, "__%x", a);
  else
    {
      bool bitfield = FALSE;    // Must initialise it if we care

      if (ref)
        {
    		  if (a->IsThis && a->Expr->Var->ReferenceAbbreviation != NULL)
		        {
		          // Because an abbreviation will have been set for "this", we need to
  			      // add the & here
			        emitstr("&");
		        }
          printreference (filename, line, a->Expr->Var, a->Expr->Attribute,
                          &bitfield, ref);
        }
      else
        {
          printexpr (a->Expr, &bitfield, TRUE);
        }
      if (bitfield && ref)
        {
          err (filename, line, "Bitfields cannot be passed by reference");
        }
    }
  if (performcast)
    emitstr (")");
  if (f->TypeSpec->BaseType == ARRAY_TYPE)
    {
      bool bf;

      if (!f->TypeSpec->MinDefinite)
        {
          emit (0, ", ");
          if (slice)
            {
              bf = FALSE;
              printexpr (lowexpr, &bf, TRUE);
            }
          else if (a->ActualTypeSpec->MinDefinite)
            {
              printbigint(a->ActualTypeSpec->Low);
            }
          else
            {
              emit (0, "%s", MinName (a->Expr->Var->VarName));
            }
        }
      if (!f->TypeSpec->MaxDefinite)
        {
          emit (0, ", ");
          if (slice)
            {
              if (slicetype == ARRAY_SLICE_REFERENCE)
                {
                  if (highexpr == NULL) // actual is a[x..], must match the formal on the right
                    {
                      printbigint(f->TypeSpec->IndexTypeSpec->High);
                    }
                  else
                    {
                      bf = FALSE;
                      printexpr (highexpr, &bf, TRUE);
                    }
                }
              else
                {
                  emitstr ("(");
                  bf = FALSE;
                  printexpr (lowexpr, &bf, TRUE);
                  emitstr (") + (");
                  bf = FALSE;
                  printexpr (highexpr, &bf, TRUE);
                  emitstr (") - 1");
                }
            }
          else if (a->ActualTypeSpec->MaxDefinite)
            {
              printbigint(a->ActualTypeSpec->High);
            }
          else
            {
              emit (0, "%s", MaxName (a->Expr->Var->VarName));
            }
        }
    }
}

void
printlocks (struct tVar **locklist, unsigned numlocks, bool packagelock,
            char *packagename)
{
  unsigned j, totallocks;

  totallocks = numlocks;
  if (packagelock)
    {
      totallocks++;
    }
  emit (0, "%u", totallocks);
  for (j = 0; j < numlocks; j++)
    {
      emit (0, ", &%s", LockPrefix);
      printvarname (locklist[j]);
    }
  if (packagelock)
    {
      emit (0, ", &%s%s", LockPrefix, packagename);
    }
}

void
printcall (char *prefix, int depth, char *filename, int line,
           struct tProcedureCall *p)
{
  int apos, f;
  struct tFormalParameter *fp;
  struct tActualParameter *a;
  struct tReference *nameref;
  struct tVar *v;
  bool testcode, external;

  nameref = p->Name;
  v = FindVariable (filename, line, &nameref, 301, FALSE, TRUE);
  external = v->Type->TypeSpec->External;
  emit (depth, "%s%s", prefix, getprocname (v->Type->TypeSpec));
  emit (0, "(");
  if (p->ActualList != NULL)
    {
      a = p->ActualList;
      fp = p->Formals;
      for (f = 0; f < p->NumFormals; f++)
        {
          a = p->ActualList;
          apos = p->PositionNum[f];
          while (apos != 0 && a != NULL)
            {
              a = a->Next;
              apos = apos - 1;
            }
          if (a != NULL)
            {
              if (f != 0)
                emit (0, ", ");
              testcode = FALSE;
              if (CompilerTest && (fp->Mode != MODE_OUT))
                {
                  if (fp->TypeSpec->BaseType == INTEGER_TYPE)
                    {
                      bool unsignedcheck = (fp->TypeSpec->Low >= 0);

                      testcode = TRUE;
                      emitstr ("(");
                      StartRangeCheckCall (0, filename, line, unsignedcheck);
                      printactualparam (filename, line, a, fp, TRUE,
                                        external);
                      if (unsignedcheck)
                        {
                          emit (0, ", %" PRINTBIG "u, %" PRINTBIG "u),(",
                                fp->TypeSpec->Low, fp->TypeSpec->High);
                        }
                      else
                        {
                          emit (0, ", %" PRINTBIG ", %" PRINTBIG "),(",
                                fp->TypeSpec->Low, fp->TypeSpec->High);
                        }
                    }
                  else if (fp->TypeSpec->BaseType == ACCESS_TYPE
                           && (!fp->TypeSpec->CanBeNull))
                    {
                      testcode = TRUE;
                      emit (0, "(__notnull_check(%d, ", line);
                      printactualparam (filename, line, a, fp, TRUE,
                                        external);
                      emitstr ("),(");
                    }
                }
              printactualparam (filename, line, a, fp, FALSE, external);
              if (testcode)
                {
                  emitstr ("))");
                }
            }
          fp = fp->Next;
        }
    }
  emit (0, ")");
  AddToCalls (prefix, getprocname (v->Type->TypeSpec));
}

void
emitcyclicdispatch (int depth)
{
  if (UnitTest)
    {
      emit (depth, "/* Cyclic dispatch omitted under unit test */");
    }
  else
    {
      NumResumePoints++;
      if (Annotate)
        {
          emit(depth, "/* Dispatch */");
        }
      if (TaskImplementation == CONTIKI)
        {
          emit(depth, "process_post(PROCESS_CURRENT(), PROCESS_EVENT_CONTINUE, NULL);\n");
        }
      emit (depth, "__resume = %u;\n", NumResumePoints);
      if (TaskImplementation == CONTIKI)
        {
          emit(depth, "return PT_YIELDED;\n");
        }
      else
        {
          emit (depth, "return;\n");
        }
      emit (depth, "__resume%u: ;\n", NumResumePoints);  // The ; is needed to avoid an error at the end of blocks
      if (TaskImplementation == CONTIKI)
        {
          // Continue to yield with the current resume point until the continue condition is met
          emit(depth, "if (ev != PROCESS_EVENT_CONTINUE) return PT_YIELDED;\n");
        }
    }
}

#define MAX_LOCKS 100
void
printprocedurecall (int depth, char *filename, int line,
                    struct tProcedureCall *p,
                    struct tShareClause *shareclause /* or NULL */ ,
                    bool trailingsemi)
{
  struct tActualParameter *a;
  bool haveaux;
  struct tReference *ref;
  struct tVar *var;
  struct tVar *locklist[MAX_LOCKS];
  unsigned numlocks;
  bool bf, acquirepackagelock;
  char *packagename;
  struct tPackage *varsharedpackage;
  bool hasshareclause;
  struct tShareLockElt *shares;


  ref = p->Name;
  var = FindVariable (filename, line, &ref, 302, FALSE, TRUE);
  //
  numlocks = 0;
  a = p->ActualList;
  while (a != NULL)
    {
      struct tVar *varlist[MAX_VARS_IN_EXPR + 1];
      ScanForVars(filename, line, a->Expr, varlist);
      struct tVar **vp = varlist;
      struct tVar *v;
      while (*vp != NULL)
        {
          v = *vp;
          // Check for a shared variable that is not locked already
          if (v->Mode == MODE_SHARED && (!v->ShareLock))
            {
              if (numlocks == MAX_LOCKS)
                {
                  err (filename, line, "Too many locks");
                }
              else
                {
                  locklist[numlocks] = v;
                  numlocks++;
                }
            }
          vp++;
        }
      a = a->Next;
    }
  shares = var->Type->TypeSpec->ShareLockList;
  while (shares != NULL)
    {
      if (!shares->Var->ShareLock)
        {
          if (numlocks == MAX_LOCKS)
            {
              err (filename, line, "Too many locks");
            }
          else
            {
              locklist[numlocks] = shares->Var;
              numlocks++;
            }
        }
      shares = shares->Next;
    }
  //
  // If calling a public routine in a shared package from outside that package then acquire the package lock
  varsharedpackage =
    SharedAncestorPackage (var->EnclosingPackage, var->EnclosingPackage);
  if (var->Access == PUBLIC_ACCESS && varsharedpackage != NULL
      && varsharedpackage != CurrentPackage)
    {
      if (varsharedpackage->Dispatches || (TaskImplementation == PTHREAD))
        {
          // Another subsystem could run before the call returns, so it
          // is necessary to acquire the package lock
          acquirepackagelock = TRUE;
          packagename = varsharedpackage->PathName;
        }
      else
        {
          // Calls to the shared package will always return without interruption
          // from other subsystems, so it is not possible for two subsystems
          // to be running within the package simultaneously. Package lock
          // is not required.
          acquirepackagelock = FALSE;
          packagename = NULL;
        }
    }
  else
    {
      // Not calling to a different shared package
      acquirepackagelock = FALSE;
      packagename = NULL;
    }
  if (shareclause == NULL)
    {
      hasshareclause = FALSE;
    }
  else if (shareclause->Expr == NULL)
    {
      hasshareclause = FALSE;
    }
  else
    {
      hasshareclause = TRUE;
    }
  if ((numlocks != 0) || acquirepackagelock || hasshareclause)
    {
      CheckTaskImplementation (filename, line);
      if (!CyclicTaskModel())
        {
          if (CurrentProc != NULL)
            {
              if (CurrentProc->NoLocks)
                {
                  err(filename, line, "Locks are not allowed here due to 'no_locks' clause");
                }
              CurrentProc->Dispatches = TRUE;
            }
          emit (depth, "__acquire_locks(");
          printlocks (locklist, numlocks, acquirepackagelock, packagename);
          emit (0, ");\n");
        }
        // Under the cyclic tasking models locks on variables are not required, because
        // any called routine must return before another subsystem can run
      if (hasshareclause)
        {
          if (CyclicTaskModel() && (!WithResume) && (!InUnitTestCode ()))
            {
              err (filename, line,
                   "Under cyclic tasking only main procedures can call guarded routines");
            }
          emit (depth, "while (!");
          printcall (GATE_PREFIX, 0, filename, line, p);
          emit (0, ") {\n");
          if (UnitTest)
            {
              emit (depth,
                    "    printf(\"[%s:%d] Guard expressions must not be allowed to fail under unit test builds\\n\");\n",
                    filename, line);
              emit (depth, "    exit(1);\n");
            }
          else
            {
              if (CyclicTaskModel())
                {
                  emitcyclicdispatch (depth + 1);
                }
              else
                {
                  emit (depth + 1, "__reacquire_locks(");
                  printlocks (locklist, numlocks, acquirepackagelock,
                              packagename);
                  emit (0, ");\n");
                }
            }
          emit (depth, "}\n");
        }
    }
  haveaux = FALSE;
  a = p->ActualList;
  while (a != NULL && !haveaux)
    {
      if (a->AuxType != NULL)
        haveaux = TRUE;
      a = a->Next;
    }
  if (haveaux)
    {
      emit (depth, "{\n");
      a = p->ActualList;
      while (a != NULL)
        {
          if (a->AuxType != NULL)
            {
              emit (depth + 1, "%s __%x;\n", a->AuxType->CName, a);
            }
          a = a->Next;
        }
    }
  printcall ("", depth, filename, line, p);
  if (trailingsemi)
    emit (0, ";\n");
  if (haveaux)
    {
      a = p->ActualList;
      while (a != NULL)
        {
          if (a->AuxType != NULL)
            {
              emit (depth, "");
              bf = FALSE;
              printexpr (a->Expr, &bf, TRUE);   // Will only be a reference because it will be out or in out
              emit (0, " = (%s)__%x;\n", a->ActualType->CName, a);
            }
          a = a->Next;
        }
      emit (depth, "}");
    }
  if ((numlocks != 0) || acquirepackagelock)
    {
      if (!CyclicTaskModel())
        {
          emit (depth, "__free_locks(");
          printlocks (locklist, numlocks, acquirepackagelock, packagename);
          emit (0, ");\n");
        }
    }
}

bool
IsUnit (struct tNode *p)
{
  struct tReference *ref;
  struct tVar *v;

  if (p->Op == _VAR)
    {
      ref = p->Var;
      v = FindVariable (p->FileName, p->Line, &ref, 500, FALSE, TRUE);
      if (v->Type->TypeSpec->BaseType == UNIT_TYPE)
        {
          return TRUE;
        }
    }
  return FALSE;
}

bool
PreferCast (struct tNode * ExprTo, struct tNode * SubExpr)
{
  if (SubExpr->Op == _CONST_INT || SubExpr->Op == _CONST_FLOAT)
    {
      return FALSE;
    }
  if (ExprTo->ComputationType == NULL || SubExpr->ComputationType == NULL)
    {
      return FALSE;
    }
  if (!(SubExpr->ComputationType->UseForArithmetic))
    {
      return TRUE;
    }
  if (ExprTo->ComputationType == SubExpr->ComputationType)
    {
      return FALSE;
    }
  return TRUE;
}

/* Write out an expression tree */
void
printexpr (struct tNode *p, bool * bitfield, bool outer)
{
  char *filename = p->FileName;
  int line = p->Line;

  if (p == NULL)
    return;
  if (p->Op == _CONST_INT)
    {
      printintegerconstant (p->Value, NULL, NULL, NULL);
    }
  else if (p->Op == _CONST_FLOAT)
    {
      emit (0, "%lf", p->FloatValue);
    }
  else if (p->Op == _CONST_BOOL)
    {
      if (p->Value != FALSE)
        {
          emitstr ("1");
          if (Annotate)
            emitstr (" /*true*/");
        }
      else
        {
          emitstr ("0");
          if (Annotate)
            emitstr (" /*false*/");
        }
    }
  else if (p->Op == _CONST_STRING)
    {
      emit (0, "%s", p->String);
    }
  else if (p->Op == _CONST_CHARACTER)
    {
      emit (0, "%s", p->String);
    }
  else if (p->Op == _CONST_NULL)
    {
      emit (0, "NULL");
    }
  else if (p->Op == _VAR)
    {
      printreference (filename, line, p->Var, p->Attribute, bitfield, FALSE);
    }
  else if (p->Op == _FN_CALL)
    {
      printprocedurecall (0, filename, line, p->Call->Ptr, NULL, FALSE);
    }
  else
    {
      if (p->Op == _NOT)
        {
          emit (0, "(!");
          printexpr (p->Left, bitfield, FALSE);
          emit (0, ")");
        }
      else
        {
          enum tAttribute attr = GetExprAttribute (p->Right);
          if ((p->Op == _DIVIDE || p->Op == _TIMES)
              && ((p->Right->Op == _CONST_INT && p->Right->Value == 1)
                  || (p->Right->Op == _CONST_FLOAT
                      && p->Right->FloatValue == 1.0) || (p->Right->Op == _VAR
                                                          && (attr ==
                                                              ATTR_UNIT
                                                              || attr ==
                                                              ATTR_INDEX_UNIT))
                  || IsUnit (p->Right)))
            {
              // Special case of multiplication or division by 1 (probably for unit conversion)
              // Just emit the LHS
              printexpr (p->Left, bitfield, FALSE);
            }
          else
            {
              if (!outer)
                emit (0, "(");
              if (PreferCast (p, p->Left))
                {
                  emit (0, "(%s)", p->ComputationType->CName);
                }
              printexpr (p->Left, bitfield, FALSE);
              emit (0, " %s ", COpString (p->Op));
              if (PreferCast (p, p->Right))
                {
                  emit (0, "(%s)", p->ComputationType->CName);
                }
              printexpr (p->Right, bitfield, FALSE);
              if (!outer)
                emit (0, ")");
            }
        }
    }
}

void
printcode (int depth, char *codestr)
{
  emit (depth, "%s\n", codestr);
}

//----------

void TestIntegerExpression (struct tNode *expr, BIGINT * min, BIGINT * max,
                            struct tUnitDef **unit, bool parameter,
                            bool rangefor, bool * virtualexpression);
void TestAssignment (struct tReference *lhsref, struct tVar *v,
                     struct tTypeSpec *reftype, struct tNode *expr,
                     bool parameter, bool makenew, bool allowcstringparameter,
                     BIGINT * minval, BIGINT * maxval);
void AnalyseAssignment (int depth, char *filename, int line,
                        struct tTypeSpec *reftype, struct tRHS *rhs,
                        struct tReference *basename, bool baseinitialised,
                        enum tBaseType basebasetype, bool * finalisedthis,
                        bool makenew, bool fieldassignment,
                        bool initialisingwholeobject);
void AnalyseAddConstant (int depth, char *filename, int line,
                         struct tConstant *constdecl,
                         enum tAccess nameaccess);
struct tVar *AnalyseMakeConstant (int depth, char *filename, int line,
                                  char *name, struct tNode *expression,
                                  struct tUnitDef *unit,
                                  enum tAccess nameaccess,
                                  struct tRepresentationClause
                                  *representation, bool allowcondition,
                                  bool startofgroup, bool endofgroup);
struct tVar *AnalyseVarDeclaration (int depth, char *filename, int line,
                                    struct tVariableDeclaration *v,
                                    enum tAccess access);

void DescribeLocals (int depth, char *filename, int line);

bool
CheckSubsystemBoundary (struct tVar * v)
{
  if (v->Type->TypeSpec->BaseType == PACKAGE_TYPE)
    {
      if (v->Type->TypeSpec->PackageSpec->PackageType == PACKAGE_SUBSYSTEM)
        {
          // Crossing a subsystem boundary into or out of a subsystem
          return TRUE;
        }
    }
  return FALSE;
}

//
// Follow possible whole object references to their destination
//

struct tVar *TraceWholeReferents(struct tVar *var)
{
  struct tVar *v;

  v = var;
  while (v->Referent != NULL && v->WholeObjectReference)
    {
      v = v->Referent;
    }
  return v;
}

struct tVar *TraceAllReferents(struct tVar *var)
{
  struct tVar *v;

  v = var;
  while (v->Referent != NULL)
    {
      v = v->Referent;
    }
  return v;
}

struct tVar *
SearchForVariable (char *filename, int line, struct tReference **name, int from,
                   bool currentscopeonly, bool errorifnotfound, bool typeorattributesearch,
                   bool *subsystemboundary)
{
  int j;
  struct tReference *r, *matchr, *varref;
  bool found, packagesearch;
  struct tVar *v, *matchv, **vp;

  v = NULL;                     // Avoid a warning - actually OK
  r = *name;
  varref = r;
  packagesearch = FALSE;
  *subsystemboundary = FALSE;
  matchr = NULL;
  while (TRUE)
    {
      found = FALSE;
      if (packagesearch)
        {
          while (v != NULL)
            {
              if (!strcmp (v->Name, r->VarName))
                {
                  found = TRUE;
                  break;
                }
              v = v->Next;
            }
        }
      else
        {
          vp = &CurrentVars->Vars[CurrentVars->NumVars];
          for (j = CurrentVars->NumVars; j > 0; j--)
            {
              vp--;
              v = *vp;
              if (!strcmp (v->Name, r->VarName))
                {
                  found = TRUE;
                  break;
                }
              else if (v->Referent == NULL)
                {
                  // Check for crossing back out of a subsystem that is currently being processed
                  *subsystemboundary = *subsystemboundary
                    || (CheckSubsystemBoundary (v)
                        && (!v->Type->TypeSpec->PackageClosed));
                }
            }
        }
      if (!found)
        break;
      if (v->SubsystemBoundary)
        {
          // Reference through a 'renames' whose base variable crosses a boundary
          *subsystemboundary = TRUE;
        }
      v = TraceWholeReferents(v);
      matchr = r;
      matchv = v;
      if (currentscopeonly)
        break;
      r = r->Next;
      if (r == NULL)
        break;
      if (r->ReferenceType != FIELD_REFERENCE)
        break;
      if (v->Type->TypeSpec->BaseType == PACKAGE_TYPE)
        {
          // Check for crossing into a subsystem
          *subsystemboundary = *subsystemboundary || CheckSubsystemBoundary (v);
          v = v->Type->TypeSpec->PackageVars;
          packagesearch = TRUE;
          matchr = NULL;
        }
      else
        break;
    }
  if (matchr != NULL)
    {
      if (matchv->Hidden)
        {
          if (errorifnotfound)
            {
              err(filename, line, "Symbol %s is hidden here due to pragma hide", matchv->Name);
              return &DummyVar;
            }
          else
            {
              return NULL;
            }
        }
      if (*subsystemboundary)
        {
          struct tTypeSpec *t = matchv->Type->TypeSpec;
          if (IsVariable (matchv))
            {
              if ((!typeorattributesearch) && (!NamedConstantType (t)) && (matchv->ConstValue == NULL))
                {
                  if (matchv->Mode != MODE_SHARED
                      && (!CurrentExemptions.SubsystemAccess)
                      && (!InUnitTestCode ())
                      && (!matchv->AccessSharedAtomic)
                      && (!(CurrentExemptions.SharedAtomicAccess
                            && (t->BaseType == INTEGER_TYPE
                                || t->BaseType == BOOLEAN_TYPE)
                            && (t->CType->AtomicType))))
                    {
                      err (filename, line,
                           "Access to non-shared non-constant variable %s across a subsystem boundary",
                           matchv->Name);
                    }
                }
            }
          else if (t->BaseType == PROCEDURE_TYPE)
            {
              // if it's not a shared procedure or function, and it's not a closed function, and
              // no subsystem access exemption applies, then the call is in error
              if ((!t->PrefixShared)
                  && (!(t->ReturnType != NULL && t->PrefixClosed))
                  && (!LoopScan)
                  && (!CurrentExemptions.SubsystemAccess)
                  && (!InUnitTestCode ()))
                {
                  err (filename, line,
                       "Only shared routines and closed functions can be accessed across a subsystem boundary");
                }
            }
        }
      *name = matchr;
      return matchv;
    }
  if (errorifnotfound)
    {
      errstart (filename, line);
      errcont ("Not found (%d,%d): '", from, currentscopeonly);
      errwritevariable (varref);
      errcont ("'");
      errend ();
      return &DummyVar;
    }
  else
    return NULL;
}

struct tVar *
FindVariable (char *filename, int line, struct tReference **name, int from,
              bool currentscopeonly, bool errorifnotfound)
{
  bool subsystemboundary;

  return SearchForVariable(filename, line, name, from, currentscopeonly, errorifnotfound, FALSE, &subsystemboundary);
}

struct tType *
FindType (char *filename, int line, struct tTypeIdentifier *name)
{
  struct tReference *r;
  struct tType *typefound;
  struct tTypeSpec *reftype;
  struct tVar *basevar;

  r = name->Name;
  {
    BIGINT minvalue, maxvalue, initialmin, initialmax;
    enum tBaseType basetype;
    bool initialised;
    struct tUnitDef *unit;
    struct tTypeSpec *accessedtype;
    enum tMode mode;
    int lock;
    bool wholeobject, constantaccess, subsystemboundary;

    AnalyseReference (filename, line, r, &basetype, &basevar, &reftype,
                      &minvalue, &maxvalue, &initialmin, &initialmax, &unit,
                      &accessedtype, &initialised, &mode, &lock, FALSE,
                      &wholeobject, TRUE, &constantaccess, TRUE, &subsystemboundary);
  }

  if (name->Attribute != NULL)
    {
      struct tTypeSpec *vtype = reftype;
      struct tAttributeChainEntry *attr = name->Attribute;

      TraceTypeChain (filename, line, &attr, &vtype);
      if (attr != NULL)
        {
          err (filename, line, "Type reference expected");
        }
      if (vtype->BaseType == TYPE_TYPE)
        {
          typefound = vtype->Type;
        }
      else
        {
          typefound = vtype->ThisType->TypeSpec->Type;
        }
    }
  else if (basevar->Type->TypeSpec->BaseType != TYPE_TYPE)
    {
      typefound = NULL;
    }
  else
    {
      if (reftype->ThisType == NULL)
        {
          // Deferred: type t;, or not a type
          typefound = reftype->Type;
        }
      else
        {
          if (reftype->ThisType->TypeSpec->BaseType != TYPE_TYPE)
            {
              typefound = NULL;
            }
          else
            {
              // Type declaration to type definition
              typefound = reftype->ThisType->TypeSpec->Type;
            }
        }
    }
  //
  if (typefound == NULL)
    {
      // Not a type
      err (filename, line, "Not a type: %s", basevar->Name);
    }
  else
    {
      if (typefound->TypeSpec == BooleanTypespec
          || typefound->TypeSpec == CharacterTypespec
          || typefound->TypeSpec == UniversalIndexTypespec)
        {
          if (typefound->TypeSpec->CType == NULL)
            {
              err (filename, line,
                   "The target type for '%s' has not been specified",
                   typefound->TypeSpec->Name);
            }
        }
    }
  return typefound;
}

// lhs and rhs are variable IDs
struct tRelation *
FindRelation (struct tVar *lhs, enum tAttribute lhsattr, struct tVar *rhs,
              enum tAttribute rhsattr)
{
  struct tRelation *r;
  unsigned n;
  struct tVar *lhsvar, *rhsvar;

  if (CurrentVars->NumRelations == 0)
    return NULL;
  lhsvar = TraceWholeReferents(lhs);
  rhsvar = TraceWholeReferents(rhs);
  r = &CurrentVars->Relations[CurrentVars->NumRelations];
  for (n = 0; n < CurrentVars->NumRelations; n++)
    {
      r--;
      if ((r->LowSet || r->HighSet) && r->LHS == lhsvar && r->RHS == rhsvar
          && r->LHSAttr == lhsattr && r->RHSAttr == rhsattr)
        {
          return r;
        }
    }
  return NULL;
}

void
StoreRelation (char *filename, int line, struct tVar *lhsvar,
               enum tAttribute lhsattr, struct tVar *rhsvar,
               enum tAttribute rhsattr, enum tOp op, BIGINT minoffset,
               BIGINT maxoffset, bool * relationchanged)
{
  struct tRelation *r;
  bool change;
  enum tOp testop;
  BIGINT tmp;
  int j;

  if ((lhsvar->AccessVolatile && lhsattr == ATTR_NULL) || (rhsvar->AccessVolatile && rhsattr == ATTR_NULL))
    {
      return;
    }
  change = TRUE;
  r = FindRelation (lhsvar, lhsattr, rhsvar, rhsattr);
  if (r != NULL)
    {
      testop = op;
    }
  else
    {
      r = FindRelation (rhsvar, rhsattr, lhsvar, lhsattr);
      if (r != NULL)
        {
          // Reverse A > B + n => B < A - n etc.
          switch (op)
            {
            case _LT:
              testop = _GT;
              break;
            case _GT:
              testop = _LT;
              break;
            case _LEQ:
              testop = _GEQ;
              break;
            case _GEQ:
              testop = _LEQ;
              break;
            default:
              testop = op;
              break;            // _EQ, _NEQ
            }
          tmp = -maxoffset;
          maxoffset = -minoffset;
          minoffset = tmp;
        }
	  else
		{
		  testop = op;
		}
    }
  if (r == NULL)
    {
      // Search for an empty slot
      for (j = 0; j < CurrentVars->NumRelations; j++)
        {
          if (!
              (CurrentVars->Relations[j].LowSet
               || CurrentVars->Relations[j].HighSet))
            {
              r = &CurrentVars->Relations[j];
              break;
            }
        }
      if (r == NULL)
        {
          // Allocate a new slot
          if (CurrentVars->NumRelations >= MAX_RELATIONS)
            {
              err (filename, line, "Overflow of relations table");
            }
          r = &CurrentVars->Relations[CurrentVars->NumRelations];
          CurrentVars->NumRelations++;
        }
      r->LHS = lhsvar;
      r->LHSAttr = lhsattr;
      r->RHS = rhsvar;
      r->RHSAttr = rhsattr;
      r->LowSet = FALSE;
      r->HighSet = FALSE;
    }
  switch (testop)    //1
    {
    case _LT:
      if (!r->HighSet)
        {
          r->High = maxoffset - 1;
          r->HighSet = TRUE;
        }
      else if (r->High > maxoffset - 1)
        r->High = maxoffset - 1;
      else
        change = FALSE;
      break;
    case _GT:
      if (!r->LowSet)
        {
          r->Low = minoffset + 1;
          r->LowSet = TRUE;
        }
      else if (r->Low < minoffset + 1)
        r->Low = minoffset + 1;
      else
        change = FALSE;
      break;
    case _LEQ:
      if (!r->HighSet)
        {
          r->High = maxoffset;
          r->HighSet = TRUE;
        }
      else if (r->High > maxoffset)
        r->High = maxoffset;
      else
        change = FALSE;
      break;
    case _GEQ:
      if (!r->LowSet)
        {
          r->Low = minoffset;
          r->LowSet = TRUE;
        }
      else if (r->Low < minoffset)
        r->Low = minoffset;
      else
        change = FALSE;
      break;
    case _EQ:
      if ((!r->LowSet) || (!r->HighSet) || r->Low != minoffset
          || r->High != maxoffset)
        {
          r->Low = minoffset;
          r->High = maxoffset;
          r->LowSet = TRUE;
          r->HighSet = TRUE;
        }
      else
        change = FALSE;
      break;
    case _NEQ:
      if (minoffset == maxoffset)
        {
          if (r->LowSet)
            {
              if (r->Low == minoffset)
                r->Low = minoffset + 1;
              else
                change = FALSE;
            }
          if (r->HighSet)
            {
              if (r->High == minoffset)
                r->High = minoffset - 1;
              else
                change = FALSE;
            }
        }
      break;
    default:
      err (filename, line, "Internal error 4");
    }
  *relationchanged = change;
}

void
UpdateRight (char *filename, int line, struct tVar *leftvar,
             enum tAttribute leftattr, enum tOp op, struct tNode *rightleft,
             enum tOp rightop, struct tNode *rightright,
             struct tVar **rightvar, bool * resetrelations, bool * relchanged)
{
  BIGINT minvalue, maxvalue, tmp;
  struct tUnitDef *unit;
  int j;
  struct tRelation *rel;
  enum tAttribute rightattr;

  if (SimpleVar (rightleft, rightvar, &rightattr, FALSE))
    {
      enum tAttribute rightleftattr = GetExprAttribute (rightleft);

      //
      // leftvar op var +- expr
      //**********HANDLE expr +- op too
      //
      // For assignments, op is _EQ
      // a := a + c doesn't imply that a - a = c, but a := b + c does imply that a - b = c
      //
      bool virtualexpr;
      TestIntegerExpression (rightright, &minvalue, &maxvalue, &unit, FALSE,
                             FALSE, &virtualexpr);
      // Normalise to +
      if (rightop == _MINUS)
        {
          tmp = -maxvalue;
          maxvalue = -minvalue;
          minvalue = tmp;
        }
      //
      if ((*rightvar) == leftvar)
        {
          bool initialrelationfound;

          initialrelationfound = FALSE;
          // Relative change to leftvar
          // Update all relations of leftvar
          for (j = 0; j < CurrentVars->NumRelations; j++)
            {
              rel = &CurrentVars->Relations[j];
              // !(LowSet || HighSet) means an empty slot
              if (rel->LowSet || rel->HighSet)
                {
                  if (rel->LHS == leftvar)
                    {
                      // leftvar - rel->RHS in [{rel->Low}..{rel->High}]
                      if (rel->LowSet)
                        {
                          rel->Low = rel->Low + minvalue;
                        }
                      if (rel->HighSet)
                        {
                          rel->High = rel->High + maxvalue;
                        }
                    }
                  else if (rel->RHS == leftvar)
                    {
                      // rel->LHS - leftvar in [{rel->Low}..{rel->High}]
                      if (rel->LowSet)
                        {
                          rel->Low = rel->Low - maxvalue;
                        }
                      if (rel->HighSet)
                        {
                          rel->High = rel->High - minvalue;
                        }
                    }
                  if (rel->LHS == leftvar && rel->RHS == leftvar
                      && (rel->LHSAttr == ATTR_INITIAL
                          || rel->RHSAttr == ATTR_INITIAL))
                    {
                      initialrelationfound = TRUE;
                    }
                }
            }
          if (leftattr == ATTR_NULL && rightleftattr == ATTR_NULL
              && (!initialrelationfound))
            {
              // Record relation to leftvar'initial if one doesn't already exist (in which case it would have
              // been updated above)
              StoreRelation (filename, line, leftvar, ATTR_NULL, leftvar,
                             ATTR_INITIAL, op, minvalue, maxvalue,
                             relchanged);
            }
          // In this case don't reset all relations
          *resetrelations = FALSE;
        }
      else
        {
          StoreRelation (filename, line, leftvar, leftattr, *rightvar,
                         rightleftattr, op, minvalue, maxvalue, relchanged);
        }
    }
}

// Store or update the relation for leftvar op rightexpr
// If Assignment is true, then check for a relative update of leftvar; if so, then update
// relations that involve leftvar, otherwise delete them
void
UpdateRelations (char *filename, int line, struct tVar *leftvar,
                 enum tAttribute leftattr, enum tOp op,
                 struct tNode *rightexpr, bool assignment)
{
  enum tBaseType basetype = leftvar->Type->TypeSpec->BaseType;
  struct tVar *rightvar;
  bool relchanged;
  struct tVar *rightID;
  struct tRelation *rel;
  int j;
  bool resetrelations;
  enum tAttribute rightattr;

  if (basetype == INTEGER_TYPE
      || (basetype == ARRAY_TYPE && leftattr != ATTR_NULL))
    {
      // Usually reset all relations of leftvar after an assignment to leftvar (as opposed to an if condition etc.)
      resetrelations = assignment;
      // Initialise rightvar to NULL because it might not be used but needs to be checked below
      rightvar = NULL;
      //
      // Store or update the relation implied by the expression
      //
      if (SimpleVar (rightexpr, &rightvar, &rightattr, TRUE))
        {
          //
          // leftvar op var
          //
          // for assignments leftvar := var, op is _EQ
          // for a := a we don't need to record that a - a = 0
          //
          enum tAttribute rightexprattr = GetExprAttribute (rightexpr);
          if (rightvar != leftvar || rightexprattr != leftattr)
            {
              StoreRelation (filename, line, leftvar, leftattr, rightvar,
                             rightexprattr, op, 0, 0, &relchanged);
            }
        }
      else if (rightexpr->Op == _PLUS || rightexpr->Op == _MINUS)
        {
          // Update a op b + c and a op c + b, or a op b - c
          UpdateRight (filename, line, leftvar, leftattr, op, rightexpr->Left,
                       rightexpr->Op, rightexpr->Right, &rightvar,
                       &resetrelations, &relchanged);
          if (rightexpr->Op == _PLUS)
            {
              UpdateRight (filename, line, leftvar, leftattr, op,
                           rightexpr->Right, rightexpr->Op, rightexpr->Left,
                           &rightvar, &resetrelations, &relchanged);
            }
        }
      if (resetrelations)
        {
          // Reset relations of leftvar except those involving rightvar
          if (rightvar == NULL)
            rightID = NULL;
          else
            rightID = rightvar;
          for (j = 0; j < CurrentVars->NumRelations; j++)
            {
              rel = &CurrentVars->Relations[j];
              if (rel->LowSet || rel->HighSet)
                {
                  if ((rel->LHS == leftvar && rel->RHS != rightID)
                      || (rel->RHS == leftvar && rel->LHS != rightID))
                    {
                      rel->LowSet = FALSE;
                      rel->HighSet = FALSE;
                    }
                }
            }
        }
    }
}

void
ResetRelations (struct tVar *v)
{
  struct tRelation *r;
  int j;

  for (j = 0; j < CurrentVars->NumRelations; j++)
    {
      r = &CurrentVars->Relations[j];
      if (r->LHS == v || r->RHS == v)
        {
          r->LowSet = FALSE;
          r->HighSet = FALSE;
        }
    }

}

struct tVar *
CopyVar (struct tVar *src)
{
  struct tVar *p = checked_malloc (sizeof *p);       // NOT mallocvar! (e.g. preserve ID)

  TotalVarMallocs++;
  *p = *src;
  return p;
}

void
CopyVarSet (struct tVars *src, struct tVars *dest)
{
  *dest = *src;
}

struct tDynamicListValue
{
  struct tVar *var;
  bool Initialised;             // initialised yet?
  bool Finalised;               // finalised?
  bool Accessed;                // Read since the last time it was updated?
  bool EverAccessed;            // Read at any point?
  BIGINT Low, High;             // ranges for integer types
  struct tDynamicListValue *Next;
};

struct tDynamicValues
{
  struct tDynamicListValue *ValueList;
  int NumRelations;
  struct tRelation Relations[MAX_RELATIONS];
};

// Copy the ranges of variable types from CurrentVars and from AllPublics to a new list

void
StoreVarValue (struct tVar *v, struct tDynamicListValue **head,
               struct tDynamicListValue **tail)
{
  if (IsVariable (v))
    {
      struct tDynamicListValue *p;

      p = checked_malloc (sizeof *p);
      p->var = v;
      p->Initialised = v->Initialised;
      p->Finalised = v->Finalised;
      p->Accessed = v->Accessed;
      p->EverAccessed = v->EverAccessed;
      p->Low = v->Low;
      p->High = v->High;
      if (*head == NULL)
        {
          *head = p;
        }
      else
        {
          (*tail)->Next = p;
        }
      *tail = p;
    }
}

struct tDynamicValues *
StoreDynamicValues ()
{
  struct tDynamicValues *newvalues;
  int j;
  struct tVar *v;
  struct tDynamicListValue *head, *tail;

  head = NULL;
  j = 0;
  while (j < CurrentVars->NumVars)
    {
      StoreVarValue (CurrentVars->Vars[j], &head, &tail);
      j++;
    }
  v = PublicVarList;
  while (v != NULL)
    {
      StoreVarValue (v, &head, &tail);
      v = v->PrivatePublicNext;
    }
  if (head != NULL)
    {
      tail->Next = NULL;
    }
  newvalues = checked_malloc (sizeof *newvalues);
  newvalues->ValueList = head;
  newvalues->NumRelations = CurrentVars->NumRelations;
  for (j = 0; j < CurrentVars->NumRelations; j++)
    {
      newvalues->Relations[j] = CurrentVars->Relations[j];
    }
  return newvalues;
}

// Copy the dynamic values to CurrentVars
void
RestoreDynamicValues (struct tDynamicValues *values)
{
  struct tDynamicListValue *p;
  struct tVar *v;
  int j;

  p = values->ValueList;
  while (p != NULL)
    {
      v = p->var;
      v->Initialised = p->Initialised;
      v->Finalised = p->Finalised;
      v->Accessed = p->Accessed;
      v->EverAccessed = p->EverAccessed;
      v->Low = p->Low;
      v->High = p->High;
      p = p->Next;
    }
  CurrentVars->NumRelations = values->NumRelations;
  for (j = 0; j < values->NumRelations; j++)
    {
      CurrentVars->Relations[j] = values->Relations[j];
    }
}

// Copy a dynamic values list
struct tDynamicValues *
CopyDynamicValues (struct tDynamicValues *src)
{
  struct tDynamicValues *newvalues;
  struct tDynamicListValue *p, *head, *tail, *srcptr;
  int j;

  head = NULL;
  tail = NULL;                  // Avoid spurious warning
  srcptr = src->ValueList;
  while (srcptr != NULL)
    {
      p = checked_malloc (sizeof *p);
      *p = *srcptr;
      if (head == NULL)
        {
          head = p;
        }
      else
        {
          tail->Next = p;
        }
      tail = p;
      srcptr = srcptr->Next;
    }
  if (head != NULL)
    {
      tail->Next = NULL;
    }
  newvalues = checked_malloc (sizeof *newvalues);
  newvalues->ValueList = head;
  newvalues->NumRelations = src->NumRelations;
  for (j = 0; j < src->NumRelations; j++)
    {
      newvalues->Relations[j] = src->Relations[j];
    }
  return newvalues;
}

// Free a set of dynamic values
void
FreeDynamicValues (struct tDynamicValues *values)
{
  struct tDynamicListValue *p, *next;

  if (values != NULL)
    {
      p = values->ValueList;
      while (p != NULL)
        {
          next = p->Next;
          free (p);
          p = next;
        }
      free (values);
    }
}

void
CheckAccessed (char *filename, int line, struct tVar *v)
{
  if (v->Referent == NULL && (!v->AccessMappedDevice)
      && (!v->TestMappedDevice) && (v->Mode != MODE_SHARED)
      && (!(v->AccessMappedDevice || v->AccessSharedAtomic)))
    {
      if (v->Initialised && (!v->Accessed) && (v->ConstValue == NULL)
          && (!InUnitTestCode ()))
        {
          err_and_continue (filename, line,
                            "Value of variable %s has not been used since being updated",
                            v->Name);
        }
    }
}

void
SetInitialised (struct tVar *v)
{
  struct tVar *base = TraceWholeReferents(v);

  base->Initialised = TRUE;
  if ((base->Mode == MODE_GLOBAL || base->Mode == MODE_SHARED)
      && InPackageInitialisation)
    {
      // Record that the variable was initialised globally
      base->InitialisedGlobally = TRUE;
    }
  base->Accessed = FALSE;
  base->Finalised = FALSE;
}

void
SetFinalised (struct tVar *v)
{
  struct tVar *base = TraceWholeReferents(v);

  base->Finalised = TRUE;
  base->Initialised = FALSE;
}

void
SetAccessed (struct tVar *v)
{
  struct tVar *base = TraceAllReferents(v);

  base->Accessed = TRUE;
  if (base->Mode != MODE_GLOBAL && base->Mode != MODE_SHARED)
    {
      base->EverAccessed = TRUE;
    }
}

struct tGlobalAccess **
GetGlobalsAccess (void)
{
  if (CurrentProc == NULL)
    {
      if (BuildingUnitTest)
        return &UnitTestSectionGlobals;
      else
        return &InitialisationGlobals;
    }
  else
    return &CurrentProc->GlobalsAccess;
}

// Record that the current procedure being defined accesses var (or in read and write), but
// only if var is global
void
DoRecordGlobalAccess (int line, struct tGlobalAccess **List, struct tVar *var,
                      bool read, bool write, bool requireinitialisation,
                      bool initialised, bool localaccess, bool inheritedaccess, bool lock)
{
  struct tGlobalAccess *p;
  struct tVar *gvar;

  gvar = TraceAllReferents(var);
  if ((IsVariable (gvar)
       && (gvar->Mode == MODE_GLOBAL || gvar->Mode == MODE_SHARED))
      || (gvar->Type->TypeSpec->BaseType == PACKAGE_TYPE
          && gvar->Mode == MODE_SHARED))
    {                           // for shared package locking
      p = *List;
      while (p != NULL)
        {
          if (p->Var == gvar)
            {
              p->Read |= read;
              p->Write |= write;
              p->RequireInitialisation |= requireinitialisation;
              p->Initialised |= initialised;
              p->LocalAccess |= localaccess;
              p->InheritedAccess |= inheritedaccess;
              p->Lock |= lock;
              return;
            }
          p = p->Next;
        }
      p = checked_malloc (sizeof *p);
      p->Line = line;
      p->Var = gvar;
      p->Read = read;
      p->Write = write;
      p->RequireInitialisation = requireinitialisation;
      p->Initialised = initialised;
      p->LocalAccess = localaccess;
      p->InheritedAccess = inheritedaccess;
      p->Lock = lock;
      p->Next = *List;
      *List = p;
    }
}

void
RecordGlobalAccess (int line, struct tGlobalAccess **List, struct tVar *var,
                    bool read, bool write, bool requireinitialisation,
                    bool initialised)
{
  DoRecordGlobalAccess(line, List, var, read, write, requireinitialisation, initialised, TRUE, FALSE, FALSE);
}

void
RecordInheritedGlobalAccess (int line, struct tGlobalAccess **List, struct tVar *var,
                             bool read, bool write, bool requireinitialisation,
                             bool initialised)
{
  DoRecordGlobalAccess(line, List, var, read, write, requireinitialisation, initialised, FALSE, TRUE, FALSE);
}

void
RecordNeutralGlobalAccess (int line, struct tGlobalAccess **List, struct tVar *var,
                           bool read, bool write, bool requireinitialisation,
                           bool initialised)
{
  DoRecordGlobalAccess(line, List, var, read, write, requireinitialisation, initialised, FALSE, FALSE, FALSE);
}

void
RecordLock(int line, struct tGlobalAccess **List, struct tVar *var)
{
  DoRecordGlobalAccess(line, List, var, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE);
}

void
SetInitialRanges (struct tVar *v)
{
  struct tType *t = v->Type;

  v->Low = t->TypeSpec->Low;
  v->High = t->TypeSpec->High;
  v->InitialLow = v->Low;
  v->InitialHigh = v->High;
  if (t->TypeSpec->BaseType == ARRAY_TYPE)
    {
      v->MinDefinite = t->TypeSpec->MinDefinite;
      v->MaxDefinite = t->TypeSpec->MaxDefinite;
      if (t->TypeSpec->MinDefinite)
        {
          v->FirstLow = t->TypeSpec->Low;
          v->FirstHigh = t->TypeSpec->Low;
        }
      else
        {
          v->FirstLow = t->TypeSpec->Low;
          v->FirstHigh = t->TypeSpec->High;
        }
      if (t->TypeSpec->MaxDefinite)
        {
          v->LastLow = t->TypeSpec->High;
          v->LastHigh = t->TypeSpec->High;
        }
      else
        {
          v->LastLow = t->TypeSpec->Low;
          v->LastHigh = t->TypeSpec->High;
        }
    }
}

void
CheckForDuplicateOrCaseDifference (char *filename, int line, char *newname)
{
  int j;
  struct tVar **v;

  v = &CurrentVars->Vars[CurrentVars->NumVars];
  for (j = 0; j < CurrentVars->NumVars; j++)
    {
      v--;
      if (!strcasecmp ((*v)->Name, newname))
        {
          if (strcmp ((*v)->Name, newname))
            {
              err (filename, line,
                   "It is illegal for two symbols is the same scope to differ only by case: %s, %s",
                   (*v)->Name, newname);
              return;
            }
          else
            {
              err (filename, line,
                   "Duplicate symbol within the same scope: '%s' (other is at %s:%d)",
                   newname, (*v)->FileName, (*v)->Line);
              return;
            }
        }
    }

}

// Check for a conditional declaration; return TRUE if there is no condition, or the condition is true
bool
CheckBuildCondition (char *filename, int line,
                     struct tRepresentationClause * representation,
                     bool allowconditionhere)
{
  struct tRepresentationClause *rep;

  rep = representation;
  while (rep != NULL)
    {
      if (!strcasecmp (rep->Name, "build_condition"))
        {
          BIGINT cond;
          bool condsuccess;

          if (!allowconditionhere)
            {
              err (filename, line, "Build condition not allowed here");
            }
          cond = EvalStaticBoolean (rep->Value, &condsuccess, TRUE);
          if (condsuccess && (cond == 0))
            {
              // Omit this declaration
              return FALSE;
            }
        }
      rep = rep->Next;
    }
  return TRUE;
}

bool
DuplicateAddressClause (char *filename, int line, bool addressset,
                        char *cname)
{
  if (addressset || (cname != NULL))
    {
      err (filename, line,
           "Only one 'address', 'external' or 'cname' can be given");
      return TRUE;
    }
  else
    return FALSE;
}

void
CheckForVarOverflow (char *filename, int line)
{
  if (CurrentVars->NumVars >= MAX_VARS)
    {                           //CurrentVars->FreePtr - 1) {
      err (filename, line, "Too many variables (max %d)", MAX_VARS);
    }
}

enum tUsage
GetUsage (char *filename, int line, char *name)
{
  enum tUsage usage;

  if (!strcasecmp (name, "library"))
    {
      usage = LIBRARY;
    }
  else if (!strcasecmp (name, "target_library"))
    {
      usage = TARGET_LIBRARY;
    }
  else if (!strcasecmp (name, "required"))
    {
      usage = REQUIRED;
    }
  else
    {
      err (filename, line,
           "Error in 'usage' clause: 'library', 'target_library' or 'required' expected");
      usage = REQUIRED;         // won't be used
    }
  return usage;
}

struct tType *AnalyseAddType (int depth, char *filename, int line,
                              struct tTypeDeclaration *typedecl,
                              enum tAccess nameaccess);

// newtypestructureaccess is used to override access in order to allow export of the structure of
// enumerations that are defined anonymously in formal parameters, so that relevant public routines
// can be called from outside their packages
struct tVarListEntry *
AnalyseAddVar (int depth, char *filename, int line, struct tVariableDeclaration *var, struct tType *type,       // used to override the typename in var
               enum tAccess access, bool allowindefinitearray, enum tAccess newtypestructureaccess)
{
  struct tType *t;
  struct tVar *v;
  bool success;
  struct tRepresentationClause *rep;
  BIGINT addr;
  char *cname = NULL, *targetattribute = NULL, *targetattributeline = NULL;
  struct tUnitDef *unit;
  enum tUsage usage;
  bool literaladdress = FALSE;
  struct tIdentifierListEntry *varlist;
  struct tVarListEntry *vfirst, *vend, *vnew;

  // Check for an anonymous type, e.g. var : range 1..10;

  // Check for a conditional declaration
  if (!CheckBuildCondition (filename, line, var->Representation, TRUE))
    {
      return NULL;
    }
  //
  if (var->ConstValue != NULL && var->VarNames->Next != NULL)
    {
      err(filename, line, "Multi-variable declarations with initialisation expressions are not allowed due to ambiguity");
    }
  //
  if (var->TypeSpec != NULL)
    {
      struct tTypeDeclaration *typedecl =
        checked_malloc (sizeof *typedecl);

      typedecl->Name = MakeFieldName (var->VarNames->SymbolName, "type");    // Can't have a symbol called 'type', so this shouldn't clash with anything
      typedecl->TypeSpec = var->TypeSpec;
      typedecl->TypeSpec->StructureAccess = newtypestructureaccess;
      AnalyseAddType (depth, filename, line, typedecl, PRIVATE_ACCESS);
      var->TypeName =
        MakeTypeIdentifier (MakeReference
                            (IDENTIFIER_REFERENCE, typedecl->Name, NULL,
                             NULL), NULL);
      var->TypeSpec = NULL;
    }
  //
  if (type == NULL)
    t = FindType (filename, line, var->TypeName);
  else
    t = type;
  if (!(t->TypeSpec->MinDefinite && t->TypeSpec->MaxDefinite))
    {
      if (!((t->TypeSpec->BaseType == ARRAY_TYPE) && allowindefinitearray))
        {
          err (filename, line, "Cannot declare variables of indefinite type");
          return NULL;
        }
    }
  if (t->TypeSpec->BaseType == ARRAY_TYPE
      && t->TypeSpec->IndexTypeSpec == UniversalIndexTypespec)
    {
      err (filename, line,
           "Cannot declare array variables with index type universal_index");
    }
  //
  varlist = var->VarNames;
  vend = NULL; vfirst = NULL;
  while (varlist != NULL)
    {
      CheckForDuplicateOrCaseDifference (filename, line, varlist->SymbolName);
      CheckForVarOverflow (filename, line);
      v = MallocVar (filename, line);       //var->FileName, var->Line);
      CurrentVars->Vars[CurrentVars->NumVars] = v;
      // Check representation
      if (var->Mode == MODE_GLOBAL || var->Mode == MODE_SHARED)
        {
          usage = DefaultUsage;
        }
      else
        {
          usage = REQUIRED;
        }
      v->AddressSpecified = FALSE;
      v->AccessVolatile = FALSE;
      v->AccessMappedDevice = FALSE;
      v->TestMappedDevice = FALSE;
      v->AccessImage = FALSE;
      v->ExemptionAliasing = FALSE;
      v->Initialised = FALSE;       // May be set by representation clause below
      v->EverInitialised = FALSE;
      rep = var->Representation;
      while (rep != NULL)
        {
          if (!strcasecmp (rep->Name, "address"))
            {
              if (var->Mode != MODE_GLOBAL && var->Mode != MODE_SHARED)
                {
                  err (filename, line,
                       "'address' can only be specified for a global variable");
                }
              else
                if (!DuplicateAddressClause
                    (filename, line, v->AddressSpecified, cname))
                {
                  char buff[200];

                  if (rep->Value->Op == _CONST_STRING)
                    {
                      sprintf (buff, "%s", DeQuote (rep->Value->String));
                    }
                  else
                    {
                      addr =
                        EvalStaticInteger (rep->Value, &success, TRUE, &unit);
                      if (success)
                        {
                          sprintf (buff, "0x%" PRINTBIGx "", addr);
                        }
                    }
                  cname = strdup (buff);  // This will be used below - see 'literaladdress'
                  v->AddressSpecified = TRUE;
                  literaladdress = TRUE;
                }
            }
          else if (!strcasecmp (rep->Name, "external"))
            {
              if (var->Mode != MODE_GLOBAL && var->Mode != MODE_SHARED)
                {
                  err (filename, line,
                       "'external' can only be specified for a global variable");
                }
              else
                {
                  if (!DuplicateAddressClause
                      (filename, line, v->AddressSpecified, cname))
                    {
                      if (rep->Value->Op == _CONST_STRING)
                        {
                          cname = DeQuote (rep->Value->String);
                        }
                      if (cname == NULL)
                        {
                          err (filename, line,
                               "Syntax error in 'external' clause: expected 'external => \"name\"'");
                        }
                      else
                        {
                          v->AddressSpecified = TRUE;
                          //----------
                          SwitchToHeader ();
                          emit (0, "extern %s %s;\n", t->TypeSpec->CName, cname);
                          SwitchToSource ();
                          //----------
                        }
                    }
                }
            }
          else if (!strcasecmp (rep->Name, "reference"))
            {
              if (var->Mode != MODE_GLOBAL && var->Mode != MODE_SHARED)
                {
                  err (filename, line,
                       "'reference' can only be specified for a global variable");
                }
              else
                {
                  if (!DuplicateAddressClause
                      (filename, line, v->AddressSpecified, cname))
                    {
                      if (rep->Value->Op == _CONST_STRING)
                        {
                          cname = DeQuote (rep->Value->String);
                        }
                      if (cname == NULL)
                        {
                          err (filename, line,
                               "Syntax error in 'reference' clause: expected 'reference => \"entity\"'");
                        }
                      else
                        {
                          v->AddressSpecified = TRUE;
                        }
                    }
                }
            }
          else if (!strcasecmp (rep->Name, "cname"))
            {
              if (!DuplicateAddressClause
                  (filename, line, v->AddressSpecified, cname))
                {
                  if (rep->Value->Op == _CONST_STRING)
                    {
                      cname = DeQuote (rep->Value->String);
                    }
                  if (cname == NULL)
                    {
                      err (filename, line,
                           "Syntax error in 'cname' clause: expected 'cname => \"name\"'");
                    }
                }
            }
          else if (!strcasecmp (rep->Name, "cprefix"))
            {
              if (rep->Value->Op == _CONST_STRING)
                {
                  if (!UnitTest)
                    {
                      targetattribute = DeQuote (rep->Value->String);
                    }
                }
              else
                {
                  err (filename, line,
                       "Syntax error in 'cprefix' clause: expected 'cprefix => \"string\"'");
                }
            }
          else if (!strcasecmp (rep->Name, "cprefix_line"))
            {
              if (rep->Value->Op == _CONST_STRING)
                {
                  if (!UnitTest)
                    {
                      targetattributeline = DeQuote (rep->Value->String);
                    }
                }
              else
                {
                  err (filename, line,
                       "Syntax error in 'cprefix_line' clause: expected 'cprefix_line => \"string\"'");
                }
            }
          else if (!strcasecmp (rep->Name, "access_mode"))
            {
              if (v->AccessImage || v->AccessMappedDevice || v->AccessVirtual
                  || v->AccessVolatile || v->AccessSharedAtomic)
                {
                  err (filename, line, "Only one access_mode be specified");
                }
              if (rep->Value->Op == _VAR)
                {
                  if (!strcasecmp (rep->Value->Var->VarName, "volatile"))
                    {
                      v->AccessVolatile = TRUE;
                    }
                  else if (!strcasecmp (rep->Value->Var->VarName, "image"))
                    {
                      v->AccessImage = TRUE;
                      v->EverInitialised = TRUE;
                      if (var->ConstValue == NULL)
                        {
                          err (filename, line,
                               "Image declarations must have initialisation expressions");
                        }
                    }
                  else
                    if (!strcasecmp (rep->Value->Var->VarName, "mapped_device"))
                    {
                      v->AccessVolatile = TRUE;     // Must be volatile
                      v->AccessMappedDevice = TRUE;
                      v->Initialised = TRUE;
                      v->InitialisedGlobally = TRUE;
                      v->EverInitialised = TRUE;
                      if (var->ConstValue != NULL)
                        {
                          err (filename, line,
                               "Mapped device declarations cannot have initialisation expressions");
                        }
                    }
                  else
                    if (!strcasecmp (rep->Value->Var->VarName, "shared_atomic"))
                    {
                      if (t->TypeSpec->BaseType != INTEGER_TYPE
                          && t->TypeSpec->BaseType != BOOLEAN_TYPE)
                        {
                          err (filename, line,
                               "access mode shared_atomic can only be applied to integer and boolean variables");
                        }
                      if (!t->TypeSpec->CType->AtomicType)
                        {
                          err (filename, line, "Target type '%s' is not atomic",
                               t->TypeSpec->CType->RepresentationName);
                        }
                      v->AccessVolatile = TRUE;     // Must be volatile
                      v->AccessSharedAtomic = TRUE;
                    }
                  else if (!strcasecmp (rep->Value->Var->VarName, "virtual"))
                    {
                      if (t->TypeSpec->BaseType != INTEGER_TYPE
                          && t->TypeSpec->BaseType != BOOLEAN_TYPE)
                        {
                          err (filename, line,
                               "Access_mode virtual can only be applied to integers and booleans");
                        }
                      if (var->ConstValue != NULL)
                        {
                          err (filename, line,
                               "Virtual variables cannot have initialisation expressions");
                        }
                      v->AccessVirtual = TRUE;
                    }
                  else
                    {
                      err (filename, line, "Unknown access mode '%s'",
                           rep->Value->Var->VarName);
                    }
                }
              else
                {
                  err (filename, line, "Syntax error in 'access_mode' clause");
                }
            }
          else if (!strcasecmp (rep->Name, "build_condition"))
            {
              ;                     // Handled above
            }
          else if (!strcasecmp (rep->Name, "exemption"))
            {
              if (rep->Value->Op == _VAR)
                {
                  if (!strcasecmp (rep->Value->Var->VarName, "unchecked_alias"))
                    {
                      if (var->NameFor == NAME_VAR)
                        {
                          v->ExemptionAliasing = TRUE;
                        }
                      else
                        {
                          err (filename, line,
                               "'exemption => unchecked_alias' can only be applied to 'renames' declarations that refer to variables");
                        }
                    }
                  else
                    {
                      err (filename, line, "Unknown exemption '%s'",
                           rep->Value->Var->VarName);
                    }
                }
              else
                {
                  err (filename, line, "Syntax error in 'exemption' clause");
                }
            }
          else if (!strcasecmp (rep->Name, "usage"))
            {
              if (rep->Value->Op == _VAR)
                {
                  usage = GetUsage (filename, line, rep->Value->Var->VarName);
                }
              else
                {
                  err (filename, line,
                       "Syntax error in 'usage' clause: expected a symbol");
                }
            }
          else
            {
              err (filename, line, "Unknown representation for a variable '%s'",
                   rep->Name);
            }
          rep = rep->Next;
        }
      //
      v->Name = varlist->SymbolName;
      v->Type = t;
      v->Finalised = FALSE;
      v->Accessed = FALSE;
      v->EverAccessed = FALSE;
      //
      // For unit test builds, change mapped devices to ordinary variables (but still with attribute volatile)
      // Turn off initialisation exemption, so that unit test code must initialise it.
      // If a variable has a literal address specification, then cancel that and make it an ordinary global.
      if (UnitTest)
        {
          if (v->AccessMappedDevice)
            {
              v->AccessMappedDevice = FALSE;        // but still volatile
              v->TestMappedDevice = TRUE;   // required to avoid access-after-update and global usage checks
              v->Initialised = FALSE;
              v->InitialisedGlobally = FALSE;
            }
          if (v->AddressSpecified && literaladdress)
            {
              v->AddressSpecified = FALSE;
              cname = NULL;
            }
        }
      //
      SetInitialRanges (v);
      v->ConstValue = var->ConstValue;
      if (var->Mode == MODE_IN || var->Mode == MODE_IN_OUT
          || var->Mode == MODE_FINAL_IN_OUT)
        v->Initialised = TRUE;
      v->Access = access;
      v->Mode = var->Mode;
      if (cname != NULL)
        {
          if (literaladdress)
            {
              // For a literal address, write a #define that maps the symbolic name to the address
              char buff[200];

              if (v->AccessVolatile)
                {
                  sprintf (buff, "(*(%s volatile *)(%s))", t->TypeSpec->CName, cname);
                }
              else
                {
                  sprintf (buff, "(*(%s *)(%s))", t->TypeSpec->CName, cname);
                }
              //
              v->CName = MakeCName (v->Name);
              //----------
              SwitchToHeader ();
              emit (0, "#define %s %s\n\n", v->CName, buff);
              SwitchToSource ();
              //----------
            }
          else
            {
              v->CName = cname;
            }
        }
      else if (var->Mode == MODE_GLOBAL || var->Mode == MODE_SHARED)
        {
          v->CName = MakeCName (v->Name);
        }
      else
        {
          v->CName = v->Name;
        }
      v->TargetAttribute = targetattribute;
      v->TargetAttributeLine = targetattributeline;
      v->Lock = 0;
      v->Referent = NULL;
      v->WholeObjectReference = FALSE;
      v->ShareList = NULL;
      v->Usage = usage;
      v->PublicOut = var->PublicOut;
      CurrentVars->NumVars++;
      //
      vnew = checked_malloc(sizeof *vnew);
      vnew->Var = v;
      vnew->Next = NULL;
      if (vend == NULL)
        {
          vfirst = vnew;
        }
      else
        {
          vend->Next = vnew;
        }
      vend = vnew;
      varlist = varlist->Next;
    }
  return vfirst;
}

void
GenerateAddVar (struct tVariableDeclaration *v)
{
  struct tIdentifierListEntry *varlist;

  varlist = v->VarNames;
  while (varlist != NULL)
    {
      emit (0, " %s %s;\n", v->TypeName->Name->VarName, varlist->SymbolName);        //*****=====
      varlist = varlist->Next;
    }
}

struct tType *
AllocateTType (void)
{
  struct tType *t;

  t = checked_malloc (sizeof *t);
  t->TypeSpec = NULL;
  t->DeferredTypespecs = NULL;
  t->Name = NULL;
  t->Next = NULL;
  return t;
}

void CreateAccessType (int depth, char *filename, int line,
                       struct tTypeSpec *typespec,
                       struct tTypeSpec *accessedtype);

void
AddTypeSymbol (int depth, char *filename, int line,
               struct tTypeDeclaration *typedecl, struct tTypeSpec *typespec,
               enum tAccess nameaccess, struct tType **newtype,
               struct tVar **newvar)
{
  struct tVar *v, **vp;
  struct tType *t;
  int j;
  struct tTypeSpecChain *chain;

  // Test whether this is the completion of an incomplete type
  vp = &CurrentVars->Vars[CurrentVars->NumVars];
  for (j = 0; j < CurrentVars->NumVars; j++)
    {
      vp--;
      if (!strcmp ((*vp)->Name, typedecl->Name))
        {
          t = (*vp)->Type->TypeSpec->Type;
          if (t->TypeSpec == NULL)
            {
              // Graft in the full definition
              t->TypeSpec = typespec;
              (*vp)->CName = typespec->CName;
              chain = t->DeferredTypespecs;
              while (chain != NULL)
                {
                  CreateAccessType (depth, filename, line, chain->TypeSpec,
                                    typespec);
                  chain = chain->Next;
                }
              if (Annotate)
                {
                  emit (depth, "/* (Completed specification of type %s) */\n",
                        (*vp)->Name);
                }
              typespec->ThisType = (*vp)->Type;
              *newtype = t;
              *newvar = *vp;
              return;
            }
        }
    }
  //
  CheckForDuplicateOrCaseDifference (filename, line, typedecl->Name);
  CheckForVarOverflow (filename, line);
  t = AllocateTType ();
  t->Name = typedecl->Name;
  t->NameAccess = nameaccess;
  t->Next = NULL;
  t->TypeSpec = NewTypespec (line);
  t->TypeSpec->BaseType = TYPE_TYPE;
  t->TypeSpec->Line = line;
  t->TypeSpec->StructureAccess = PRIVATE_ACCESS;
  t->TypeSpec->Type = AllocateTType ();
  t->TypeSpec->Type->Name = typedecl->Name;
  t->TypeSpec->Type->NameAccess = nameaccess;
  t->TypeSpec->Type->Next = NULL;
  t->TypeSpec->Type->TypeSpec = typespec;
  v = MallocVar (filename, line);
  CurrentVars->Vars[CurrentVars->NumVars] = v;
  v->Name = typedecl->Name;
  v->Type = t;
  v->ConstValue = NULL;
  v->Initialised = TRUE;        // So that 'unit expressions etc. will count as initialised
  v->Accessed = TRUE;           // For used-after-written checks
  v->Access = nameaccess;
  v->Mode = MODE_GLOBAL;
  if (typespec != NULL)
    {                           // typespec would be NULL for a forward type declaration
      v->CName = typespec->CName;
      typespec->ThisType = t;
    }
  CurrentVars->NumVars++;
  *newtype = t;
  *newvar = v;
}

void
SetIntegerWordType (char *filename, int line, char *typename,
                    struct tTypeSpec *typespec, struct tNode *wordspec)
{
  struct tTargetType *ctype;

  ctype = GetWordType (filename, line, TARGET_INTEGER, wordspec);
  if (ctype != NULL)
    {
      if (ctype->Min <= typespec->Low && ctype->Max >= typespec->High)
        typespec->CType = ctype;
      else
        {
          err (filename, line,
               "Requested word type '%s' cannot accommodate the range %"
               PRINTBIG "..%" PRINTBIG " of type '%s'",
               ctype->RepresentationName, typespec->Low, typespec->High,
               typename);
        }
    }

}

void
printtypeaname (struct tTypeSpec *t)
{
  char *a = MakeAName (t->CName);
  emitstr (a);
  free (a);
}

// Return TRUE if the type contains access types
bool
RequiresFreeing (struct tTypeSpec *type)
{
  if (type->BaseType == RECORD_TYPE)
    {
      struct tFieldSpec *f;

      f = type->FieldList;
      while (f != NULL)
        {
          if (f->Typespec != NULL)
            {
              // Do not inspect null fields
              if (RequiresFreeing (f->Typespec))
                return TRUE;
            }
          f = f->Next;
        }
      return FALSE;
    }
  else if (type->BaseType == ARRAY_TYPE)
    {
      return (RequiresFreeing (type->ElementTypeSpec));
    }
  else if (type->BaseType == ACCESS_TYPE && (!type->UnmanagedAccess))
    return TRUE;
  else
    return FALSE;
}

void
CallFreeExternal (int depth, char *filename, int line, struct tReference *ref,
                  struct tTypeSpec *type, bool reftopointer)
{
  bool bitfield = FALSE;

  emit (depth, "");
  if (type->UnmanagedAccess)
    {
      emit (0, "%s(", type->FreeFunction);
      if (reftopointer)
        emit (0, "*");
      printreference (filename, line, ref, NULL, &bitfield, FALSE);
      emitstr (");\n");

    }
  else
    {
      printtypeaname (type->ElementTypeSpec);
      emit (0, "__free_external(");
      if (reftopointer)
        emit (0, "*");
      printreference (filename, line, ref, NULL, &bitfield, FALSE);
      if (CompilerTest)
        {
          emit (0, ", %d", line);
        }
      emitstr (");\n");
    }
}

void
WriteCallFreeInternal (int depth, int line, char *prefix, char *name,
                       struct tTypeSpec *type, bool lineparam)
{
  emit (depth, "");
  if (type->BaseType == ACCESS_TYPE)
    {
      if (type->ElementTypeSpec == NULL)
        {
          // This happens in the case of a forward declaration for the target of an access type
          emitstr (MakeCName (type->ElementName));
        }
      else
        {
          printtypeaname (type->ElementTypeSpec);
        }
      if (CompilerTest)
        {
          if (lineparam)
            {
              emit (0, "__free_external(%s%s, line);\n", prefix, name);
            }
          else
            {
              emit (0, "__free_external(%s%s, %d);\n", prefix, name, line);
            }
        }
      else
        {
          emit (0, "__free_external(%s%s);\n", prefix, name);
        }
    }
  else if (type->BaseType == ARRAY_TYPE)
    {
      printtypeaname (type);
      if (CompilerTest)
        {
          if (lineparam)
            {
              emit (0, "__free_internal(%s%s, line);\n", prefix, name);
            }
          else
            {
              emit (0, "__free_internal(%s%s, %d);\n", prefix, name, line);
            }
        }
      else
        {
          emit (0, "__free_internal(%s%s);\n", prefix, name);
        }
    }
  else
    {
      printtypeaname (type);
      if (CompilerTest)
        {
          if (lineparam)
            {
              emit (0, "__free_internal(&%s%s, line);\n", prefix, name);
            }
          else
            {
              emit (0, "__free_internal(&%s%s, %d);\n", prefix, name, line);
            }
        }
      else
        {
          emit (0, "__free_internal(&%s%s);\n", prefix, name);
        }
    }
}

void
CallFreeInternal (int depth, int line, char *prefix, char *name,
                  struct tTypeSpec *type)
{
  if (!type->UnmanagedAccess)
    {
      WriteCallFreeInternal (depth, line, prefix, name, type, FALSE);
    }
}

void
CallFreeInternalLine (int depth, int line, char *prefix, char *name,
                      struct tTypeSpec *type)
{
  if (!type->UnmanagedAccess)
    {
      WriteCallFreeInternal (depth, line, prefix, name, type, TRUE);
    }
}

void
CallCountInternal (int depth, char *namepart1, char *namepart2,
                   struct tTypeSpec *type)
{
  emit (depth, "");
  if (type->BaseType == ACCESS_TYPE)
    {
      if (type->ElementTypeSpec == NULL)
        {
          // This happens in the case of a forward declaration for the target of an access type
          emitstr (MakeAName (MakeCName (type->ElementName)));
        }
      else
        {
          printtypeaname (type->ElementTypeSpec);
        }
      emit (0, "__count(%s%s);\n", namepart1, namepart2);
    }
  else if (type->BaseType == ARRAY_TYPE)
    {
      printtypeaname (type);
      emit (0, "__count_internal(%s%s);\n", namepart1, namepart2);
    }
  else
    {
      printtypeaname (type);
      emit (0, "__count_internal(&%s%s);\n", namepart1, namepart2);
    }
}

void
CallAllocate (int depth, char *filename, int line,
              struct tReference *ref, struct tTypeSpec *type)
{
  bool bf = FALSE;

  emit (depth, "");
  printreference (filename, line, ref, NULL, &bf, FALSE);
  emit (0, " = ");
  if (type->UnmanagedAccess)
    {
      emit (0, "%s(sizeof(%s));\n", type->AllocateFunction,
            type->ElementTypeSpec->CName);
    }
  else
    {
      printtypeaname (type->ElementTypeSpec);
      emit (0, "__allocate();\n");
    }
}

void
CallCount (int depth, char *filename, int line,
           struct tReference *ref, struct tTypeSpec *type, bool reftopointer)
{
  bool bf = FALSE;

  if (!type->UnmanagedAccess)
    {
      emit (depth, "");
      printtypeaname (type->ElementTypeSpec);
      emit (0, "__count(");
      if (reftopointer)
        emit (0, "*");
      printreference (filename, line, ref, NULL, &bf, FALSE);
      emit (0, ");\n");
    }
}

bool
CommonRepresentation (struct tRepresentationClause *rep)
{
  return FALSE;
}

void
CheckUsage (struct tVar *proc);

void
CreateAccessType (int depth, char *filename, int line,
                  struct tTypeSpec *typespec, struct tTypeSpec *accessedtype)
{
  struct tRepresentationClause *rep;

  //
  typespec->ElementTypeSpec = accessedtype;     // Do this first

  if (accessedtype->Controlled && (!typespec->Controlled) && typespec->UnmanagedAccess)
    {
      err (filename, line,
           "Unmanaged access type %s must be marked controlled because type %s is controlled", typespec->Name, accessedtype->Name);
    }

  if (accessedtype->Controlled && accessedtype->AccessFinalisation == NULL && (!typespec->UnmanagedAccess))
    {
      err (filename, line,
           "A managed access type cannot access a controlled type unless access_finalisation is specified for the accessed type");
    }
  // 0 is used to represent null, 1 to represent non-null
  if (typespec->CanBeNull)
    {
      typespec->Low = 0;
    }
  else
    {
      typespec->Low = 1;
    }
  typespec->High = 1;
  //
  typespec->AllocateFunction = DefaultAllocateFunction;
  typespec->FreeFunction = DefaultFreeFunction;
  //
  rep = typespec->Representation;
  while (rep != NULL)
    {
      if (!strcasecmp (rep->Name, "allocate"))
        {
          if (rep->Value->Op == _CONST_STRING)
            {
              typespec->AllocateFunction = rep->Value->String;
            }
          else
            {
              err (filename, line,
                   "Syntax error: expected 'allocate => \"name\"'");
            }
        }
      else if (!strcasecmp (rep->Name, "free"))
        {
          if (rep->Value->Op == _CONST_STRING)
            {
              typespec->FreeFunction = rep->Value->String;
            }
          else
            {
              err (filename, line,
                   "Syntax error: expected 'free => \"name\"'");
            }
        }
      else
        {
          err (filename, line,
               "Incorrect representation clause '%s' for access type - expected allocate or free",
               rep->Name);
        }
      rep = rep->Next;
    }
  //----------
  if (typespec->UnmanagedAccess)
    {
      char *ptrto;

      ptrto = accessedtype->CName;
      typespec->CName =
        MakeUnmanagedAccessName (ptrto, typespec->ConstantAccess, FALSE);
    }
  else
    {
      typespec->CName = MakeAccessName (MakeAName (accessedtype->CName));
      // Create the structures and routines to manage the accessed type, if they have not been created already
      // (if the type is not complete then create them later)
      if (!accessedtype->AccessRoutinesCreated)
        {
          struct tAccessDecl *adeclptr, *newadecl;
          bool adeclfound;
          char *accessname;

          accessname = MakeAName (accessedtype->CName);
          SwitchToHeader();
          emitstr ("\n");
          emit (0, "/*\n");
          emit (0, " * Type and access routines for %s\n",
                typespec->AccessedElementType->TypeIdentifier->Name->VarName);
          emit (0, " */\n\n");
          adeclptr = AccessDeclarations;
          adeclfound = FALSE;
          while (adeclptr != NULL)
            {
              if (!strcmp (adeclptr->Name, accessname))
                {
                  adeclfound = TRUE;
                  break;
                }
              adeclptr = adeclptr->Next;
            }
          // Avoid redefinition if the ctype happens to match another type
          if (adeclfound)
            {
              emit (0, "/* Already defined for %s */\n\n",
                    accessedtype->CName);
            }
          else
            {
              // Create a structure with a reference count
              emit (0, "struct %s__a {\n", accessname);
              emit (1, "");
              printtypename (accessedtype);
              emit (0, " all;\n");
              emit (1, "unsigned count;\n");
              emit (0, "};\n\n");
              //
              // Create a procedure to free one of these records
              if (CompilerTest)
                {
                  emit (0,
                        "void %s__free_external(struct %s__a *p, int line);\n\n",
                        accessname, accessname);
                }
              else
                {
                  emit (0,
                        "void %s__free_external(struct %s__a *p);\n\n",
                        accessname, accessname);
                }
              SwitchToSource ();
              emitstr ("\n");
              if (CompilerTest)
                {
                  emit (0,
                        "void %s__free_external(struct %s__a *p, int line)\n",
                        accessname, accessname);
                }
              else
                {
                  emit (0, "void %s__free_external(struct %s__a *p)\n",
                        accessname, accessname);
                }
              emit (0, "{\n");
              emit (1, "if (p == NULL) return;\n");        // null is possible if this type is referenced by an 'or null' access type
              //**********AllowCircularAccess
              emit (1, "if (p->count == 0) return;\n");    // reached again through a circular structure
              if (CompilerTest)
                {
                  emit (1,
                        "__deallocation_check(p->count, line);\n");
                }
              emit (1, "p->count--;\n");
              emit (1, "if (p->count == 0) {\n");
              if (accessedtype->AccessFinalisation != NULL)
                {
                  CheckUsage(accessedtype->AccessFinalisation);
                  emit (2, "");
                  printprocname (accessedtype->AccessFinalisation->
                                 Type->TypeSpec);
                  emit (0, "(&p->all);\n");
                }
              if (RequiresFreeing (accessedtype))
                {
                  char *prefix;

                  if (accessedtype->BaseType == ACCESS_TYPE)
                    {
                      emit (2, "");
                      if (accessedtype->UnmanagedAccess)
                        {
                          emit (0, "%s(", accessedtype->FreeFunction);
                          emit (0, "p->all");
                          emitstr (");\n");
                        }
                      else
                        {
                          printtypeaname (accessedtype->ElementTypeSpec);
                          emit (0, "__free_external(");
                          emit (0, "p->all");
                          if (CompilerTest)
                            {
                              emit (0, ", %d", line);
                            }
                          emitstr (");\n");
                        }
                    }
                  else
                    {
                      if (accessedtype->BaseType == ARRAY_TYPE)
                        prefix = "";
                      else
                        prefix = "&";
                      if (CompilerTest)
                        {
                          emit (2,
                                "%s__free_internal(%sp->all, line);\n",
                                accessname, prefix);
                        }
                      else
                        {
                          emit (2, "%s__free_internal(%sp->all);\n",
                                accessname, prefix);
                        }
                    }
                }
              // Call the deallocator unless this is a compiler test, in which case leave the
              // object in place with count = 0 in order to help detect accesses of deallocated
              // objects
              if (!CompilerTest)
                {
                  emit (2, "%s(p);\n", typespec->FreeFunction);
                }
              if (CompilerTest)
                {
                  emit (2, "__allocated_objects--;\n");
                }
              emit (1, "}\n");
              emit (0, "}\n\n");
              //
              // And one to allocate a record
              //SwitchToPrivHeader ();
              SwitchToHeader();
              emit (0, "struct %s__a *%s__allocate(void);\n\n", accessname,
                    accessname);
              SwitchToSource ();
              emit (0, "struct %s__a *%s__allocate(void)\n", accessname,
                    accessname);
              emit (0, "{\n");
              emit (1,
                    "struct %s__a *p = %s(sizeof(struct %s__a));\n",
                    accessname, typespec->AllocateFunction, accessname);
              emit (1, "p->count = 1;\n");
              if (CompilerTest)
                {
                  emit (1, "__allocated_objects++;\n");
                }
              emit (1, "return p;\n");
              emit (0, "}\n\n");
              //
              // And one to count an additional reference
              //SwitchToPrivHeader ();
              SwitchToHeader();
              emit (0, "void %s__count(struct %s__a *p);\n\n", accessname,
                    accessname);
              SwitchToSource ();
              emit (0, "void %s__count(struct %s__a *p)\n", accessname,
                    accessname);
              emit (0, "{\n");
              emit (1, "if (p != NULL) p->count++;\n");
              emit (0, "}\n\n");
              //
              newadecl = checked_malloc (sizeof *newadecl);
              newadecl->Name = accessname;
              newadecl->Next = AccessDeclarations;
              AccessDeclarations = newadecl;
            }
          //
          accessedtype->AccessRoutinesCreated = TRUE;
        }
    }
}

void
AnalyseAddIntegerType (int depth, char *filename, int line,
                       struct tTypeDeclaration *typedecl, enum tAccess nameaccess,
                       struct tTypeSpec *typespec,
                       struct tRepresentationClause **targetattribute,
                       struct tRepresentationClause **targetattributeline,
                       struct tRepresentationClause **targetcast
)
{
  struct tUnitDef *unit;
  struct tRepresentationClause *rep;
  bool staticint;

  if (typespec->EnumList != NULL)
    {
      // This is 'type t is (a, b, c, ...)'
      BIGINT count, startvalue;
      struct tEnum *e;
      struct tUnitDeclaration unitdecl;

      unitdecl.Line = line;
      unitdecl.UnitName = MakeFieldName (typedecl->Name, "unit");       // 'unit' is reserved so shouldn't clash
      unitdecl.unit = NULL;
      typespec->Unit =
        AnalyseAddUnit (depth, filename, line, &unitdecl, nameaccess);
      // Need to scan for first_value before creating the constants
      startvalue = 0;   // Default for first enumeration value
      rep = typespec->Representation;
      while (rep != NULL)
        {
          if (!strcasecmp (rep->Name, "first_value"))
            {
              bool success;
              struct tUnitDef *unit;
              startvalue =
                EvalStaticInteger (rep->Value, &success, TRUE, &unit);
            }
          rep = rep->Next;
        }
      count = startvalue;
      e = typespec->EnumList;
      while (e != NULL)
        {
          e->ConstVar = AnalyseMakeConstant (depth, filename, line,
                                             e->Name,
                                             NewNode (filename, line,
                                                      _CONST_INT,
                                                      NULL, NULL,
                                                      count, 0.0,
                                                      NULL, NULL,
                                                      NULL, NULL),
                                             typespec->Unit,
                                             typespec->StructureAccess,
                                             e->Representation, FALSE,
                                             e == typespec->EnumList,
                                             e->Next == NULL);
          count++;
          e = e->Next;
        }
      typespec->Low = startvalue;
      typespec->High = count - 1;
    }
  else if (typespec->LowExpr == NULL)
    {
      // This is 'type t is range for ...'
      bool virtualexpr;
      TestIntegerExpression (typespec->HighExpr, &typespec->Low,
                             &typespec->High, &unit, FALSE, TRUE,
                             &virtualexpr);
      typespec->Unit = unit;
    }
  else
    {
      struct tUnitDef *lunit, *runit, *newunit, *testunit;

      typespec->Low =
        EvalStaticInteger (typespec->LowExpr, &staticint, TRUE,
                           &lunit);
      typespec->High =
        EvalStaticInteger (typespec->HighExpr, &staticint, TRUE,
                           &runit);
      if (typespec->High < typespec->Low)
        {
          err (filename, line,
               "The high part of a range cannot be less than the low part");
        }
      TestUnitMatch (lunit, runit, typespec->HighExpr, TRUE);
      if (lunit != NULL)
        {
          testunit = lunit;
        }
      else
        {
          testunit = runit;
        }
      if (typespec->ReqUnit != NULL)
        {
          newunit =
            AnalyseUnitOrUnique (filename, line, typespec->ReqUnit);
          TestUnitMatch (newunit, testunit, typespec->HighExpr, TRUE);
        }
      else
        {
          newunit = testunit;
        }
      typespec->Unit = newunit;
    }
  typespec->CType = GetTargetIntegerType (filename, line, typespec->Low, typespec->High, FALSE, TRUE);  // default, usual case
  rep = typespec->Representation;
  while (rep != NULL)
    {
      if (!strcasecmp (rep->Name, "word_type"))
        {
          SetIntegerWordType (filename, line, typedecl->Name,
                              typespec, rep->Value);
        }
      else if (!strcasecmp (rep->Name, "cprefix"))
        {
          if (!UnitTest)
            {
              *targetattribute = rep;
            }
        }
      else if (!strcasecmp (rep->Name, "cprefix_line"))
        {
          if (!UnitTest)
            {
              *targetattributeline = rep;
            }
        }
      else if (!strcasecmp (rep->Name, "ccast"))
        {
          *targetcast = rep;
        }
      else if ((!strcasecmp (rep->Name, "first_value"))
               && typespec->EnumList != NULL)
        {
          ;             // Handled above
        }
      else if (!strcasecmp (rep->Name, "build_condition"))
        {
          ;             // Handled above
        }
      else
        {
          err (filename, line,
               "Unknown representation for integer type '%s'",
               rep->Name);
        }
      rep = rep->Next;
    }
  typespec->CName = typespec->CType->CName;
}

void
AnalyseAddFloatType (int depth, char *filename, int line,
                     struct tTypeDeclaration *typedecl, enum tAccess nameaccess,
                     struct tTypeSpec *typespec,
                     struct tRepresentationClause **targetattribute,
                     struct tRepresentationClause **targetattributeline,
                     struct tRepresentationClause **targetcast)
{
  struct tRepresentationClause *rep;
  
  typespec->Unit =
    AnalyseUnitOrUnique (filename, line, typespec->ReqUnit);
  typespec->CType =
    GetTargetFloatType (filename, line, typespec->Digits,
                        typespec->Magnitude, FALSE);
  rep = typespec->Representation;
  while (rep != NULL)
    {
      if (!strcmp (rep->Name, "word_type"))
        {
          struct tTargetType *ctype;

          ctype =
            GetWordType (filename, line, TARGET_FLOAT, rep->Value);
          if (ctype != NULL)
            {
              if (ctype->Magnitude >= typespec->Magnitude)
                typespec->CType = ctype;
              else
                {
                  err (filename, line,
                       "Requested word type '%s' cannot accommodate the magnitude %"
                       PRINTBIG " of type '%s'",
                       ctype->RepresentationName, typespec->Magnitude,
                       typedecl->Name);
                }
            }
        }
      else if (!strcasecmp (rep->Name, "cprefix"))
        {
          if (!UnitTest)
            {
              *targetattribute = rep;
            }
        }
      else if (!strcasecmp (rep->Name, "cprefix_line"))
        {
          if (!UnitTest)
            {
              *targetattributeline = rep;
            }
        }
      else if (!strcasecmp (rep->Name, "ccast"))
        {
          *targetcast = rep;
        }
      else if (!strcasecmp (rep->Name, "build_condition"))
        {
          ;             // Handled above
        }
      else
        {
          err (filename, line,
               "Unknown representation for floating type '%s'",
               rep->Name);
        }
      rep = rep->Next;
    }
  typespec->CName = typespec->CType->CName;
}

void AnalyseAddRecordType(int depth, char *filename, int line,
                          struct tTypeDeclaration *typedecl, enum tAccess nameaccess,
                          struct tTypeSpec *typespec,
                          struct tRepresentationClause **targetattribute,
                          struct tRepresentationClause **targetattributeline,
                          struct tRepresentationClause **targetcast)
{
  struct tFieldSpec *f, **fp;
  struct tTypeSpec *ts;
  enum tAccess access;
  unsigned numfields;
  bool success, fieldoffsetset, firstbitset, numbitsset, havefields,
    havenonfields, havebitnames, mergebitfields;
  // Intialise to avoid warnings - actually OK:
  //   multiplier: StorageSet
  //   fieldoffset: fieldoffsetset
  //   firstbit: firstbitset
  //   numbits: field related tests
  BIGINT multiplier = 0;
  unsigned fieldoffset = 0, firstbit = 0, numbits = 0;
  // Initialise ctype to avoid a warning - actually OK because of StorageSet test
  // Initialise fieldctype to avoid a warning - actually OK because of fieldoffsetset tests
  struct tTargetType *ctype = NULL, *fieldctype = NULL;
  struct tUnitDef *runit;
  struct tRepresentationClause *rep;

  typespec->CName = MakeCName (typedecl->Name);
  //
  typespec->StorageSet = FALSE;
  rep = typespec->Representation;
  while (rep != NULL)
    {
      if (!strcasecmp (rep->Name, "storage"))
        {
          if (rep->Value->Op == _TIMES)
            {
              multiplier =
                EvalStaticInteger (rep->Value->Left, &success, TRUE,
                                   &runit);
              if (success)
                {
                  ctype =
                    GetWordType (filename, line, TARGET_INTEGER,
                                 rep->Value->Right);
                  if (ctype != NULL)
                    typespec->StorageSet = TRUE;
                }
            }
          else
            {
              err (filename, line, "Expected 'n * word_type'");
            }
        }
      else if (!strcasecmp (rep->Name, "cprefix"))
        {
          if (!UnitTest)
            {
              *targetattribute = rep;
            }
        }
      else if (!strcasecmp (rep->Name, "cprefix_line"))
        {
          if (!UnitTest)
            {
              *targetattributeline = rep;
            }
        }
      else if (!strcasecmp (rep->Name, "ccast"))
        {
          *targetcast = rep;
        }
      else if (!strcasecmp (rep->Name, "build_condition"))
        {
          ;             // Handled above
        }
      else
        {
          err (filename, line,
               "Unknown representation for record type '%s'",
               rep->Name);
        }
      rep = rep->Next;
    }
  // Analyse the fields
  // Look up and store the type specs because the names won't be available outside
  // the package
  f = typespec->FieldList;
  numfields = 0;
  havefields = FALSE;
  havenonfields = FALSE;
  havebitnames = FALSE;
  while (f != NULL)
    {
      f->FieldSet = FALSE;
      f->BitFieldSet = FALSE;
      if (f->Comment != NULL)
        {
          // Comments will be removed below
        }
      else if (f->Code != NULL)
        {
          havenonfields = TRUE;
        }
      else if (f->Type == NULL)
        {
          f->Typespec = NULL;
          numfields++;
        }
      else
        {
          if (f->Type->TypeSpec != NULL)
            {
              // Create an anonymous type
              struct tTypeDeclaration t;
              struct tType *fieldtype;

              t.TypeSpec = f->Type->TypeSpec;
              t.TypeSpec->StructureAccess = typedecl->TypeSpec->StructureAccess; //nameaccess;
              t.Name = MakeFieldName (typedecl->Name, f->Name);
              fieldtype =
                AnalyseAddType (depth, filename, line, &t,
                                nameaccess);
              SwitchToHeader ();        // The previous recursive call might change the output to something else
              fieldtype->TypeSpec->Type->TypeSpec->StructureAccess =
                typedecl->TypeSpec->StructureAccess;
              f->Type->TypeIdentifier =
                MakeTypeIdentifier (MakeReference
                                    (IDENTIFIER_REFERENCE,
                                     fieldtype->Name, NULL, NULL),
                                    NULL);
            }
          //
          ts =
            GetTypeSpec (filename, line, f->Type->TypeIdentifier,
                         &access);
          if (ts == NULL)
            {
              err (filename, line,
                   "Cannot use incomplete types as record fields");
            }
          if (!(ts->MinDefinite && ts->MaxDefinite))
            {
              err (filename, line,
                   "Cannot use indefinite types as record fields");
            }
          if (ts->BaseType == ARRAY_TYPE
              && ts->IndexTypeSpec == UniversalIndexTypespec)
            {
              err (filename, line,
                   "Cannot use arrays with index type universal_index as record fields");
            }
          f->Typespec = ts;
          if (ts->Controlled)
            {
              if (!typespec->Controlled)
                {
                  err (filename, line,
                       "Type %s must be marked as controlled because field %s is controlled",
                       typedecl->Name, f->Name);
                  typespec->Controlled = TRUE;
                }
            }
          //
          fieldoffsetset = FALSE;
          firstbitset = FALSE;
          numbitsset = FALSE;
          rep = f->Representation;
          while (rep != NULL)
            {
              if (!strcasecmp (rep->Name, "word_offset"))
                {
                  if (f->Typespec->BaseType != INTEGER_TYPE
                      && f->Typespec->BaseType != BOOLEAN_TYPE)
                    {
                      err (filename, line,
                           "word_offset can only be applied to integers and booleans");
                    }
                  if (rep->Value->Op == _TIMES)
                    {
                      fieldoffset =
                        (unsigned) EvalStaticInteger (rep->
                                                      Value->Left,
                                                      &success, TRUE,
                                                      &runit);
                      if (success)
                        {
                          fieldctype =
                            GetWordType (filename, line,
                                         TARGET_INTEGER,
                                         rep->Value->Right);
                          if (fieldctype != NULL)
                            fieldoffsetset = TRUE;
                        }
                    }
                  else
                    {
                      err (filename, line,
                           "Expected 'n * word_type'");
                    }
                }
              else if (!strcasecmp (rep->Name, "bit_offset"))
                {
                  if (ts->BaseType != INTEGER_TYPE
                      && ts->BaseType != BOOLEAN_TYPE)
                    {
                      err (filename, line,
                           "%s can only be applied to integer, enumeration and boolean types",
                           rep->Name);
                    }
                  else
                    {
                      firstbit =
                        (unsigned) EvalStaticInteger (rep->Value,
                                                      &success, TRUE,
                                                      &runit);
                      if (success)
                        firstbitset = TRUE;
                    }
                }
              else if (!strcasecmp (rep->Name, "bits"))
                {
                  if (ts->BaseType != INTEGER_TYPE
                      && ts->BaseType != BOOLEAN_TYPE)
                    {
                      err (filename, line,
                           "%s can only be applied to integer, enumeration and boolean types",
                           rep->Name);
                    }
                  else
                    {
                      numbits =
                        (unsigned) EvalStaticInteger (rep->Value,
                                                      /*rep->Value->Attribute, */
                                                      &success, TRUE,
                                                      &runit);
                      if (success)
                        numbitsset = TRUE;
                    }
                }
              else if (!CommonRepresentation (rep))
                {
                  err (filename, line,
                       "Unknown representation for a field '%s'",
                       rep->Name);
                }
              rep = rep->Next;
            }
          if (firstbitset || numbitsset)
            {
              if (!(fieldoffsetset && firstbitset && numbitsset))
                {
                  err (filename, line,
                       "'bit_offset' and 'bits' must be used together with 'word_offset'");
                }
              else if (f->Typespec->BaseType == BOOLEAN_TYPE)
                {
                  if (numbits < 1)
                    {
                      err (filename, line,
                           "At least one bit is required for booleans");
                    }
                }
              else
                {
                  BIGINT L, H, M;
                  unsigned sgn, log2;

                  L = f->Typespec->Low;
                  H = f->Typespec->High;
                  if (L < 0 && H > 0)
                    sgn = 1;
                  else
                    sgn = 0;
                  if (L < 0)
                    L = -L;
                  if (H < 0)
                    H = -H;
                  if (H > L)
                    M = H;
                  else
                    M = L;
                  if (M == 0)
                    log2 = 1;   // special case for 'range 0..0' - still allocate at least one bit
                  else
                    {
                      log2 = 0;
                      while (M != 0)
                        {
                          log2++;
                          M = M / 2;
                        }
                    }
                  if (log2 + sgn > numbits)
                    {
                      err (filename, line,
                           "Field size %u is too small for type %s%s%s",
                           numbits,
                           f->Type->TypeIdentifier->Name->VarName,
                           f->Type->TypeIdentifier->Attribute ==
                           NULL ? "" : "..");
                    }
                  if (firstbit + numbits > fieldctype->SizeInBits)
                    {
                      err (filename, line,
                           "Field %u at %u exceeds a %s", numbits,
                           firstbit, fieldctype->CName);
                    }
                }
            }
          //----------
          if (fieldoffsetset)
            {
              f->FieldSet = TRUE;
              f->FieldOffset = fieldoffset;
              f->FieldCType = fieldctype;
              f->UseShortBitFieldForm = FALSE;
              if (firstbitset && numbitsset)
                {
                  f->BitFieldSet = TRUE;
                  f->BitOffset = firstbit;
                  f->NumBits = numbits;
                  if (numbits == 1)
                    {
                      // Write a symbolic name for this common case
                      f->BitName =
                        checked_malloc (strlen
                                        (gettypename (typespec)) + 1 +
                                        strlen (f->Name) + 5);
                      sprintf (f->BitName, "%s_%s_BIT",
                               gettypename (typespec), f->Name);
                      emit (0, "#define %s %s\n", f->BitName,
                            SingleBitMask[firstbit]);
                      havebitnames = TRUE;
                    }
                  else
                    {
                      f->BitName = NULL;
                    }
                }
              havefields = TRUE;
            }
          else
            havenonfields = TRUE;
          //----------
          numfields++;
        }
      //----------
      f = f->Next;
    }
  if (havebitnames)
    {
      emitstr ("\n");
    }
  //
  //----------
  //
  mergebitfields = FALSE;

  if (havefields && (!havenonfields) && (!typespec->StorageSet))
    {
      // Check for all bit fields within the first word (common case)
      struct tTargetType *firstctype;

      f = typespec->FieldList;
      if (f != NULL)
        {
          mergebitfields = TRUE;
          firstctype = f->FieldCType;
          while (f != NULL)
            {
              if (!
                  (f->Code == NULL && f->FieldSet
                   && f->FieldOffset == 0
                   && f->FieldCType == firstctype))
                {
                  mergebitfields = FALSE;
                  break;
                }
              f = f->Next;
            }
        }
    }
  //
  emit (0, "typedef ");
  if (mergebitfields)
    {
      emit (0, "%s ", typespec->FieldList->FieldCType->CName);
      f = typespec->FieldList;
      while (f != NULL)
        {
          f->UseShortBitFieldForm = TRUE;
          f = f->Next;
        }
    }
  else
    {
      if (typespec->StorageSet || havefields)
        {
          emit (0, "union {\n");
          if (typespec->StorageSet)
            {
              emit (1, "%s _s[", ctype->CName);
              printbigint (multiplier);
              emit (0, "];\n");
            }
          if (havefields)
            {
              f = typespec->FieldList;
              while (f != NULL)
                {
                  if ((f->Code == NULL) && f->FieldSet)
                    {
                      emit (1, "%s %s[%u];\n", f->FieldCType->CName,
                            f->Name, f->FieldOffset + 1);
                    }
                  f = f->Next;
                }
              typespec->StorageSet = TRUE;
            }
        }
      if (havenonfields)
        {
          if (typespec->RecordType != REC_UNCHECKED_UNION)
            emit (0, "struct _%s {\n", gettypename(typespec));
          if (typespec->RecordType == REC_UNION
              || typespec->RecordType == REC_UNCHECKED_UNION)
            emit (0, "union {\n");
          //----------
          // Generate the fields
          // Output literal field definitions (code) and then delete them from the type specification
          fp = &typespec->FieldList;
          f = *fp;
          while (f != NULL)
            {
              if (f->Comment != NULL)
                {
                  emit (1, "/* %s */\n", f->Comment);
                }
              else if (f->Code != NULL)
                {
                  emit (1, "%s\n", f->Code);
                  *fp = f->Next;
                }
              else if (f->Type == NULL)
                {
                  // This is a null field
                  fp = &f->Next;
                }
              else if (!(f->FieldSet))
                {
                  emit (1, "");
                  printtypename (f->Typespec);
                  emit (0, " %s;\n", f->Name);
                  fp = &f->Next;
                }
              f = f->Next;
            }
        }
      //----------
      if (havenonfields)
        {
          if (typespec->RecordType == REC_UNION)
            {
              emit (0, "} _u;\n");
              emit (0, "unsigned tag;\n");      //*****unsigned
            }
          emit (0, "} ");
          if (typespec->StorageSet)
            {
              emit (0, "_v;\n} ");
            }
        }
      else
        {
          emit (0, "} ");
        }
    }

  // Remove comments
  {
    struct tFieldSpec *pf;
    bool havenoncomments;

    f = typespec->FieldList;
    pf = NULL;
    havenoncomments = FALSE;
    while (f != NULL)
      {
        if (f->Comment != NULL)
          {
            if (pf == NULL)
              {
                typespec->FieldList = f->Next;
              }
            else
              {
                pf->Next = f->Next;
              }
          }
        else
          {
            pf = f;
            havenoncomments = TRUE;
          }
        f = f->Next;
      }
    if (!havenoncomments)
      {
        err(filename, line, "A record must have at least one field");
      }
  }

  // Store the range of field numbers
  typespec->Low = 0;
  typespec->High = numfields - 1;
  printtypename (typespec);
  emit (0, ";\n\n");
  //----------
  if (RequiresFreeing (typespec))
    {
      char *accessname;

      SwitchToHeader();
      accessname = MakeAName (typespec->CName);
      // Create a procedure to free the internals of one of these records
      if (CompilerTest)
        {
          emit (0, "void %s__free_internal(%s *p, int line);\n\n",
                accessname, accessname);
        }
      else
        {
          emit (0, "void %s__free_internal(%s *p);\n\n", accessname,
                accessname);
        }
      SwitchToSource ();
      if (CompilerTest)
        {
          emit (0, "void %s__free_internal(%s *p, int line)\n",
                accessname, accessname);
        }
      else
        {
          emit (0, "void %s__free_internal(%s *p)\n", accessname,
                accessname);
        }
      emit (0, "{\n");
      f = typespec->FieldList;
      while (f != NULL)
        {
          if (RequiresFreeing (f->Typespec))
            {
              if (CompilerTest)
                {
                  CallFreeInternalLine (depth + 1, line, "p->",
                                        f->Name, f->Typespec);
                }
              else
                {
                  CallFreeInternal (depth + 1, line, "p->", f->Name,
                                    f->Typespec);
                }
            }
          f = f->Next;
        }
      emit (0, "}\n\n");
      SwitchToHeader();
      //
      // Create a procedure to count pointer references in the internals of one of these records
      emit (0, "void %s__count_internal(%s *p);\n\n", accessname,
            accessname);
      SwitchToSource ();
      emit (0, "void %s__count_internal(%s *p)\n", accessname,
            accessname);
      emit (0, "{\n");
      f = typespec->FieldList;
      while (f != NULL)
        {
          if (RequiresFreeing (f->Typespec))
            {
              CallCountInternal (depth + 1, "p->", f->Name,
                                 f->Typespec);
            }
          f = f->Next;
        }
      emit (0, "}\n\n");
      SwitchToAppHeader ();
    }
}

void AnalyseAddArrayType(int depth, char *filename, int line,
                         struct tTypeDeclaration *typedecl, enum tAccess nameaccess,
                         struct tTypeSpec *typespec,
                         struct tRepresentationClause **targetattribute,
                         struct tRepresentationClause **targetattributeline,
                         struct tRepresentationClause **targetcast)
{
  enum tAccess access;
  char *accessname;
  struct tRepresentationClause *rep;

  // Do this first, if it is required,  before we get part way through the process
  if (typespec->IndexSpec->IndexType == INDEX_SPEC)
    {
      // Make an index type from the requested range
      struct tTypeDeclaration t;
      struct tType *indextype;

      t.TypeSpec = typespec->IndexSpec->IndexTypeSpec;
      t.TypeSpec->StructureAccess = typedecl->TypeSpec->StructureAccess;
      t.Name = MakeFieldName (typedecl->Name, "index_type");
      indextype =
        AnalyseAddType (depth, filename, line, &t, nameaccess);
      indextype->TypeSpec->Type->TypeSpec->StructureAccess =
        typedecl->TypeSpec->StructureAccess;
      SwitchToHeader ();        // The previous recursive call might change the output to something else
      typespec->IndexType =
        MakeTypeIdentifier (MakeReference
                            (IDENTIFIER_REFERENCE, indextype->Name,
                             NULL, NULL), NULL);
    }
  else
    {
      typespec->IndexType = typespec->IndexSpec->IndexName;
    }
  //
  if (typespec->ArrayElementType->TypeSpec != NULL)
    {
      // Make an element type
      struct tTypeDeclaration t;
      struct tType *elementtype;

      t.TypeSpec = typespec->ArrayElementType->TypeSpec;
      t.TypeSpec->StructureAccess = typedecl->TypeSpec->StructureAccess;
      t.Name = MakeFieldName (typedecl->Name, "element_type");
      elementtype =
        AnalyseAddType (depth, filename, line, &t, nameaccess);
      elementtype->TypeSpec->Type->TypeSpec->StructureAccess =
        typedecl->TypeSpec->StructureAccess;
      SwitchToHeader ();        // The previous recursive call might change the output to something else
      typespec->ArrayElementType->TypeIdentifier =
        MakeTypeIdentifier (MakeReference
                            (IDENTIFIER_REFERENCE, elementtype->Name,
                             NULL, NULL), NULL);
      typespec->ArrayElementType->TypeSpec = NULL;
    }
  //

  typespec->CName = MakeCName (typedecl->Name);
  accessname = MakeAName (typespec->CName);

  rep = typespec->Representation;
  while (rep != NULL)
    {
      if (!strcasecmp (rep->Name, "cprefix"))
        {
          if (!UnitTest)
            {
              *targetattribute = rep;
            }
        }
      else if (!strcasecmp (rep->Name, "cprefix_line"))
        {
          if (!UnitTest)
            {
              *targetattributeline = rep;
            }
        }
      else if (!strcasecmp (rep->Name, "ccast"))
        {
          *targetcast = rep;
        }
      else if (!strcasecmp (rep->Name, "build_condition"))
        {
          ;             // Handled above
        }
      else
        {
          err (filename, line,
               "Unknown representation for array type '%s'",
               rep->Name);
        }
      rep = rep->Next;
    }

  // Replace names as above
  typespec->IndexTypeSpec =
    GetTypeSpec (filename, line, typespec->IndexType, &access);
  if (typespec->IndexTypeSpec == NULL)
    {
      err (filename, line,
           "Cannot use incomplete types as array indexes");
    }
  //
  CheckUniversalIndexSet(filename, line);
  if (typespec->IndexTypeSpec->Low < UniversalIndexMin || typespec->IndexTypeSpec->High > UniversalIndexMax)
    {
      err(filename, line, "Index type exceeds the range of universal_index (see pragma universal_index_type)");
    }
  typespec->Low = typespec->IndexTypeSpec->Low;
  typespec->High = typespec->IndexTypeSpec->High;
  //
  typespec->ElementTypeSpec =
    GetTypeSpec (filename, line,
                 typespec->ArrayElementType->TypeIdentifier, &access);
  if (typespec->ElementTypeSpec == NULL)
    {
      err (filename, line,
           "Cannot use incomplete types as array elements");
    }
  if (!
      (typespec->ElementTypeSpec->MinDefinite
       && typespec->ElementTypeSpec->MaxDefinite))
    {
      err (filename, line,
           "Cannot declare arrays of indefinite types");
    }
  if (typespec->ElementTypeSpec->BaseType == ARRAY_TYPE
      && typespec->ElementTypeSpec->IndexTypeSpec ==
      UniversalIndexTypespec)
    {
      err (filename, line,
           "Cannot use arrays with index type universal_index as array elements");
    }
  if (typespec->ElementTypeSpec->Controlled)
    {
      if (!typespec->Controlled)
        {
          err (filename, line,
               "Type %s must be marked as controlled because its elements are controlled",
               typedecl->Name);
          typespec->Controlled = TRUE;
        }
    }
  //
  //----------
  if (typespec->MinDefinite && typespec->MaxDefinite)
    {
      emit (0, "typedef ");
      printtypename (typespec->ElementTypeSpec);
      emit (0, " ");
      printtypename (typespec);
      if (typespec->IndexTypeSpec == UniversalIndexTypespec)
        {
          emit (0, "[1]");
        }
      else
        {
          emit (0, "[%" PRINTBIG "]",
                (typespec->High - typespec->Low) + 1);
        }
      emit (0, ";\n\n");
    }
  else
    {
      emit (0, "typedef ");
      printtypename (typespec->ElementTypeSpec);
      emit (0, " *");
      printtypename (typespec);
      emit (0, ";\n\n");
      emit (depth, "typedef const ");
      printtypename (typespec->ElementTypeSpec);
      emit (0, " * _const");
      printtypename (typespec);
      emit (0, ";\n\n");
    }

  //----------
  if (RequiresFreeing (typespec->ElementTypeSpec))
    {
      // Create procedures to free amd count the elements of one of these arrays
      char *elementaccessname =
        MakeAName (typespec->ElementTypeSpec->CName);

      SwitchToHeader();
      if (CompilerTest)
        {
          emit (0, "void %s__free_internal(%s *p, int line);\n\n",
                accessname, elementaccessname);
        }
      else
        {
          emit (0, "void %s__free_internal(%s *p);\n\n", accessname,
                elementaccessname);
        }
      SwitchToSource ();
      if (CompilerTest)
        {
          emit (0, "void %s__free_internal(%s *p, int line)\n",
                accessname, elementaccessname);
        }
      else
        {
          emit (0, "void %s__free_internal(%s *p)\n", accessname,
                elementaccessname);
        }
      emit (0, "{\n");
      emit (1, "unsigned j;\n");
      emit (1, "for (j = 0; j < %" PRINTBIG "; j++) {\n",
            typespec->High - typespec->Low + 1);
      CallFreeInternal (2, line, "(*p)", "",
                        typespec->ElementTypeSpec);
      emit (2, "p++;\n");
      emit (1, "}\n");
      emit (0, "}\n");
      SwitchToHeader();
      //
      // Create a procedure to count pointer references in the elements of one of these arrays
      emit (0, "void %s__count_internal(%s *p);\n\n", accessname,
            elementaccessname);
      SwitchToSource ();
      emit (0, "void %s__count_internal(%s *p)\n", accessname,
            elementaccessname);
      emit (0, "{\n");
      emit (1, "unsigned j;\n");
      emit (1, "for (j = 0; j < %" PRINTBIG "; j++) {\n",
            typespec->High - typespec->Low + 1);
      CallCountInternal (depth + 2, "(*p)", "",
                         typespec->ElementTypeSpec);
      emit (2, "p++;\n");
      emit (1, "}\n");
      emit (0, "}\n");
      SwitchToAppHeader ();
      free (elementaccessname);
    }
}

void AnalyseAddAccessType(int depth, char *filename, int line,
                          struct tTypeDeclaration *typedecl, enum tAccess nameaccess,
                          struct tTypeSpec *typespec)
{
  struct tTypeSpec *accessedtype;
  enum tAccess access;
  struct tRepresentationClause *rep;

  rep = typespec->Representation;
  while (rep != NULL)
    {
      if (!strcasecmp (rep->Name, "build_condition"))
        {
          ;             // Handled above
        }
      else
        {
          err (filename, line,
               "Unknown representation for an access type '%s'",
               rep->Name);
        }
      rep = rep->Next;
    }
  if (typespec->AccessedElementType->TypeSpec != NULL)
    {
      // Make an element type
      struct tTypeDeclaration t;
      struct tType *elementtype;

      t.TypeSpec = typespec->AccessedElementType->TypeSpec;
      t.TypeSpec->StructureAccess = typedecl->TypeSpec->StructureAccess;
      t.Name = MakeFieldName (typedecl->Name, "accessed_type");
      elementtype =
        AnalyseAddType (depth, filename, line, &t, nameaccess);
      elementtype->TypeSpec->Type->TypeSpec->StructureAccess =
        typedecl->TypeSpec->StructureAccess;
      SwitchToHeader ();        // The previous recursive call might change the output to something else
      typespec->AccessedElementType->TypeIdentifier =
        MakeTypeIdentifier (MakeReference
                            (IDENTIFIER_REFERENCE, elementtype->Name,
                             NULL, NULL), NULL);
      typespec->AccessedElementType->TypeSpec = NULL;
    }
  accessedtype =
    GetTypeSpec (filename, line,
                 typespec->AccessedElementType->TypeIdentifier,
                 &access);
  if (accessedtype == NULL)
    {
      // The type has not been completely defined yet
      // Defer the completion of the access type until it is
      struct tType *ttype;
      char *accessedtypecname, *accessname;
      struct tTypeSpecChain *chain;

      ttype =
        FindType (filename, line,
                  typespec->AccessedElementType->TypeIdentifier);
      typespec->ElementName = ttype->Name;
      chain = checked_malloc (sizeof *chain);
      chain->TypeSpec = typespec;
      chain->Next = ttype->DeferredTypespecs;
      ttype->DeferredTypespecs = chain;
      // The type still needs a name from now on; since it is referring to a forward declaration,
      // we know that the accessed type will be in the same package, so we can
      // predict its name
      accessedtypecname = MakeCName (ttype->Name);
      accessname = MakeAName (accessedtypecname);
      // Create the access type name and forward-declare the struct name
      if (typespec->UnmanagedAccess)
        {
          typespec->CName =
            MakeUnmanagedAccessName (accessname, typespec->ConstantAccess, TRUE);
          emit (0, "struct _%s;\n\n", accessname);
        }
      else
        {
          typespec->CName = MakeAccessName (accessname);
          emit (0, "struct %s__a;\n\n", accessname);
        }
      free (accessname);
    }
  else
    {
      CreateAccessType (depth, filename, line, typespec,
                        accessedtype);
    }
  //----------
}

struct tType *
AnalyseAddType (int depth, char *filename, int line,
                struct tTypeDeclaration *typedecl, enum tAccess nameaccess)
{
  struct tTypeSpec *typespec;
  struct tRepresentationClause *targetattribute = NULL, *targetattributeline =
    NULL, *targetcast = NULL;
  struct tType *newtype;
  struct tVar *newvar;

  // Check for conditional compilation
  if (typedecl->TypeSpec != NULL)
    {
      // For forward declarations 'type t;' there is no typespec yet
      if (!CheckBuildCondition
          (filename, line, typedecl->TypeSpec->Representation, TRUE))
        {
          return NULL;
        }
    }
  //
  // Select the public header file for public declarations, or the app header file otherwise
  SwitchToHeader ();            // Need to write to the module header even if private (because a public type might have private components)
  if (typedecl->TypeSpec == NULL)
    {
      // If the typespec is NULL then this a forward 'type t;' declaration
      typespec = NULL;
    }
  else
    {
      // Copy the typespec because the typedecl might be used in more than one generic instantiation
      typespec = checked_malloc (sizeof *typespec);
      *typespec = *(typedecl->TypeSpec);
      typespec->Name = typedecl->Name;  // Save the name for error messages
      //
      if (typespec->BaseType == INTEGER_TYPE)
      {
        AnalyseAddIntegerType(depth, filename, line, typedecl, nameaccess, typespec,
                              &targetattribute, &targetattributeline, &targetcast);
      }
      else if (typespec->BaseType == FLOATING_TYPE)
      {
        AnalyseAddFloatType(depth, filename, line, typedecl, nameaccess, typespec,
                            &targetattribute, &targetattributeline, &targetcast);
      }
      else if (typespec->BaseType == RECORD_TYPE)
      {
        AnalyseAddRecordType(depth, filename, line, typedecl, nameaccess, typespec,
                             &targetattribute, &targetattributeline, &targetcast);
      }
      else if (typespec->BaseType == ARRAY_TYPE)
      {
        AnalyseAddArrayType(depth, filename, line, typedecl, nameaccess, typespec,
                             &targetattribute, &targetattributeline, &targetcast);
      }
      else if (typespec->BaseType == BOOLEAN_TYPE)
        {
          typespec->CType =
            GetTargetIntegerType (filename, line, 0, 1, FALSE, TRUE);
          typespec->CName = typespec->CType->CName;
        }
      else if (typespec->BaseType == CSTRING_TYPE)
        {
          typespec->CName = "const char *";
        }
      else if (typespec->BaseType == ADDRESS_TYPE)
        {
          typespec->CName = "const void *";
          // 0 is used to represent null, 1 to represent non-null
          typespec->Low = 0;
          typespec->High = 1;
        }
      else if (typespec->BaseType == ACCESS_TYPE)
      {
        AnalyseAddAccessType(depth, filename, line, typedecl, nameaccess, typespec);
      }
      else
        {
          err (filename, line, "??? %d", typespec->BaseType);   //**********
        }
    }
  if (typespec != NULL)
    {
      if (targetattribute != NULL)
        {
          if (targetattribute->Value->Op == _CONST_STRING)
            {
              typespec->TargetAttribute =
                DeQuote (targetattribute->Value->String);
            }
          else
            {
              err (filename, line, "Syntax error in cprefix clause");
            }
        }
      else
        {
          typespec->TargetAttribute = NULL;
        }
      if (targetattributeline != NULL)
        {
          if (targetattributeline->Value->Op == _CONST_STRING)
            {
              typespec->TargetAttributeLine =
                DeQuote (targetattributeline->Value->String);
            }
          else
            {
              err (filename, line, "Syntax error in cprefix_line clause");
            }
        }
      else
        {
          typespec->TargetAttributeLine = NULL;
        }
      if (targetcast != NULL)
        {
          if (targetcast->Value->Op == _CONST_STRING)
            {
              typespec->TargetCast = DeQuote (targetcast->Value->String);
            }
          else
            {
              err (filename, line, "Syntax error in ccast clause");
            }
        }
      else
        {
          typespec->TargetCast = NULL;
        }
    }
  SwitchToSource ();
  AddTypeSymbol (depth, filename, line, typedecl, typespec, nameaccess,
                 &newtype, &newvar);
  if (Annotate && typedecl->Name != NULL && typespec != NULL && typespec->CName != NULL)
    {
      emit (depth, "/* type %s = %s */\n", typedecl->Name, typespec->CName);
    }
  return newtype;
}

void
AnalyseAddRepresentation (char *filename, int line,
                          struct tRepresentationSpec *newrep)
{
  struct tRepresentationClause *rep;
  struct tReference *vref;
  struct tVar *v, *var;
  struct tReference *ref, *refp;

  ref = MakeNewReference ();
  ref->ReferenceType = IDENTIFIER_REFERENCE;
  ref->VarName = newrep->TypeName;
  ref->Next = NULL;
  ref->ReferenceAbbreviation = NULL;    //*****MAKE A PROCEDURE TO INITIALISE tReference variables
  ref->ReferenceAbbreviationEnd = NULL;
  refp = ref;
  var = FindVariable (filename, line, &refp, 16, FALSE, TRUE);
  if (var == NULL)
    return;                     // The error message should already have been logged
  rep = newrep->Representation;
  while (rep != NULL)
    {
      if ((!strcasecmp (rep->Name, "access_finalisation"))
          || (!strcasecmp (rep->Name, "access_finalization")))
        {
          if (var->Type->TypeSpec->BaseType != TYPE_TYPE)
            {
              err (filename, line, "Type expected for 'access_finalisation'");
            }
          else if (!var->Type->TypeSpec->Type->TypeSpec->Controlled)
            {
              err (filename, line, "The type for access_finalisation must be controlled");
            }
          else
            {
              if (rep->Value->Op == _VAR)
                {
                  vref = rep->Value->Var;
                  v = FindVariable (filename, line, &vref, 15, FALSE, TRUE);
                  if (v != NULL)
                    {
                      if (v->Type->TypeSpec->BaseType == PROCEDURE_TYPE)
                        {
                          struct tFormalParameter *f;
                          bool match;

                          f = v->Type->TypeSpec->Formals;
                          match = FALSE;
                          if (f != NULL
                              && v->Type->TypeSpec->ReturnType == NULL)
                            {
                              if (f->Next == NULL)
                                {
                                  if (f->TypeSpec ==
                                      var->Type->TypeSpec->Type->TypeSpec
                                      && f->Mode == MODE_FINAL_IN_OUT)
                                    {
                                      match = TRUE;
                                    }
                                }
                            }
                          if (!match)
                            {
                              err (filename, line,
                                   "'access_finalisation' must name a procedure that accepts a single final in out parameter of the correct type");
                            }
                          var->Type->TypeSpec->Type->
                            TypeSpec->AccessFinalisation = v;
                        }
                    }
                }
              else
                {
                  err (filename, line,
                       "Syntax error in access_finalisation clause: expected procedure name");
                }
            }
        }
      else
        {
          err (filename, line, "Only access_finalisation is allowed in this context");
        }
      rep = rep->Next;
    }
}

struct tUnitDef *
AnalyseUnit (char *filename, int line, struct tUnitTerm *unit)
{
  // All terms must exist
  struct tUnitTerm *u;
  struct tReference *r;
  struct tVar *v;
  struct tUnitDef *terms, *newterm;
  struct tUnitDef *ptr, *newunitdef, *newptr;
  bool found;
  enum tBaseType basetype;

  u = unit;
  newunitdef = NULL;
  ptr = NULL;
  while (u != NULL)
    {
      r = u->Ref;
      v = FindVariable (filename, line, &r, 10, FALSE, TRUE);
      basetype = v->Type->TypeSpec->BaseType;
      if (v->Type->TypeSpec->BaseType != UNIT_TYPE && (!u->GetUnit))
        {
          err (filename, line, "Not a unit and not a 'unit term: %s",
               v->Name);
        }
      if (u->GetUnit)
        {
          if (basetype == INTEGER_TYPE || basetype == FLOATING_TYPE)
            {
              terms = v->Type->TypeSpec->Unit;
            }
          else if (basetype == TYPE_TYPE)
            {
              terms = v->Type->TypeSpec->Type->TypeSpec->Unit;
            }
          else
            {
              err (filename, line, "'unit cannot be applied to %s", v->Name);
              terms = NULL;
            }
        }
      else
        {
          terms = v->Unit;
        }
      // Update or graft in the terms of the unit from the unit that has just been found
      // The generated unit will be normalised so that any given basic unit only
      // appears once.
      while (terms != NULL)
        {
          newptr = newunitdef;
          found = FALSE;
          while (newptr != NULL)
            {
              // A: M*S^-2
              // S^3 * A^2 -> S^3 * M^2 * (S^-2)^2 -> S^3*S^-4 * M^2 -> S^-1 * M*2
              if (newptr->Name == terms->Name)
                {
                  newptr->Power = newptr->Power + terms->Power * u->Power;
                  found = TRUE;
                  break;
                }
              newptr = newptr->Next;
            }
          if (!found)
            {
              // Add the term (raised to the appropriate power) to the output
              newterm = checked_malloc (sizeof *newterm);
              newterm->Name = terms->Name;
              newterm->Power = terms->Power * u->Power;
              newterm->Next = NULL;
              if (ptr == NULL)
                {
                  ptr = newterm;
                  newunitdef = ptr;
                }
              else
                {
                  ptr->Next = newterm;
                  ptr = newterm;
                }
            }
          terms = terms->Next;
        }
      u = u->Next;
    }
  return newunitdef;
}

struct tUnitDef *
AnalyseUnitOrUnique (char *filename, int line, struct tUnitTerm *unit)
{
  if (unit != NULL)
    {
      if (unit->Next == NULL)
        {
          if (unit->Ref == NULL)
            {
              struct tUnitDeclaration unitdecl;

              unitdecl.Line = line;
              unitdecl.UnitName = MakeUniqueName ("_unique_unit");
              unitdecl.unit = NULL;
              return AnalyseAddUnit (0, filename, line, &unitdecl,
                                     HIDDEN_ACCESS);
            }
        }
    }
  return AnalyseUnit (filename, line, unit);
}

struct tUnitDef *
AnalyseAddUnit (int depth, char *filename, int line,
                struct tUnitDeclaration *unitdecl, enum tAccess nameaccess)
{
  struct tVar *v;
  struct tUnitDef *newunitdef;

  CheckForDuplicateOrCaseDifference (filename, line, unitdecl->UnitName);
  CheckForVarOverflow (filename, line);
  if (unitdecl->unit != NULL)
    {
      // unit x is ...
      newunitdef = AnalyseUnit (filename, line, unitdecl->unit);
    }
  else
    {
      // unit x
      // Allocate the proper name of the unit that is independent of the tVar
      char *uname =
        checked_malloc (strlen (globalprefix) + 1 +
                        strlen (unitdecl->UnitName) + 1);
      strcpy (uname, globalprefix);
      strcat (uname, ".");
      strcat (uname, unitdecl->UnitName);
      newunitdef = checked_malloc (sizeof *newunitdef);
      newunitdef->Name = uname;
      newunitdef->Power = 1;
      newunitdef->Next = NULL;
    }
  v = MallocVar (filename, line);
  CurrentVars->Vars[CurrentVars->NumVars] = v;
  v->Name = unitdecl->UnitName;
  v->CName = unitdecl->UnitName;
  v->ConstValue = NULL;
  v->Initialised = TRUE;
  v->Accessed = TRUE;
  v->Access = nameaccess;
  v->ConstUnit = NULL;
  v->ConstAccess = NULL;
  v->ConstAccessType = NULL;
  v->Mode = MODE_GLOBAL;
  v->Unit = newunitdef;
  v->Type = &UnitType;
  CurrentVars->NumVars++;
  return newunitdef;
}

void
InitialiseForConstant (struct tVar *v, char *filename, int line, char *name,
                       enum tAccess nameaccess, struct tUnitDef *unit,
                       struct tReference *accessedobject,
                       struct tTypeSpec *accessedtype,
                       struct tRepresentationClause *representation)
{
  struct tRepresentationClause *rep;
  char *cname;

  cname = NULL;
  rep = representation;
  while (rep != NULL)
    {
      if (!strcasecmp (rep->Name, "cname"))
        {
          if (rep->Value->Op == _CONST_STRING)
            {
              cname = DeQuote (rep->Value->String);
            }
          if (cname == NULL)
            {
              err (filename, line,
                   "Syntax error in 'cname' clause: expected 'cname => \"name\"'");
            }
        }
      else if (!strcasecmp (rep->Name, "build_condition"))
        {
          ;                     // Handled elsewhere
        }
      else
        {
          err (filename, line, "Unknown representation for a constant '%s'",
               rep->Name);
        }
      rep = rep->Next;
    }
  v->Line = line;
  v->Name = name;
  if (cname != NULL)
    {
      v->CName = cname;
    }
  else
    {
      v->CName = MakeCName (name);
    }
  v->ConstValue = NULL;
  v->Initialised = TRUE;
  v->Access = nameaccess;
  v->ConstUnit = unit;
  v->ConstAccess = accessedobject;
  v->ConstAccessType = accessedtype;
  v->Mode = MODE_GLOBAL;
}

void
SetupConstantString (int depth, char *filename, int line, struct tVar *var,
                     struct tNode *expression)
{
  if (CharacterTypespec->CType == NULL)
    {
      err (filename, line,
           "The target type for character has not been defined");
    }
  var->Type = &ConstantStringType;
  SwitchToHeader ();
  emit (0, "extern const %s *%s;\n", CharacterTypespec->CName, var->CName);
  SwitchToSource ();
  if (expression->Op == _CONST_STRING || expression->Op == _CONST_NULL)
    {
      emit (depth, "const %s *%s = %s;\n", CharacterTypespec->CName, var->CName,
            expression->String);
    }
  else
    {
      struct tReference *ref;
      struct tVar *v;

      ref = expression->Var;
      v = FindVariable (filename, line, &ref, 4000, FALSE, TRUE);
      var->CName = v->CName;
    }
}

void
WriteConstantToHeader (int depth, char *filename, int line,
                       enum tBaseType exprtype, struct tVar *v, char *name,
                       bool startofgroup, bool endofgroup)
{
  SwitchToHeader ();
  if (exprtype == INTEGER_TYPE || exprtype == BOOLEAN_TYPE)
    {
      emit (0, "#define %s ", v->CName);
      printbigint (v->High);
      emit (0, "\n");
    }
  else if (exprtype == FLOATING_TYPE)
    {
      emit (0, "#define %s ", v->CName);
      emit (0, "%lf\n", v->FloatVal);
    }
  if (endofgroup)
    {
      emitstr ("\n");
    }
  SwitchToSource ();
  if (Annotate)
    {
      if (startofgroup)
        {
          emit (depth, "/* ");
        }
      else
        {
          emit(depth, " ");
        }
      emit (0, "%s = ", name);
      if (exprtype == INTEGER_TYPE || exprtype == BOOLEAN_TYPE)
        {
          printbigint (v->High);
        }
      else if (exprtype == FLOATING_TYPE)
        emit (0, "%lf", v->FloatVal);
      else if (exprtype == ACCESS_TYPE)
        {
          if (v->ConstAccess == NULL)
            {
              emit (0, "NULL");
            }
          else
            {
              bool bf = FALSE;

              emitstr ("&");
              printreference (filename, line, v->ConstAccess, NULL, &bf,
                              FALSE);
            }
        }
      if (endofgroup)
        {
          if (startofgroup)
            emitstr (" */\n\n");
          else
            {
              emitstr ("\n");
              emit(depth, "*/\n\n");
            }
        }
      else
        {
          emitstr (",\n  ");
        }
    }
}

struct tVar *
AnalyseMakeConstant (int depth, char *filename, int line, char *name,
                     struct tNode *expression, struct tUnitDef *unit,
                     enum tAccess nameaccess,
                     struct tRepresentationClause *representation,
                     bool allowcondition, bool startofgroup, bool endofgroup)
{
  struct tVar *v;
  bool success;
  BIGINT intval;
  BIGFLOAT floatval;
  bool boolval;
  struct tReference *accessedobject;
  struct tTypeSpec *accessedtype;
  bool uncheckedaccess;
  enum tBaseType exprtype;
  struct tUnitDef *exprunit, *constantunit;

  if (!CheckBuildCondition (filename, line, representation, allowcondition))
    {
      // Omit this declaration
      return NULL;
    }
  //
  CheckForDuplicateOrCaseDifference (filename, line, name);
  CheckForVarOverflow (filename, line);
  v = MallocVar (filename, line);
  CurrentVars->Vars[CurrentVars->NumVars] = v;
  if (TestForCString (expression, FALSE, TRUE))
    {
      // Constant strings are a special case because they are not expressions
      InitialiseForConstant (v, filename, line, name, nameaccess, unit, NULL,
                             NULL, representation);
      SetupConstantString (depth, filename, line, v, expression);
    }
  else
    {
      EvalStaticExpression (expression, &exprtype, &intval, &floatval,
                            &boolval, &accessedtype, &uncheckedaccess,
                            &exprunit, &success);
      accessedobject = NULL;
      if (exprtype == INTEGER_TYPE || exprtype == FLOATING_TYPE)
        {
          TestUnitMatch (unit, exprunit, expression, TRUE);
          if (unit == NULL)
            constantunit = exprunit;
          else
            constantunit = unit;
        }
      else if (exprtype == ACCESS_TYPE)
        {
          if ((!success) || (!uncheckedaccess))
            {
              err (filename, line,
                   "Access constants must be unchecked and refer to static objects (globals)");
            }
          if (accessedtype != NULL)
            {
              accessedobject = expression->Var; // else NULL from above
            }
          constantunit = NULL;
        }
      else
        {
          constantunit = NULL;
        }
      InitialiseForConstant (v, filename, line, name, nameaccess,
                             constantunit, accessedobject, accessedtype,
                             representation);
      if (exprtype == INTEGER_TYPE)
        {
          v->High = intval;
          v->Low = intval;
          v->Type = &ConstantIntegerType;
        }
      else if (exprtype == FLOATING_TYPE)
        {
          v->FloatVal = floatval;
          v->Type = &ConstantFloatType;
        }
      else if (exprtype == BOOLEAN_TYPE)
        {
          v->High = boolval;
          v->Low = boolval;
          v->Type = &ConstantBooleanType;
        }
      else if (exprtype == ACCESS_TYPE)
        {
          v->High = intval;
          v->Low = intval;
          v->Type = &ConstantAccessType;
        }
      //
      WriteConstantToHeader (depth, filename, line, exprtype, v, name,
                             startofgroup, endofgroup);
    }
  //
  CurrentVars->NumVars++;
  return v;
}

void
AnalyseAddConstant (int depth, char *filename, int line,
                    struct tConstant *constdecl, enum tAccess nameaccess)
{
  struct tUnitDef *u = AnalyseUnit (filename, line, constdecl->Unit);
  struct tIdentifierListEntry *varlist;

  varlist = constdecl->Names;
  while (varlist != NULL)
    {
      AnalyseMakeConstant (depth, filename, line, varlist->SymbolName,
                           constdecl->Expression, u, nameaccess,
                           constdecl->Representation, TRUE, TRUE, TRUE);
      varlist = varlist->Next;
    }
}

char *
TargetINameToCName (char *filename, int line, char *iname)
{
  int j;

  for (j = 0; j < NumTargetTypes; j++)
    {
      if (TargetTypes[j].BaseType == TARGET_INTEGER)
        {
          if (!strcasecmp (TargetTypes[j].RepresentationName, iname))
            {
              return TargetTypes[j].CName;
            }
        }
    }
  err (filename, line,
       "No target integer type with internal name %s was found", iname);
  return NULL;                  // For the sake of -Wall
}

struct tTargetType *
GetTargetTypeByName (enum tTargetBaseType basetype, char *wordname)
{
  int j;

  for (j = 0; j < NumTargetTypes; j++)
    {
      if (TargetTypes[j].BaseType == basetype
          && (!strcmp (TargetTypes[j].RepresentationName, wordname)))
        return &TargetTypes[j];
    }
  return NULL;
}

struct tUnitDef *
MergeFromList (struct tUnitDef *from, struct tUnitDef *to, int sense)
{
  struct tUnitDef *p, *u, *s;
  bool found;

  p = from;
  while (p != NULL)
    {
      s = to;
      found = FALSE;
      while (s != NULL)
        {
          if (s->Name == p->Name)
            {
              s->Power = s->Power + p->Power * sense;
              found = TRUE;
              break;
            }
          s = s->Next;
        }
      if (!found)
        {
          u = (struct tUnitDef *) checked_malloc (sizeof *u);
          *u = *p;
          u->Power = u->Power * sense;
          u->Next = NULL;
          if (to == NULL)
            {
              to = u;
            }
          else
            {
              s = to;
              while (s->Next != NULL)
                s = s->Next;
              s->Next = u;
            }
        }
      p = p->Next;
    }
  return to;
}

struct tUnitDef *
RemoveNullUnits (struct tUnitDef *u)
{
  struct tUnitDef *p;

  if (u == NULL)
    return u;
  p = u;
  while (p->Next != NULL)
    {
      if (p->Next->Power == 0)
        p->Next = p->Next->Next;
      else
        p = p->Next;
    }
  if (u->Power == 0)
    {
      u = u->Next;
    }
  return u;
}

struct tUnitDef *
MergeUnit (struct tUnitDef *u1, struct tUnitDef *u2, int sense)
{
  struct tUnitDef *u;

  u = MergeFromList (u1, NULL, 1);      // Copy u1 to u
  u = MergeFromList (u2, u, sense);     // merge u2 into u
  u = RemoveNullUnits (u);              // remove ^0 terms from u
  return u;
}

bool
CheckUnitMatch (struct tUnitDef * u1, struct tUnitDef * u2,
                bool AllowMatchToNull)
{
  bool match;

  if (u1 == NULL && u2 == NULL)
    match = TRUE;
  else if (u1 == NULL || u2 == NULL)
    match = AllowMatchToNull;
  else if (MergeUnit (u1, u2, -1) == NULL)
    match = TRUE;
  else
    match = FALSE;
  return match;
}

void
TestUnitMatch (struct tUnitDef *u1, struct tUnitDef *u2,
               struct tNode *expr, bool AllowMatchToNull)
{
  bool match;

  match = CheckUnitMatch (u1, u2, AllowMatchToNull);

  if (!match)
    {
      errstart (expr->FileName, expr->Line);
      if (expr != NULL)
        {
          errcont ("In ");
          errDumpExpr (expr);   //**********FIX THESE ERROR OUTPUTS
          errcont (":\n  ");
        }
      errcont ("Unit ");
      errDumpUnit (u1);
      errcont (" is not compatible with ");
      errDumpUnit (u2);
      errend ();
    }
}

struct tTypeSpec *
GetTypeSpec (char *filename, int line, struct tTypeIdentifier *t,
             enum tAccess *structureaccess)
{
  struct tType *ttype;

  ttype = FindType (filename, line, t);
  if (ttype->TypeSpec != NULL)
    {
      *structureaccess = ttype->TypeSpec->StructureAccess;
    }
  return ttype->TypeSpec;
}

struct tTargetType *
AdjustComputationType (struct tTargetType *comptype, struct tNode *subexpr)
{
  if (subexpr == NULL)
    return comptype;
  if (subexpr->ComputationType == NULL)
    return comptype;
  if (comptype->Max <= subexpr->ComputationType->Max
      && comptype->Min >= subexpr->ComputationType->Min
      && subexpr->ComputationType->Preferred && (!comptype->Preferred))
    return subexpr->ComputationType;
  return comptype;
}

void SetLRUnit(struct tUnitDef **unit, struct tUnitDef *lunit, struct tUnitDef *runit)
{
  if (lunit != NULL)
    {
      *unit = lunit;
    }
  else
    {
      *unit = runit;
    }
}


void
TestCopy (struct tReference *lhsref, struct tVar *v,
          struct tTypeSpec *reftype, struct tNode *expr, bool parameter)
{
  BIGINT minvalue, maxvalue, initialmin, initialmax;
  enum tBaseType basetype;
  struct tVar *basevar;
  bool initialised;
  struct tUnitDef *unit;
  struct tTypeSpec *accessedtype;
  struct tTypeSpec *ref;
  enum tMode mode;
  int lock;
  bool match, wholeobject, constantaccess, subsystemboundary;

  if (expr->Op == _VAR)
    {
      AnalyseReference (expr->FileName, expr->Line, expr->Var, &basetype,
                        &basevar, &ref, &minvalue, &maxvalue, &initialmin,
                        &initialmax, &unit, &accessedtype, &initialised,
                        &mode, &lock, parameter, &wholeobject, FALSE,
                        &constantaccess, FALSE, &subsystemboundary);
      RecordGlobalAccess (expr->Line, GetGlobalsAccess (), basevar,
                          TRUE, FALSE, !basevar->Initialised,
                          FALSE);
      if (ref != reftype)
        {
          match = FALSE;
          if (ref->BaseType == ARRAY_TYPE && reftype->BaseType == ARRAY_TYPE)
            {
              if (ref->ElementTypeSpec == reftype->ElementTypeSpec
                  && CheckUnitMatch (ref->IndexTypeSpec->Unit,
                                     reftype->IndexTypeSpec->Unit, FALSE))
                {
                  if (!(reftype->MinDefinite && reftype->MaxDefinite))
                    match = TRUE;
                  else if (IsSlice (expr->Var))
                    match = TRUE;
                  else if (lhsref != NULL)
                    {
                      if (IsSlice (lhsref))
                        match = TRUE;
                    }
                }
            }
          if (!match)
            {
              errstart (expr->FileName, expr->Line);
              errDumpReference (expr->Var);
              errcont (" is not of the same type as the target");
              errend ();
            }
        }
      if (!initialised)
        {
          errstart (expr->FileName, expr->Line);
          errDumpReference (expr->Var);
          errcont (" might not be initialised (5)");
          errend ();
        }
    }
  else
    {
      err (expr->FileName, expr->Line,
           "Only simple copies are allowed for arrays and records");
    }
}

bool
TestForCString (struct tNode *expr, bool parameter, bool constantonly)
{
  bool initialised, lock, wholeobject;
  enum tMode mode;
  enum tBaseType basetype;
  struct tVar *basevar;
  struct tTypeSpec *reftype;
  BIGINT minvalue, maxvalue, initialmin, initialmax;
  struct tUnitDef *unit;
  struct tTypeSpec *accessedtype;
  bool constantaccess, subsystemboundary;

  if (expr->Op == _CONST_STRING || expr->Op == _CONST_NULL)
    {
      return TRUE;
    }
  if (expr->Op == _VAR)
    {
      AnalyseReference (expr->FileName, expr->Line, expr->Var, &basetype,
                        &basevar, &reftype, &minvalue, &maxvalue, &initialmin,
                        &initialmax, &unit, &accessedtype, &initialised,
                        &mode, &lock, parameter, &wholeobject, FALSE,
                        &constantaccess, FALSE, &subsystemboundary);
      if (!initialised)
        {
          err (expr->FileName, expr->Line, "%s might not be initialised (6)",
               expr->Var->VarName);
        }
      if (reftype->BaseType == CSTRING_TYPE)
        {
          if ((reftype == ConstantStringTypespec) || (!constantonly))
            {
              return TRUE;
            }
        }
    }
  return FALSE;
}

void UpdateVarRange(struct tVar *v, BIGINT minval, BIGINT maxval)
{
  if (v != NULL)
    {
      if (minval < v->Low)
        {
          /* The extra tests here against the range of the type are only present
           * in case we are carrying on after an error that was detected above
           */
          if (minval < v->Type->TypeSpec->Low)
            v->Low = v->Type->TypeSpec->Low;
          else if (!v->AccessVolatile)
            v->Low = minval;
        }
      else if (minval > v->Low && minval <= v->Type->TypeSpec->High && (!v->AccessVolatile))
        {
          v->Low = minval;
        }
      if (maxval > v->High)
        {
          if (maxval > v->Type->TypeSpec->High)
            v->High = v->Type->TypeSpec->High;
          else if (!v->AccessVolatile)
            v->High = maxval;
        }
      else if (maxval < v->High && maxval >= v->Type->TypeSpec->Low && (!v->AccessVolatile))
        {
          v->High = maxval;
        }
    }
}

// reftype refers to the target of the assignment (simple variable, record field, array element etc.)
// v is non-NULL if the LHS is a simple variable
void
TestAssignment (struct tReference *lhsref, struct tVar *v,
                struct tTypeSpec *reftype, struct tNode *expr, bool parameter,
                bool makenew, bool allowcstringparameter, BIGINT * minval,
                BIGINT * maxval)
{
  struct tUnitDef *unit;
  int digits, magnitude;
  bool virtualexpr = FALSE;     // FALSE by default for some of the cases

  if (makenew && (reftype->BaseType != ACCESS_TYPE))
    {
      err (expr->FileName, expr->Line,
           "'new' can only be used for access types");
    }
  if (makenew && (reftype->PersistentAccess))
  {
      err (expr->FileName, expr->Line,
           "'new' can only be used for non-persistent access types");
  }

  if (reftype->BaseType == INTEGER_TYPE)
    {
      TestIntegerExpression (expr, minval, maxval, &unit, parameter, FALSE,
                             &virtualexpr);
      TestUnitMatch (reftype->Unit, unit, expr, FALSE);
      if (*minval < reftype->Low || *maxval > reftype->High)
        {
          if (CurrentExemptions.UncheckedRange)
            {
              if (*minval < reftype->Low)
                {
                  *minval = reftype->Low;
                }
              if (*maxval > reftype->High)
                {
                  *maxval = reftype->High;
                }
            }
          else
            {
              errstart (expr->FileName, expr->Line);
              errcont ("Potential out of range error: ");
              errDumpTypeSpec (reftype);
              errcont (" has range %" PRINTBIG "..%" PRINTBIG
                       ", expression has range %" PRINTBIG "..%" PRINTBIG "",
                       reftype->Low, reftype->High, *minval, *maxval);
              errend ();
            }
        }
      UpdateVarRange(v, *minval, *maxval);
    }
  else if (reftype->BaseType == BOOLEAN_TYPE)
    {
      TestBooleanExpression (expr, parameter, minval, maxval, FALSE,
                             &virtualexpr);
      UpdateVarRange(v, *minval, *maxval);
    }
  else if (reftype->BaseType == ARRAY_TYPE)
    {
      if (TestForCString (expr, parameter, FALSE))
        {
          if (reftype->ElementTypeSpec != CharacterTypespec)
            {
              err (expr->FileName, expr->Line,
                   "strings can only be assigned to arrays of character");
            }
        }
      else
        {
          TestCopy (lhsref, v, reftype, expr, parameter);
        }
    }
  else if (reftype->BaseType == RECORD_TYPE)
    {
      TestCopy (lhsref, v, reftype, expr, parameter);
    }
  else if (reftype->BaseType == FLOATING_TYPE)
    {
      TestFloatExpression (expr, &unit, &digits, &magnitude, parameter,
                           &virtualexpr);
      TestUnitMatch (reftype->Unit, unit, expr, FALSE);
    }
  else if (reftype->BaseType == CSTRING_TYPE)
    {
      bool match, initialised, lock, wholeobject;
      enum tMode mode;
      enum tBaseType basetype;
      struct tVar *basevar;
      struct tTypeSpec *ref;
      struct tTypeSpec *accessedtype;
      bool constantaccess, subsystemboundary;
      BIGINT initialmin, initialmax;

      match = FALSE;
      if (TestForCString (expr, parameter, FALSE))
        {
          match = TRUE;
        }
      else if (expr->Op == _VAR)
        {
          AnalyseReference (expr->FileName, expr->Line, expr->Var, &basetype,
                            &basevar, &ref, minval, maxval, &initialmin,
                            &initialmax, &unit, &accessedtype, &initialised,
                            &mode, &lock, parameter, &wholeobject, FALSE,
                            &constantaccess, FALSE, &subsystemboundary);
          if (ref->BaseType == ARRAY_TYPE)
            {
              if (ref->ElementTypeSpec == CharacterTypespec)
                {
                  match = TRUE;
                }
            }
          if (basevar->Mode != MODE_GLOBAL && basevar->Mode != MODE_SHARED
              && (!allowcstringparameter))
            {
              err (expr->FileName, expr->Line,
                   "strings can only reference global character arrays, except as 'in' parameters of functions and closed procedures");
            }
          if (!initialised)
            {
              err (expr->FileName, expr->Line,
                   "%s might not be initialised (7)", expr->Var->VarName);
            }
        }
      else if (expr->Op == _FN_CALL)
        {
          struct tProcedureCall *p = expr->Call->Ptr;
          struct tTypeSpec *returntype;
          bool tmpbool, success;
          struct tShareClause *shareclause;

          AnalyseProcedureCall (expr->FileName, expr->Line, p, &returntype,
                                &tmpbool, &success, TRUE, FALSE,
                                &shareclause);
          if (returntype != NULL)
            {
              if (returntype->BaseType == CSTRING_TYPE)
                {
                  match = TRUE;
                }
            }
        }
      if (!match)
        {
          err (expr->FileName, expr->Line,
               "string or array of character expected");
        }
    }
  else if (reftype->BaseType == ACCESS_TYPE)
    {
      bool couldbenull, isnull, isaccessvalue;
      struct tTypeSpec *exprtype;
      bool uncheckedaccess, accessconstant, accesspersistent, accessnew, addressexpr;
      BIGINT testminval, testmaxval;

      if (makenew)
        {
          TestAssignment (lhsref, NULL, reftype->ElementTypeSpec, expr,
                          parameter, FALSE, allowcstringparameter,
                          &testminval, &testmaxval);
        }
      else
        {
          TestAccessExpression (expr, parameter, &couldbenull, &isnull,
                                &exprtype, &uncheckedaccess, &isaccessvalue,
                                minval, maxval, &virtualexpr, &accessconstant,
                                &accesspersistent, &accessnew, &addressexpr);
          if (isaccessvalue)
            {
              // Check the type
              if ((!isnull) && exprtype != NULL)
                {
                  // exprtype = NULL and not isnull would indicate an address value
                  if (exprtype != reftype->ElementTypeSpec)
                    {
                      err (expr->FileName, expr->Line, "Type mismatch");
                    }
                }
            }
          // Checks the kind of access values, unless access conversion exemption is in force
          if (isaccessvalue && (!CurrentExemptions.AccessConversion))
            {
              if (!isnull)
                {
                  if (exprtype != NULL)
                    {
                      // exprtype = NULL and not isnull would indicate an address value
                      if ((!uncheckedaccess) !=
                               (!reftype->UnmanagedAccess))
                        {
                          err (expr->FileName, expr->Line,
                               "Managed and unmanaged access values cannot be mixed directly");
                        }
                    }
                }
              if (couldbenull)
                {
                  if (!reftype->CanBeNull)
                    {
                      err (expr->FileName, expr->Line,
                           "Possible assignment of null to a non-null access variable");
                    }
                }
              if (accessconstant && (!reftype->ConstantAccess))
                {
                  err (expr->FileName, expr->Line,
                       "Assignment of access constant value to general access variable");
                }
              if (addressexpr && (!reftype->UnmanagedAccess))
                {
                  err (expr->FileName, expr->Line,
                       "Address values cannot be assigned to managed access variables");
                }
              if (accesspersistent && (!reftype->PersistentAccess))
              {
            	  err(expr->FileName, expr->Line,
            			  "Assignment of persistent access type to non-persistent access variable");

              }
              if ((!accesspersistent) && reftype->PersistentAccess)
              {
            	  err(expr->FileName, expr->Line,
            			  "Assignment of non-persistent access value to persistent access variable");

              }
              if (uncheckedaccess && (!accessnew) && (!isnull) && reftype->NewAccess)
              {
            	  err(expr->FileName, expr->Line,
            			  "Assignment of non-new access value to access new variable");

              }
              if (uncheckedaccess && (!accesspersistent) && (!accessnew) && (lhsref != NULL) && (!isnull)) {
            	  if (lhsref->ReferenceType == IDENTIFIER_REFERENCE) {
            		  struct tVar *v;
            		  struct tReference *ref;

            		  ref = lhsref;
            		  v = FindVariable (expr->FileName, expr->Line, &ref, 1, FALSE, TRUE);
            		  if (v->Mode == MODE_GLOBAL || v->Mode == MODE_SHARED) {
            			  err(expr->FileName, expr->Line,
            				  "Non-persistent unmanaged non-new access values cannot be assigned to global variables");
            		  }
            	  }
              }
            }
          if (v != NULL)
            {
              v->Low = *minval;
              v->High = *maxval;
            }
        }
    }
  else if (reftype->BaseType == ADDRESS_TYPE)
    {
      bool couldbenull, isnull, isaccessvalue;
      struct tTypeSpec *exprtype;
      bool uncheckedaccess, accessconstant, accesspersistent, accessnew, addressexpr;

      TestAccessExpression (expr, parameter, &couldbenull, &isnull,
                            &exprtype, &uncheckedaccess, &isaccessvalue,
                            minval, maxval, &virtualexpr, &accessconstant, &accesspersistent,
                            &accessnew, &addressexpr);
      if ((!uncheckedaccess) && (!CurrentExemptions.AccessConversion))
        {
          err (expr->FileName, expr->Line,
               "Managed access values cannot be assigned to address variables");
        }
      if (v != NULL)
        {
          v->Low = 0;
          v->High = 1;
        }
    }
  else
    {
      err (expr->FileName, expr->Line, "Illegal assignment");
    }
  if (virtualexpr)
    {
      if (v == NULL)
        {
          if (parameter)
            {
              err (expr->FileName, expr->Line,
                   "Virtual expressions cannot be passed as parameters");
            }
          else
            {
              err (expr->FileName, expr->Line,
                   "Assignments of virtual expressions can only be made to variables");
            }
        }
      else
        {
          if (!v->AccessVirtual)
            {
              err (expr->FileName, expr->Line,
                   "Assignments of virtual expressions can only be made to virtual variables");
            }
        }
    }
  if (v != NULL)
    {
      SetInitialised (v);
    }
}

void
ControlledReinitError (char *filename, int line)
{
  note
    ("A controlled variable must be initialised before being used for the first\n"
     "time, and must be finalised before being initialised again. A controlled\n"
     "global can only be initialised within the body of a package initialisation\n"
     "section; it can be passed as an 'out' parameter from there in order to\n"
     "initialise it, or assigned directly. A controlled global can only be\n"
     "finalised within the body of a package finalisation section\n");
  err_and_continue (filename, line,
                    "Controlled variables can only be initialised once before being finalised");
}

void
AnalyseArrayAssignment (int depth, char *filename, int line,
                        struct tTypeSpec *reftype,
                        struct tArrayInitialisation *arr,
                        struct tReference *basename, bool baseinitialised,
                        enum tBaseType basebasetype, bool * elementsfinalised,
                        bool makenew)
{
  struct tVar *v;
  struct tVariableDeclaration var;
  int SavedNumVars, SavedNumRelations;
  struct tType *paramtype;
  struct tReference *refp, *prevp, *savedrefpnext;
  struct tReference *indexref;
  struct tNode indexexpr;
  struct tReference *indexvarref;
  char *oldrefa;
  struct tReference *oldrefaend;
  struct tTypeSpec *arrtype;
  bool slice;
  struct tNode *lowexpr, *highexpr;
  bool bf;
  enum tReferenceType slicetype;
  char *relop;
  BIGINT minlow, maxlow, minhigh, maxhigh;
  struct tReference *simplebaseref;     // Only used for simple variables
  struct tVar *simplebasevar;
  struct tVarListEntry *vfirst;

  indexref = MakeNewReference ();
  indexvarref = MakeNewReference ();
  if (reftype->BaseType == ACCESS_TYPE)
    {
      if (makenew)
        {
          if (baseinitialised)
            {
              // Free the pointer
              if (RequiresFreeing (reftype))
                {
                  CallFreeExternal (depth, filename, line, basename, reftype,
                                    FALSE);
                }
            }
          CallAllocate (depth, filename, line, basename, reftype);
          baseinitialised = TRUE;
        }
      else if (!baseinitialised)
        {
          err (filename, line,
               "Attempt to assign through an uninitialised access variable");
        }
      else if (reftype->CanBeNull)
        {
          err (filename, line, "Attempt to assign through a possibly null access variable");
        }
      else if (reftype->PersistentAccess)
      {
    	  err(filename, line, "Attempt to assign through a persistent access variable");
      }
      arrtype = reftype->ElementTypeSpec;
    }
  else
    arrtype = reftype;

  if (arrtype->BaseType != ARRAY_TYPE)
    {
      err (filename, line, "array expected on lhs");
      return;
    }
  //
  CheckForSlice (basename, &slice, &lowexpr, &highexpr, &slicetype);
  if (slice)
    {
      enum tBaseType exprtype;
      int digits, magnitude;
      bool staticexpression;
      BIGINT intval;
      BIGFLOAT floatval;
      bool boolval;
      struct tUnitDef *unitlow, *unithigh;
      struct tTypeSpec *accessed;
      bool ucheckedaccess, accessconstant, accesspersistent, accessnew;
      bool virtualexpr;

      if (!arrtype->MinDefinite)
        {
          err (filename, line,
               "A slice of a formal parameter with an indefinite low bound cannot be assigned to");
        }
      AnalyseExpression (lowexpr,
                         &exprtype,
                         &minlow, &maxlow,
                         &digits, &magnitude,
                         &unitlow,
                         &staticexpression, &intval, &floatval, &boolval,
                         &accessed, &ucheckedaccess,
                         FALSE, FALSE, FALSE, &virtualexpr, &accessconstant, &accesspersistent, &accessnew);
      if (exprtype != INTEGER_TYPE)
        {
          err (filename, line,
               "Integer expression expected in low part of array slice");
        }
      if (virtualexpr)
        {
          err (filename, line,
               "Virtual expressions cannot be used as bounds");
        }
      if (highexpr == NULL)
        {
          minhigh = arrtype->IndexTypeSpec->High;
          maxhigh = minhigh;
        }
      else
        {
          if (!arrtype->MaxDefinite)
            {
              err (filename, line,
                   "A slice of a formal parameter with an indefinite high bound can only be assigned to if it is of the form a[x..]");
            }
          AnalyseExpression (highexpr,
                             &exprtype,
                             &minhigh, &maxhigh,
                             &digits, &magnitude,
                             &unithigh,
                             &staticexpression, &intval, &floatval, &boolval,
                             &accessed, &ucheckedaccess,
                             FALSE, FALSE, FALSE, &virtualexpr,
                             &accessconstant, &accesspersistent, &accessnew);
          if (exprtype != INTEGER_TYPE)
            {
              err (filename, line,
                   "Integer expression expected in high part of array slice");
            }
          if (virtualexpr)
            {
              err (filename, line,
                   "Virtual expressions cannot be used as bounds");
            }
        }
      //*****unit match
      if (slicetype == ARRAY_SLICE_REFERENCE)
        {
          if (maxlow > minhigh)
            {
              err (filename, line,
                   "The low bound of the slice (<= %" PRINTBIG
                   ") is potentially greater than the high bound (>= %"
                   PRINTBIG ")", maxlow, minhigh);
            }
          if (minlow < arrtype->IndexTypeSpec->Low
              || maxhigh > arrtype->IndexTypeSpec->High)
            {
              err (filename, line,
                   "The bounds of the slice (%" PRINTBIG "..%" PRINTBIG
                   ")..(%" PRINTBIG "..%" PRINTBIG
                   ") are potentially outside the bounds of the array %"
                   PRINTBIG "..%" PRINTBIG "", minlow, maxlow, minhigh,
                   maxhigh, arrtype->IndexTypeSpec->Low,
                   arrtype->IndexTypeSpec->High);
            }
        }
      else
        {
          if (minhigh <= 0)
            {
              err (filename, line,
                   "The length of the slice (%" PRINTBIG "..%" PRINTBIG
                   ") is potentially negative or zero", minhigh, maxhigh);
            }
          if (minlow < arrtype->IndexTypeSpec->Low
              || minlow + maxhigh > arrtype->IndexTypeSpec->High)
            {
              err (filename, line,
                   "The bounds of the slice (%" PRINTBIG "..%" PRINTBIG
                   ")..(%" PRINTBIG "..%" PRINTBIG
                   ") are potentially outside the bounds of the array %"
                   PRINTBIG "..%" PRINTBIG "", minlow, maxlow,
                   minlow + minhigh, maxlow + maxhigh,
                   arrtype->IndexTypeSpec->Low, arrtype->IndexTypeSpec->High);
            }
        }
    }
  // Find the array variable for use below
  if (basename->Next == NULL)
    {
      simplebaseref = basename;
      simplebasevar =
        FindVariable (filename, line, &simplebaseref, 801, TRUE, TRUE);
    }
  else
    {
      simplebasevar = NULL;
    }
  // Find the last entry in the target variable's reference chain and
  // graft in the additional index
  prevp = basename;
  refp = basename;
  while (refp->Next != NULL)
    {
      prevp = refp;
      refp = refp->Next;
    }
  if (slice)
    refp = prevp;
  indexref->ReferenceType = ARRAY_REFERENCE;
  indexref->ReferenceAbbreviation = NULL;
  indexref->ReferenceAbbreviationEnd = NULL;
  indexref->Next = NULL;
  savedrefpnext = refp->Next;
  refp->Next = indexref;        // Will restore this below
  //
  oldrefa = basename->ReferenceAbbreviation;
  oldrefaend = basename->ReferenceAbbreviationEnd;
  //
  basename->ReferenceAbbreviation = NULL;
  basename->ReferenceAbbreviationEnd = NULL;
  //
  if (arr->ExplicitInit == NULL)
    {
      bool definitemax;

      // Copy the current variable state
      SavedNumVars = CurrentVars->NumVars;
      SavedNumRelations = CurrentVars->NumRelations;
      var.ConstValue = NULL;
      var.Mode = MODE_LOCAL;
      var.TypeName = NULL;
      var.TypeSpec = NULL;
      var.VarNames = MakeIdentifierListEntry(arr->VarName);
      var.Representation = NULL;
      var.PublicOut = FALSE;
      paramtype = AllocateTType ();
      paramtype->Name = NULL;
      paramtype->NameAccess = PRIVATE_ACCESS;
      paramtype->TypeSpec = arrtype->IndexTypeSpec;
      paramtype->Next = NULL;
      vfirst =
        AnalyseAddVar (depth, filename, line, &var, paramtype, PRIVATE_ACCESS,
                       FALSE, PRIVATE_ACCESS);
      v = vfirst->Var;
      if (slice)
        {
          v->Low = minlow;
          if (slicetype == ARRAY_SLICE_REFERENCE)
            {
              v->High = maxhigh;
            }
          else
            {
              v->High = maxlow + maxhigh - 1;
            }

        }                       // then
      SetInitialised (v);
      //
      // For simple base variables, set relations between the index variable and
      // the bounds of the array
      // indexvar >= array'first, indexvar <= array'last
      if (simplebasevar != NULL)
        {
          // Simple name
          bool change;

          StoreRelation (filename, line, v, ATTR_NULL, simplebasevar,
                         ATTR_FIRST, _GEQ, 0, 0, &change);
          StoreRelation (filename, line, v, ATTR_NULL, simplebasevar,
                         ATTR_LAST, _LEQ, 0, 0, &change);
        }
      //
      indexvarref->ReferenceType = IDENTIFIER_REFERENCE;
      indexvarref->Next = NULL;
      indexvarref->VarName = var.VarNames->SymbolName;
      indexvarref->ReferenceAbbreviation = NULL;
      indexvarref->ReferenceAbbreviationEnd = NULL;
      indexexpr.Op = _VAR;
      indexexpr.Var = indexvarref;
      indexexpr.Attribute = NULL;
      indexexpr.Line = line;
      if (arrtype->ElementTypeSpec->StructureAccess == HIDDEN_ACCESS)
        {
          err (filename, line, "The type of the array elements (%s) is hidden", arrtype->ElementTypeSpec->Name);
        }
      indexref->Index = &indexexpr;
      //----------
      // Code a loop
      //
      emit (depth, "{ %s %s = ", DefaultArrayIndex, indexvarref->VarName);
      if (slice)
        {
          bf = FALSE;
          printexpr (lowexpr, &bf, FALSE);
        }
      else if (arrtype->MinDefinite)
        {
          printbigint (paramtype->TypeSpec->Low);
        }
      else
        {
          emit (0, "%s", MinName (basename->VarName));
        }
      emitstr (";\n");
      definitemax = FALSE;
      if (slice)
        {
          if (slicetype == ARRAY_SLICE_REFERENCE)
            {
              if (highexpr == NULL)
                {
                  if (arrtype->MaxDefinite)
                    {
                      definitemax = TRUE;
                    }
                  else
                    {
                      emit (depth + 1, "%s _end_%s = %s;\n",
                            DefaultArrayIndex, indexvarref->VarName,
                            MaxName (basename->VarName));
                    }
                }
              else
                {
                  emit (depth + 1, "%s _end_%s = ", DefaultArrayIndex,
                        indexvarref->VarName);
                  bf = FALSE;
                  printexpr (highexpr, &bf, FALSE);
                  emitstr (";\n");
                }
            }
          else
            {
              emit (depth + 1, "%s _end_%s = ", DefaultArrayIndex,
                    indexvarref->VarName);
              emitstr ("(");
              bf = FALSE;
              printexpr (lowexpr, &bf, FALSE);
              emitstr (") + (");
              bf = FALSE;
              printexpr (highexpr, &bf, FALSE);
              emitstr (");\n");
            }
        }
      else if (arrtype->MaxDefinite)
        {
          definitemax = TRUE;
        }
      else
        {
          emit (depth + 1, "%s _end_%s = %s;\n", DefaultArrayIndex,
                indexvarref->VarName, MaxName (basename->VarName));
        }
      emit (depth + 1, "");
      printtypename (arrtype->ElementTypeSpec);
      emit (0, " *_ptr_%s = &", indexvarref->VarName);
      {
        bool bitfield = FALSE;

        printreference (filename, line, basename, NULL, &bitfield, FALSE);
        if (bitfield)
          {
            err (filename, line, "Cannot take a reference to a bitfield");
          }
      }
      emit (0, ";\n");
      //printf("for (%s = ", indexvarref.VarName);
      if (slice && slicetype == ARRAY_SLICE_LENGTH_REFERENCE)
        {
          relop = "<";
        }
      else
        {
          relop = "<=";
        }
      emit (depth + 1, "while (%s %s ", indexvarref->VarName, relop);
      if (definitemax)
        {
          printbigint (paramtype->TypeSpec->High);
        }
      else
        {
          emit (0, "_end_%s", indexvarref->VarName);
        }
      emit (0, ") {\n");
      basename->ReferenceAbbreviation = checked_malloc (7 + strlen (indexvarref->VarName) + 1 + 1); //8 + 1
      strcpy (basename->ReferenceAbbreviation, "(*_ptr_");
      strcat (basename->ReferenceAbbreviation, indexvarref->VarName);
      strcat (basename->ReferenceAbbreviation, ")");
      basename->ReferenceAbbreviationEnd = indexref;
      //----------
      //******check makenew?
      AnalyseAssignment (depth + 2, filename, line, arrtype->ElementTypeSpec,
                         arr->RHS, basename, baseinitialised, basebasetype,
                         elementsfinalised, FALSE, TRUE, TRUE);
      //----------
      emit (depth + 2, "%s++; _ptr_%s++;\n", indexvarref->VarName,
            indexvarref->VarName);
      //*****CHECK FOR WRAP
      emit (depth + 1, "}\n");
      emit (depth, "}\n");
      //----------
      free (basename->ReferenceAbbreviation);
      //
      CurrentVars->NumVars = SavedNumVars;
      CurrentVars->NumRelations = SavedNumRelations;
      //CleanVars(CurrentVars);
      //
      AddToLocals ("", DefaultArrayIndex);
      AddToLocals ("", DefaultArrayIndex);
      AddToLocals ("*", gettypename (arrtype->ElementTypeSpec));
    }
  else
    {
      // Code the individual cases
      struct tArrayInitialisationEntry *a;
      struct tRange *r;
      BIGINT minvalue, maxvalue, n, incrementingindex;

      if (slice)
        {
          err (filename, line, "An array slice cannot be assigned by individual elements");     //*****with static range would be possible
        }
      a = arr->ExplicitInit;
      if (a->Ranges == NULL)
        {
          // [ 1, 2, 3 ] form; count the entries
          unsigned numentries;
          unsigned expectedentries =
            (arrtype->IndexTypeSpec->High - arrtype->IndexTypeSpec->Low) + 1;
          struct tArrayInitialisationEntry *aptr;

          numentries = 0;
          aptr = a;
          while (aptr != NULL)
            {
              numentries++;
              aptr = aptr->Next;
            }
          if (numentries != expectedentries)
            {
              err (filename, line,
                   "Incorrect number of entries in array initialisation expression, expected %u, got %u",
                   expectedentries, numentries);
            }
        }
      else
        {
          // [ x1 => 1, x2 => 2, x3 => 3 ] form; check the cases
          CheckRanges (filename, line, NULL, a, arrtype->IndexTypeSpec->Low,
                       arrtype->IndexTypeSpec->High, arrtype->IndexTypeSpec);
        }
      indexexpr.Attribute = NULL;
      indexexpr.Op = _CONST_INT;
      indexexpr.Line = line;
      indexref->Index = &indexexpr;
      incrementingindex = arrtype->IndexTypeSpec->Low;  // for [ 1, 2, 3 ] form
      while (a != NULL)
        {
          r = a->Ranges;
          if (r == NULL)
            {
              // [ 1, 2, 3 ] form
              indexexpr.Value = incrementingindex;
              //******check makenew?
              AnalyseAssignment (depth, filename, line,
                                 arrtype->ElementTypeSpec, a->RHS, basename,
                                 baseinitialised, basebasetype,
                                 elementsfinalised, FALSE, TRUE, TRUE);
              incrementingindex++;
            }
          else
            {
              // [ x1 => 1, x2 => 2, x3 => 3 ] form
              if (r->Next != NULL)
                {
                  // For X | Y ... => expr, check that expr is static
                  // This test is there to avoid possible mistakes of interpretation by the programmer
                  // e.g. does X | Y => expr mean X => expr, Y => expr, or tmp = expr, X => tmp, Y => tmp ?
                  // If expr is static then it doesn't matter.
                  // Check for constant string first, then for other static expressions
                  if (!TestForCString (a->RHS->Expr, FALSE, TRUE))
                    {
                      enum tBaseType exprtype;
                      BIGINT intval;
                      BIGFLOAT floatval;
                      bool boolval;
                      struct tTypeSpec *accessedtype;
                      bool uncheckedaccess;
                      struct tUnitDef *exprunit;
                      bool success;
                      EvalStaticExpression (a->RHS->Expr, &exprtype, &intval,
                                            &floatval, &boolval,
                                            &accessedtype, &uncheckedaccess,
                                            &exprunit, &success);
                      if (!success)
                        {
                          err (filename, line,
                               "The RHS of constructions with multipart LHS 'X | Y ... =>' must be simple static expressions");
                        }
                    }
                }
              while (r != NULL)
                {
                  minvalue =
                    EvalCaseValue (filename, line, r->MinValue,
                                   arrtype->IndexTypeSpec);
                  maxvalue =
                    EvalCaseValue (filename, line, r->MaxValue,
                                   arrtype->IndexTypeSpec);
                  for (n = minvalue; n <= maxvalue; n++)
                    {
                      indexexpr.Value = n;
                      //******check makenew?
                      AnalyseAssignment (depth, filename, line,
                                         arrtype->ElementTypeSpec, a->RHS,
                                         basename, baseinitialised,
                                         basebasetype, elementsfinalised,
                                         FALSE, TRUE, TRUE);
                    }
                  r = r->Next;
                }
            }
          a = a->Next;
        }
    }
  //
  basename->ReferenceAbbreviation = oldrefa;
  basename->ReferenceAbbreviationEnd = oldrefaend;
  //
  refp->Next = savedrefpnext;
}

struct tFieldSpec *
FindField (struct tFieldSpec *fields, char *name, unsigned *fieldnumber)
{
  unsigned fnum;

  fnum = 0;
  while (fields != NULL)
    {
      if (!strcmp (fields->Name, name))
        {
          *fieldnumber = fnum;
          return fields;
        }
      fields = fields->Next;
      fnum++;
    }
  return NULL;
}

void
AnalyseRecordAssignment (int depth, char *filename, int line,
                         struct tTypeSpec *reftype,
                         struct tFieldInitialisation *f,
                         struct tReference *basename, bool baseinitialised,
                         enum tBaseType basebasetype, bool * recordfinalised,
                         bool makenew)
{
  struct tFieldSpec *field;
  struct tTypeSpec *fieldtype;
  struct tReference *refp;
  struct tReference *fieldref;
  int actualcount, typefields, numfinalised, controlledfields;
  unsigned fieldnumber;
  bool finalisation, finalised, allfinalised;
  struct tTypeSpec *rectype;
  bool bf, allfound;

  fieldref = MakeNewReference ();
  //**********CHECK FOR DUPLICATES IN f!!!
  if (reftype->BaseType == ACCESS_TYPE)
    {
      if (makenew)
        {
          if (baseinitialised)
            {
              if (RequiresFreeing (reftype))
                {
                  // Free the pointer
                  CallFreeExternal (depth, filename, line, basename, reftype,
                                    FALSE);
                }
            }
          CallAllocate (depth, filename, line, basename, reftype);
          baseinitialised = TRUE;
        }
      else if (!baseinitialised)
        {
          err (filename, line,
               "Attempt to assign through an uninitialised access variable");
        }
      else if (reftype->CanBeNull)
        {
          err (filename, line, "Attempt to assign through a possibly null access variable");
        }
      else if (reftype->PersistentAccess)
      {
    	  err(filename, line, "Attempt to assign through a persistent access variable");
      }
      rectype = reftype->ElementTypeSpec;
    }
  else
    {
      rectype = reftype;
    }
  finalisation = FALSE;
  allfinalised = TRUE;
  actualcount = 0;
  numfinalised = 0;
  //
  // Remove trailing .all, because x.all := (a => 1, b => 2) means x->a := 1; x->b := 2
  allfound = FALSE;
  refp = basename;
  while (refp->Next != NULL)
    {
      if (refp->Next->ReferenceType == FIELD_REFERENCE
          && refp->Next->Next == NULL)
        {
          if (!strcasecmp (refp->Next->VarName, "all"))
            {
              refp->Next = NULL;
              allfound = TRUE;
              break;
            }
        }
      refp = refp->Next;
    }
  if (allfound && makenew)
    {
      err (filename, line, "'new' cannot be used with 'all' here");
    }
  if (reftype->BaseType == ACCESS_TYPE && (!allfound) && (!makenew))
    {
      err (filename, line, "'new' expected");
    }
  //
  while (f != NULL)
    {
      field = FindField (rectype->FieldList, f->Name, &fieldnumber);
      if (field == NULL)
        {
          err (filename, line, "Field %s not found", f->Name);
          return;
        }
      fieldtype = field->Typespec;
      if (fieldtype == NULL)
        {
          if (f->RHS != NULL)
            {
              err (filename, line,
                   "Null fields must be initialised by name only (i.e. (name =>))");
            }
        }
      else
        {
          if (f->RHS == NULL)
            {
              err (filename, line,
                   "A non-null field must have an initialisation expression (name => expr)");
            }
          if (fieldtype->StructureAccess == HIDDEN_ACCESS)
            {
              bool hiddenhere = TRUE;

              // Allow procedure calls that initialise the field as an out parameter, e.g. x => proc(this),
              // as opposed to function calls, e.g. x => fn()
              if (f->RHS->Expr->Op == _FN_CALL)
                {
                  struct tProcedureCall *p = f->RHS->Expr->Call->Ptr;
                  enum tBaseType basetype;
                  struct tVar *basevar;
                  struct tTypeSpec *procreftype;
                  BIGINT minvalue, maxvalue, initialmin, initialmax;
                  struct tUnitDef *unit;
                  struct tTypeSpec *accessedtype;
                  bool initialised;
                  enum tMode mode;
                  int lock;
                  bool wholeobject, constantaccess, subsystemboundary;

                  AnalyseReference (filename, line, p->Name, &basetype,
                                    &basevar, &procreftype, &minvalue,
                                    &maxvalue, &initialmin, &initialmax,
                                    &unit, &accessedtype, &initialised, &mode,
                                    &lock, FALSE, &wholeobject, FALSE,
                                    &constantaccess, FALSE, &subsystemboundary);
                  if (basetype == PROCEDURE_TYPE)
                    if (procreftype->ReturnType == NULL)
                      hiddenhere = FALSE;
                }
              if (hiddenhere)
                {
                  err (filename, line, "The type of field %s is hidden",
                       f->Name);
                }
            }
          fieldref->ReferenceType = FIELD_REFERENCE;
          fieldref->VarName = f->Name;
          fieldref->Next = NULL;
          fieldref->ReferenceAbbreviation = NULL;
          fieldref->ReferenceAbbreviationEnd = NULL;
          refp = basename;
          while (refp->Next != NULL)
            refp = refp->Next;
          refp->Next = fieldref;
          AnalyseAssignment (depth, filename, line, fieldtype, f->RHS,
                             basename, baseinitialised, basebasetype,
                             &finalised, f->MakeNew, TRUE, TRUE);
          if (finalised)
            {
              finalisation = TRUE;
              numfinalised++;
            }
          if (fieldtype->Controlled && (!finalised))
            allfinalised = FALSE;
          refp->Next = NULL;
        }
      actualcount++;
      f = f->Next;
      if (rectype->RecordType == REC_UNION)
        {
          //----------
          bf = FALSE;
          emit (depth, "(");
          printreference (filename, line, basename, NULL, &bf, FALSE);
          emit (0, ").tag = %u;\n", fieldnumber);
          //----------
        }
      if (rectype->RecordType == REC_UNION
          || reftype->RecordType == REC_UNCHECKED_UNION)
        {
          if (f != NULL)
            {
              err (filename, line,
                   "Exactly one field of a union must be initialised or finalised");
              break;
            }
        }
    }
  if (finalisation)
    {
      *recordfinalised = TRUE;
      controlledfields = 0;
      field = rectype->FieldList;
      while (field != NULL)
        {
          if (field->Typespec->Controlled)
            controlledfields++;
          field = field->Next;
        }
      if (numfinalised != controlledfields)
        allfinalised = FALSE;
      if (!allfinalised)
        {
          err (filename, line,
               "Some, but not all, controlled fields have been finalised");
        }
    }
  else
    {
      *recordfinalised = FALSE;
      typefields = 0;
      field = rectype->FieldList;
      while (field != NULL)
        {
          typefields++;
          field = field->Next;
        }
      if ((rectype->RecordType == REC_RECORD) && (actualcount != typefields))
        {
          err (filename, line, "Not all fields have been assigned");
        }
    }
}

void
CheckSliceBounds (char *filename, int line,
                  struct tNode *lowexpr, struct tNode *highexpr,
                  enum tReferenceType slicetype, bool lowfixed,
                  BIGINT lowbound, bool highfixed, BIGINT highbound)
{
  BIGINT minvaluelow, maxvaluelow, minvaluehigh, maxvaluehigh;
  struct tUnitDef *unitlow, *unithigh;
  bool virtualexpr;

  TestIntegerExpression (lowexpr, &minvaluelow, &maxvaluelow, &unitlow, FALSE,
                         FALSE, &virtualexpr);
  if (highexpr == NULL)
    {
      minvaluehigh = highbound;
      maxvaluehigh = highbound;
    }
  else
    {
      TestIntegerExpression (highexpr, &minvaluehigh, &maxvaluehigh,
                             &unithigh, FALSE, FALSE, &virtualexpr);
    }
  if (slicetype == ARRAY_SLICE_LENGTH_REFERENCE)
    {
      minvaluehigh = minvaluelow + minvaluehigh;
      maxvaluehigh = maxvaluelow + maxvaluehigh;
    }
  if (lowfixed && (minvaluelow != lowbound || maxvaluelow != lowbound))
    {
      err (filename, line, "Slice low bound must be fixed at %" PRINTBIG "\n",
           lowbound);
    }
  else if (highfixed
           && (minvaluehigh != highbound || maxvaluehigh != highbound))
    {
      err (filename, line,
           "Slice high bound must be fixed at %" PRINTBIG "\n", highbound);
    }
  else if (minvaluelow < lowbound || maxvaluelow > highbound
           || minvaluehigh < lowbound || maxvaluehigh > highbound)
    {
      err (filename, line,
           "Slice on LHS <(%" PRINTBIG "..%" PRINTBIG ")..(%" PRINTBIG "..%"
           PRINTBIG ")> could exceed array bounds [%" PRINTBIG "..%" PRINTBIG
           "]", minvaluelow, maxvaluelow, minvaluehigh, maxvaluehigh,
           lowbound, highbound);
    }
}

void
CheckForStaticSlice (char *filename, int line,
                     struct tTypeSpec *reftype, struct tNode *lowexpr,
                     struct tNode *highexpr, BIGINT * low, BIGINT * high)
{
  bool lowstatic, highstatic;
  struct tUnitDef *lowunit, *highunit;

  *low = EvalStaticInteger (lowexpr, &lowstatic, FALSE, &lowunit);
  *high = EvalStaticInteger (highexpr, &highstatic, FALSE, &highunit);
  if (!(lowstatic && highstatic))
    {
      err (filename, line, "Static array slices expected");
    }
  if (*low > *high)
    {
      err (filename, line,
           "The low bound of the slice %" PRINTBIG
           " is greater than the high bound %" PRINTBIG "", *low, *high);
    }
  if (*low < reftype->Low || *high > reftype->High)
    {
      err (filename, line,
           "The bounds of the slice %" PRINTBIG "..%" PRINTBIG
           " are outside the bounds of the array %" PRINTBIG "..%" PRINTBIG
           "", *low, *high, reftype->Low, reftype->High);
    }
}

void
CheckForStaticLengthSlice (char *filename, int line,
                           struct tTypeSpec *reftype, struct tNode *lowexpr,
                           struct tNode *lengthexpr, BIGINT * length)
{
  bool lengthstatic;
  struct tUnitDef *lowunit, *lengthunit;
  BIGINT minlow, maxlow, maxhigh;
  bool virtualexpr;

  TestIntegerExpression (lowexpr, &minlow, &maxlow, &lowunit, FALSE, FALSE,
                         &virtualexpr);
  *length = EvalStaticInteger (lengthexpr, &lengthstatic, FALSE, &lengthunit);
  if (!lengthstatic)
    {
      err (filename, line, "Static array length expected");
    }
  if (*length <= 0)
    {
      err (filename, line,
           "The length of the slice %" PRINTBIG
           " is less than or equal to zero", *length);
    }
  maxhigh = maxlow + *length;
  if (minlow < reftype->Low || maxhigh > reftype->High)
    {
      err (filename, line,
           "The bounds of the slice (%" PRINTBIG "..%" PRINTBIG ")..(%"
           PRINTBIG "..%" PRINTBIG ") are outside the bounds of the array %"
           PRINTBIG "..%" PRINTBIG "", minlow, maxlow, minlow + *length,
           maxhigh, reftype->Low, reftype->High);
    }
}

void
DirectAssignmentToArray (int depth, char *filename, int line, struct tReference *lhs,
                         struct tTypeSpec *lhsobjtype,
                         struct tNode *rhsexpr, bool * wholeobjectinitialised,
                         bool newmanaged)
{
  // Assignment to an array
  bool destslice, bf;
  BIGINT destlow, desthigh, destlength;
  struct tNode *destlowexpr, *desthighexpr;
  enum tReferenceType destslicetype;

  // Check that the LHS is of definite size
  if (!(lhsobjtype->MinDefinite && lhsobjtype->MaxDefinite))
    {
      err (filename, line,
           "Direct assignments other than from [range ...] expressions can only be made to closed arrays");
    }
  // Slices are not allowed on the LHS unless the length is static
  CheckForSlice (lhs, &destslice, &destlowexpr, &desthighexpr,
                 &destslicetype);
  if (destslice)
    {
      if (destslicetype == ARRAY_SLICE_REFERENCE)
        {
          if (desthighexpr == NULL)
            {
              err (filename, line,
                   "Slice expressions of the form a[x..] are not allowed on the left hand side of direct assignments");
            }
          CheckForStaticSlice (filename, line, lhsobjtype, destlowexpr,
                               desthighexpr, &destlow, &desthigh);
          destlength = desthigh - destlow + 1;
        }
      else
        {
          CheckForStaticLengthSlice (filename, line, lhsobjtype, destlowexpr,
                                     desthighexpr, &destlength);
        }
    }
  else
    {
      destlow = lhsobjtype->Low;
      desthigh = lhsobjtype->High;
      destlength = desthigh - destlow + 1;
    }
  // Check for an assignment from a string constant
  if (TestForCString (rhsexpr, FALSE, FALSE))
    {
      emit (depth, "%s(", StringToArrayCopy);
      StringToArrayReqd = TRUE;
      bf = FALSE;
      printreference (filename, line, lhs, NULL, &bf, FALSE);
      if (newmanaged)
        emit (0, "->all");
      emit (0, ", ");
      bf = FALSE;
      printexpr (rhsexpr, &bf, TRUE);
      emit (0, ", ");
      emit (0, "%" PRINTBIG ");\n", destlength);
    }
  else
    {
      // Array to array copy
      BIGINT rhsminvalue, rhsmaxvalue, initialmin, initialmax;
      struct tUnitDef *rhsunit;
      struct tTypeSpec *rhsaccessedtype;
      bool rhsinitialised;
      enum tBaseType rhsbasetype;
      struct tVar *rhsbasevar;
      enum tMode rhsmode;
      struct tTypeSpec *rhsreftype;
      int rhslock;
      bool rhswholeobject;
      BIGINT srclow, srchigh, srclength;
      bool srcslice;
      enum tReferenceType srcslicetype;
      struct tNode *srclowexpr, *srchighexpr;
      bool virtualexpr, constantaccess, subsystemboundary;

      // Check that the RHS is an array of the same length
      AnalyseReference (rhsexpr->FileName, rhsexpr->Line, rhsexpr->Var,
                        &rhsbasetype, &rhsbasevar, &rhsreftype, &rhsminvalue,
                        &rhsmaxvalue, &initialmin, &initialmax, &rhsunit,
                        &rhsaccessedtype, &rhsinitialised, &rhsmode, &rhslock,
                        FALSE, &rhswholeobject, FALSE, &constantaccess,
                        FALSE, &subsystemboundary);
      if (!(rhsreftype->MinDefinite && rhsreftype->MaxDefinite))
        {
          err (filename, line,
               "The RHS of an array assignment must be a closed array");
        }
      CheckForSlice (rhsexpr->Var, &srcslice, &srclowexpr, &srchighexpr,
                     &srcslicetype);
      if (srcslice)
        {
          if (srcslicetype == ARRAY_SLICE_REFERENCE)
            {
              if (srchighexpr == NULL)
                {
                  // a[x..]
                  BIGINT minlow, maxlow;
                  struct tUnitDef *unit;

                  TestIntegerExpression (srclowexpr, &minlow, &maxlow, &unit,
                                         FALSE, FALSE, &virtualexpr);
                  if (maxlow + destlength - 1 > rhsreftype->High)
                    {
                      err (filename, line,
                           "Required source maximum index (%" PRINTBIG "..%"
                           PRINTBIG
                           ") potentially exceeds that of the source array (%"
                           PRINTBIG ")", minlow + destlength - 1,
                           maxlow + destlength - 1, rhsreftype->High);
                    }
                  srclength = destlength;
                }
              else
                {
                  CheckForStaticSlice (filename, line, rhsreftype, srclowexpr,
                                       srchighexpr, &srclow, &srchigh);
                  srclength = srchigh - srclow + 1;
                }
            }
          else
            {
              CheckForStaticLengthSlice (filename, line, rhsreftype,
                                         srclowexpr, srchighexpr, &srclength);
            }

        }
      else
        {
          srclow = rhsreftype->Low;
          srchigh = rhsreftype->High;
          srclength = srchigh - srclow + 1;
        }
      if (destlength != srclength)
        {
          err (filename, line,
               "The length of the destination of an array assignment (here %"
               PRINTBIG ") must equal the length of the source (here %"
               PRINTBIG ")", destlength, srclength);
        }
      emit (depth, "%s(", ArrayToArrayCopy);
      bf = FALSE;
      printreference (filename, line, lhs, NULL, &bf, FALSE);
      if (newmanaged)
        emit (0, "->all");
      emit (0, ", ");
      bf = FALSE;
      printexpr (rhsexpr, &bf, TRUE);
      emit (0, ", ");
      if (CompilerTest)
        {
          emit (0, "__length_check(%d, %" PRINTBIG ", ", line, destlength);     // i.e. destlength calculated here
          if (destslice)
            {
              bf = FALSE;
              emit (0, "%" PRINTBIG " - ", lhsobjtype->High);    //***** check goes negative?
              printexpr (destlowexpr, &bf, TRUE);
              emitstr (" + 1");
            }
          else
            {
              emit (0, "%" PRINTBIG "",
                    lhsobjtype->High - lhsobjtype->Low + 1);
            }
          emitstr (")");
        }
      else
        {
          emit (0, "%" PRINTBIG "", destlength);
        }
      emitstr (" * sizeof(");
      printtypename (lhsobjtype->ElementTypeSpec);
      emitstr ("));\n");
    }
  *wholeobjectinitialised = !destslice;
}

void
AnalyseAssignment (int depth, char *filename, int line,
                   struct tTypeSpec *reftype, struct tRHS *rhs,
                   struct tReference *basename, bool baseinitialised,
                   enum tBaseType basebasetype, bool * finalisedthis,
                   bool makenew, bool fieldassignment,
                   bool initialisingwholeobject)
{
  struct tProcedureCall *p;
  enum tBaseType basetype;
  struct tVar *basevar;
  struct tTypeSpec *procreftype;
  BIGINT minvalue, maxvalue, initialmin, initialmax;
  struct tUnitDef *unit;
  struct tTypeSpec *accessedtype;
  bool initialised;
  enum tMode mode;
  bool proccall;
  int lock;
  bool proccallsuccess;
  bool wholeobject;
  struct tShareClause *shareclause;
  bool constantaccess, subsystemboundary;

  *finalisedthis = FALSE;
  if (rhs->RHSType == EXPR_RHS)
    {
      if (reftype->BaseType == ACCESS_TYPE)
        {
          if (makenew)
            {
              if (baseinitialised)
                {
                  if ((!initialisingwholeobject) && RequiresFreeing (reftype))
                    {
                      CallFreeExternal (depth, filename, line, basename,
                                        reftype, FALSE);
                    }
                }
              CallAllocate (depth, filename, line, basename, reftype);
            }
          else if ((basebasetype == ACCESS_TYPE) && (!baseinitialised))
            {
              // The model is that if the base variable has not been initialised then neither has anything
              // that is reachable from it
              err (filename, line,
                   "Assignment through an uninitialised access variable");
            }
        }
      proccall = FALSE;
      if (rhs->Expr->Op == _FN_CALL)
        {
          p = rhs->Expr->Call->Ptr;
          AnalyseReference (filename, line, p->Name, &basetype, &basevar,
                            &procreftype, &minvalue, &maxvalue, &initialmin,
                            &initialmax, &unit, &accessedtype, &initialised,
                            &mode, &lock, FALSE, &wholeobject, FALSE,
                            &constantaccess, FALSE, &subsystemboundary);
          if (basetype == PROCEDURE_TYPE)
            if (procreftype->ReturnType == NULL)
              proccall = TRUE;
        }
      //
      if (proccall)
        {
          struct tTypeSpec *returntype;
          struct tActualParameter *ap;
          bool thisfound;

          thisfound = FALSE;
          ap = p->ActualList;
          if (ap == NULL)
            {
              err (filename, line, "A parameter is required");
            }
          else
            {
              while (ap != NULL)
                {
                  if (ap->Expr->Op == _VAR)
                    if (ap->Expr->Var->ReferenceType == IDENTIFIER_REFERENCE)
                      if (!strcmp (ap->Expr->Var->VarName, "this"))
                        {
                          if (ap->Expr->Var->Next != NULL)
                            {
                              err(ap->Expr->FileName, ap->Expr->Line,
                                  "'this' cannot be dereferenced or indexed");
                            }
                          ap->Expr->Var = basename;
                          ap->IsThis = TRUE;
                          thisfound = TRUE;
                        }
                  ap = ap->Next;
                }
            }
          if (!thisfound)
            {
              err (filename, line,
                   "At least one actual parameter must be 'this'");
            }
          AnalyseProcedureCall (filename, line, p, &returntype, finalisedthis,
                                &proccallsuccess, FALSE, TRUE, &shareclause);
          if (proccallsuccess)
            {
              //----------
              printprocedurecall (depth, filename, line, p, shareclause,
                                  TRUE);
              //----------
            }
        }
      else
        {
          bool bf, wholeinit;
          BIGINT testminval, testmaxval;

          TestAssignment (basename, NULL, reftype, rhs->Expr, FALSE, makenew,
                          FALSE, &testminval, &testmaxval);
          //----------
          if (basename != NULL)
            {
              if (reftype->BaseType ==
                  ARRAY_TYPE )
                {               //**********CHECK SIZES!
                  DirectAssignmentToArray (depth, filename, line, basename,
                                           reftype, rhs->Expr,
                                           &wholeinit, makenew
                    );
                }
              else
                {
                  struct tFieldSpec *bitfieldspec;

                  if (reftype->BaseType == ACCESS_TYPE && (!makenew))
                    {           // makenew is handled above - ***** reorganise this
                      if (baseinitialised)
                        {
                          if ((!initialisingwholeobject)
                              && RequiresFreeing (reftype))
                            {
                              CallFreeExternal (depth, filename, line,
                                                basename, reftype, FALSE);
                            }
                        }
                    }
                  emit (depth, "");
                  if (makenew && reftype->UnmanagedAccess)
                    {
                      emitstr ("(*");
                    }
                  bf = FALSE;
                  printLHSorRHSreference (filename, line, basename, NULL, &bf,
                                          FALSE, TRUE, &bitfieldspec);
                  if (makenew)
                    {
                      if (reftype->UnmanagedAccess)
                        {
                          emitstr (")");
                        }
                      else
                        {
                          emitstr ("->all");
                        }
                    }
                  if ((bitfieldspec != NULL)
                      && (bitfieldspec->NumBits == 1)
                      && (bitfieldspec->BitOffset < BIT_MASK_LENGTH)
                      && (reftype->BaseType == INTEGER_TYPE
                          || reftype->BaseType == BOOLEAN_TYPE)
                      && (testminval == testmaxval))
                    {
                      // Assigning a one-bit constant to a bit field
                      // Use a shorter form for this common special case
                      char *bitstr;

                      if (bitfieldspec->BitName != NULL)
                        {
                          bitstr = bitfieldspec->BitName;
                        }
                      else
                        {
                          bitstr = SingleBitMask[bitfieldspec->BitOffset];
                        }
                      if (testminval == 0)
                        {
                          emit (0, " &= ~%s;\n", bitstr);
                        }
                      else if (testminval == 1)
                        {
                          emit (0, " |= %s;\n", bitstr);
                        }
                      else
                        {
                          err (filename, line,
                               "Internal error in bitfield assignment");
                        }
                    }
                  else
                    {
                      emit (0, " = ");
                      if (bitfieldspec != NULL)
                        {
                          emit (0, "(");
                          printLHSorRHSreference (filename, line, basename,
                                                  NULL, &bf, FALSE, TRUE,
                                                  &bitfieldspec);
                          emit (0, " & (~(%s << %u))) | ((",
                                BitMask[bitfieldspec->NumBits - 1],
                                bitfieldspec->BitOffset);
                        }
                      bf = FALSE;
                      printexpr (rhs->Expr, &bf, TRUE);
                      if (bitfieldspec != NULL)
                        {
                          emit (0, ") << %u)", bitfieldspec->BitOffset);
                        }
                      emit (0, ";\n");
                    }
                  if (CompilerTest)
                    {
                      if (reftype->BaseType == INTEGER_TYPE)
                        {
                          bool unsignedcheck = (reftype->Low >= 0);

                          StartRangeCheckCall (depth, filename, line,
                                               unsignedcheck);
                          bf = FALSE;
                          printreference (filename, line, basename, NULL, &bf,
                                          FALSE);
                          if (unsignedcheck)
                            {
                              emit (0, ", %" PRINTBIG "u, %" PRINTBIG "u);\n",
                                    reftype->Low, reftype->High);
                            }
                          else
                            {
                              emit (0, ", %" PRINTBIG ", %" PRINTBIG ");\n",
                                    reftype->Low, reftype->High);
                            }
                        }
                      else if (reftype->BaseType == ACCESS_TYPE
                               && (!reftype->CanBeNull))
                        {
                          emit (depth, "__notnull_check(%d, ", line);
                          bf = FALSE;
                          printreference (filename, line, basename, NULL, &bf,
                                          FALSE);
                          emitstr (");\n");
                        }
                    }
                  if ((reftype->BaseType == ACCESS_TYPE) && (!makenew))
                    {
                      if (RequiresFreeing (reftype))
                        {
                          CallCount (depth, filename, line, basename, reftype,
                                     FALSE);
                        }
                    }
                }
            }
          //----------
        }
    }
  else if (rhs->RHSType == ARRAY_RHS)
  {
    AnalyseArrayAssignment (depth, filename, line, reftype, rhs->ArrayExpr,
                            basename, baseinitialised, basebasetype,
                            finalisedthis, makenew);
  }
  else if (rhs->RHSType == RECORD_RHS)
    {
      AnalyseRecordAssignment (depth, filename, line, reftype,
                               rhs->RecordExpr, basename, baseinitialised,
                               basebasetype, finalisedthis, makenew);
    }
}

void
AnalyseAssignmentStmt (int depth, char *filename, int line,
                       struct tAssignment *a, bool initconsts)
{
  struct tVar *v;
  BIGINT minvalue, maxvalue, initialmin, initialmax;
  struct tUnitDef *unit;
  struct tTypeSpec *accessedtype;
  bool initialised, finalised;
  enum tBaseType basetype;
  struct tVar *basevar;
  struct tTypeSpec *reftype;
  struct tReference *lhsref;
  enum tMode mode;
  bool ref, wholeobject;
  struct tTargetType *desttype;
  int lock;
  bool simple;                  //, accessedbefore;
  bool constantaccess, subsystemboundary;

  // LHS
  AnalyseReference (filename, line, a->Var, &basetype, &basevar, &reftype,
                    &minvalue, &maxvalue, &initialmin, &initialmax, &unit,
                    &accessedtype, &initialised, &mode, &lock, FALSE,
                    &wholeobject, TRUE, &constantaccess, FALSE, &subsystemboundary);
  if (NamedConstantType (reftype))
    {
      err (filename, line, "Cannot assign to a constant");
      return;
    }
  else if (mode == MODE_IN)
    {
      err (filename, line, "Assignment to 'in' parameter");
      return;
    }
  else if (mode == MODE_SHARED
           && (!(InPackageInitialisation || InPackageFinalisation)) && (!basevar->ShareLock) && (!InUnitTestCode()))
    {
      err (filename, line,
           "Direct assignment to shared variable outside of package/subsystem initialisation/finalisation without lock");
      return;
    }
  if (constantaccess)
    {
      err (filename, line,
           "Attempted assignment through an access constant value");
    }
  if (InFunction && (mode != MODE_LOCAL)
      && ((!CurrentExemptions.SideEffect) || RoutinePrefixClosed)
      && (!basevar->AccessVirtual))
    {
      if (CurrentExemptions.SideEffect && RoutinePrefixClosed)
        {
          err (filename, line,
               "Unchecked_side_effect cannot be used in a closed function");
        }
      else
        {
          err (filename, line, "Functions cannot have side-effects");
        }
      return;
    }
  else if (RoutinePrefixClosed
           && (mode != MODE_LOCAL) && (mode != MODE_OUT)
           && (mode != MODE_IN_OUT) && (mode != MODE_FINAL_IN_OUT)
           && (!CurrentExemptions.SideEffect)
           && (!basevar->AccessVirtual))
    {
      err (filename, line,
           "Closed procedures cannot have global side-effects");
      return;
    }
  else if (RoutinePrefixShared
           && (mode != MODE_LOCAL) && (mode != MODE_OUT)
           && (mode != MODE_IN_OUT) && (mode != MODE_FINAL_IN_OUT)
           && (!basevar->AccessSharedAtomic)
           && (!basevar->ShareLock)
           && (!CurrentExemptions.SideEffect)
           && (!basevar->AccessVirtual))
    {
      // Routines in shared packages can update globals within that package because
      // calls are serialised, and those globals cannot be public
      if (SharedAncestorPackage (CurrentPackage, basevar->EnclosingPackage) ==
          NULL)
        {
          err (filename, line,
               "Shared procedures cannot have global side-effects (apart from accesses within shared packages, accesses to shared atomic variables and accesses to locked variables)");
          return;
        }
    }
  //
  RecordGlobalAccess (line, GetGlobalsAccess (), basevar, FALSE, TRUE, FALSE,
                      FALSE);
  //
  lhsref = a->Var;
  if (lhsref->ReferenceType == IDENTIFIER_REFERENCE)
    {
      struct tReference *varref;

      varref = lhsref;
      finalised = FALSE;
      v = FindVariable (filename, line, &varref, 3, FALSE, TRUE);
      if (v->PublicOut && (v->PrivatePublicNext != NULL))
        {
          err (filename, line,
               "Cannot assign to a 'public out' variable from outside its package");
        }
      if (lock != 0)
        {
          err (filename, line, "Cannot assign to %s here due to alias lock",
               v->Name);
        }
      else if ((v->ConstValue != NULL) && (!initconsts))
        {
          err (filename, line, "Assignment to constant");
        }
      if (varref->Next == NULL)
        simple = TRUE;
      else if (varref->Next->ReferenceType == ARRAY_SLICE_REFERENCE
               || varref->Next->ReferenceType == ARRAY_SLICE_LENGTH_REFERENCE)
        simple = TRUE;
      else
        simple = FALSE;
      if (simple)
        {
          // Assignment to a simple variable (rather than a field etc.)
          bool wholeinit = TRUE;
          bool initialisedcontrolled;

          if (v->Type->TypeSpec->Controlled
              && (v->Mode == MODE_GLOBAL || v->Mode == MODE_SHARED)
              && v->Initialised)
            {
              initialisedcontrolled = TRUE;
            }
          else
            {
              initialisedcontrolled = FALSE;
            }
          if (a->RHS->RHSType == EXPR_RHS)
            {
              struct tTypeSpec *objtype;
              bool bf, casttolhs;
              BIGINT testminval, testmaxval;

              if (initialisedcontrolled)
                {
                  ControlledReinitError (filename, line);
                }
              TestAssignment (varref, v, reftype, a->RHS->Expr, FALSE,
                              a->MakeNew, FALSE, &testminval, &testmaxval);
              if (reftype->BaseType == ACCESS_TYPE)
                {
                  if (initialised)
                    {
                      if (RequiresFreeing (reftype))
                        {
                          CallFreeExternal (depth, filename, line, varref, reftype, FALSE);     //*****FALSE
                        }
                    }
                  if (a->MakeNew)
                    {
                      CallAllocate (depth, filename, line, varref, reftype);
                      objtype = reftype->ElementTypeSpec;
                    }
                  else
                    {
                      objtype = reftype;
                    }
                }
              else
                objtype = reftype;
              //----------
              if (objtype->BaseType == INTEGER_TYPE
                  || objtype->BaseType == FLOATING_TYPE)
                desttype = objtype->CType;
              else
                desttype = NULL;
              //
              if (objtype->BaseType == ARRAY_TYPE)
                {
                  DirectAssignmentToArray (depth, filename, line, a->Var,
                                           /*v, */ objtype, a->RHS->Expr,
                                           &wholeinit, a->MakeNew
                                           && (!reftype->UnmanagedAccess));
                }
              else if (!v->AccessVirtual)
                {
                  // Not an array

                  ref = PassedByReference (v);
                  emit (depth, "");
                  if (a->MakeNew && reftype->UnmanagedAccess)
                    {
                      emitstr ("(*");
                    }
                  if (CompilerTest && a->MakeNew)
                    {
                      if (v->Type->TypeSpec->UnmanagedAccess)
                        {
                          emit (0, "((%s *)__deref_check(",
                                v->Type->TypeSpec->ElementTypeSpec->CName);
                        }
                      else
                        {
                          char *aname =
                            MakeAName (v->Type->TypeSpec->
                                       ElementTypeSpec->CName);
                          emit (0, "((struct %s__a *)__deref_check(", aname);
                          free (aname);
                        }
                    }
                  if (ref)
                    emit (0, "*");
                  printvarname (v);
                  if (a->MakeNew)
                    {
                      if (CompilerTest)
                        {
                          emitstr (", ");
                          if (v->Type->TypeSpec->UnmanagedAccess)
                            {
                              emitstr ("1");
                            }
                          else
                            {
                              emitstr ("(");
                              printvarname (v);
                              emitstr (" == NULL) ? 0 : ");
                              printvarname (v);
                              emitstr ("->count");
                            }
                          emit (0, ", %d", line);
                          emitstr ("))");
                        }
                      if (reftype->UnmanagedAccess)
                        {
                          emitstr (")");
                        }
                      else
                        {
                          emitstr ("->all");
                        }
                    }
                  emit (0, " = ");
                  if (desttype != NULL
                      && (desttype != a->RHS->Expr->ComputationType)
                      && (a->RHS->Expr->ComputationType != NULL)
                      && (a->RHS->Expr->Op != _CONST_INT)
                      && (a->RHS->Expr->Op != _CONST_FLOAT))
                    {
                      emit (0, "(%s)(", desttype->CName);
                      casttolhs = TRUE;
                    }
                  else
                    {
                      casttolhs = FALSE;
                    }
                  bf = FALSE;
                  printexpr (a->RHS->Expr, &bf, TRUE);
                  if (casttolhs)
                    emit (0, ")");
                  emit (0, ";\n");
                  if (CompilerTest)
                    {
                      if (reftype->BaseType == INTEGER_TYPE)
                        {
                          bool unsignedcheck = (reftype->Low >= 0);

                          StartRangeCheckCall (depth, filename, line,
                                               unsignedcheck);
                          if (ref)
                            emitstr ("*");
                          printvarname (v);
                          if (unsignedcheck)
                            {
                              emit (0, ", %" PRINTBIG "u, %" PRINTBIG "u);\n",
                                    reftype->Low, reftype->High);
                            }
                          else
                            {
                              emit (0, ", %" PRINTBIG ", %" PRINTBIG ");\n",
                                    reftype->Low, reftype->High);
                            }
                        }
                      else if (reftype->BaseType == ACCESS_TYPE
                               && (!reftype->CanBeNull))
                        {
                          emit (depth, "__notnull_check(%d, ", line);
                          if (ref)
                            emitstr ("*");
                          printvarname (v);
                          emitstr (");\n");
                        }
                    }
                }
              if (reftype->BaseType == ACCESS_TYPE)
                {
                  if (a->MakeNew)
                    {
                      if (RequiresFreeing (reftype->ElementTypeSpec))
                        {
                          CallCountInternal (depth, getvarname (v), "->all",
                                             objtype);
                        }
                    }
                  else
                    {
                      if (RequiresFreeing (reftype))
                        {
                          CallCount (depth, filename, line, varref, reftype, FALSE);    //*****FALSE
                        }
                    }
                }
              else if (RequiresFreeing (reftype))
                {
                  CallCountInternal (depth, "", getvarname (v), reftype);
                }
              //----------
            }
          else
            {
              AnalyseAssignment (depth, filename, line, reftype, a->RHS,
                                 lhsref, initialised, basetype, &finalised,
                                 a->MakeNew, FALSE, FALSE);
              if (initialisedcontrolled && (!finalised))
                {
                  ControlledReinitError (filename, line);
                }
            }
          if (wholeinit)
            {
              SetInitialised (v);
              if (reftype->BaseType == INTEGER_TYPE)
                {
                  UpdateRelations (filename, line, v, ATTR_NULL, _EQ,
                                   a->RHS->Expr, TRUE);
                }
            }
          //}
        }
      else
        {
          AnalyseAssignment (depth, filename, line, reftype, a->RHS, lhsref,
                             initialised, basetype, &finalised, a->MakeNew,
                             FALSE, FALSE);
        }
      if (finalised)
        SetFinalised (v);
    }
  else
    {
      AnalyseAssignment (depth, filename, line, reftype, a->RHS, lhsref,
                         initialised, basetype, &finalised, a->MakeNew, FALSE,
                         FALSE);
    }
}

// Test for a single variable
bool
SingleObject (struct tNode *n)
{
  if (n->Op == _VAR)
    {
      return TRUE;
    }
  else
    {
      return FALSE;
    }
}

void
RangeErrorGTLT (char *filename, int line,
                char *varname, char *op, BIGINT varmin, BIGINT varmax,
                BIGINT comp)
{
  err (filename, line,
       "%s(%" PRINTBIG "..%" PRINTBIG ") cannot be %s %" PRINTBIG "", varname,
       varmin, varmax, op, comp);
}

void
TestLT (char *filename, int line,
        BIGINT * low, BIGINT * high, BIGINT minvalue, BIGINT maxvalue,
        char *name)
{
  if (*low >= maxvalue)
    {
      RangeErrorGTLT (filename, line, name, "<", *low, *high, maxvalue);
    }
  else
    {
      if (*high >= maxvalue)
        *high = maxvalue - 1;
      /* Adjust low if necessary so that the analysis doesn't fail,
         even though the code woudn't run
       */
      if (*low > *high)
        *low = *high;
    }
}

void
TestGT (char *filename, int line,
        BIGINT * low, BIGINT * high, BIGINT minvalue, BIGINT maxvalue,
        char *name)
{
  if (*high <= minvalue)
    {
      RangeErrorGTLT (filename, line, name, ">", *low, *high, minvalue);
    }
  else
    {
      if (*low <= minvalue)
        *low = minvalue + 1;
      /* Adjust low if necessary so that the analysis doesn't fail,
         even though the code woudn't run
       */
      if (*low > *high)
        *high = *low;
    }
}

void
TestLEQ (char *filename, int line,
         BIGINT * low, BIGINT * high, BIGINT minvalue, BIGINT maxvalue,
         char *name)
{
  if (*low > maxvalue)
    {
      RangeErrorGTLT (filename, line, name, "<=", *low, *high, maxvalue);
    }
  else
    {
      if (*high > maxvalue)
        *high = maxvalue;
      /* Adjust low if necessary so that the analysis doesn't fail,
         even though the code woudn't run
       */
      if (*low > *high)
        *low = *high;
    }
}

void
TestGEQ (char *filename, int line,
         BIGINT * low, BIGINT * high, BIGINT minvalue, BIGINT maxvalue,
         char *name)
{
  if (*high < minvalue)
    {
      RangeErrorGTLT (filename, line, name, ">=", *low, *high, minvalue);
    }
  else
    {
      if (*low < minvalue)
        *low = minvalue;
      /* Adjust low if necessary so that the analysis doesn't fail,
         even though the code woudn't run
       */
      if (*low > *high)
        *high = *low;
    }
}

void
TestEQ (char *filename, int line,
        BIGINT * low, BIGINT * high, BIGINT minvalue, BIGINT maxvalue,
        char *name)
{
  if (*high < minvalue || *low > maxvalue)
    {
      err (filename, line,
           "%s(%" PRINTBIG "..%" PRINTBIG ") cannot be in the range %"
           PRINTBIG "..%" PRINTBIG "", name, *low, *high, minvalue, maxvalue);
    }
  else
    {
      *low = minvalue;
      *high = maxvalue;
    }
}

void
TestNEQ (char *filename, int line,
         BIGINT * low, BIGINT * high, BIGINT minvalue, BIGINT maxvalue,
         char *name)
{
  if (*low == *high && minvalue == maxvalue && *low == minvalue)
    {
      err (filename, line,
           "%s(%" PRINTBIG "..%" PRINTBIG ") cannot be /= %" PRINTBIG "",
           name, *low, *high, minvalue);
    }
  else
    {
      if (minvalue == maxvalue && *low != *high)
        {
          if (*low == minvalue)
            *low = minvalue + 1;
          if (*high == maxvalue)
            *high = maxvalue - 1;
        }
    }
}

/*
   Analyse a boolean expression and attempt to update the ranges of integer and boolean
   variables in it on the assumption that the expression is true. This is used in if tests.

   Set success to FALSE if the test cannot be analysed.
   InvertSense is used for 'not expr'

   Note We only ever set alwayspasses to false and alwaysfails to true. We
   start with the assumption (alwayspasses and not alwaysfails) and try to
   disprove it. This is so that AND and OR work.
*/
void
UpdateRanges (struct tNode *cond, bool InvertSense, bool * success,
              bool allowpartialsuccess, bool testingpostcondition)
{
  enum tOp op;
  struct tNode *exp, *var;
  struct tUnitDef *varunit, *expunit;
  struct tTypeSpec *varaccessedtype;
  bool initialised, array, first = FALSE;       // Avoid a warning on first - actually OK
  int digits, magnitude;
  enum tBaseType basetype;
  struct tVar *basevar;
  BIGINT minvalue, maxvalue, varminvalue, varmaxvalue, initialmin, initialmax;
  struct tTypeSpec *reftype;
  enum tMode mode;
  int lock;
  bool wholeobject;
  struct tVar *leftvar;
  enum tAttribute leftattr;
  //
  char *filename = cond->FileName;
  int line = cond->Line;
  bool virtualexpr;
  bool constantaccess, subsystemboundary;

#ifdef TRACE
  int tracenum = tracepoint;
  tracepoint++;

  printf ("UpdateRanges %d:\n", tracenum);
  printf ("Test: ");
  DumpExpr (cond);
  printf ("\n");
  printf ("Invert: %d\n", InvertSense);
#endif

  if ((InvertSense && cond->Op == _OR)
      || ((!InvertSense) && cond->Op == _AND))
    {
      if (allowpartialsuccess)
        {
          bool successleft, successright;
          struct tDynamicValues *savedvars;

          successleft = TRUE;
          successright = TRUE;
          //
          savedvars = StoreDynamicValues ();
          UpdateRanges (cond->Left, InvertSense, &successleft,
                        allowpartialsuccess, testingpostcondition);
          if (successleft)
            {
              // We will keep the LHS updates even if the RHS cannot be analysed
              //                      StoreRanges(CurrentVars, savedvars);
              savedvars = StoreDynamicValues ();
            }
          else
            {
              // Restore the vars because the LHS couldn't be analysed
              RestoreDynamicValues (savedvars);
            }
          UpdateRanges (cond->Right, InvertSense, &successright,
                        allowpartialsuccess, testingpostcondition);
          if (!successright)
            {
              // Restore the vars from before the RHS test
              RestoreDynamicValues (savedvars);
            }
          if (!(successleft || successright))
            {
              *success = FALSE;
            }
          FreeDynamicValues (savedvars);
        }
      else
        {
          UpdateRanges (cond->Left, InvertSense, success, allowpartialsuccess,
                        testingpostcondition);
          if (*success)
            {
              // Only continue if successful so far
              UpdateRanges (cond->Right, InvertSense, success,
                            allowpartialsuccess, testingpostcondition);
            }
        }
    }
  else if ((InvertSense && cond->Op == _AND)
           || ((!InvertSense) && cond->Op == _OR))
    {
      // NOT ANALYSABLE! UpdateRanges(cond->Left, InvertSense, success, alwaysfails, alwayspasses);
      //                 UpdateRanges(cond->Right, InvertSense, success, alwaysfails, alwayspasses);
      /* The test cannot be analysed */
      *success = FALSE;
    }
  else if (cond->Op == _NOT)
    {
      UpdateRanges (cond->Left, !InvertSense, success, allowpartialsuccess,
                    testingpostcondition);
    }
  else if (IsRelop (cond->Op))
    {
      /* Terms of the form 'var relop expr' can be analysed */
      if (SingleObject (cond->Left) && cond->Left->Attribute == NULL)
        {
          exp = cond->Right;
          var = cond->Left;
          AnalyseReference (filename, line, var->Var, &basetype, &basevar,
                            &reftype, &varminvalue, &varmaxvalue, &initialmin,
                            &initialmax, &varunit, &varaccessedtype,
                            &initialised, &mode, &lock, testingpostcondition,
                            &wholeobject, FALSE, &constantaccess, FALSE, &subsystemboundary);
          basetype = reftype->BaseType;
          if (basetype == INTEGER_TYPE
              || (basetype == ARRAY_TYPE && var->Attribute != NULL))
            {
              TestIntegerExpression (exp, &minvalue, &maxvalue, &expunit, testingpostcondition, FALSE, &virtualexpr);   //ACCESS
              if (basetype == ARRAY_TYPE)
                TestUnitMatch (reftype->IndexTypeSpec->Unit, expunit, cond,
                               TRUE);
              else
                TestUnitMatch (varunit, expunit, cond, TRUE);
              cond->ComputationType =
                GetTargetIntegerType (exp->FileName, exp->Line,
                                      MinBigInt (minvalue, varminvalue),
                                      MaxBigInt (maxvalue, varmaxvalue), TRUE,
                                      TRUE);
            }
          else if (basetype == ACCESS_TYPE || basetype == ADDRESS_TYPE)
            {
              bool couldbenull, isnull, isaccessvalue;
              struct tTypeSpec *exprtype;
              bool uncheckedaccess, accessconstant, accesspersistent, accessnew, addressexpr;

              TestAccessExpression (exp, FALSE, &couldbenull, &isnull,
                                    &exprtype, &uncheckedaccess,
                                    &isaccessvalue, &minvalue, &maxvalue,
                                    &virtualexpr, &accessconstant, &accesspersistent,
                                    &accessnew, &addressexpr);
            }
          else if (basetype == FLOATING_TYPE)
            {
              TestFloatExpression (exp, &expunit, &digits, &magnitude,
                                   testingpostcondition, &virtualexpr);
              TestUnitMatch (varunit, expunit, cond, TRUE);
              if (reftype->Magnitude < magnitude)       // for comparisons use magnitude
                cond->ComputationType =
                  GetTargetFloatType (filename, line, digits, magnitude,
                                      FALSE);
              else
                cond->ComputationType = reftype->CType;
            }
          else if (basetype == BOOLEAN_TYPE)
            {
              TestBooleanExpression (exp, testingpostcondition, &minvalue,
                                     &maxvalue, FALSE, &virtualexpr);
            }
          else
            {
              err (filename, line, "Composite objects cannot be compared");     //**********
            }
          if ((basetype == INTEGER_TYPE || basetype == BOOLEAN_TYPE
               || basetype == ARRAY_TYPE || basetype == ACCESS_TYPE || basetype == ADDRESS_TYPE)
              && SimpleVar (cond->Left, &leftvar, &leftattr, FALSE)
              && (basetype == ARRAY_TYPE || cond->Left->Attribute == NULL))
            {

              struct tVar *v = leftvar;

              op = cond->Op;
              //v = leftvar;
              //leftattr = cond->Left->Attribute; //above
              /* var op expr */
              if (basetype == ARRAY_TYPE)
                {
                  enum tAttribute varattr = GetExprAttribute (var);
                  array = TRUE;
                  if (varattr != ATTR_NULL)
                    {
                      if (varattr == ATTR_FIRST)
                        first = TRUE;
                      else if (varattr == ATTR_LAST)
                        first = FALSE;
                      else
                        {
                          err (filename, line,
                               "Attribute %s not allowed here",
                               AttributeNames[varattr]);
                        }
                    }
                  else
                    {
                      err (filename, line, "Attribute expected for array");
                    }
                }
              else
                {
                  array = FALSE;
                  if (!v->Initialised)
                    {
                      if (v->Mode == MODE_GLOBAL || v->Mode == MODE_SHARED)
                        {
                          RecordGlobalAccess (line, GetGlobalsAccess (), v,
                                              FALSE, FALSE, TRUE, FALSE);
                        }
                      else
                        {
                          err (filename, line,
                               "Variable %s declared at line %d of %s might not be initialised (8)",
                               v->Name, v->Line, v->FileName);
                        }
                    }
                }
              if (InvertSense)
                {
                  switch (op)
                    {
                    case _LT:
                      op = _GEQ;
                      break;
                    case _GT:
                      op = _LEQ;
                      break;
                    case _LEQ:
                      op = _GT;
                      break;
                    case _GEQ:
                      op = _LT;
                      break;
                    case _EQ:
                      op = _NEQ;
                      break;
                    case _NEQ:
                      op = _EQ;
                      break;
                    default:
                      err (filename, line, "Internal error 5");
                    }
                }
              if ((basetype == ACCESS_TYPE || basetype == ADDRESS_TYPE) && op != _EQ && op != _NEQ)
                {
                  err (filename, line,
                       "Only tests for equality (or inequality) are allowed for access and address types");
                }
              if (basetype == BOOLEAN_TYPE && op != _EQ && op != _NEQ)
                {
                  err (filename, line,
                       "Only tests for equality (or inequality) are allowed for boolean types");
                }
              //
              // Perform the test
              //
              // For array bounds, v->Low and v->High are the particular values of 'first and 'last during this test, not the ranges
              // within which the values might fall
              //
              switch (op)
                {
                case _LT:
                  if (array)
                    {
                      if (first)        // a'first
                        TestLT (filename, line, &v->FirstLow, &v->FirstHigh,
                                minvalue, maxvalue, v->Name);
                      else      // a'last
                        TestLT (filename, line, &v->LastLow, &v->LastHigh,
                                minvalue, maxvalue, v->Name);
                    }
                  else
                    {
                      TestLT (filename, line, &v->Low, &v->High, minvalue,
                              maxvalue, v->Name);
                    }
                  break;
                case _GT:
                  if (array)
                    {
                      if (first)        // a'first
                        TestGT (filename, line, &v->FirstLow, &v->FirstHigh,
                                minvalue, maxvalue, v->Name);
                      else      // a'last
                        TestGT (filename, line, &v->LastLow, &v->LastHigh,
                                minvalue, maxvalue, v->Name);
                    }
                  else
                    {
                      TestGT (filename, line, &v->Low, &v->High, minvalue,
                              maxvalue, v->Name);
                    }
                  break;
                case _LEQ:
                  if (array)
                    {
                      if (first)        // a'first
                        TestLEQ (filename, line, &v->FirstLow, &v->FirstHigh,
                                 minvalue, maxvalue, v->Name);
                      else      // a'last
                        TestLEQ (filename, line, &v->LastLow, &v->LastHigh,
                                 minvalue, maxvalue, v->Name);
                    }
                  else
                    {
                      TestLEQ (filename, line, &v->Low, &v->High, minvalue,
                               maxvalue, v->Name);
                    }
                  break;
                case _GEQ:
                  if (array)
                    {
                      if (first)        // a'first
                        TestGEQ (filename, line, &v->FirstLow, &v->FirstHigh,
                                 minvalue, maxvalue, v->Name);
                      else      // a'last
                        TestGEQ (filename, line, &v->LastLow, &v->LastHigh,
                                 minvalue, maxvalue, v->Name);
                    }
                  else
                    {
                      TestGEQ (filename, line, &v->Low, &v->High, minvalue,
                               maxvalue, v->Name);
                    }
                  break;
                case _EQ:
                  if (array)
                    {
                      if (first)        // a'first
                        TestEQ (filename, line, &v->FirstLow, &v->FirstHigh,
                                minvalue, maxvalue, v->Name);
                      else      // a'last
                        TestEQ (filename, line, &v->LastLow, &v->LastHigh,
                                minvalue, maxvalue, v->Name);
                    }
                  else
                    {
                      if (basetype == ACCESS_TYPE || basetype == ADDRESS_TYPE)
                        {
                          // Only tests against null have any effect
                          if (minvalue == 0 && maxvalue == 0)
                            {
                              v->Low = 0;
                              v->High = 0;
                            }
                        }
                      else
                        {
                          TestEQ (filename, line, &v->Low, &v->High, minvalue,
                                  maxvalue, v->Name);
                        }
                    }
                  break;
                case _NEQ:
                  if (array)
                    {
                      if (first)        // a'first
                        TestNEQ (filename, line, &v->FirstLow, &v->FirstHigh,
                                 minvalue, maxvalue, v->Name);
                      else      // a'last
                        TestNEQ (filename, line, &v->LastLow, &v->LastHigh,
                                 minvalue, maxvalue, v->Name);
                    }
                  else
                    {
                      if (basetype == ACCESS_TYPE || basetype == ADDRESS_TYPE)
                        {
                          // Only tests against null have any effect
                          if (minvalue == 0 && maxvalue == 0)
                            {
                              v->Low = 1;
                              v->High = 1;
                            }
                        }
                      else
                        {
                          TestNEQ (filename, line, &v->Low, &v->High,
                                   minvalue, maxvalue, v->Name);
                        }
                    }
                  break;
                default:
                  err (filename, line, "Internal error 6");
                }
              // Check for the form var relop var [+/- expr], e.g. a > b, a > b + 1
              UpdateRelations (filename, line, leftvar, leftattr, op,
                               cond->Right, FALSE);
            }
          else
            {
              *success = FALSE;
            }
        }
      else
        {
          /* The test cannot be analysed */
          *success = FALSE;
        }
    }
  else if (cond->Op == _VAR || cond->Op == _FN_CALL)
    {
      // The variable must be of a boolean type for this to be a valid subexpression in this context.
      // If the variable is definitely false and we are not inverting, then the overall expression
      // fails (A and B)
      // If the variable is definitely true and we are inverting, then the overall expression fails
      // (not(A or B) -> (NOT A) and (not B))
      TestBooleanExpression (cond, testingpostcondition, &minvalue, &maxvalue,
                             FALSE, &virtualexpr);
    }
  else if (cond->Op == _CONST_BOOL)
    {
      ;
    }
  else
    {
      err (filename, line, "Boolean expression expected 2");
      *success = FALSE;
    }
#ifdef TRACE
  printf ("UpdateRanges %d: done\n", tracenum);
#endif
}

BIGINT
MinRange (BIGINT a, BIGINT b)
{
  if (a < b)
    return a;
  else
    return b;
}

BIGINT
MaxRange (BIGINT a, BIGINT b)
{
  if (a > b)
    return a;
  else
    return b;
}

void
NonStaticConstBoolean (char *filename, int line, bool val)
{
  char *s;

  if (val)
    s = "true";
  else
    s = "false";
  err_and_continue (filename, line,
       "A test cannot be non-static but ineffective (in this case always %s)",
       s);
}

// Merge two sets of dynamic values, with reference to the previous state, into CurrentVars
void
MergeDynamicValues (struct tDynamicValues *setA, struct tDynamicValues *setB,
                    struct tDynamicValues *setPrev)
{
  struct tDynamicListValue *a, *b, *prev;
  struct tVar *v;

  a = setA->ValueList;
  while (a != NULL)
    {
      v = a->var;
      b = setB->ValueList;
      while (b != NULL)
        {
          if (b->var == v)
            break;
          b = b->Next;
        }
      prev = setPrev->ValueList;
      while (prev != NULL)
        {
          if (prev->var == v)
            break;
          prev = prev->Next;
        }
      if (b != NULL && prev != NULL)
        {
          if (!(a->Low == prev->Low && b->Low == prev->Low
                && a->High == prev->High && b->High == prev->High
                && a->Initialised && b->Initialised && prev->Initialised
                && (!a->Finalised) && (!b->Finalised) && (!prev->Finalised)))
            {
              // There has been a change
              v->Low = MinRange (a->Low, b->Low);
              v->High = MaxRange (a->High, b->High);
              if (a->Initialised && b->Initialised)
                {
                  SetInitialised (v);
                  ResetRelations (v);
                }
            }
          if (a->Finalised && b->Finalised)
            v->Finalised = TRUE;
          // Set accessed if an access occurred in either
          if (a->Accessed || b->Accessed)
            SetAccessed (v);
          if (a->EverAccessed || b->EverAccessed)
            v->EverAccessed = TRUE;
        }
      a = a->Next;
    }
}

// Restore relations from a saved set to CurrentVars
void
RestoreRelations (struct tDynamicValues *src)
{
  int j;

  CurrentVars->NumRelations = src->NumRelations;
  for (j = 0; j < src->NumRelations; j++)
    {
      CurrentVars->Relations[j] = src->Relations[j];
    }
}

void
MergeRelation (struct tRelation *r1, struct tRelation *r2, bool reverse)
{
  struct tRelation r;

  r.High = 0;
  r.Low = 0;                    // These are just defaults to avoid unnecessary warnings
  if (reverse)
    {
      if (r1->HighSet && r2->LowSet)
        {
          r.HighSet = TRUE;
          if (r1->High > -r2->Low)
            r.High = r1->High;
          else
            r.High = -r2->Low;
        }
      else
        {
          r.HighSet = FALSE;
        }
      if (r1->LowSet && r2->HighSet)
        {
          r.LowSet = TRUE;
          if (r1->Low < -r2->High)
            r.Low = r1->Low;
          else
            r.Low = -r2->High;
        }
      else
        {
          r.LowSet = FALSE;
        }
    }
  else
    {
      if (r1->HighSet && r2->HighSet)
        {
          r.HighSet = TRUE;
          if (r1->High > r2->High)
            r.High = r1->High;
          else
            r.High = r2->High;
        }
      else
        {
          r.HighSet = FALSE;
        }
      if (r1->LowSet && r2->LowSet)
        {
          r.LowSet = TRUE;
          if (r1->Low < r2->Low)
            r.Low = r1->Low;
          else
            r.Low = r2->Low;
        }
      else
        {
          r.LowSet = FALSE;
        }
    }
  if (r.LowSet || r.HighSet)
    {
      r.LHS = r1->LHS;
      r.LHSAttr = r1->LHSAttr;
      r.RHS = r1->RHS;
      r.RHSAttr = r1->RHSAttr;
      CurrentVars->Relations[CurrentVars->NumRelations] = r;
      CurrentVars->NumRelations++;
    }
}

void
EmitCoverage (int depth, char *filename, int line)
{
  if (UnitTest && (!InlineToHeader) && (!InUnitTestCode ()))
    {
      if (BranchNumber >= CurrentCoverageSize)
        {
          CoverageMap =
            realloc (CoverageMap,
                     CurrentCoverageSize +
                     COVERAGE_STEP * sizeof (struct tCoverage));
          if (CoverageMap == NULL)
            {
              printf
                ("Cannot allocate/extend coverage map (current size = %u)\n",
                 CurrentCoverageSize);
              exit(3);
            }
          CurrentCoverageSize += COVERAGE_STEP;
        }
      emit (depth, "__coverage[%u] = 1;\n", BranchNumber);
      CoverageMap[BranchNumber].FileName = filename;
      CoverageMap[BranchNumber].Line = line;
      BranchNumber++;
    }
}

//
// Analyse if statements; also used for while loops
//
// For the use of elsecanbecommentsonly, see the definition of tWhen
//
void
AnalyseWhen (int depth,
             struct tNode *cond,
             struct tElt *when_stmt, struct tElt *else_stmt, bool whileloop,
             bool elsecanbecommentsonly)
{
  struct tDynamicValues *savedvars, *whenvars = NULL, *elsevars =
    NULL, *initialvars, *testvars = NULL;
  int j;
  bool success, alwaysfails, alwayspasses;
  bool successelse;
  bool bf;
  bool staticcond, haveelse;
  BIGINT constbool, minvalue, maxvalue;
  bool virtualexpr;

#ifdef TRACE
  int tracenum = tracepoint;
  tracepoint++;

  printf ("AnalyseWhen %d:\n", tracenum);
  printf ("Test: ");
  DumpExpr (cond);
  printf ("\n");
  printf ("When:\n");
  DumpStmt (when_stmt, 0);
  printf ("Else:\n");
  DumpStmt (else_stmt, 0);
  printf ("------- %d\n", tracenum);
#endif

  /* Copy the current variable state */
  savedvars = StoreDynamicValues ();    //MallocVars();
  initialvars = CopyDynamicValues (savedvars);
  //
  // Test for a static boolean expression, i.e. one where all the terms are compile-time
  // constants. It is not an error for code to be excluded by a static boolean in an if test
  // (this is conditional compilation). See below.
  //
  constbool = EvalStaticBoolean (cond, &success, FALSE);
  //
  if (success)
    {
      staticcond = TRUE;
      // While loops cannot have static conditions
      if (whileloop)
        {
          err (cond->FileName, cond->Line,
               "While loops cannot ever have static tests (use a loop statement)");
        }
      if (constbool == 0)
        {
          alwaysfails = TRUE;
          alwayspasses = FALSE;
        }
      else
        {
          alwaysfails = FALSE;
          alwayspasses = TRUE;
        }
    }
  else
    {
      //
      // Not a static boolean expression
      //
      staticcond = FALSE;
      //
      // Test for any valid boolean expression and save the range of the result (where
      // 0 is false and 1 is true)
      //
      TestBooleanExpression (cond, FALSE, &minvalue, &maxvalue, FALSE,
                             &virtualexpr);
      //
      // Cond is a valid boolean expression
      //
      // Test for an expression that is not static but can only take one value (this
      // would be an error)
      //
      if (maxvalue == 0 && (!InUnitTestCode()))
        {
          NonStaticConstBoolean (cond->FileName, cond->Line, FALSE);
        }
      else if (minvalue == 1 && (!InUnitTestCode()))
        {
          NonStaticConstBoolean (cond->FileName, cond->Line, TRUE);
        }
      //
      // Attempt to update ranges based on the assumption that the expression
      // is true.
      //
      success = TRUE;
      alwaysfails = FALSE;  //**********THESE ARE REUNDANT NOW
      alwayspasses = TRUE;
      UpdateRanges (cond, FALSE, &success, TRUE, FALSE);
      // If we have just proved that it always fails, then it can't always pass (even if we
      // haven't actually proved that)
      // Save the var changes from the test because accessing a variable in the test is also regarded as
      // an access of it
      testvars = StoreDynamicValues ();
    }
  // Note that this is asymmetrical because the initial assumption is that it always passes
  // and does not always fail.
#ifdef TRACE
  printf ("After UpdateRanges %d\n", tracenum);
#endif
  if (!staticcond)
    {
      // The test has effect at runtime
      //----------
      if (whileloop)
        {
          emit (depth, "while (");
        }
      else
        {
          emit (depth, "if (");
        }
      bf = FALSE;
      printexpr (cond, &bf, TRUE);
      emit (0, ") {\n");
      //----------
    }
  //
  if (staticcond && alwaysfails)
    {
      // This can happen when code is excluded in conditional compilation
      // The 'if' part is the vars from before the if statement
      whenvars = CopyDynamicValues (savedvars);
    }
  else
    {
      int whendepth;

      if (staticcond)
        {
          whendepth = depth;
        }
      else
        {
          whendepth = depth + 1;
        }
      // Analyse the body
      EmitCoverage (whendepth, when_stmt->FileName, when_stmt->Line);
      //
      Analyse (whendepth, when_stmt, TRUE, FALSE);
      // Save the variables after the if part
      whenvars = StoreDynamicValues ();
    }
  if (!staticcond)
    {
      //----------
      emit (depth, "}\n");
      //----------
    }
  if (staticcond && alwayspasses)
    {
      haveelse = FALSE;         // nothing
    }
  else
    {
      // 'if test then ... end if' is analysed as 'if test then ... else null end if'
      // because the negation of the test might be important for range
      // analysis, e.g. in
      // if x > 10 then
      //   x := 10;
      // end if
      // now x <= 10
      int elsedepth;

      if ((else_stmt != NULL) && (!staticcond))
        {
          //----------
          emit (depth, "else {\n");
          //----------
          elsedepth = depth + 1;
        }
      else
        {
          elsedepth = depth;
        }
      // Restore the original variables
      RestoreDynamicValues (savedvars);
      // Attempt to update the ranges based on the negation of the test
      successelse = TRUE;
      //alwaysfailselse = FALSE;
      //alwayspasseselse = TRUE;
      UpdateRanges (cond, TRUE, &successelse, TRUE, FALSE);
      // Analyse the else body
      if (else_stmt != NULL)
        {
          EmitCoverage (elsedepth, else_stmt->FileName, else_stmt->Line);
          Analyse (elsedepth, else_stmt, TRUE, elsecanbecommentsonly);
        }
      elsevars = StoreDynamicValues ();
      haveelse = TRUE;
      if ((else_stmt != NULL) && (!staticcond))
        {
          //----------
          emit (depth, "}\n");
          //----------
        }
    }
  /* Update ranges and initialisations after the if statement */
  RestoreDynamicValues (savedvars);
  if (!haveelse)
    {                           //else_stmt == NULL) {
      // In either case (compiled or not) whenvars is right
      RestoreDynamicValues (whenvars);
    }
  else
    {
      // if test then ... [else null] end if or if test then ... else ... end if
      if (staticcond && alwaysfails)
        {
          /* The if part was excluded, so the result is the outcome of the
           * else part
           */
          RestoreDynamicValues (elsevars);
        }
      else
        {
          /* One of the two parts must have been run, so merge the results
           * of both of them, i.e. whenvars (if) and elsevars (else)
           */
          MergeDynamicValues (whenvars, elsevars, savedvars);
        }
    }
  // An access in the test expression is an access in any case
  if (testvars != NULL)
    {
      struct tDynamicListValue *p, *init;

      p = testvars->ValueList;
      while (p != NULL)
        {
          init = initialvars->ValueList;
          while (init != NULL)
            {
              if (init->var == p->var)
                {
                  if (p->Accessed && (!init->Accessed))
                    {
                      SetAccessed (p->var);
                      break;
                    }
                }
              init = init->Next;
            }
          p = p->Next;
        }
    }
  //
  // Update relations
  //
  if (!haveelse)
    {
      RestoreRelations (whenvars);
    }
  else
    {
      // if ... else ... end if
      if (staticcond && alwaysfails)
        {
          /* The if part was excluded, so the result is the outcome of the
           * else part
           */
          RestoreRelations (elsevars);
        }
      else
        {
          struct tRelation *whenrel, *elserel;
          int k;

          CurrentVars->NumRelations = 0;
          for (j = 0; j < whenvars->NumRelations; j++)
            {
              whenrel = &whenvars->Relations[j];
              if (whenrel->LowSet || whenrel->HighSet)
                {
                  // Search for a corresponding relation in elsevars
                  for (k = 0; k < elsevars->NumRelations; k++)
                    {
                      elserel = &elsevars->Relations[k];
                      if (elserel->LowSet || elserel->HighSet)
                        {
                          if (whenrel->LHS == elserel->LHS
                              && whenrel->LHSAttr == elserel->LHSAttr
                              && whenrel->RHS == elserel->RHS
                              && whenrel->RHSAttr == elserel->RHSAttr)
                            {
                              MergeRelation (whenrel, elserel, FALSE);
                              break;
                            }
                          else if (whenrel->LHS == elserel->RHS
                                   && whenrel->LHSAttr == elserel->RHSAttr
                                   && whenrel->RHS == elserel->LHS
                                   && whenrel->RHSAttr == elserel->LHSAttr)
                            {
                              MergeRelation (whenrel, elserel, TRUE);
                              break;
                            }
                        }
                    }
                }
            }
        }
    }

  FreeDynamicValues (savedvars);
  if (elsevars != NULL)
    FreeDynamicValues (elsevars);
  if (whenvars != NULL)
    FreeDynamicValues (whenvars);
#ifdef TRACE
  printf ("AnalyseWhen %d: done\n", tracenum);
#endif
}

void
ResetVar (struct tVar *v, bool resetinitialisationandaccess)
{
  enum tMode m;

  v->Hidden = FALSE;
  m = v->Mode;
  if (m == MODE_GLOBAL || m == MODE_SHARED)
    {
      v->ShareLock = FALSE;
      if ((!NamedConstantType (v->Type->TypeSpec)) && v->ConstValue == NULL)
        {
          v->Low = v->Type->TypeSpec->Low;
          v->High = v->Type->TypeSpec->High;
          ResetRelations (v);
          if (resetinitialisationandaccess)
            {
              if (!v->InitialisedGlobally)
                {
                  // By setting this to FALSE we will record the fact that
                  // the routine being compiled requires it to have
                  // been initialised, or that it initialises it
                  // itself.
                  v->Initialised = FALSE;
                }
              // We don't know that it wasn't accessed before the call
              // to this procedure, so we can't assume that it wasn't
              v->Accessed = TRUE;
            }
        }
    }
}

void
ResetGlobalRanges (void)
{
  struct tVar *v;
  int j;

  j = 0;
  while (j < CurrentVars->NumVars)
    {
      v = CurrentVars->Vars[j];
      ResetVar (v, TRUE);
      j++;
    }
  v = PublicVarList;
  while (v != NULL)
    {
      ResetVar (v, TRUE);
      v = v->PrivatePublicNext;
    }
  v = PrivateVarList;
  while (v != NULL)
    {
      ResetVar (v, TRUE);
      v = v->PrivatePublicNext;
    }
}

/* Find all the variables that are modified inside the body,
   and set their ranges to the maximum for their types.
*/
void
ScanLoopBody (struct tElt *elt)
{
  bool savedloopscan;
  jmp_buf saved_recovery;

  if (elt == NULL)
    {
      return;
    }
  savedloopscan = LoopScan;
  LoopScan = TRUE;
  memcpy (saved_recovery, error_recovery, sizeof (jmp_buf));
  // Trap errors here so we can restore LoopScan
  while (elt != NULL)
    {

      if (setjmp (error_recovery) == 0)
        {
          if (elt->Type == ASSIGNMENT_ELT)
            {
              struct tAssignment *a = elt->Ptr;
              if (a->Var->Next == NULL
                  && a->Var->ReferenceType == IDENTIFIER_REFERENCE)
                {
                  /* Direct assignment to a simple variable */
                  struct tReference *r;
                  struct tVar *v;

                  r = a->Var;
                  v =
                    FindVariable (elt->FileName, elt->Line, &r, 6, TRUE,
                                  FALSE);
                  if (v != NULL)
                    {
                      if (v->Type->TypeSpec->BaseType == INTEGER_TYPE
                          || v->Type->TypeSpec->BaseType == ACCESS_TYPE
                          || v->Type->TypeSpec->BaseType == BOOLEAN_TYPE)
                        {
                          v->Low = v->Type->TypeSpec->Low;
                          v->High = v->Type->TypeSpec->High;
                          ResetRelations (v);
                        }
                    }
                }
            }
          else if (elt->Type == PROCEDURE_CALL_ELT)
            {
              struct tProcedureCall *p = elt->Ptr;
              enum tBaseType basetype;
              struct tVar *basevar;
              struct tTypeSpec *reftype;
              BIGINT minvalue, maxvalue, initialmin, initialmax;
              struct tUnitDef *unit;
              struct tTypeSpec *accessedtype;
              enum tMode mode;
              int lock;
              bool initialised, wholeobject;
              bool finalisedthis, mismatch, sharedcall, constantaccess, subsystemboundary;

              AnalyseReference (elt->FileName, elt->Line, p->Name, &basetype,
                                &basevar, &reftype, &minvalue, &maxvalue,
                                &initialmin, &initialmax, &unit,
                                &accessedtype, &initialised, &mode, &lock,
                                FALSE, &wholeobject, FALSE, &constantaccess,
                                FALSE, &subsystemboundary);
              if (basetype == PROCEDURE_TYPE)
                {
                  MatchActualsToFormals (elt->FileName, elt->Line, p, reftype,
                                         basevar->Name, FALSE, &finalisedthis,
                                         &mismatch, &sharedcall, FALSE, NULL);
                  ResetOutputParameterRanges (0, p);
                  // Reset ranges of global variables that could have been changed by the called routine
                  struct tGlobalAccess *g;

                  g = reftype->GlobalsAccess;
                  while (g != NULL)
                    {
                      if (g->Write)
                        {
                          ResetVar (g->Var, FALSE);
                        }
                      g = g->Next;
                    }
                  //
                }
            }
          else if (elt->Type == WHEN_ELT)
            {
              struct tWhen *w = elt->Ptr;
              ScanLoopBody (w->When_stmt);
              ScanLoopBody (w->Else_stmt);
            }
          else if (elt->Type == LOOP_ELT)
            {
              struct tLoop *p = elt->Ptr;
              ScanLoopBody (p->Body);
            }
          else if (elt->Type == WHILE_ELT)
            {
              struct tWhile *p = elt->Ptr;
              ScanLoopBody (p->Body);
            }
          else if (elt->Type == DO_ELT)
            {
              struct tDo *p = elt->Ptr;
              ScanLoopBody (p->Body);
            }
          else if (elt->Type == FOR_ELT)
            {
              struct tFor *p = elt->Ptr;
              ScanLoopBody (p->Body);
            }
          else if (elt->Type == CASE_ELT)
            {
              struct tCaseOption *p = ((struct tCase *) (elt->Ptr))->Options;

              while (p != NULL)
                {
                  ScanLoopBody (p->Stmts);
                  p = p->Next;
                }
            }
          else if (elt->Type == DECLARE_BLOCK_ELT)
            {
              struct tDeclareBlock *p = elt->Ptr;

              // Ignore unit test only blocks if not compiling for unit test, and non unit test only
              // blocks if compiling for unit test
              if (((!UnitTest) && p->NotUnitTest)
                  || (UnitTest && p->UnitTest))
                {
                  ScanLoopBody (p->Statements);
                }
            }
          else if (elt->Type == SEPARATE_BLOCK_ELT)
            {
              struct tSeparateBlock *p = elt->Ptr;

              ScanLoopBody (p->Statements);
            }
        }
      else
        {
        }
      elt = elt->Next;
    }
  memcpy (error_recovery, saved_recovery, sizeof (jmp_buf));
  LoopScan = savedloopscan;
}

// Scan a simple expression looking for uses of indexvar apart from indexing
// Return FALSE if such a use is found
bool
ScanSimpleExpression (struct tNode *expr, struct tVar *indexvar)
{
  switch (expr->Op)
    {
    case _PLUS:
    case _MINUS:
    case _TIMES:
    case _DIVIDE:
    case _MOD:
    case _IAND:
    case _IOR:
    case _IXOR:
    case _LT:
    case _GT:
    case _LEQ:
    case _GEQ:
    case _EQ:
    case _NEQ:
    case _AND:
    case _OR:
      if (!ScanSimpleExpression (expr->Left, indexvar))
        return FALSE;
      if (!ScanSimpleExpression (expr->Right, indexvar))
        return FALSE;
      break;
    case _NOT:
      if (!ScanSimpleExpression (expr->Left, indexvar))
        return FALSE;
      break;
    case _CONST_INT:
    case _CONST_BOOL:
    case _CONST_FLOAT:
    case _CONST_STRING:
    case _CONST_CHARACTER:
    case _CONST_NULL:
      break;
    case _VAR:
      if (expr->Var->ReferenceType == IDENTIFIER_REFERENCE)
        {
          struct tReference *name = expr->Var;
          struct tVar *v;

          v =
            FindVariable (expr->FileName, expr->Line, &name, 212, FALSE,
                          TRUE);
          if (v == indexvar)
            {
              return FALSE;
            }
        }
      break;
    case _FN_CALL:
      {
        struct tProcedureCall *p = expr->Call->Ptr;
        struct tActualParameter *a;

        a = p->ActualList;
        while (a != NULL)
          {
            if (!ScanSimpleExpression (a->Expr, indexvar))
              return FALSE;
            a = a->Next;
          }
      }
      break;
    default:
      return FALSE;
    }
  return TRUE;
}

void
AnalyseLoop (int depth, struct tElt *body)
{
  //
  ScanLoopBody (body);
  //----------
  emit (depth, "for (;;) {\n");
  //----------
  Analyse (depth + 1, body, TRUE, FALSE);
  //----------
  emit (depth, "}\n");
  //----------
  ScanLoopBody (body);
  //
}

void
AnalyseWhile (int depth, struct tNode *cond, struct tElt *body)
{
  bool success;

  ScanLoopBody (body);
  AnalyseWhen (depth, cond, body, NULL, TRUE, FALSE);
  ScanLoopBody (body);
  // Assert the negation of the condition
  success = TRUE;
  UpdateRanges (cond, TRUE, &success, TRUE, FALSE);
}

void
AnalyseDo (int depth, struct tElt *body)
{
  emit (depth, "do {\n");
  Analyse (depth + 1, body, TRUE, FALSE);
  emit (depth, "} while (0);\n");
}

void
AnalyseExit (int depth)
{
  //----------
  emit (depth, "break;\n");
  //----------
}

void
AnalyseRepeat (int depth)
{
  //----------
  emit (depth, "continue;\n");
  //----------
}

void
SetControlVarRelation (char *filename, int line,
                       struct tNode *range, struct tVar *controlvar,
                       enum tOp op)
{
  if (range != NULL)
    {
      if (range->Op == _VAR)
        {
          if (range->Var->Next == NULL)
            {
              struct tReference *ref = range->Var;
              struct tVar *lvar =
                FindVariable (filename, line, &ref, 860, FALSE, TRUE);
              bool change;
              StoreRelation (filename, line, controlvar, ATTR_NULL, lvar,
                             GetExprAttribute (range), op, 0, 0, &change);
            }
        }
    }
}

void
AnalyseFor (int depth, char *filename, int line, struct tFor *f)
{
  int SavedNumVars, SavedNumRelations;
  struct tReference *controlref, *highref;
  struct tAssignment assignment;
  struct tVariableDeclaration controldecl, highdecl;
  struct tRHS rhs;
  // Initialise highvar to avoid a warning - actually OK because of statichigh
  struct tVar *controlvar, *highvar = NULL;
  BIGINT loophigh, looplow;
  BIGINT lowmin, lowmax, highmin, highmax;
  struct tUnitDef *highunit, *lowunit;
  bool statichigh, virtualexpr;  //, staticlow

  controlref = MakeNewReference ();
  highref = MakeNewReference ();
  SavedNumVars = CurrentVars->NumVars;
  SavedNumRelations = CurrentVars->NumRelations;
  //
  //----------
  emit (depth, "{\n");
  //
  controldecl = *(struct tVariableDeclaration *) (f->ControlVar->Ptr);
  controldecl.Mode = MODE_LOCAL;
  controlvar =
    AnalyseVarDeclaration (depth + 1, filename, line, &controldecl,
                           PRIVATE_ACCESS);
  if (f->RangeHigh == NULL)
    {
      // for x : t loop
      loophigh = controlvar->High;
      highmin = loophigh;
      highmax = loophigh;
      highunit = controlvar->Unit;
      statichigh = TRUE;
    }
  else
    {
      // for x : t in L..H loop
      TestIntegerExpression (f->RangeHigh, &highmin, &highmax, &highunit,
                             FALSE, FALSE, &virtualexpr);
      loophigh = highmax;
      controlvar->High = loophigh;
      if (highmin == highmax)
        {
          statichigh = TRUE;
        }
      else
        {
          statichigh = FALSE;
        }
    }
  controlref->ReferenceType = IDENTIFIER_REFERENCE;
  controlref->VarName = controldecl.VarNames->SymbolName;
  controlref->ReferenceAbbreviation = NULL;
  controlref->Next = NULL;
  rhs.RHSType = EXPR_RHS;
  if (f->RangeLow == NULL)
    {
      // for x : t loop
      rhs.Expr =
        NewNode (filename, line, _CONST_INT, NULL, NULL, controlvar->Low, 0.0,
                 NULL, NULL, NULL, NULL);
      looplow = controlvar->Low;
      lowmin = looplow;
      lowmax = looplow;
    }
  else
    {
      // for x : t in L..H loop
      rhs.Expr = f->RangeLow;
      TestIntegerExpression (f->RangeLow, &lowmin, &lowmax, &lowunit, FALSE,
                             FALSE, &virtualexpr);
      looplow = lowmin;
      controlvar->Low = looplow;
    }
  if (!statichigh)
    {
      highdecl = controldecl;
      highdecl.VarNames = MakeIdentifierListEntry(MakeEndName (controldecl.VarNames->SymbolName));
      highvar =
        AnalyseVarDeclaration (depth + 1, filename, line, &highdecl,
                               PRIVATE_ACCESS);
    }
  assignment.MakeNew = FALSE;
  assignment.Var = controlref;
  assignment.RHS = &rhs;
  AnalyseAssignmentStmt (depth + 1, filename, line, &assignment, FALSE);
  // Reset the range of the control var after the assignment and lock it
  controlvar->Low = looplow;
  controlvar->High = loophigh;
  controlvar->Lock = 1;
  if (statichigh)
    {
      if (loophigh > controlvar->Type->TypeSpec->High)
        {
          err (filename, line,
               "Upper bound of loop %" PRINTBIG
               " is greater than the upper bound %" PRINTBIG " of type %s",
               loophigh, controlvar->Type->TypeSpec->High,
               controlvar->Type->Name);
        }
      TestUnitMatch (highunit, controlvar->Type->TypeSpec->Unit, f->RangeHigh,
                     FALSE);
    }
  else
    {
      highref->ReferenceType = IDENTIFIER_REFERENCE;
      highref->VarName = highdecl.VarNames->SymbolName;
      highref->ReferenceAbbreviation = NULL;
      highref->Next = NULL;
      rhs.RHSType = EXPR_RHS;
      rhs.Expr = f->RangeHigh;
      assignment.MakeNew = FALSE;
      assignment.Var = highref;
      assignment.RHS = &rhs;
      AnalyseAssignmentStmt (depth + 1, filename, line, &assignment, FALSE);
      highvar->Lock = 1;
    }
  //
  if (Annotate)
    {
      emit (depth + 1,
            "/* The potential range for the loop is %" PRINTBIG "..%" PRINTBIG
            " */\n", controlvar->Low, loophigh);
    }
  if (statichigh)
    {
      emit (depth + 1, "while (%s <= %" PRINTBIG ") {\n", controlvar->CName,
            loophigh);
    }
  else
    {
      emit (depth + 1, "while (%s <= %s) {\n", controlvar->CName,
            highvar->CName);
    }
  if (looplow > loophigh)
    {
      err (filename, line,
           "The minimum lower bound of the loop %" PRINTBIG
           " is always greater than the maximum upper bound %" PRINTBIG "",
           looplow, loophigh);
    }
  //
  EmitCoverage (depth + 2, f->Body->FileName, f->Body->Line);
  //
  // General case; reset ranges of variables that are modified in the loop
  ResetRelations (controlvar);
  // See above
  SetControlVarRelation (filename, line, f->RangeLow, controlvar, _GEQ);
  SetControlVarRelation (filename, line, f->RangeHigh, controlvar, _LEQ);
  // Then scan the body, which will might undo relations just set in the case of simple integer variables
  // that are modified within the loop
  ScanLoopBody (f->Body);
  // Assert relations for the
  Analyse (depth + 2, f->Body, TRUE, FALSE);
  ScanLoopBody (f->Body);       // Repeat the scan after the loop because we don't know the
  // actual starting ranges before the analysis, in general
  // In the case of a range that extends to the maximum value of the underlying type, we would
  // get a wrap by incrementing, so we have to test for exact equality first
  if (controlvar->Type->TypeSpec->CType->Max == loophigh)
    {
      if (Annotate)
        {
          emit (depth + 2,
                "/* An extra test is required in this case to avoid wrap problems */\n");
        }
      emit (depth + 2, "if (%s == ", controlvar->CName);
      printbigint (loophigh);
      emit (0, ") break;\n");
    }
  emit (depth + 2, "%s++;\n", controlvar->CName);
  emit (depth + 1, "}\n");
  //
  emit (depth, "}\n");
  //
  CurrentVars->NumVars = SavedNumVars;
  CurrentVars->NumRelations = SavedNumRelations;
}

void errDumpRange (BIGINT minvalue, BIGINT maxvalue);

BIGINT
EvalCaseValue (char *filename, int line, struct tNode *expr,
               struct tTypeSpec *switchtype)
{
  bool staticint, fieldsearch;
  struct tFieldSpec *f;
  unsigned fieldnum;
  struct tUnitDef *unit;

  fieldsearch = FALSE;
  if (switchtype != NULL)
    {
      if (switchtype->BaseType == RECORD_TYPE)
        {
          fieldsearch = TRUE;
        }
    }
  if (fieldsearch)
    {
      if (expr->Op == _VAR)
        {
          if (expr->Var->Next == NULL)
            {
              f = switchtype->FieldList;
              fieldnum = 0;
              while (f != NULL)
                {
                  if (!strcmp (f->Name, expr->Var->VarName))
                    {
                      return fieldnum;
                    }
                  f = f->Next;
                  fieldnum++;
                }
              err (filename, line, "Unknown field name %s",
                   expr->Var->VarName);
            }
          else
            {
              err (filename, line, "Simple field name expected");
            }
        }
      else
        {
          err (filename, line, "Field name expected");
        }
      return 0;                 // In case analysis proceeds
    }
  else
    {
      return EvalStaticInteger (expr, &staticint, TRUE, &unit);
    }
}

void
CheckRanges (char *filename, int line,
             struct tCaseOption *caseopts,
             struct tArrayInitialisationEntry *arr, BIGINT lowvalue,
             BIGINT highvalue, struct tTypeSpec *rangetype)
{
  int numranges;
  struct tCaseOption *opt;
  struct tArrayInitialisationEntry *arrp;
  struct tRange *r;
  BIGINT testvalue;
  bool matched, endofrange, error;
  BIGINT minvalue, maxvalue;

  error = FALSE;
  numranges = 0;
  opt = caseopts;
  arrp = arr;
  while (opt != NULL || arrp != NULL)
    {
      if (opt != NULL)
        r = opt->Ranges;
      else
        r = arrp->Ranges;
      while (r != NULL)
        {
          r->Used = FALSE;
          numranges++;
          r = r->Next;
        }
      if (opt != NULL)
        opt = opt->Next;
      else
        arrp = arrp->Next;
    }
  // Compute the ranges from the variable and the type (the latter in case continuing from
  // a range error)
  //
  testvalue = lowvalue;
  endofrange = FALSE;
  while (testvalue <= highvalue)
    {
      matched = FALSE;
      opt = caseopts;
      arrp = arr;
      while (opt != NULL || arrp != NULL)
        {
          if (opt != NULL)
            r = opt->Ranges;
          else
            r = arrp->Ranges;
          while (r != NULL)
            {
              if (!r->Used)
                {
                  minvalue =
                    EvalCaseValue (filename, line, r->MinValue, rangetype);
                  maxvalue =
                    EvalCaseValue (filename, line, r->MaxValue, rangetype);
                  if (minvalue < testvalue)
                    {
                      if (testvalue == lowvalue)
                        {
                          errstart (filename, line);
                          errcont ("Range ");
                          errDumpRange (minvalue, maxvalue);
                          errcont
                            (" in the case options extends less than the range of the control variable");
                          errend ();
                        }
                      else
                        {
                          errstart (filename, line);
                          errcont ("Overlapping range ");
                          errDumpRange (minvalue, maxvalue);
                          errend ();
                        }
                      error = TRUE;
                    }
                  else if (minvalue == testvalue)
                    {
                      r->Used = TRUE;
                      matched = TRUE;
                      numranges--;
                      if (maxvalue == highvalue)
                        {
                          endofrange = TRUE;
                        }
                      else if (maxvalue > highvalue)
                        {
                          errstart (filename, line);
                          errcont ("Range ");
                          errDumpRange (minvalue, maxvalue);
                          errcont
                            (" in the case options extends greater than the range of the control variable");
                          errend ();
                          error = TRUE;
                          endofrange = TRUE;
                        }
                      else
                        testvalue = maxvalue + 1;
                    }
                }
              if (endofrange)
                break;
              r = r->Next;
            }
          if (endofrange)
            break;
          if (opt != NULL)
            opt = opt->Next;
          else
            arrp = arrp->Next;
        }
      if (!matched)
        {
          err (filename, line, "Value %" PRINTBIG " is not covered",
               testvalue);
          error = TRUE;
          break;
        }
      if (endofrange)
        break;
    }
  if (error)
    {
      err (filename, line,
           "Could not uniquely match all values in the range %" PRINTBIG "..%"
           PRINTBIG "", lowvalue, highvalue);
    }
  if ((!error) && numranges != 0)
    {
      error = TRUE;
      errstart (filename, line);
      errcont ("%d ranges unused:\n", numranges);
      opt = caseopts;
      arrp = arr;
      while (opt != NULL || arrp != NULL)
        {
          if (opt != NULL)
            r = opt->Ranges;
          else
            r = arrp->Ranges;
          while (r != NULL)
            {
              if (!r->Used)
                {
                  errcont ("  ");
                  minvalue =
                    EvalCaseValue (filename, line, r->MinValue, rangetype);
                  maxvalue =
                    EvalCaseValue (filename, line, r->MaxValue, rangetype);
                  errDumpRange (minvalue, maxvalue);
                  errcont ("\n");
                }
              r = r->Next;
            }
          if (opt != NULL)
            opt = opt->Next;
          else
            arrp = arrp->Next;
        }
      errend ();
    }
}

void
AnalyseCase (int depth, char *filename, int line, struct tCase *c)
{
  struct tVar *casev;
  struct tCaseOption *opt;
  struct tDynamicValues *initialvars, *PreviousVars = NULL;
  BIGINT min, max;
  BIGINT lowvalue, highvalue;
  struct tRange *r;
  BIGINT minvalue, maxvalue, testvalue;
  bool first, testingtag, virtualexpr;
  struct tUnitDef *unit;
  struct tTypeSpec *casetypespec;

  casev = NULL;
  testingtag = FALSE;
  if (c->CaseExpr->Op == _VAR)
    {
      struct tReference *refp = c->CaseExpr->Var;

      if (refp->ReferenceType == IDENTIFIER_REFERENCE && refp->Next == NULL)
        {
          casev = FindVariable (filename, line, &refp, 7, FALSE, TRUE);
          if (casev->Mode == MODE_GLOBAL || casev->Mode == MODE_SHARED)
            {
              RecordGlobalAccess (line, GetGlobalsAccess (), casev,
                                  TRUE, FALSE, (!casev->Initialised), FALSE);
            }
          else
            {
              if (!casev->Initialised)
                {
                  err (filename, line, "Variable %s might not be initialised (9)",
                       casev->Name);
                }
            }
          if (   casev->Type->TypeSpec->BaseType == RECORD_TYPE
              && casev->Type->TypeSpec->RecordType == REC_UNION)
            {
              // Special case of testing the tag of a union record
              testingtag = TRUE;
              lowvalue = casev->Type->TypeSpec->Low;
              if (lowvalue < casev->Low)
                lowvalue = casev->Low;
              highvalue = casev->Type->TypeSpec->High;
              if (highvalue > casev->High)
                highvalue = casev->High;
            }
        }
    }
  if (!testingtag)
    {
      TestIntegerExpression (c->CaseExpr, &lowvalue, &highvalue,
                             &unit, FALSE, FALSE, &virtualexpr);
    }
  opt = c->Options;
  //
  if (casev != NULL)
    {
      casetypespec = casev->Type->TypeSpec;
    }
  else
    {
      casetypespec = NULL;
    }
  CheckRanges (filename, line, opt, NULL, lowvalue, highvalue, casetypespec);
  /* Analyse case bodies */
  //----------
  if (testingtag)
    {
      emit (depth, "switch (");
      if (PassedByReference (casev))
        emit (0, "%s->", casev->CName);
      else
        emit (0, "%s.", casev->CName);
      emit (0, "tag) {\n");
      SetAccessed (casev);              // Using the variable for a case test counts as accessing it
    }
  else
    {
      bool bf = FALSE;

      emit (depth, "switch (");
      printexpr (c->CaseExpr, &bf, TRUE);
      emit(0, ") {\n");
    }
  //----------
  initialvars = StoreDynamicValues ();
  // Within the case statement, disallow whole record modifications to the switch variable because they
  // could invalidate the test
  opt = c->Options;
  first = TRUE;
  if (casev != NULL)
    {
      casev->Lock++;
    }
  while (opt != NULL)
    {
      //----------
      r = opt->Ranges;
      while (r != NULL)
        {
          minvalue =
            EvalCaseValue (filename, line, r->MinValue, casetypespec);
          maxvalue =
            EvalCaseValue (filename, line, r->MaxValue, casetypespec);
          for (testvalue = minvalue; testvalue <= maxvalue; testvalue++)
            {
              bool casewritten = FALSE;

              if (casev != NULL)
                {
                  if (casev->Type->TypeSpec->EnumList != NULL)
                    {
                      struct tEnum *e;
                      BIGINT enumval;

                      enumval = casev->Type->TypeSpec->Low;
                      e = casev->Type->TypeSpec->EnumList;
                      while (e != NULL && enumval != testvalue)
                        {
                          e = e->Next;
                          enumval++;
                        }
                      if (e != NULL)
                        {
                          if (e->ConstVar != NULL)
                            {
                              emit (depth + 1, "case %s:\n", e->ConstVar->CName);
                              casewritten = TRUE;
                            }
                        }
                    }
                }
              if (!casewritten)
                {
                  emit (depth + 1, "case %" PRINTBIG ":\n", testvalue);
                }
            }
          r = r->Next;
        }
      //----------
      r = opt->Ranges;
      min = EvalCaseValue (filename, line, r->MinValue, casetypespec);
      max = EvalCaseValue (filename, line, r->MaxValue, casetypespec);
      r = r->Next;
      while (r != NULL)
        {
          minvalue =
            EvalCaseValue (filename, line, r->MinValue, casetypespec);
          maxvalue =
            EvalCaseValue (filename, line, r->MaxValue, casetypespec);
          if (minvalue < min)
            min = minvalue;
          if (maxvalue > max)
            max = maxvalue;
          r = r->Next;
        }
      if (casev != NULL)
        {
          if (casev->Low < min)
            casev->Low = min;
          if (casev->High > max)
            casev->High = max;
        }
      EmitCoverage (depth + 2, opt->Stmts->FileName, opt->Stmts->Line);
      Analyse (depth + 2, opt->Stmts, TRUE, FALSE);
      //----------
      emit (depth + 2, "break;\n");
      //----------
      if (!first)
        {
          // Merge the ranges with the previous case option. At least one of the case branches must
          // be run.
          struct tDynamicListValue *p;
          struct tVar *vp;

          p = PreviousVars->ValueList;
          while (p != NULL)
            {
              vp = p->var;
              vp->Low = MinRange (vp->Low, p->Low);
              vp->High = MaxRange (vp->High, p->High);
              // Initialised if initialised after EVERY branch
              vp->Initialised = (vp->Initialised && p->Initialised);
              // Ever initialised if ever initialised after ANY branch
              //CurrentVars->Vars[j]->EverInitialised =
              //  CurrentVars->Vars[j]->EverInitialised || PreviousVars->Vars[j]->EverInitialised;
              // Finalised if finalised after EVERY branch
              vp->Finalised = (vp->Finalised && p->Finalised);
              // Accessed if accessed after ANY branch
              vp->Accessed = (vp->Accessed || p->Accessed);
              // Ever accessed if ever accessed after ANY branch
              vp->EverAccessed = (vp->EverAccessed || p->EverAccessed);
              //
              p = p->Next;
            }
        }
      FreeDynamicValues (PreviousVars);
      PreviousVars = StoreDynamicValues ();
      first = FALSE;
      RestoreDynamicValues (initialvars);
      opt = opt->Next;
    }
  if (casev != NULL)
    {
      casev->Lock--;
    }
  //----------
  if (CompilerTest)
    {
      emit (depth + 1, "default : __case_error(%d);\n", line);
    }
  emit (depth, "}\n");
  //----------
  {
    struct tDynamicListValue *p;
    struct tVar *vp;

    p = PreviousVars->ValueList;
    while (p != NULL)
      {
        vp = p->var;
        vp->Low = p->Low;
        vp->High = p->High;
        if (p->Initialised)
          {
            SetInitialised (vp);
          }
        if (p->Finalised)
          vp->Finalised = TRUE;
        if (p->Accessed)
          SetAccessed (vp);
        //if (PreviousVars->Vars[j]->EverInitialised)
        //      CurrentVars->Vars[j]->EverInitialised = TRUE;
        if (p->EverAccessed)
          vp->EverAccessed = TRUE;
        p = p->Next;
      }
    p = initialvars->ValueList;
    while (p != NULL)
      {
        if (p->var->Initialised && (!p->Initialised))
          {
            // Initialised in the switch statement
            ResetRelations (p->var);
          }
        p = p->Next;
      }
  }
  FreeDynamicValues (initialvars);
  FreeDynamicValues (PreviousVars);
}

void
AnalyseInitialisations (int depth, struct tElt *elt)
{
  struct tVariableDeclaration *v;
  struct tReference *ref;
  struct tAssignment a;
  struct tVarListEntry *vlist;

  ref = MakeNewReference ();
  while (elt != NULL)
    {
      if (elt->Type == VAR_DECLARATION_ELT)
        {
          v = elt->Ptr;
          vlist = v->Vars;
          while (vlist != NULL)
            {
              if (vlist->Var->AccessImage || vlist->Var->AccessMappedDevice)
                {
                  vlist->Var->Initialised = TRUE;
                  // TestMappedDevices must be initialised by unit test code
                }
              else if (v->ConstValue != NULL)
                {
                  ref->ReferenceType = IDENTIFIER_REFERENCE;
                  ref->VarName = vlist->Var->Name;
                  ref->Index = NULL;
                  ref->Next = NULL;
                  ref->ReferenceAbbreviation = NULL;
                  ref->ReferenceAbbreviationEnd = NULL;
                  a.Var = ref;
                  a.RHS = v->ConstValue;
                  a.MakeNew = FALSE;
                  AnalyseAssignmentStmt (depth, elt->FileName, elt->Line, &a,
                                         TRUE);
                }
              vlist = vlist->Next;
            }
        }
      elt = elt->Next;
    }
}

void
ApplyAliasLocks (int varfrom)
{
  int j;

  for (j = varfrom; j < CurrentVars->NumVars; j++)
    {
      if (CurrentVars->Vars[j]->Referent != NULL
          && (!CurrentVars->Vars[j]->WholeObjectReference))
        CurrentVars->Vars[j]->Referent->Lock++;
    }
}

void
ReverseAliasLocks (int varfrom)
{
  int j;

  for (j = varfrom; j < CurrentVars->NumVars; j++)
    {
      if (CurrentVars->Vars[j]->Referent != NULL
          && (!CurrentVars->Vars[j]->WholeObjectReference))
        CurrentVars->Vars[j]->Referent->Lock--;
    }
}

void
CheckFinalisationAndAccessOfLocals (int depth, char *filename, int line,
                                    int varfrom, bool checkaccessbeforeexit)
{
  int j;
  struct tVar *v;

  for (j = varfrom; j < CurrentVars->NumVars; j++)
    {
      v = CurrentVars->Vars[j];
      if (v->Mode == MODE_LOCAL)
        {
          if (v->Type->TypeSpec->Controlled)
            {
              if (!v->Finalised)
                {
                  err_and_continue (filename, line,
                                    "Local variable %s has not been finalised",
                                    v->Name);
                }
            }
          if (checkaccessbeforeexit)
            {
              CheckAccessed (filename, line, v);
            }
          if (v->Referent == NULL && (!v->AccessMappedDevice)
              && (!v->TestMappedDevice) && (!v->EverAccessed))
            {
              err_and_continue (filename, line,
                                "Local variable %s is never used", v->Name);
            }
          if (RequiresFreeing (v->Type->TypeSpec))
            {
              CallFreeInternal (depth, line, "", v->CName, v->Type->TypeSpec);
            }
        }
    }
}

void
printformalparameterlist (int depth, struct tFormalParameter *formalslist,
                          bool toheader)
{
  struct tFormalParameter *formal;
  bool byref;

  emit (0, "(");
  if (formalslist == NULL)
    {
      emit (0, "void");
    }
  else
    {
      formal = formalslist;
      while (formal != NULL)
        {
          byref = ByReference (formal);
          if (formal != formalslist)
            emitstr (",");
          emitstr ("\n");
          emit (depth, "");
          if (formal->Var->AccessVolatile)
            {                   // includes mapped device and shared atomic
              emitstr ("volatile ");
            }
          if (formal->Mode == MODE_IN)
            {
              if (byref)
                {
                  emitstr ("const ");
                }
              else if (formal->TypeSpec->BaseType == CSTRING_TYPE || formal->TypeSpec->BaseType == ADDRESS_TYPE)
                {
                  // Already have a const prefix
                }
              else if (formal->TypeSpec->BaseType == ARRAY_TYPE)
                {
                  if (formal->TypeSpec->MinDefinite
                      && formal->TypeSpec->MaxDefinite)
                    {
                      emitstr ("const ");
                    }
                  else
                    {
                      emitstr ("_const");
                    }
                }
            }
          printtypename (formal->TypeSpec);
          emit (0, " ");
          if (byref)
            emit (0, "*");
          emit (0, "%s", formal->Name);
          if (!toheader)
            {
              if (byref)
                {
                  AddToLocals ("*", gettypename (formal->TypeSpec));
                }
              else
                {
                  AddToLocals ("", gettypename (formal->TypeSpec));
                }
            }
          // For indefinite arrays, add extra parameters for the index bounds
          if (formal->TypeSpec->BaseType == ARRAY_TYPE)
            {
              if (!formal->TypeSpec->MinDefinite)
                {
                  emit (0, ", %s %s", DefaultArrayIndex,
                        MinName (formal->Name));
                  AddToLocals ("", DefaultArrayIndex);
                }
              if (!formal->TypeSpec->MaxDefinite)
                {
                  emit (0, ", %s %s", DefaultArrayIndex, MaxName (formal->Name));       //
                  AddToLocals ("", DefaultArrayIndex);
                }
            }
          formal = formal->Next;
        }
    }
  emit (0, ")");
}

void
WriteVarDeclaration (int depth, struct tVar *var,
                     struct tVariableDeclaration *vardecl, bool writelineattr,
                     struct tVar *namedvar, bool constaccess)
{
  if (writelineattr)
    {
      if (var->TargetAttributeLine != NULL)
        {
          emit (depth, "%s\n", var->TargetAttributeLine);
        }
      if (var->Type->TypeSpec->TargetAttributeLine != NULL)
        {
          emit (depth, "%s\n", var->Type->TypeSpec->TargetAttributeLine);
        }
    }
  if (depth != 0)
    {
      emit (depth, "");
    }
  if (vardecl->Mode == MODE_LOCAL
      && (WithResume || (UnitTestWithResume && UnitTest)))
    {
      emitstr ("static ");
    }
  if (var->AccessVolatile)
    emitstr ("volatile ");
  if (var->AccessImage)
    emitstr ("const ");
  if (var->TargetAttribute != NULL)
    {
      emit (0, "%s ", var->TargetAttribute);
    }
  if (var->Type->TypeSpec->TargetAttribute != NULL)
    {
      emit (0, "%s ", var->Type->TypeSpec->TargetAttribute);
    }
  printtypename (var->Type->TypeSpec);
  emitstr (" ");
  if (vardecl->Names != NULL)
    {
      if (namedvar->AccessVolatile)
        emit (0, "volatile ");
      if (constaccess)
        emit(0, "const ");
      emit (0, "*");
    }
  printvarname (var);
}

void
WriteLockDeclaration (struct tVar *var)
{
  emit (0, "__LOCK_TYPE %s", LockPrefix);
  printvarname (var);
  emit (0, ";\n\n");
}

void
WriteInitialisationTerm (int depth, char *filename, int line,
                         struct tTypeSpec *typespec, struct tRHS *rhs,
                         bool sameline)
{
  bool success;
  enum tBaseType basetype;
  enum tRHSType rhstype;
  int indent;

  basetype = typespec->BaseType;
  rhstype = rhs->RHSType;
  if (sameline)
    {
      indent = 0;
    }
  else
    {
      indent = depth;
    }
  if (basetype == INTEGER_TYPE)
    {
      BIGINT IntVal;
      struct tUnitDef *unit;

      if (rhstype != EXPR_RHS)
        {
          err (filename, line, "Integer expression expected");
        }
      IntVal = EvalStaticInteger (rhs->Expr, &success, TRUE, &unit);
      TestUnitMatch (unit, typespec->Unit, rhs->Expr, FALSE);
      if (IntVal >= typespec->Low && IntVal <= typespec->High)
        {
          bool bf = FALSE;
          printexpr (rhs->Expr, &bf, TRUE);
          if (Annotate && (rhs->Expr->Op != _CONST_INT))
            {
              emit (0, " /* ");
              printbigint (IntVal);
              emit (0, " */");
            }
        }
      else
        {
          err (rhs->Expr->FileName, rhs->Expr->Line,
               "Initialisation value %" PRINTBIG " is out of range", IntVal);
        }
    }
  else if (basetype == FLOATING_TYPE)
    {
      BIGFLOAT FloatVal;
      struct tUnitDef *unit;
      bool bf = FALSE;

      if (rhstype != EXPR_RHS)
        {
          err (filename, line, "Floating point expression expected");
        }
      FloatVal = EvalStaticFloat (rhs->Expr, &success, TRUE, &unit);
      printexpr (rhs->Expr, &bf, TRUE);
      if (Annotate && (rhs->Expr->Op != _CONST_FLOAT))
        {
          emit (0, " /* ");
          emit (0, "%lf", (double) FloatVal);
          emit (0, " */");
        }
    }
  else if (basetype == BOOLEAN_TYPE)
    {
      BIGINT BoolVal;
      bool bf = FALSE;

      if (rhstype != EXPR_RHS)
        {
          err (filename, line, "Boolean expression expected 3");
        }
      BoolVal = EvalStaticBoolean (rhs->Expr, &success, TRUE);
      printexpr (rhs->Expr, &bf, TRUE);
      if (Annotate && (rhs->Expr->Op != _CONST_BOOL))
        {
          emit (0, " /* ");
          if (BoolVal)
            emit (0, "true");
          else
            emit (0, "false");
          emit (0, " */");
        }
    }
  else if (basetype == CSTRING_TYPE)
    {
      if (rhstype != EXPR_RHS)
        {
          err (filename, line, "String expression expected");
        }
      if (rhs->Expr->Op == _CONST_STRING)
        {
          emit (indent, "%s", rhs->Expr->String);
        }
      else if (rhs->Expr->Op == _CONST_NULL)
        {
          emit (indent, "NULL");
        }
      else if (rhs->Expr->Op == _VAR)
        {
          enum tBaseType basetype;
          struct tVar *basevar;
          struct tTypeSpec *reftype;
          BIGINT min, max, initialmin, initialmax;
          struct tUnitDef *unit;
          struct tTypeSpec *accessedtype;
          enum tMode mode;
          int lock;
          bool initialised, wholeobject, constantaccess, subsystemboundary;

          AnalyseReference (rhs->Expr->FileName, rhs->Expr->Line,
                            rhs->Expr->Var, &basetype, &basevar, &reftype,
                            &min, &max, &initialmin, &initialmax, &unit,
                            &accessedtype, &initialised, &mode, &lock, FALSE,
                            &wholeobject, FALSE, &constantaccess, FALSE, &subsystemboundary);
          if (reftype == ConstantStringTypespec)
            {
              emit (indent, "%s", basevar->CName);
            }
          else
            {
              err (filename, line, "String constant expected");
            }
        }
      else
        {
          err (filename, line, "String expression or null expected");
        }
    }
  else if (basetype == ACCESS_TYPE)
    {
      bool bf = FALSE;
      struct tTypeSpec *accessedtype;

      if (!typespec->UnmanagedAccess)
        {
          err (filename, line,
               "Only unchecked access types can be used in static initialisation");
        }
      if (rhstype != EXPR_RHS)
        {
          err (filename, line, "Access expression expected");
        }
      EvalStaticAccess (rhs->Expr, &success, &accessedtype, TRUE);
      if (accessedtype != NULL)
        {
          // NULL would be the constant null
          if (accessedtype != typespec->ElementTypeSpec)
            {
              err (rhs->Expr->FileName, rhs->Expr->Line,
                   "Access type mismatch in static initialisation");
            }
        }
      printexpr (rhs->Expr, &bf, TRUE);
    }
  else if (basetype == ARRAY_TYPE)
    {
      struct tArrayInitialisationEntry *init, *a;
      unsigned numactual, numexpected, k;
      BIGINT *actualindexes;
      struct tRHS **actualexpressions;
      bool found;
      BIGINT j;
      struct tUnitDef *unit;
      BIGINT low = typespec->Low;
      BIGINT high = typespec->High;
      struct tRange *rangeptr;

      if (rhstype != ARRAY_RHS)
        {
          err (filename, line, "Constant array expected");
        }
      init = rhs->ArrayExpr->ExplicitInit;
      if (init == NULL)
        {
          err (filename, line,
               "Explicit initialisation list required for constant array (not [range ...])");
        }
      a = init;
      numactual = 0;
      while (a != NULL)
        {
          rangeptr = a->Ranges;
          if (rangeptr == NULL)
            {                   // [ 1, 2, 3 ] form
              numactual++;
            }
          else
            {
              while (rangeptr != NULL)
                {
                  numactual++;
                  rangeptr = rangeptr->Next;
                }
            }
          a = a->Next;
        }
      numexpected = high - low + 1;
      if (numactual != numexpected)
        {
          err (filename, line,
               "In array initialisation expected %u elements, got %u",
               numexpected, numactual);
        }
      // Store the actual indexes
      actualindexes = checked_malloc (numactual * sizeof (BIGINT));
      actualexpressions = checked_malloc (numactual * sizeof (struct tRHS *));
      a = init;
      j = low;
      for (k = 0; k < numactual; k++)
        {
          if (a->Ranges == NULL)
            {
              actualindexes[k] = j;     // [1, 2, 3] form
            }
          else
            {
              actualindexes[k] =
                EvalStaticInteger (a->Ranges->MinValue, &success, TRUE,
                                   &unit);
            }
          actualexpressions[k] = a->RHS;
          a = a->Next;
          j++;
        }
      emit (indent, "{\n");
      for (j = low; j <= high; j++)
        {
          found = FALSE;
          for (k = 0; k < numactual; k++)
            {
              if (actualindexes[k] == j)
                {
                  found = TRUE;
                  if (!sameline)
                    emit (depth + 1, "");
                  WriteInitialisationTerm (depth + 1, filename, line,
                                           typespec->ElementTypeSpec,
                                           actualexpressions[k], TRUE);
                  if (j != high)
                    emitstr (",");
                  if (sameline)
                    {
                      if (j != high)
                        {
                          emitstr (" ");
                        }
                    }
                  else
                    {
                      emitstr ("\n");
                    }
                  break;
                }
            }
          if (!found)
            {
              err (filename, line,
                   "Array index %" PRINTBIG
                   " not found in constant initialisation expression", j);
            }
        }
      emit (indent, "}");
      free (actualindexes);
      free (actualexpressions);
    }
  else if (basetype == RECORD_TYPE)
    {
      struct tFieldInitialisation *init, *a;
      struct tFieldSpec *f;
      unsigned numactual, numexpected;
      bool found;

      if (rhstype != RECORD_RHS)
        {
          err (filename, line, "Record initialisation expression expected");
        }
      if (typespec->RecordType != REC_RECORD)
        {
          err (filename, line,
               "Unions cannot be initialised as constant images");
        }
      init = rhs->RecordExpr;
      a = init;
      numactual = 0;
      while (a != NULL)
        {
          numactual++;
          a = a->Next;
        }
      f = typespec->FieldList;
      numexpected = 0;
      while (f != NULL)
        {
          if (f->FieldSet)
            {
              err (filename, line,
                   "Records with bit field offsets cannot be initialised as constant images");
            }
          numexpected++;
          f = f->Next;
        }
      if (numactual != numexpected)
        {
          err (filename, line,
               "In record initialisation expected %u fields, got %u",
               numexpected, numactual);
        }
      emit (indent, "{");
      f = typespec->FieldList;
      while (f != NULL)
        {
          a = init;
          found = FALSE;
          while (a != NULL)
            {
              if (!strcmp (f->Name, a->Name))
                {
                  found = TRUE;
                  if (!sameline)
                    emit (depth + 1, "");
                  WriteInitialisationTerm (depth + 1, filename, line,
                                           f->Typespec, a->RHS, TRUE);
                  if (f->Next != NULL)
                    emitstr (",");
                  if (sameline)
                    {
                      if (f->Next != NULL)
                        {
                          emitstr (" ");
                        }
                    }
                  else
                    {
                      emitstr ("\n");
                    }
                  break;
                }
              a = a->Next;
            }
          if (!found)
            {
              err (filename, line,
                   "Record field %s not found in constant initialisation expression",
                   f->Name);
            }
          f = f->Next;
        }
      emit (indent, "}");
    }
  else
    {
      err (filename, line, "Error in constant initialisation: unknown type");
    }
}

void
WriteInitialiser (char *filename, int line, struct tVar *var)
{
  if (var->ConstValue == NULL)
    {
      err (filename, line, "Constant initialisation expression expected");
    }
  emitstr ("\n");
  WriteInitialisationTerm (1, filename, line, var->Type->TypeSpec,
                           var->ConstValue, FALSE);
}

struct tVar *
CreateWholeVarAlias (char *filename, int line,
                     char *name, struct tVar *basevar)
{
  struct tVar *var;

  CheckForVarOverflow (filename, line);
  var = MallocVar (filename, line);
  CurrentVars->Vars[CurrentVars->NumVars] = var;
  var->Name = name;
  var->Referent = basevar;
  var->WholeObjectReference = TRUE;
  var->Type = &ConstantIntegerType;     // A type is required; constant integer will have no other effect
  CurrentVars->NumVars++;
  return var;
}

void
EmitLibraryInclude (char *emittype, char *name)
{
  emit (0, "#%s INCLUDE_%s\n", emittype, name);
}

struct tVar *
AnalyseVarDeclaration (int depth, char *filename, int line,
                       struct tVariableDeclaration *v, enum tAccess access)
{
  struct tVar *var = NULL;
  BIGINT minvalue, maxvalue, initialmin, initialmax;
  struct tUnitDef *unit;
  struct tTypeSpec *accessedtype;
  bool initialised, lock, wholeobject;
  enum tBaseType basetype;
  struct tVar *basevar;
  struct tTypeSpec *reftype;
  enum tMode mode;
  struct tReference *nameref;
  bool constantaccess, subsystemboundary;
  struct tPackage *sharedpack =
    SharedAncestorPackage (CurrentPackage, CurrentPackage);
  bool allpublic;
  struct tVarListEntry *vfirst, *vlist;

  // Test for nested public packages down to a shared ancestor package (or to the system if none)
  allpublic = TRUE;
  {
    struct tPackage *p = CurrentPackage;
    while (p != sharedpack)
      {
        if (p->Access != PUBLIC_ACCESS)
          {
            allpublic = FALSE;
            break;
          }
      }
  }
  basevar = NULL;               // Initially; may be overwritten by name for something other than a variable
  if (v->NameFor == NAME_TYPE)
    {
      if (v->TypeName->Attribute != NULL)
        {
          // type name for t'attr
          struct tType *namedtype;

          namedtype = FindType (filename, line, v->TypeName);
          basevar = MallocVar (filename, line);
          CurrentVars->Vars[CurrentVars->NumVars] = basevar;
          basevar->Name = MakeUniqueName ("renames");
          basevar->Type = namedtype->TypeSpec->ThisType;
          basevar->ConstValue = NULL;
          basevar->Initialised = TRUE;  // So that 'unit expressions etc. will count as initialised
          basevar->Accessed = TRUE;     // For used-after-written checks
          basevar->Access = access;
          basevar->Mode = MODE_GLOBAL;
          CurrentVars->NumVars++;
          wholeobject = TRUE;
          nameref = NULL;
        }
      else
        {
          // type name for t
          nameref = v->TypeName->Name;
        }
    }
  else
    {
      // either name for something other than type, or not name for anything
      nameref = v->Names;
    }
  if (nameref != NULL)
    {
      // name for whole object (including 'type renames t;')
      AnalyseReference (filename, line, nameref, &basetype, &basevar,
                        &reftype, &minvalue, &maxvalue, &initialmin,
                        &initialmax, &unit, &accessedtype, &initialised,
                        &mode, &lock, FALSE, &wholeobject, FALSE,
                        &constantaccess, TRUE, &subsystemboundary);
    }
  if (v->NameFor != NAME_NONE && v->NameFor != NAME_VAR)
    {
      // This is a 'renames' but not for a variable
      // It must name a whole object
      bool match;

      if (wholeobject)
        {
          switch (v->NameFor)
            {
            case NAME_PACKAGE:
              match = (basetype == PACKAGE_TYPE);
              if (match)
                match =
                  (basevar->Type->TypeSpec->PackageSpec->PackageType ==
                   PACKAGE_PACKAGE);
              break;
            case NAME_SUBSYSTEM:
              match = (basetype == PACKAGE_TYPE);
              if (match)
                match =
                  (basevar->Type->TypeSpec->PackageSpec->PackageType ==
                   PACKAGE_SUBSYSTEM);
              break;
            case NAME_PROCEDURE:
              match = (basetype == PROCEDURE_TYPE);
              if (match)
                match = (basevar->Type->TypeSpec->ReturnType == NULL);
              break;
            case NAME_FUNCTION:
              match = (basetype == PROCEDURE_TYPE);
              if (match)
                match = (basevar->Type->TypeSpec->ReturnType != NULL);
              break;
            case NAME_TYPE:
              match = TRUE;     // tested in FindType
              break;
            case NAME_UNIT:
              match = (basetype == UNIT_TYPE);
              break;
            case NAME_CONSTANT:
              match = NamedConstantType (basevar->Type->TypeSpec);
              break;
            default:
              match = FALSE;
              break;
            }
          if (match)
            {
              struct tIdentifierListEntry *varlist;

              varlist = v->VarNames;
              while (varlist != NULL)
                {
                  var = CreateWholeVarAlias (filename, line, varlist->SymbolName, basevar);
                  var->Access = access;
                  varlist = varlist->Next;
                }
            }
          else
            {
              err (filename, line, "Reference type mismatch");
              var = NULL;
            }
        }
      else
        {
          err (filename, line, "Whole object reference expected");
          var = NULL;
        }
    }
  else
    {
      vfirst = AnalyseAddVar (depth, filename, line, v, NULL, access, FALSE, access);
      vlist = vfirst;
      while (vlist != NULL)
        {
          var = vlist->Var;
          //
          if (access == PUBLIC_ACCESS
              && (sharedpack == CurrentPackage || ((sharedpack != NULL) && allpublic))
              && v->NameFor == NAME_NONE && (!var->AccessSharedAtomic))
            {
              err (filename, line,
                   "Shared packages cannot export public global variables other than shared_atomic");
              // Public variables in nested packages are OK if they are not exported from the shared package
            }
          if (v->Mode == MODE_SHARED && sharedpack != NULL && v->NameFor == NAME_NONE)
            {
              err (filename, line, "Shared packages cannot contain shared variables");
            }
          //
          if (nameref != NULL)
            {
              if (reftype != var->Type->TypeSpec)
                {
                  err (filename, line, "Type mismatch in renames declaration");
                }
              var->Referent = basevar;
              if (wholeobject)
                {
                  var->WholeObjectReference = TRUE;
                }
              else
                {
                  var->Initialised = initialised;
                  var->Finalised = basevar->Finalised;
                  var->Lock = lock;     // Set the initial lock for the alias
                  var->Low = minvalue;
                  var->High = maxvalue;
                  if (mode == MODE_IN || basevar->ConstValue != NULL || lock)
                    {
                      // Lock the alias against updates because the referent is an in parameter or constant or locked
                      var->Lock++;
                    }
                  var->WholeObjectReference = FALSE;
                }
              // Record that references to the new symbol will cross a subsystem boundary
              // if references to the base variable would have
              var->SubsystemBoundary = subsystemboundary;
            }
          //----------
          // Write the declaration of the variable, but not if it is a whole object reference, because they
          // are translated directly to the referent
          if (!((nameref != NULL) && wholeobject))
            {
              if ((!var->AddressSpecified) && (!var->AccessVirtual))
                {
                  bool bitfield = FALSE;

                  if (var->Usage == LIBRARY)
                    {
                      EmitLibraryInclude ("ifdef", getvarname (var));
                    }
                  WriteVarDeclaration (depth, var, v, TRUE, basevar, var->Lock);
                  if (nameref != NULL)
                    {
                      if (var->AccessImage)
                        {
                          err (filename, line,
                               "access => image cannot be used with 'renames' declarations");
                        }
                      emitstr (" = &");
                      printreference (filename, line, v->Names, NULL,
                                      &bitfield, FALSE);
                      if (bitfield)
                        {
                          err (filename, line,
                               "References to bit fields cannot be taken");
                        }
                    }
                  else if (var->Type->TypeSpec->BaseType == ACCESS_TYPE
                           || var->Type->TypeSpec->BaseType == ADDRESS_TYPE)
                    {
                      emit (0, " = NULL");
                    }
                  else if (var->AccessImage)
                    {
                      emit (0, " = ");
                      WriteInitialiser (filename, line, var);
                    }
                  emitstr (";\n");
                  if (var->Usage == LIBRARY)
                    {
                      emit (0, "#endif\n");
                    }
                  if (var->Mode == MODE_GLOBAL || var->Mode == MODE_SHARED)
                    {
                      // Add a blank line for readability
                      emitstr ("\n");
                    }
                  //
                  // Write global variables into header files
                  if ((v->Mode == MODE_GLOBAL || v->Mode == MODE_SHARED)
                      && v->Names == NULL && (!var->AccessVirtual))
                    {
                      // Write public variables into the package header
                          SwitchToHeader ();
                          if ((var->Access != PUBLIC_ACCESS) && Annotate)
                            {
                              emit(depth, "/* Private */\n");
                            }
                          if (var->Usage == LIBRARY)
                            {
                              EmitLibraryInclude ("ifdef", getvarname (var));
                            }
                          emit (depth, "extern ");
                          WriteVarDeclaration (0, var, v, FALSE, basevar, FALSE);
                          emitstr (";\n");
                          if (var->Usage == LIBRARY)
                            {
                              emit (0, "#endif\n");
                            }
                    }
                  SwitchToSource ();
                  AddToLocals ("", gettypename (var->Type->TypeSpec));
                }
              if (var->Mode == MODE_SHARED)
                {
                  CheckTaskImplementation (filename, line);
                  emit (depth, "");
                  if (!CyclicTaskModel())
                    {
                      WriteLockDeclaration (var);
                      SwitchToAppHeader ();
                      emitstr ("extern ");
                      WriteLockDeclaration (var);
                      SwitchToSource ();
                    }
                  HaveSharedVariables = TRUE;
                }
            }
          //----------
          vlist = vlist->Next;
        }
      if (vfirst != NULL)
        {
          var = vfirst->Var;
        }
      else
        {
          var = NULL;
        }
      v->Vars = vfirst;
    }
  return var;
}

void
WriteStackUsageStructure (char *Prefix, char *ProcName,
                          unsigned StackOverhead, bool InterruptHandler)
{
  if (TaskImplementation != NO_TASKING && (!CyclicTaskModel()))
    {
      if (!FixedProcedureOverheadSet)
        {
          err ("", 0,
               "Fixed function call stack usage has not been specified (use pragma call_overhead)");
        }
      if (InterruptHandler && (!InterruptOverheadSet))
        {
          err ("", 0,
               "Interrupt overhead has not been set (use pragma interrupt_overhead)");
        }
      SwitchToAuxHeader ();
      emit (0, "struct __stack%s%s", Prefix, ProcName);
      emitstr (" {\n");
      emitstr (localsbuff);
      if (callsbuff[0])
        {
          emitstr ("union {\n");
          emitstr (callsbuff);
          emitstr ("} calls;\n");
        }
      emit (0, "%s overhead[%u];\n", TargetStackUnit,
            FixedProcedureOverhead + StackOverhead);
      if (InterruptHandler)
        {
          emit (0, "%s interrupt_overhead[%u];\n", TargetStackUnit,
                InterruptOverhead);
        }
      emitstr ("};\n");
    }
  SwitchToSource ();
}

void
ShowGlobalsAccess (char *name, struct tGlobalAccess *globals)
{
  struct tGlobalAccess *g;
  char RR, WW, II, QQ, LL, NN, KK;

  printf ("Global accesses by %s: [", name);
  g = globals;
  while (g != NULL)
    {
      if (g->Read)
        RR = 'R';
      else
        RR = '_';
      if (g->Write)
        WW = 'W';
      else
        WW = '_';
      if (g->Initialised)
        II = 'I';
      else
        II = '_';
      if (g->RequireInitialisation)
        QQ = 'Q';
      else
        QQ = '_';
      if (g->LocalAccess)
        LL = 'l';
      else
        LL = '_';
      if (g->InheritedAccess)
        NN = 'i';
      else
        NN = '_';
      if (g->Lock)
        KK = 'L';
      else
        KK = '_';
      printf (" %s:%c%c%c%c%c%c%c", g->Var->Name, RR, WW, II, QQ, LL, NN, KK);
      g = g->Next;
    }
  printf (" ]\n");
}

// Record initial values at the start of a procedure
void
SetInitialValues ()
{
  int j;
  struct tVar *v, *varlist;

  j = 0;
  while (j < CurrentVars->NumVars)
    {
      v = TraceWholeReferents(CurrentVars->Vars[j]);
      v->InitialLow = v->Low;
      v->InitialHigh = v->High;
      j++;
    }
  varlist = PublicVarList;
  while (varlist != NULL)
    {
      v = TraceWholeReferents(varlist);
      v->InitialLow = v->Low;
      v->InitialHigh = v->High;
      varlist = varlist->PrivatePublicNext;
    }
}

bool IntialisationRequired(void);

void WriteContikiStartupCode(void)
{
  struct tStartEntry *s;
  unsigned procnum;

  // Start up the other processes
  if (IntialisationRequired())
    {
      emit(1, "__app_initialisation();\n");
    }
  s = StartList;
  procnum = 1;
  while (s != NULL)
    {
      emit(1, "__process_ids[%u] = &%s;\n", procnum, s->ProcedureName);
      emit(1, "process_start(&%s, NULL);\n", s->ProcedureName);
      procnum++;
      s = s->Next;
    }
}

void
AnalyseProcedure (int depth, char *filename, int line, struct tProcedure *p,
                  enum tAccess access, enum tPrefix requestedprefix)
{
  bool SavedInFunction, ambiguous;
  bool SavedRoutinePrefixShared, SavedRoutinePrefixClosed;
  enum tPrefix SavedRequestedRoutinePrefix;
  struct tTypeSpec *proctype, *SavedCurrentProc;
  // Initialise ReturnVar to avoid a warning - actually OK because of test on ReturnType
  int SavedNumVars, SavedPrefixLen, NumVarsBeforeDeclarations, ReturnVar = 0,
    NumVarsBeforeFormals, NumVarsAfterFormals;
  struct tVar *procv;
  int j;
  struct tTypeSpec *returntype;
  struct tElt *f;
  struct tVariableDeclaration procvar;
  struct tFormalParameter *formal, *prevformal, *formal1;
  struct tVariableDeclaration *vardecl;
  struct tType *type;
  enum tBaseType basetype;
  struct tType *procttype;
  struct tElt *elt;
  struct tRepresentationClause *rep;
  char *cname, *targetattribute, *targetattributeline;
  bool externalreference;
  // Initialise requirenamed to avoid a warning - actually OK because of requirenamedset
  bool requirenamed = FALSE, requirenamedset;
  enum tUsage usage;
  bool inlinecode, macrocode;
  struct tNode *precondition = NULL, *postcondition = NULL;
  bool nullbody, utwithresume;
  bool InterruptHandlerEnabled = FALSE, InterruptHandlerDisabled = FALSE;
  jmp_buf saved_recovery;
  bool haderror;
  bool nolocks;
  bool restartable;
  bool prefixshared, prefixclosed;

  if (requestedprefix == PREFIX_UNIT_TEST && (!UnitTest))
    {
      // Omit this routine
      return;
    }
  if (requestedprefix == PREFIX_NOT_UNIT_TEST && (UnitTest))
    {
      // Omit this routine
      return;
    }
  // Check for conditional compilation
  if (!CheckBuildCondition (filename, line, p->Representation, TRUE))
    {
      return;
    }
  //
  WithResume = FALSE;
  UnitTestWithResume = FALSE;
  UnitTestResumePointSet = FALSE;
  prefixshared = FALSE;
  prefixclosed = FALSE;
  //
  if (VerboseBuild)
    {
      printf ("Analysing procedure %s\n", p->Name);
    }
  //
  // Main procedures automatically get with_resume under appropriate task models
  if ((requestedprefix == PREFIX_MAIN) && CyclicTaskModel())
    {
      WithResume = TRUE;
    }
  // Set the actual prefix
  // All routines in shared packages are shared
  if (   (requestedprefix == PREFIX_SHARED)
      || SharedAncestorPackage (CurrentPackage, CurrentPackage) != NULL)
    {
      prefixshared = TRUE;
    }
  if (requestedprefix == PREFIX_CLOSED)
    {
      prefixclosed = TRUE;
    }
  if (p->ReturnType != NULL && access == PUBLIC_ACCESS
      && CurrentPackage->Shared)
    {
      err (filename, line, "Shared packages cannot contain public functions");
    }

  SavedInFunction = InFunction;
  SavedRoutinePrefixShared = RoutinePrefixShared;
  SavedRoutinePrefixClosed = RoutinePrefixClosed;
  SavedRequestedRoutinePrefix = RequestedRoutinePrefix;
  SavedPrefixLen = strlen (globalprefix);
  SavedCurrentProc = CurrentProc;
  returntype = NULL;
  BuildingUnitTest = FALSE;
  //printf(">PROC %s\n", p->Name);
  SavedNumVars = CurrentVars->NumVars;
  FirstLocalVar = CurrentVars->NumVars;
  CurrentVars->NumRelations = 0;
  ResetGlobalRanges ();
  ResetLocalsCallsBuffs ();

  memcpy (saved_recovery, error_recovery, sizeof (jmp_buf));
  if (setjmp (error_recovery) == 0)
    {
      //
      RoutinePrefixShared = prefixshared;
      RoutinePrefixClosed = prefixclosed;
      RequestedRoutinePrefix = requestedprefix;
      //
      // Allocate a typespec for the new procedure
      proctype = NewTypespec (line);
      //
      proctype->FirstBranch = BranchNumber;
      //
      // Save the current scope before formals and locals have been added for use in precondition tests
      // (formals are added when analysing the call)
      proctype->Scope = MallocVarSet ();
      CopyVarSet (CurrentVars, proctype->Scope);
      //
      // Formal parameters
      NumVarsBeforeFormals = CurrentVars->NumVars;
      elt = p->Formals;
      while (elt != NULL)
        {
          vardecl = elt->Ptr;
          vardecl->Vars =
            AnalyseAddVar (depth, elt->FileName, elt->Line, vardecl, NULL,
                           PRIVATE_ACCESS, TRUE, access);
          elt = elt->Next;
        }
      NumVarsAfterFormals = CurrentVars->NumVars;
      //
      // Set the initial ranges of globals and parameters for use in analysis pragmas and the postcondition
      SetInitialValues ();
      //
      if (p->ReturnType != NULL)
        {
          f = p->Formals;
          while (f != NULL)
            {
              vardecl = (struct tVariableDeclaration *) (f->Ptr);
              if (vardecl->Mode != MODE_IN)
                {
                  err (filename, line,
                       "A function can only have 'in' parameters");
                }
              f = f->Next;
            }
          vardecl = (struct tVariableDeclaration *) (p->ReturnType->Ptr);
          if (vardecl->TypeSpec != NULL)
            {
              // Create the type for anonymous return types
              struct tTypeDeclaration *typedecl =
                checked_malloc (sizeof *typedecl);

              typedecl->Name = MakeFieldName (vardecl->VarNames->SymbolName, "return");
              typedecl->TypeSpec = vardecl->TypeSpec;
              AnalyseAddType (1, filename, line, typedecl, PRIVATE_ACCESS);
              vardecl->TypeName =
                MakeTypeIdentifier (MakeReference
                                    (IDENTIFIER_REFERENCE, typedecl->Name,
                                     NULL, NULL), NULL);
              vardecl->TypeSpec = NULL;
            }
          type = FindType (filename, line, vardecl->TypeName);
          returntype = type->TypeSpec;
          basetype = returntype->BaseType;
          if (basetype == INTEGER_TYPE
              || basetype == FLOATING_TYPE
              || basetype == BOOLEAN_TYPE
              || basetype == ADDRESS_TYPE
              || basetype == CSTRING_TYPE)
            {
              ;
            }
          else if (basetype == ACCESS_TYPE)
            {
              if (!returntype->UnmanagedAccess)
                {
                  err (filename, line, "Functions cannot return managed access types");
                }
              if (returntype->NewAccess)
                {
                  err (filename, line, "Functions cannot return 'access new' types");
                }
            }
          else
            {
              err (filename, line,
                   "Function return type must be a discrete, access, address or string type");
            }
          InFunction = TRUE;
        }
      else
        {
          InFunction = FALSE;
        }
      //
      cname = NULL;
      targetattribute = NULL;
      targetattributeline = NULL;
      externalreference = FALSE;
      requirenamedset = FALSE;
      inlinecode = FALSE;
      macrocode = FALSE;
      nolocks = FALSE;
      if (requestedprefix == PREFIX_UNIT_TEST)
        {
          usage = TARGET_LIBRARY;
        }
      else
        {
          usage = DefaultUsage;
        }
      rep = p->Representation;
      while (rep != NULL)
        {
          if (!strcasecmp (rep->Name, "cname"))
            {
              if (rep->Value->Op == _CONST_STRING)
                {
                  cname = DeQuote (rep->Value->String);
                }
              if (cname == NULL)
                {
                  err (filename, line,
                       "Syntax error in 'cname' clause: expected 'cname => \"name\"'");
                }
            }
          else if (!strcasecmp (rep->Name, "cprefix"))
            {
              if (rep->Value->Op == _CONST_STRING)
                {
                  if (!UnitTest)
                    {
                      targetattribute = DeQuote (rep->Value->String);
                    }
                }
              else
                {
                  err (filename, line,
                       "Syntax error in 'cprefix' clause: expected 'cprefix => \"string\"'");
                }
            }
          else if (!strcasecmp (rep->Name, "cprefix_line"))
            {
              if (rep->Value->Op == _CONST_STRING)
                {
                  if (!UnitTest)
                    {
                      targetattributeline = DeQuote (rep->Value->String);
                    }
                }
              else
                {
                  err (filename, line,
                       "Syntax error in 'cprefix_line' clause: expected 'cprefix_line => \"string\"'");
                }
            }
          else if (!strcasecmp (rep->Name, "reference"))
            {
              if (rep->Value->Op == _CONST_STRING)
                {
                  cname = DeQuote (rep->Value->String);
                }
              if (cname == NULL)
                {
                  err (filename, line,
                       "Syntax error in 'reference' clause: expected 'reference => \"name\"'");
                }
              externalreference = TRUE;
            }
          else if (!strcasecmp (rep->Name, "require_named_parameters"))
            {
              if (rep->Value->Op == _CONST_BOOL)
                {
                  if (rep->Value->Value == 0)
                    {
                      requirenamed = FALSE;
                    }
                  else
                    {
                      requirenamed = TRUE;
                    }
                  requirenamedset = TRUE;
                }
              else
                {
                  err (filename, line,
                       "Syntax error in 'require_named_parameters' clause: expected 'true' or 'false'");
                }
            }
          else if (!strcasecmp (rep->Name, "code"))
            {
              if (rep->Value->Op == _VAR)
                {
                  if (!strcasecmp (rep->Value->Var->VarName, "inline"))
                    {
                      if (!InlinePrefixSet)
                        {
                          err (filename, line,
                               "The inline prefix has not been set (use pragma inline_prefix or inline_prefix_line)");
                        }
                      // Don't use inline for unit tests (messes up coverage testing)
                      if (!UnitTest)
                        {
                          inlinecode = TRUE;
                        }
                    }
                  else if (!strcasecmp (rep->Value->Var->VarName, "macro"))
                    {
                      macrocode = TRUE;
                    }
                  else
                    {
                      err(filename, line, "Error in 'code' clause: expected 'inline' or 'macro'");
                    }
                }
              else
                {
                  err (filename, line,
                       "Syntax error in 'code' clause: expected a symbol");
                }
            }
          else if (!strcasecmp (rep->Name, "build_condition"))
            {
              ;                 // Handled above
            }
          else if (!strcasecmp (rep->Name, "precondition"))
            {
              if (requestedprefix == PREFIX_MAIN)
                {
                  err (filename, line, "main procedures cannot have preconditions");  // because of the possibility of restart
                }
              if (precondition != NULL)
                {
                  err (filename, line, "A precondition has already been set");
                }
              precondition = rep->Value;
            }
          else if (!strcasecmp (rep->Name, "postcondition"))
            {
              if (postcondition != NULL)
                {
                  err (filename, line,
                       "A postcondition has already been set");
                }
              postcondition = rep->Value;
            }
          else if (!strcasecmp (rep->Name, "usage"))
            {
              if (rep->Value->Op == _VAR)
                {
                  usage = GetUsage (filename, line, rep->Value->Var->VarName);
                }
              else
                {
                  err (filename, line,
                       "Syntax error in 'usage' clause: expected a symbol");
                }
            }
          else if (!strcasecmp (rep->Name, "unit_test_with_resume"))
            {
              if (rep->Value->Op == _CONST_BOOL)
                {
                  if (rep->Value->Value == 0)
                    {
                      UnitTestWithResume = FALSE;
                    }
                  else
                    {
                      UnitTestWithResume = TRUE;
                    }
                }
              else
                {
                  err (filename, line,
                       "Syntax error in 'unit_test_with_resume' clause: expected 'true' or 'false'");
                }
            }
          else if (!strcasecmp (rep->Name, "interrupt_handler"))
            {
              if (rep->Value->Op == _VAR)
                {
                  if (InterruptHandlerEnabled || InterruptHandlerDisabled)
                    {
                      err (filename, line,
                           "'interrupt_handler' has already been declared for this routine");
                    }
                  if (!strcasecmp
                      (rep->Value->Var->VarName, "with_interrupt"))
                    {
                      InterruptHandlerEnabled = TRUE;
                    }
                  else
                    if (!strcasecmp
                        (rep->Value->Var->VarName, "without_interrupt"))
                    {
                      InterruptHandlerDisabled = TRUE;
                    }
                  else
                    {
                      err (filename, line,
                           "Error in 'interrupt_handler' clause: expected 'with_interrupt' or 'without_interrupt'");
                    }
                }
              else
                {
                  err (filename, line,
                       "Syntax error in 'interrupt_handler' clause");
                }
            }
          else if (!strcasecmp(rep->Name, "lock"))
            {
              if (rep->Value->Op == _VAR)
                {
                  struct tReference *ref = rep->Value->Var;
                  struct tVar *v;
                  struct tShareLockElt *share;

                  if (p->ReturnType != NULL)
                    {
                      err(filename, line, "'lock' cannot be used in functions");
                      // For the same reason that shared variables cannot be passed to functions (unless locked already)
                    }
                  v = FindVariable (filename, line, &ref, 36, FALSE, TRUE);
                  if (v->Mode != MODE_SHARED)
                    {
                      err(filename, line, "'lock' can only be applied to shared variables");
                    }
                  v->ShareLock = TRUE;
                  share = checked_malloc(sizeof(*share));
                  share->Var = v;
                  share->Next = proctype->ShareLockList;
                  proctype->ShareLockList = share;
                }
              else
                {
                  err(filename, line, "Syntax error in 'lock' clause");
                }
            }
          else if (!strcasecmp(rep->Name, "unchecked_lock"))
            {
              if (rep->Value->Op == _VAR)
                {
                  struct tReference *ref = rep->Value->Var;
                  struct tVar *v;

                  v = FindVariable (filename, line, &ref, 36, FALSE, TRUE);
                  if (v->Mode != MODE_SHARED)
                    {
                      err(filename, line, "'unchecked_lock' can only be applied to shared variables");
                    }
                  // Act as if it is locked, but do not export the lock requirement
                  v->ShareLock = TRUE;
                }
              else
                {
                  err(filename, line, "Syntax error in 'lock' clause");
                }
            }
          else if (!strcasecmp (rep->Name, "restriction"))
            {
              if (rep->Value->Op == _VAR)
                {
                  if (!strcasecmp
                      (rep->Value->Var->VarName, "no_locks"))
                    {
                      nolocks = TRUE;
                    }
                  else
                    {
                      err(filename, line, "Unknown restriction '%s'", rep->Value->Var->VarName);
                    }
                }
              else
                {
                  err (filename, line,
                       "Syntax error in 'restriction' clause");
                }
            }
          else
            {
              err (filename, line,
                   "Unknown representation for a procedure '%s'", rep->Name);
            }
          rep = rep->Next;
        }
      //
      if (inlinecode && macrocode)
        {
          err(filename, line, "'inline' and 'macro' cannot both be used for the same routine");
        }
      if (macrocode)
        {
          if (WithResume) err(filename, line, "Main procedures cannot be macros");
          if (UnitTestWithResume) err(filename, line, "'unit_test_with_resume' cannot be used with macros");
          if (InterruptHandlerEnabled || InterruptHandlerDisabled) err(filename, line, "'interrupt_handler' cannot be used with macros");
          if (externalreference) err(filename, line, "'reference' cannot be used with macros");
        }
      //
      if (cname == NULL)
        {
          cname = MakeCName (p->Name);
        }

      proctype->CName = cname;
      proctype->TargetAttribute = targetattribute;
      proctype->TargetAttributeLine = targetattributeline;
      proctype->GlobalsAccess = NULL;
      proctype->StackOverhead = 0;
      proctype->Name = p->Name;
      proctype->NoLocks = nolocks;
      //
      CurrentProc = proctype;
      //
      prevformal = NULL;
      f = p->Formals;
      while (f != NULL)
        {
          struct tIdentifierListEntry *varlist;

          vardecl = f->Ptr;
          varlist = vardecl->VarNames;
          while (varlist != NULL)
            {
              formal = checked_malloc (sizeof *formal);
              formal->Name = varlist->SymbolName;
              formal->Mode = vardecl->Mode;
              type = FindType (filename, line, vardecl->TypeName);
              formal->TypeSpec = type->TypeSpec;
              formal->Var = vardecl->Vars->Var;
              //
              if (vardecl->Vars->Var->AccessVolatile)
                {
                  if (formal->Mode == MODE_IN)
                    {
                      if (type->TypeSpec->BaseType == INTEGER_TYPE
                          || type->TypeSpec->BaseType == FLOATING_TYPE)
                        {
                          err (filename, line,
                               "Volatile access cannot be declared for 'in' parameters of numerical types");
                        }
                    }
                }
              //
              if (prevformal == NULL)
                proctype->Formals = formal;
              else
                prevformal->Next = formal;
              prevformal = formal;
              varlist = varlist->Next;
            }
          f = f->Next;
        }
      if (prevformal == NULL)
        proctype->Formals = NULL;
      else
        prevformal->Next = NULL;
      //
      if (requirenamedset)
        {
          proctype->NamedParametersRequired = requirenamed;
        }
      else
        {
          // Test for possible ambiguity of parameters by position
          formal = proctype->Formals;
          ambiguous = FALSE;
          while (formal != NULL && (!ambiguous))
            {
              formal1 = formal->Next;
              while (formal1 != NULL && (!ambiguous))
                {
                  if (formal->TypeSpec == formal1->TypeSpec)
                    {
                      ambiguous = TRUE;
                    }
                  else
                    if ((formal->TypeSpec->BaseType == INTEGER_TYPE
                         || formal->TypeSpec->BaseType == FLOATING_TYPE)
                        && (formal1->TypeSpec->BaseType == INTEGER_TYPE
                            || formal1->TypeSpec->BaseType == FLOATING_TYPE)
                        &&
                        (CheckUnitMatch
                         (formal->TypeSpec->Unit, formal1->TypeSpec->Unit,
                          FALSE)))
                    {
                      ambiguous = TRUE;
                    }
                  formal1 = formal1->Next;
                }
              formal = formal->Next;
            }
          proctype->NamedParametersRequired = ambiguous;
        }
      //
      // Types declared within procedures have prefix _pack_pack__proc_
      // This ensures that they do not clash with packages
      //
      strcat (globalprefix, p->Name);
      strcat (globalprefix, "_");
      //
      proctype->BaseType = PROCEDURE_TYPE;
      proctype->Line = line;
      proctype->Unit = NULL;
      proctype->ReturnType = returntype;
      proctype->StructureAccess = access;
      proctype->PrefixShared = prefixshared;
      proctype->PrefixClosed = prefixclosed;
      proctype->ShareClause = p->ShareClause;
      proctype->External = externalreference;
      proctype->Precondition = precondition;
      proctype->Postcondition = postcondition;
      procttype = AllocateTType ();
      procttype->Name = p->Name;
      procttype->NameAccess = access;
      procttype->TypeSpec = proctype;
      procttype->Next = NULL;
      procvar.ConstValue = NULL;
      procvar.Mode = MODE_GLOBAL;
      procvar.TypeName = NULL;
      procvar.TypeSpec = NULL;
      procvar.VarNames = MakeIdentifierListEntry(p->Name);
      procvar.Representation = NULL;
      procvar.PublicOut = FALSE;
      //----------

      if (p->ReturnType != NULL && WithResume)
        {
          err (filename, line,
               "'main' cannot be specified for functions");
        }

      if (p->ReturnType != NULL && UnitTestWithResume)
        {
          err (filename, line,
               "'unit_test_with_resume' cannot be specified for functions");
        }

      nullbody = TRUE;
      {
        struct tElt *statements;

        statements = p->Statements;
        while (statements != NULL)
          {
            if (statements->Type != NULL_ELT
                && statements->Type != PRAGMA_ELT)
              {
                nullbody = FALSE;
                break;
              }
            statements = statements->Next;
          }
      }

      if (externalreference)
        {
          struct tElt *statements;

          if (p->Declarations != NULL)
            {
              err (filename, line,
                   "Routines declared with a reference clause cannot have local declarations");
            }
          if (!nullbody)
            {
              err (filename, line,
                   "The body of a routine declared with a reference clause can only contain null statements and pragmas (or use 'is separate')");
            }
          if (WithResume)
            {
              err (filename, line,
                   "'main' cannot be specified for procedures that are declared with a reference clause");
            }
          if (UnitTestWithResume)
            {
              err (filename, line,
                   "'unit_test_with_resume' cannot be specified for procedures that are declared with a reference clause");
            }
          statements = p->Statements;
          while (statements != NULL)
            {
              if (statements->Type == PRAGMA_ELT)
                {
                  AnalysePragma (depth, filename, line, statements->Ptr);
                }
              statements = statements->Next;
            }

        }
      else
        {
          struct tVar *v;

          if (p->Statements == NULL)
            {
              err (filename, line,
                   "A routine that is declared separate must have a reference clause");
            }
          if (inlinecode || macrocode)
            {
              // Inline definitions must go in the header file (typically 'static inline f() { ... }')
              InlineToHeader = TRUE;
            }
          else
            {
              InlineToHeader = FALSE;
            }
          SwitchToSource ();    // might be the header
          if (usage == LIBRARY)
            {
              EmitLibraryInclude ("ifdef", getprocname (proctype));
            }
          if (!RecordComments)
            {
              if (Annotate)
                {
                  PrintLineNumbers = FALSE;
                  emit (depth, "/*\n");
                  emit (depth, " * %s\n", p->Name);
                  emit (depth, " */\n");
                  PrintLineNumbers = TRUE;
                }
            }
          if (Annotate && (proctype->ShareLockList != NULL))
            {
              struct tShareLockElt *share;

              share = proctype->ShareLockList;
              emit(depth, "/* Locked:");
              while (share != NULL)
                {
                  emit(0, " %s", share->Var->CName);
                  share = share->Next;
                }
              emit(0, " */\n");
            }
          //
          if (targetattributeline != NULL)
            {
              emit (depth, "%s\n", targetattributeline);
            }
          emit (depth, "");
          if (inlinecode)
            {
              if (InlinePrefixLine)
                {
                  emit (0, "%s\n", InlinePrefix);
                  emit (depth, "");
                }
              else
                {
                  emit (0, "%s ", InlinePrefix);
                }
            }
          if (targetattribute != NULL)
            {
              emit (0, "%s ", targetattribute);
            }
          if (macrocode)
            {
              struct tFormalParameter *formal = proctype->Formals;
              struct tElt *sp = p->Statements;

              if (p->Declarations != NULL)
                {
                  err(filename, line, "Macro routines cannot contain declarations");
                }
              emit (0, "#define %s(", getprocname (proctype));
              while (formal != NULL)
                {
                  emit(0, "%s", formal->Name);
                  // For indefinite arrays, add extra parameters for the index bounds
                  if (formal->TypeSpec->BaseType == ARRAY_TYPE)
                    {
                      if (!formal->TypeSpec->MinDefinite)
                        {
                          emit (0, ", %s", MinName (formal->Name));
                        }
                      if (!formal->TypeSpec->MaxDefinite)
                        {
                          emit (0, ", %s", MaxName (formal->Name));
                        }
                    }
                  if (formal->Next != NULL)
                    {
                      emit(0, ", ");
                    }
                  formal = formal->Next;
                }
              emit(0, ")");
              sp = p->Statements;
              while (sp != NULL)
                {
                  if (sp->Type == CODE_ELT)
                    {
                      char *s = SubstituteCode (sp->FileName, sp->Line, sp->Ptr);
                      emit(0, " \\\n");
                      emit (depth + 1, "%s", s);
                      free(s);
                    }
                  else if (sp->Type == PRAGMA_ELT)
                    {
                      struct tProcedureCall *p = sp->Ptr;
                      AnalysePragma (depth, sp->FileName, sp->Line, p);
                    }
                  else
                    {
                      err(sp->FileName, sp->Line, "Macro routines can only contain C code statements and pragmas");
                    }
                  sp = sp->Next;
                }
                emit(0, "\n");
            }
          else if (WithResume && (TaskImplementation == CONTIKI))
            {
              emit(0, "PROCESS(%s, \"%s\");\n\n", getprocname(proctype), getprocname(proctype));
              emit(0, "PROCESS_THREAD(%s, ev, data)", getprocname(proctype));
            }
          else
            {
              if (returntype == NULL)
                {
                  emit (0, "void");
                }
              else
                {
                  printtypename (returntype);
                }
              emit (0, " %s", getprocname (proctype));
              printformalparameterlist (depth + 1, proctype->Formals, FALSE);
            }
          //
          if (   (!inlinecode)
              && (!macrocode)
              && (!(WithResume && (TaskImplementation == CONTIKI) && (!UnitTest)))  // Not a Contiki thread routine
             )
            {
              // Write the prototype to the header file
              SwitchToHeader ();
              if ((access != PUBLIC_ACCESS) && Annotate)
                {
                  emit(depth, "/* Private */\n");
                }
              if (usage == LIBRARY)
                {
                  EmitLibraryInclude ("ifdef", getprocname (proctype));
                }
              emit (depth, "");
              if (targetattribute != NULL)
                {
                  emit (0, "%s ", targetattribute);
                }
              if (returntype == NULL)
                emit (depth, "void");
              else
                {
                  emit (depth, "");
                  printtypename (returntype);
                }
              emit (0, " %s", getprocname (proctype));
              printformalparameterlist (depth + 1, proctype->Formals, TRUE);
              emitstr (";\n");
              if (usage == LIBRARY)
                {
                  emit (0, "#endif\n");
                }
              emitstr ("\n");
              SwitchToSource ();
            }
          //
          // If the routine has a guard then analyse it now so that parameter modes are handled
          // properly (e.g. if a parameter is read then it must have been initialised)
          if (p->ShareClause->Expr != NULL)
            {
              BIGINT min, max;
              bool virtualexpr;
              TestBooleanExpression (p->ShareClause->Expr, TRUE, &min, &max,
                                     FALSE, &virtualexpr);
            }
          //
          if (!macrocode)
            {
              emitstr ("\n");
              emit (depth, "{\n");
            }
          // Return var
          if (p->ReturnType != NULL)
            {
              ReturnVar = CurrentVars->NumVars;
              if (!macrocode)
                {
                  Analyse (depth + 1, p->ReturnType, FALSE, FALSE);
                }
            }
          NumVarsBeforeDeclarations = CurrentVars->NumVars;
          Analyse (depth + 1, p->Declarations, FALSE, FALSE);
          if (p->Declarations != NULL)
            {
              // Separate the declarations from the code
              emitstr ("\n");
            }
          // Apply alias locks
          ApplyAliasLocks (NumVarsBeforeDeclarations);
          //
          //
          // For any formal parameters of array type, assert that a'last >= a'first
          formal = proctype->Formals;
          while (formal != NULL)
            {
              if (formal->TypeSpec->BaseType == ARRAY_TYPE)
                {
                  bool changed;
                  StoreRelation (filename, line, formal->Var, ATTR_LAST,
                                 formal->Var, ATTR_FIRST, _GEQ, 0, 0,
                                 &changed);
                }
              formal = formal->Next;
            }
          //
          // If the routine has a precondition, then apply it here (i.e. proceed on the assumption
          // that it is true)
          if (precondition != NULL)
            {
              bool precondsuccess = TRUE;

              UpdateRanges (precondition, FALSE, &precondsuccess, TRUE,
                            FALSE);
              if (!precondsuccess)
                {
                  err (filename, line,
                       "The precondition could not be applied");
                }
            }
          // If the routine has a guard expression then apply it
          if (p->ShareClause->Expr != NULL)
            {
              bool guardsuccess;

              UpdateRanges (p->ShareClause->Expr, FALSE, &guardsuccess, TRUE,
                            FALSE);
            }
          //
          if (WithResume)
            {
              if (!UnitTest)
                {
                  emit (depth + 1, "static unsigned __resume = 0;\n");
                  emit (depth + 1, "goto __resume_switch;\n");
                  emit (depth + 1, "__resume0:\n");
                  if (WithResume && (TaskImplementation == CONTIKI) && (CurrentPackage->PackageType == PACKAGE_SYSTEM))
                    {
                      emit(1, "__process_ids[0] = &%s;\n", getprocname(proctype));
                      WriteContikiStartupCode();
                    }
                  emitstr("\n");
                }
              NumResumePoints = 0;
            }
          //
          restartable = (requestedprefix == PREFIX_MAIN && (!macrocode));
          //
          if (!(nullbody && (requestedprefix == PREFIX_UNIT_TEST)))
            {
              utwithresume = UnitTest && UnitTestWithResume
                && (!BuildingUnitTest) && (!InlineToHeader);
            }
          else
            {
              utwithresume = FALSE;
            }
          if (utwithresume)
            {
              // Do this here because a label cannot be part of a declaration
              emit (depth + 1,
                    "static unsigned char __test_resume = 0;\n");
            }
          if (restartable)
            {
              if (TaskImplementation == NO_TASKING) {
                // This conditional is used to avoid a possible unused label warning
                emit(0, "#ifdef __MAIN_RESTART\n");
              }
              emit (depth + 1, "__restart:\n");
              if (TaskImplementation == NO_TASKING) {
                emit(0, "#endif\n");
              }
            }
          AnalyseInitialisations (depth + 1, p->Declarations);
          if (utwithresume)
            {
              emit (depth + 1,
                    "if (__test_resume != 0) goto __unit_test_resume;\n",
                    BranchNumber);
              emit (depth + 1, "__test_resume = 1;\n");
            }
          EmitCoverage (depth + 1, p->Statements->FileName,
                        p->Statements->Line);

          //
          if (!macrocode)
            {
              InRestartableRoutine = restartable;
              Analyse (depth + 1, p->Statements, TRUE, FALSE);
              // For macros the statements are processed above
            }

          if (!CurrentProc->Incomplete)
            {
              // Check the postcondition, if one has been defined
              // If this is a macro then we can't check the postcondition, but it can still
              // be specified so that it can be asserted after the call
              if (postcondition != NULL && (!macrocode))
                {
                  BIGINT minvalue, maxvalue;
                  bool virtualexpr;

                  TestBooleanExpression (postcondition, FALSE, &minvalue,
                                         &maxvalue, FALSE, &virtualexpr);
                  if (minvalue == 1)
                    {
                      if (Annotate)
                        {
                          emit (0, "/* Postcondition test line %d confirmed */\n", line);       //**********FIX FORMATTING
                        }
                    }
                  else if (maxvalue == 0)
                    {
                      if (Annotate)
                        {
                          emit (0,
                                "/* Postcondition test at line %d is false */\n",
                                line);
                        }
                      err_and_continue (filename, line,
                                        "The postcondition is false");
                    }
                  else
                    {
                      if (Annotate)
                        {
                          emit (0,
                                "/* Postcondition test at line %d could not be confirmed */\n",
                                line);
                        }
                      err_and_continue (filename, line,
                                        "The postcondition could not be confirmed");
                    }
                }
            }
          //
          // Reverse alias locks
          ReverseAliasLocks (NumVarsBeforeDeclarations);
          //
          if ((!CurrentProc->Incomplete) && (!macrocode))
            {
              //
              // Check that all out parameters have been assigned to and that
              // all controlled variables have been finalised
              //
              if (p->ReturnType != NULL)
                {
                  if (!CurrentVars->Vars[ReturnVar]->Initialised)
                    {
                      err_and_continue (filename, line,
                                        "In procedure %s, the function result has not been initialised",
                                        p->Name);
                    }
                }
              for (j = NumVarsBeforeFormals; j < NumVarsAfterFormals; j++)
                {
                  v = CurrentVars->Vars[j];
                  if (v->Mode == MODE_OUT
                    )
                    {
                      if (!v->Initialised)
                        {
                          err_and_continue (filename, line,
                                            "In procedure %s, %s has not been initialised",
                                            p->Name, v->Name);
                        }
                    }
                  //
                  if (v->Mode == MODE_FINAL_IN_OUT)
                    {
                      if (v->Type->TypeSpec->Controlled && (!v->Finalised))
                        {
                          err_and_continue (filename, line,
                                            "In procedure %s, formal %s requires finalisation but has not been finalised",
                                            p->Name, v->Name);
                        }
                    }
                  //
                  if (v->Mode == MODE_IN)
                    {
                      if (!v->EverAccessed)
                        {
                          err_and_continue (filename, line,
                                            "In procedure %s, formal %s is never read",
                                            p->Name, v->Name);
                        }
                    }
                  //
                  if (v->Mode == MODE_IN_OUT)
                    {
                      if (!(v->EverAccessed || v->Initialised))
                        {
                          err_and_continue (filename, line,
                                            "In procedure %s, formal %s is never read or written",
                                            p->Name, v->Name);
                        }
                    }
                }
              CheckFinalisationAndAccessOfLocals (depth + 1, filename, line,
                                                  NumVarsBeforeDeclarations,
                                                  requestedprefix != PREFIX_MAIN);
              // main procedures are generally infinite loops, so only check
              // that the local was used at all in that case, not that it
              // had been accessed at the point of exit
            }
          //
          if (utwithresume)
            {
              // Cancel the resume for the next test
              emit (depth + 1, "__test_resume = 0;\n");
            }
          if (p->ReturnType != NULL && (!macrocode))
            {
              emit (depth + 1, "return %s;\n", p->Name);
            }
          //
          if (WithResume && (!UnitTest))
            {
              bool longjumps = restartable && (proctype->Restarts);

              emit (depth + 1, "goto __resume_exit;\n\n");
              emit (depth + 1, "__resume_switch:\n");
              if (longjumps)
                {
                  EmitPreDispatch(depth + 1);
                  if (CyclicTaskModel())
                    {
                      emit (depth + 1, "if (setjmp(__restartjump) == 0) {\n");
                    }
                  else if (TaskImplementation == PTHREAD)
                    {
                      emit (depth + 1, "if (setjmp(__restartjump[__get_current_routine()]) == 0) {\n");
                    }
                  else  // COROUTINE
                    {
                      emit (depth + 1, "if (setjmp(__restartjump[__current_coroutine]) == 0) {\n");
                    }
                  EmitPostDispatch(depth + 2);
                }
              {
                unsigned switchdepth;

                if (longjumps)
                  switchdepth = depth + 2;
                else
                  switchdepth = depth + 1;

                emit (switchdepth, "switch (__resume) {\n");
                for (j = 0; j <= NumResumePoints; j++)
                  {
                    emit (switchdepth + 1, "case %u: goto __resume%u;\n", j, j);
                  }
                emit (switchdepth, "}\n");
              }
              if (longjumps)
                {
                  emit(depth + 1, "} else {\n");
                  EmitPostDispatch(depth + 2);
                  emit(depth + 1, "}\n");
                }
              emit (depth + 1, "goto __restart;\n");

              emit (depth + 1, "__resume_exit:\n");
              if (TaskImplementation == CYCLIC)
                {
                  emit (depth + 1, "__resume_terminate();\n");
                }
              else if (TaskImplementation == CONTIKI)
                {
                  emit( depth + 1, "return PT_ENDED;\n");
                }
            }
          //----------
          if (!macrocode)
            {
              emit (depth, "}\n");
            }
          //
          if (usage == LIBRARY)
            {
              emit (0, "#endif\n");
            }
          emit (0, "\n");
          //
          InlineToHeader = FALSE;
          SwitchToSource ();    // In case it was the header
          //
          if (p->ShareClause->Expr != NULL)
            {
              bool bf;
              BIGINT minbool, maxbool;
              bool virtualexpr;

              TestBooleanExpression (p->ShareClause->Expr, FALSE, &minbool,
                                     &maxbool, FALSE, &virtualexpr);
              ResetLocalsCallsBuffs ();
              emit (depth, "");
              emit (0, "int %s", GATE_PREFIX);
              printprocname (proctype);
              printformalparameterlist (depth + 1, proctype->Formals, FALSE);
              emit(0, "\n");
              emit (depth, "{\n");
              emit (depth + 1, "return ");
              bf = FALSE;
              printexpr (p->ShareClause->Expr, &bf, TRUE);
              emit (0, ";\n");
              emit(depth, "}\n\n");
              //
              SwitchToHeader ();
              emit (0, "int %s", GATE_PREFIX);
              printprocname (proctype);
              printformalparameterlist (depth + 1, proctype->Formals, TRUE);
              emit (0, ";\n\n");
              SwitchToSource ();
              //
              // Emit the stack usage structure
              WriteStackUsageStructure (GATE_PREFIX, getprocname (proctype),
                                        0, FALSE);
            }
        }
      //
      // Any globals that are now initialised are recorded as initialised by this routine (since they were
      // marked uninitialised above by ResetGlobalRanges)

      {
        struct tVar *v;

        j = 0;
        while (j < CurrentVars->NumVars)
          {
            v = CurrentVars->Vars[j];
            if (v->Initialised && (!v->InitialisedGlobally))
              {
                // Don't change 'inherited' versus 'local' status
                RecordNeutralGlobalAccess (line, GetGlobalsAccess (), v, FALSE,
                                           FALSE, FALSE, TRUE);
              }
            j++;
          }
        v = PublicVarList;
        while (v != NULL)
          {
            if (v->Initialised && (!v->InitialisedGlobally))
              {
                // Don't change 'inherited' versus 'local' status
                RecordNeutralGlobalAccess (line, GetGlobalsAccess (), v, FALSE,
                                           FALSE, FALSE, TRUE);
              }
            v = v->PrivatePublicNext;
          }
      }

      //
      //----------
      // Emit the stack usage structure
      WriteStackUsageStructure ("", getprocname (proctype),
                                (unsigned) (proctype->StackOverhead),
                                (InterruptHandlerEnabled
                                 || InterruptHandlerDisabled));
      //
      if (InterruptHandlerEnabled || InterruptHandlerDisabled)
        {
          struct InterruptHandlerRecord *p =
            checked_malloc (sizeof *p);

          p->Name = getprocname (proctype);
          p->WithInterruptsEnabled = InterruptHandlerEnabled;
          p->Next = InterruptHandlerList;
          InterruptHandlerList = p;
        }
      //
      haderror = FALSE;

    }
  else
    {
      haderror = TRUE;
      proctype = NULL;          // Won't be used
    }
  memcpy (error_recovery, saved_recovery, sizeof (jmp_buf));

  globalprefix[SavedPrefixLen] = 0;
  CurrentVars->NumVars = SavedNumVars;
  //
  // Export constants that were defined in the parameter list, e.g. aa, bb, cc from 'x : in (aa, bb, cc)'
  // from the procedure
  //
  if (!haderror)
    {
      for (j = NumVarsBeforeFormals; j < NumVarsAfterFormals; j++)
        {
          if (IsNamedConstant(CurrentVars->Vars[j]))
            {
              // The symbol is a named constant
              // Export it from the procedure
              CurrentVars->Vars[CurrentVars->NumVars] = CurrentVars->Vars[j];
              CurrentVars->NumVars++;
            }
        }
    }
  //
  FirstLocalVar = 0;
  CurrentVars->NumRelations = 0;
  // After restoring NumVars, reset the globals again so that start procedures do not see 'initialised' left over as true
  ResetGlobalRanges ();
  //
  if (!haderror)
    {
      struct tVarListEntry *procl;
      procl =
        AnalyseAddVar (depth, filename, line, &procvar, procttype, access,
                       FALSE, access);
      procv = procl->Var;
      procv->Usage = usage;
      procv->CName = cname;     // mainly for `sym` translations in C code
      //
      // If the current procedure dispatches (directly or indirectly), then mark its enclosing package as dispatching
      if (CurrentProc->Dispatches)
        {
          procv->EnclosingPackage->Dispatches = TRUE;
        }
      if (WithResume)
        {
          WithResumeThisSubsystem = procv;
        }
      if (requestedprefix == PREFIX_MAIN)
        {
          CurrentPackage->Start = procv;
        }
    }
  //
  InFunction = SavedInFunction;
  RoutinePrefixShared = SavedRoutinePrefixShared;
  RoutinePrefixClosed = SavedRoutinePrefixClosed;
  RequestedRoutinePrefix = SavedRequestedRoutinePrefix;
  CurrentProc = SavedCurrentProc;

  if (!haderror)
    {
      // Record target library routines
      if (usage == TARGET_LIBRARY)
        {
          struct tCallEntry *newentry =
            checked_malloc (sizeof *newentry);
          newentry->Routine = procv;
          newentry->Next = TargetLibraryList;
          TargetLibraryList = newentry;
        }
      //
      if (VerboseBuild)
        {
          ShowGlobalsAccess (procv->Name, proctype->GlobalsAccess);
          printf ("\n");
        }
      //
      proctype->LastBranch = BranchNumber - 1;
      //
      if (UnitTestWithResume && (!UnitTestResumePointSet))
        {
          err (filename, line,
               "Unit test with resume was specified but no resume point was set");
        }
      //

    }

  WithResume = FALSE;
  UnitTestWithResume = FALSE;
  UnitTestResumePointSet = FALSE;
  //
  if ((p->UnitTest != NULL) && UnitTest && (!haderror))
    {
      struct tUnitTest *u;

      if (VerboseBuild)
        {
          printf("Unit test section for %s:\n", p->Name);
        }
      // Build the unit test procedure
      BuildingUnitTest = TRUE;
      SwitchToHeader();
      emit (0, "int %s%s(void);\n\n", getprocname (proctype),
            UNIT_TEST_SUFFIX);
      SwitchToSource ();
      emit (depth, "/* Unit test for %s */\n", p->Name);
      emit (depth, "int %s%s(void)\n", getprocname (proctype), UNIT_TEST_SUFFIX);
      emit (depth, "{\n");
      Analyse (depth + 1, p->UnitTest, TRUE, FALSE);
      emit (depth + 1, "return 0;\n");
      emit (depth, "}\n\n");
      BuildingUnitTest = FALSE;
      // Add it to the list of unit tests
      u = checked_malloc (sizeof *u);
      u->ProcType = proctype;
      u->FileName = filename;
      u->Line = line;
      u->Next = NULL;
      if (UnitTestTail == NULL)
        {
          UnitTestTail = u;
          UnitTestHead = u;
        }
      else
        {
          UnitTestTail->Next = u;
          UnitTestTail = u;
        }
    }
}

void
AnalyseDeclareBlock (int depth, char *filename, int line,
                     struct tDeclareBlock *d)
{
  int SavedNumVars, SavedNumRelations;
  struct tExemptions SavedExemptions;
  struct tRepresentationClause *rep;
  bool SavedInUnitTestOnlyBlock;
  int statementdepth;
  jmp_buf saved_recovery;

  // Ignore unit test only blocks if not compiling for unit test, and non unit test only
  // blocks if compiling for unit test
  if ((!UnitTest) && (!d->NotUnitTest))
    {
      return;
    }
  if (UnitTest && (!d->UnitTest))
    {
      return;
    }
  //
  SavedNumVars = CurrentVars->NumVars;
  SavedNumRelations = CurrentVars->NumRelations;
  SavedExemptions = CurrentExemptions;
  if (d->Declarations != NULL)
    {
      //----------
      emit (depth, "{\n");
      //----------
      Analyse (depth + 1, d->Declarations, FALSE, FALSE);
      // Separate the declarations from the code
      emitstr ("\n");
      statementdepth = depth + 1;
    }
  else
    {
      statementdepth = depth;
    }
  // Apply alias locks
  ApplyAliasLocks (SavedNumVars);
  //
  if (d->Declarations != NULL)
    {
      AnalyseInitialisations (statementdepth, d->Declarations);
    }
  //
  // Apply exemptions
  rep = d->Representation;
  while (rep != NULL)
    {
      if (!strcasecmp (rep->Name, "exemption"))
        {
          if (rep->Value->Op == _VAR)
            {
              if (!strcasecmp
                  (rep->Value->Var->VarName, "unchecked_side_effect"))
                {
                  CurrentExemptions.SideEffect = TRUE;
                }
              else
                if (!strcasecmp
                    (rep->Value->Var->VarName, "unchecked_ambiguous_order"))
                {
                  CurrentExemptions.AmbiguousOrder = TRUE;
                }
              else
                if (!strcasecmp
                    (rep->Value->Var->VarName, "unchecked_share"))
                {
                  CurrentExemptions.SubsystemAccess = TRUE;
                }
              else
                if (!strcasecmp
                    (rep->Value->Var->VarName, "shared_atomic_access"))
                {
                  CurrentExemptions.SharedAtomicAccess = TRUE;
                }
              else
                if (!strcasecmp
                    (rep->Value->Var->VarName, "unchecked_access_conversion"))
                {
                  CurrentExemptions.AccessConversion = TRUE;
                }
              else
                if (!strcasecmp (rep->Value->Var->VarName, "unchecked_range"))
                {
                  CurrentExemptions.UncheckedRange = TRUE;
                }
              else
                {
                  err (filename, line, "Unknown exemption '%s'",
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
          err (filename, line, "Unexpected modifier '%s'", rep->Name);
        }
      rep = rep->Next;
    }
  //
  SavedInUnitTestOnlyBlock = InUnitTestOnlyBlock;
  InUnitTestOnlyBlock = InUnitTestOnlyBlock || ((d->UnitTest)
                                                && (!d->NotUnitTest));
  memcpy (saved_recovery, error_recovery, sizeof (jmp_buf));
  // Trap errors here so we can restore SavedInUnitTestBlock
  if (setjmp (error_recovery) == 0)
    {
      //
      Analyse (statementdepth, d->Statements, TRUE, FALSE);
      //
    }
  memcpy (error_recovery, saved_recovery, sizeof (jmp_buf));
  InUnitTestOnlyBlock = SavedInUnitTestOnlyBlock;
  //
  if (d->Declarations != NULL)
    {
      //----------
      emit (depth, "}\n");
      //----------
    }
  // Reverse alias locks
  ReverseAliasLocks (SavedNumVars);
  // Check that newly declared variables have been finalised if required (but not names, which
  // are in out)
  CheckFinalisationAndAccessOfLocals (depth, filename, line, SavedNumVars, TRUE);
  //
  CurrentVars->NumVars = SavedNumVars;
  CurrentVars->NumRelations = SavedNumRelations;
  CurrentExemptions = SavedExemptions;
}

void
AnalyseFree (int depth, char *filename, int line, struct tReference *varref)
{
  enum tBaseType basetype;
  struct tVar *basevar;
  struct tTypeSpec *reftype;
  BIGINT min, max, initialmin, initialmax;
  struct tUnitDef *unit;
  struct tTypeSpec *accessedtype;
  bool initialised;
  enum tMode mode;
  int lock;
  bool wholeobject;
  bool bf = FALSE;
  bool constantaccess, subsystemboundary;

  AnalyseReference (filename, line, varref, &basetype, &basevar, &reftype,
                    &min, &max, &initialmin, &initialmax, &unit,
                    &accessedtype, &initialised, &mode, &lock, FALSE,
                    &wholeobject, FALSE, &constantaccess, FALSE, &subsystemboundary);
  if (reftype->BaseType != ACCESS_TYPE)
    {
      err (filename, line, "Access type expected");
    }
  if (!(reftype->UnmanagedAccess && reftype->NewAccess && (!reftype->PersistentAccess)))
    {
      err (filename, line,
           "Only 'access new' types can be used with free");
    }
  if (!initialised)
    {
      err (filename, line, "Uninitialised pointer in 'free' statement");
    }
  if (mode == MODE_IN)
    {
      err (filename, line, "Cannot free an 'in' parameter");
    }
  if (reftype->CanBeNull && min != 1)
    {
	  err(filename, line,
	      "The access value in a 'free' statement must be known not to be null");
    }
  if (basevar->Lock)
    {
      err (filename, line, "Cannot free a locked varaible");
    }
  if (wholeobject)
    {
      basevar->Initialised = FALSE;
    }
  emit (depth, "%s(", reftype->FreeFunction);
  printreference (filename, line, varref, NULL, &bf, FALSE);
  emitstr (");\n");
  emit(depth, "");
  printreference (filename, line, varref, NULL, &bf, FALSE);
  emitstr(" = NULL;\n");
}

void
AnalyseRestart (int depth, char *filename, int line)
{
  if (InPackageInitialisation || InPackageFinalisation)
    {
      err (filename, line,
           "Restart is not allowed in initialisation and finalisation sections");
    }
  if (InRestartableRoutine)
    {
      if ((TaskImplementation == NO_TASKING) && (!MainRestartDefined)) {
        SwitchToAppHeader();
        emit(0, "#define __MAIN_RESTART\n\n");
        SwitchToSource();
        MainRestartDefined = TRUE;
      }
      emit(depth, "goto __restart;\n");
    }
  else
    {
      emit (depth, "__subsystem_restart(\"%s\", %d);\n", filename, line);
      if (CurrentProc != NULL)
        {
          CurrentProc->Restarts = TRUE;
        }
      HaveRestarts = TRUE;
    }
}

struct tInitialisationRecord
{
  char *Name;
  struct tInitialisationRecord *Next;
};

struct tInitialisationRecord *InitialisationHead = NULL, *InitialisationTail =
  NULL;
struct tInitialisationRecord *FinalisationHead = NULL;

char const *InitialisationSuffix = "_initialisation";
char const *FinalisationSuffix = "_finalisation";

bool GenerateMain = TRUE;
char *SystemStart = NULL;
bool NoSystemStart = FALSE;
BIGINT SystemStackSize = 0;

// This is called for all (global) symbols at the end of analysing the system
// The conditional head must be selected before calling this routine
void
CheckEverUsed (struct tVar *v)
{
  struct tPackage *p;
  char *packagename;

  p = v->EnclosingPackage;
  if (p == NULL)
    packagename = "(none)";
  else
    packagename = p->Name;
  if (IsVariable (v))
    {
      if ((!v->AccessMappedDevice) && (!v->TestMappedDevice)
          && (v->Referent == NULL))
        {
          if (v->Usage == LIBRARY)
            {
              if ((v->EverUsed || (v->InitialisedGlobally && (!v->AccessImage)))
                  && (!v->AddressSpecified))
                {
                  EmitLibraryInclude ("define", getvarname (v));
                }
            }
          else if (v->Usage != TARGET_LIBRARY)
            {
              if (!v->EverUsed)
                {
                  GlobalUsageErrors++;
                  if (!SuppressGlobalUsageChecks)
                    {
                      err_and_continue (v->FileName, v->Line,
                                        "Global variable '%s' in package '%s' is not used",
                                        v->Name, packagename);
                    }
                }
            }
          if (v->EverUsed || v->InitialisedGlobally)
            {
              if (!v->EverInitialised)
                {
                  GlobalUsageErrors++;
                  if (!SuppressGlobalUsageChecks)
                    {
                      err_and_continue (v->FileName, v->Line,
                                        "Global variable '%s' in package '%s' is not initialised at any point",
                                        v->Name, packagename);
                    }
                }
              if ((!v->EverAccessed) && (v->Usage != LIBRARY) && (v->Usage != TARGET_LIBRARY))
                {
                  GlobalUsageErrors++;
                  if (!SuppressGlobalUsageChecks)
                    {
                      err_and_continue (v->FileName, v->Line,
                                        "Global variable '%s' in package '%s' is not read at any point",
                                        v->Name, packagename);
                    }
                }
            }
        }
    }
  else if (v->Type->TypeSpec->BaseType == PROCEDURE_TYPE)
    {
      if (v->Usage == LIBRARY)
        {
          // Library routine
          // If global usage checks are suppressed then include the routine anyway in case
          // an unused but translated routine requires it
          if ((v->EverUsed) || SuppressGlobalUsageChecks)
            {
              // Emit into conditional header
              EmitLibraryInclude ("define", getprocname (v->Type->TypeSpec));
            }
        }
      else if (v->Usage != TARGET_LIBRARY)
        {
          // Not a library routine; must be called somewhere
          if (!v->EverUsed)
            {
              GlobalUsageErrors++;
              if (!SuppressGlobalUsageChecks)
                {
                  err_and_continue (v->FileName, v->Line,
                                    "Non-library routine '%s' in package '%s' is never called",
                                    v->Name, packagename);
                }
            }
        }
    }
}

// Check that all shared variables were initialised in package initialisation
// This is called for all globals at the end of analysing the system
void
CheckSharedInitialisation (struct tVar *v)
{
  if (IsVariable (v) && (v->Mode == MODE_SHARED))
    {
      if (!v->InitialisedGlobally)
        {
          err_and_continue (v->FileName, v->Line,
                            "Shared variable %s.%s was not initialised in a package initialisation section",
                            v->EnclosingPackage->Name, v->Name);
        }
    }
}

void ShowNotInitBeforeUse(struct tVar *v)
{
  err_and_continue (v->FileName, v->Line,
                    "Global variable %s.%s was not definitely initialised before use",
                    v->EnclosingPackage->Name, v->Name);
}

// If it has been determined that a variable must be initialised in a package initialisation
// section (i.e. before the start procedures are run), then check for this
void CheckPackageInitialisation(struct tVar *v)
{
  if (v->RequirePackageInitialisation)
    {
      if (!v->InitialisedGlobally)
        {
          ShowNotInitBeforeUse(v);
        }
    }
}

void
MarkGlobalUsage (struct tGlobalAccess *globalaccesslist)
{
  struct tGlobalAccess *g;

  g = globalaccesslist;
  while (g != NULL)
    {
      g->Var->EverUsed = TRUE;
      if (g->Read)
        g->Var->EverAccessed = TRUE;
      if (g->Write)
        g->Var->EverInitialised = TRUE;
      // In the case of generic formals, update the usage of the corresponding actuals
      if (g->Var->Actual != NULL)
        {
          g->Var->Actual->EverUsed = TRUE;
          if (g->Read)
            g->Var->Actual->EverAccessed = TRUE;
          if (g->Write)
            g->Var->Actual->EverInitialised = TRUE;
        }
      g = g->Next;
    }
}

void
CheckUsage (struct tVar *proc)
{
  struct tCallEntry *p;

  if (proc->EverUsed)
    {
      // Already visited
      return;
    }
  proc->EverUsed = TRUE;
  if (proc->Actual != NULL)
    {
      proc->Actual->EverUsed = TRUE;
      if (VerboseBuild)
        {
          printf ("Procedure %s.%s is actually %s.%s\n",
                  proc->EnclosingPackage->Name, proc->Name,
                  proc->Actual->EnclosingPackage->Name, proc->Actual->Name);
        }
    }
  if (VerboseBuild)
    {
      printf ("Procedure %s.%s calls [", proc->EnclosingPackage->Name,
              proc->Name);
      p = proc->Type->TypeSpec->CallList;
      while (p != NULL)
        {
          printf (" %s.%s", p->Routine->EnclosingPackage->Name,
                  p->Routine->Name);
          p = p->Next;
        }
      printf (" ]\n");
    }
  p = proc->Type->TypeSpec->CallList;
  while (p != NULL)
    {
      CheckUsage (p->Routine);
      p = p->Next;
    }
  MarkGlobalUsage (proc->Type->TypeSpec->GlobalsAccess);
}

void
CheckInitBeforeUse (char *filename, int line, struct tGlobalAccess *g, bool packageinitsection)
{
  if (VerboseBuild)
    {
      printf (">> %s:", g->Var->Name);
      if (g->RequireInitialisation)
        printf (" requires_init");
      if (g->Var->Initialised)
        printf (" initialised");
      if (g->Var->AccessMappedDevice || g->Var->TestMappedDevice)
        printf (" mapped_device");
      if (g->Var->AccessSharedAtomic)
        printf (" shared_atomic");
      if (g->Var->Mode == MODE_SHARED)
        printf (" shared");
      printf ("\n");
    }
  // Shared globals must be initialised in a package initialisation section (checked later)
  // Non-shared globals that require initialisation but are not read by a package
  // initialisation section can be initialised in any package initalisation section
  // Mapped devices do not require initialisation
  if (g->RequireInitialisation && (!g->Var->Initialised)
      && (!g->Var->InitialisedGlobally) && (!g->Var->AccessMappedDevice)
      && (!g->Var->TestMappedDevice) && (g->Var->Mode != MODE_SHARED))
    {
      if (packageinitsection)
        {
          if (VerboseBuild)
            {
              printf("In the initialisation section of the package at %s:%d :\n", filename, line);
            }
          ShowNotInitBeforeUse(g->Var);
        }
      else
        {
          g->Var->RequirePackageInitialisation = TRUE;
        }
      // Under test builds we do require initialisation of mapped devices, so don't exempt TestMappedDevice
    }
}

void
WritePackageLockDeclaration (char *packagename)
{
  emit (0, "__LOCK_TYPE %s%s;\n\n", LockPrefix, packagename);
}

void
AddToLockVarList (char *name)
{
  struct tLockVarListEntry *p =
    checked_malloc (sizeof *p);
  p->Name = name;
  p->Next = AllLockVars;
  AllLockVars = p;
}

void
AnalysePackage (int depth, char *filename, int line,
                struct tPackage *packagedef, enum tAccess packageaccess,
                bool GenericDefinition, struct tGenericParameter *parameters,
                char *packagename, struct tTypeSpec **packagetypespec,
                struct tRepresentationClause *representation,
                enum tPrefix prefix)
{
  int j;
  struct tVar *v;
  struct tTypeSpec *packtype;
  int SavedNumVars, SavedPrefixLen;
  struct tVariableDeclaration var;
  struct tType *packttype;
  struct tGenericParameter *genericformals, *g;
  struct tReference *ref;
  struct tAssignment assignment;
  struct tInitialisationRecord *initrec;
  struct tPackage *SavedPackage, *p;
  char *SavedDefaultAllocateFunction, *SavedDefaultFreeFunction;
  enum tUsage SavedUsage;
  struct tRepresentationClause *rep;
  struct tGlobalAccess *SavedInitialisationGlobals;
  struct tVar *SavedWithResumeThisSubsystem;

  ref = MakeNewReference ();
  if (prefix == PREFIX_UNIT_TEST && (!UnitTest))
    {
      // Omit this package
      return;
    }
  if (prefix == PREFIX_NOT_UNIT_TEST && (UnitTest))
    {
      // Omit this package
      return;
    }
  // Check for a conditional declaration
  if (!CheckBuildCondition (filename, line, packagedef->Representation, TRUE))
    {
      return;
    }
  if (SharedAncestorPackage (CurrentPackage, CurrentPackage) != NULL)
    {
      if (prefix == PREFIX_SHARED)
        {
          err (filename, line,
               "Shared packages cannot contain other shared packages");
        }
      if (packagedef->PackageType != PACKAGE_PACKAGE)
        {
          err (filename, line, "Shared packages cannot contain subsystems");
        }
    }
  //
  if (VerboseBuild)
    {
      printf ("Analysing package %s as %s\n\n", packagedef->Name, packagename);
    }
  if (packagedef->PackageType == PACKAGE_SUBSYSTEM)
    {
      CheckTaskImplementation (filename, line);
    }
  // We need to update the packagedef so that the value of 'Shared' is
  // visible for checking in the processing of a separate declaration
  if (prefix == PREFIX_SHARED)
    {
      packagedef->Shared = TRUE;
    }
  else
    {
      packagedef->Shared = FALSE;
    }
  // Similarly for UnitTest
  if (prefix == PREFIX_UNIT_TEST)
    {
      packagedef->UnitTest = TRUE;
    }
  else
    {
      packagedef->UnitTest = FALSE;
    }
  // And for NotUnitTest
  if (prefix == PREFIX_NOT_UNIT_TEST)
    {
      packagedef->NotUnitTest = TRUE;
    }
  else
    {
      packagedef->NotUnitTest = FALSE;
    }
  //
  p = checked_malloc (sizeof *p);
  *p = *packagedef;
  p->Name = packagename;
  p->Access = packageaccess;
  //
  // Add the package var first so that it is visible within the body
  // Create a new package structure and set the name because this might be a generic instantiation
  packtype = NewTypespec (line);
  packtype->BaseType = PACKAGE_TYPE;
  packtype->PackageSpec = p;
  packtype->Line = line;
  packtype->Unit = NULL;
  packtype->PackageVars = NULL;
  packtype->PackageClosed = FALSE;
  packtype->StructureAccess = PRIVATE_ACCESS;
  packtype->GenericParameters = parameters;
  *packagetypespec = packtype;
  packttype = AllocateTType ();
  packttype->Name = packagename;
  packttype->NameAccess = packageaccess;
  packttype->TypeSpec = packtype;
  packttype->Next = NULL;
  var.ConstValue = NULL;
  if (prefix == PREFIX_SHARED)
    {
      var.Mode = MODE_SHARED;
    }
  else
    {
      var.Mode = MODE_GLOBAL;
    }
  var.TypeName = NULL;
  var.TypeSpec = NULL;
  var.VarNames = MakeIdentifierListEntry(packagename);
  var.Representation = NULL;
  var.PublicOut = FALSE;
  {
    struct tVarListEntry *packl;
    packl = AnalyseAddVar (depth, filename, line, &var, packttype, packageaccess,
                           FALSE, packageaccess);
      p->LockVar = packl->Var;
  }
  p->EnclosingPackage = CurrentPackage;
  //
  SavedPackage = CurrentPackage;
  CurrentPackage = p;
  //
  SavedDefaultAllocateFunction = DefaultAllocateFunction;
  SavedDefaultFreeFunction = DefaultFreeFunction;
  SavedUsage = DefaultUsage;
  if (packagedef->PackageType == PACKAGE_SUBSYSTEM
      || packagedef->PackageType == PACKAGE_SYSTEM)
    {
      SavedWithResumeThisSubsystem = WithResumeThisSubsystem;
      WithResumeThisSubsystem = NULL;
    }
  else
    {
      SavedWithResumeThisSubsystem = NULL;      // Set NULL to prevent an unnecessary warning
    }
  //
  rep = p->Representation;
  while (rep != NULL)
    {
      if (!strcasecmp (rep->Name, "usage"))
        {
          if (rep->Value->Op == _VAR)
            {
              DefaultUsage =
                GetUsage (filename, line, rep->Value->Var->VarName);
            }
          else
            {
              err (filename, line,
                   "Syntax error in 'usage' clause: expected a symbol");
            }
        }
      else if (!strcasecmp (rep->Name, "build_condition"))
        {
          ;                     // Handled above
        }
      else if (!strcasecmp (rep->Name, "task_implementation"))
        {
          if (rep->Value->Op == _VAR)
            {
              if (packagedef->PackageType != PACKAGE_SYSTEM)
                {
                  err (filename, line,
                       "task_implementation can only be applied to the system");
                }
              if (TaskImplementation != NO_TASKING)
                {
                  err (filename, line,
                       "task_implementation can only be specified once");
                }
              char *s = rep->Value->Var->VarName;
              if (!strcasecmp (s, "longjmp"))
                TaskImplementation = COROUTINE;
              else if (!strcasecmp (s, "pthreads"))
                TaskImplementation = PTHREAD;
              else if (!strcasecmp (s, "cyclic"))
                TaskImplementation = CYCLIC;
              else if (!strcasecmp (s, "contiki"))
                {
                  TaskImplementation = CONTIKI;
                  GenerateMain = FALSE;
                }
              else
                err (filename, line, "Unknown task implementation %s", s);
              SwitchToAppHeader ();
              if (TaskImplementation == COROUTINE)
                emitstr ("typedef unsigned __SUBSYSTEM_ID;\n\n");
              else if (TaskImplementation == PTHREAD)
                emitstr ("typedef pthread_t __SUBSYSTEM_ID;\n\n");
              else if (TaskImplementation == CONTIKI)
                {
                  emitstr("#include \"contiki.h\"\n");
                  emitstr("#undef  AUTOSTART_PROCESSES\n");
                  emitstr("#define AUTOSTART_PROCESSES(...)\n\n");
                }
              if (!CyclicTaskModel())
                {
                  emitstr ("typedef struct {\n");
                  emitstr ("  int InUse;\n");
                  emitstr ("  __SUBSYSTEM_ID LockedBy;\n");
                  emitstr ("} __LOCK_TYPE;\n\n");
                  emitstr ("typedef __LOCK_TYPE *__LOCK_TYPE_P;\n\n");
                }
              SwitchToSource ();
            }
          else
            {
              err (filename, line,
                   "Syntax error in 'task_implementation' clause: expected a symbol");
            }
        }
      else
        {
          err (filename, line, "Unknown representation for a package '%s'",
               rep->Name);
        }
      rep = rep->Next;
    }
  // This will hold global accesses from the initialisation and finalisation sections
  // as well as unchecked_use and unchecked_initialisation pragmas that appear as declarations
  // (hence set it to NULL before processing the declarations)
  SavedInitialisationGlobals = InitialisationGlobals;
  InitialisationGlobals = NULL;

  if (GenericDefinition)
    {
      genericformals = parameters;
      while (genericformals != NULL)
        {
          g = genericformals->Next;
          while (g != NULL)
            {
              if (!strcmp (genericformals->Name, g->Name))
                {
                  err (filename, line, "Duplicate formal parameter %s\n",
                       g->Name);
                }
              g = g->Next;
            }
          genericformals = genericformals->Next;
        }
    }
  else
    {
      SavedNumVars = CurrentVars->NumVars;
      CurrentVars->NumRelations = 0;
      SavedPrefixLen = strlen (globalprefix);
      if (packagedef->PackageType == PACKAGE_SYSTEM)
        {
          if (SystemPrefix != NULL)
            {
              strcat (globalprefix, SystemPrefix);
            }
        }
      else
        {
          strcat (globalprefix, packagename);
        }
      p->PathName = strdup (globalprefix);
      if (packagedef->PackageType != PACKAGE_SYSTEM || SystemPrefix != NULL)
        {
          strcat (globalprefix, "_");
        }
      //----------
      if (Annotate)
        {
          PrintLineNumbers = FALSE;
          emit (depth, "/*\n");
          emit (depth, " * Start of package %s\n", packagename);
          emit (depth, " */\n\n");
          PrintLineNumbers = TRUE;
        }
      //----------
      // If this is a shared package then create a lock variable
      if (prefix == PREFIX_SHARED)
        {
          CheckTaskImplementation (filename, line);
          HaveSharedVariables = TRUE;
          if (!CyclicTaskModel())
            {
              WritePackageLockDeclaration (p->PathName);
              SwitchToAppHeader ();
              emitstr ("extern ");
              WritePackageLockDeclaration (p->PathName);
              SwitchToSource ();
            }
          AddToLockVarList (p->PathName);
        }
      //
      Analyse (depth, p->Declarations, FALSE, FALSE);
      if (CurrentPackage->PackageType == PACKAGE_SYSTEM)
        {
          SwitchToAppHeader ();
          emit (0, "#include \"%s\"\n\n", HeaderFileName);
          SwitchToSource ();
        }
    }

  if (GenericDefinition)
    {
      packtype->Scope = MallocVarSet ();
      CopyVarSet (CurrentVars, packtype->Scope);
    }

  if (!GenericDefinition)
    {
      //
      bool initrequired, finalrequired;

      // Check for a start procedure
      if (p->Start != NULL)
        {
          if (   p->Start->Type->TypeSpec->Formals != NULL
              || p->Start->Type->TypeSpec->ReturnType != NULL)
            {
              err (filename, line, "'main' routines must be parameterless procedures");
            }
          SetAccessed(p->Start);
        }

      initrequired = FALSE;

      for (j = SavedNumVars; j < CurrentVars->NumVars; j++)
        {
          v = CurrentVars->Vars[j];
          if (IsVariable (v) && v->Referent == NULL)
            {
              if (v->Mode == MODE_SHARED)
                {
                  // The locks of shared variables need to be initialised
                  initrequired = TRUE;
                }
              v->Initialised = FALSE;
              if (v->ConstValue != NULL)
                {
                  // This variable (declared with x : type := expr) must be initialised in package initialisation,
                  // unless it is an image constant
                  if (v->AccessImage)
                    {
                      v->Initialised = TRUE;
                      v->InitialisedGlobally = TRUE;
                    }
                  else
                    {
                      initrequired = TRUE;
                    }
                }
            }
        }

      if (prefix == PREFIX_SHARED)
        {
          // Shared packages have a lock variable that needs to be initialised
          initrequired = TRUE;
        }

      if (p->Initialisation != NULL)
        {
          initrequired = TRUE;
        }

      if (initrequired)
        {
          if (VerboseBuild)
            {
              printf ("\nInitialisation of %s:\n", packagename);
            }
          initrec = checked_malloc (sizeof *initrec);
          initrec->Name =
            checked_malloc (strlen (globalprefix) +
                            strlen (InitialisationSuffix) + 1);
          strcpy (initrec->Name, globalprefix);
          strcat (initrec->Name, InitialisationSuffix);
          initrec->Next = NULL;
          if (InitialisationTail != NULL)
            InitialisationTail->Next = initrec;
          else
            InitialisationHead = initrec;
          InitialisationTail = initrec;
          //
          //----------
          emit (0, "\n");
          if (Annotate)
            {
              emit (depth, "/*\n");
              emit (depth, " * Initialisation of package %s\n", packagename);
              emit (depth, " */\n");
            }
          emit (depth, "void %s(void)\n", initrec->Name);
          emit (depth, "{\n");
          SwitchToAppHeader ();
          emit (0, "void %s(void);\n\n", initrec->Name);
          SwitchToSource ();
          //----------
          InPackageInitialisation = TRUE;
          ResetGlobalRanges ();
          ResetLocalsCallsBuffs ();
          for (j = SavedNumVars; j < CurrentVars->NumVars; j++)
            {
              v = CurrentVars->Vars[j];
              if (IsVariable (v))
                {
                  // Not a compile-time constant declaration or a type, procedure etc.
                  if (v->ConstValue != NULL)
                    {
                      if (!v->AccessImage)
                        {
                          // Has an initialisation expression, and it is not an image constant
                          // Write initialisation code
                          ref->ReferenceType = IDENTIFIER_REFERENCE;
                          ref->VarName = v->Name;
                          ref->Index = NULL;
                          ref->Next = NULL;
                          ref->ReferenceAbbreviation = NULL;
                          ref->ReferenceAbbreviationEnd = NULL;
                          assignment.Var = ref;
                          assignment.RHS = v->ConstValue;
                          assignment.MakeNew = FALSE;
                          //accessedbefore = v->EverAccessed;
                          AnalyseAssignmentStmt (depth + 1, v->FileName,
                                                 v->Line, &assignment, TRUE);
                          //v->EverAccessed = accessedbefore;
                        }
                    }
                }
            }
          if (p->Initialisation != NULL)
            {
              Analyse (depth + 1, p->Initialisation, TRUE, FALSE);
            }

          {
            struct tGlobalAccess *g;

            g = InitialisationGlobals;
            while (g != NULL)
              {
                CheckInitBeforeUse(filename, line, g, TRUE);
                g = g->Next;
              }
          }

          for (j = SavedNumVars; j < CurrentVars->NumVars; j++)
            {
              v = CurrentVars->Vars[j];
              if (IsVariable (v))
                {
                  if (v->Mode == MODE_SHARED)
                    {
                      if (!CyclicTaskModel())
                        {
                          emit (depth + 1, "%s", LockPrefix);
                          printvarname (v);
                          emit (0, ".InUse = 0;\n");
                        }
                      AddToLockVarList (getvarname (v));
                    }
                }
            }
          // If this is a shared package then initialise the lock variable
          if (prefix == PREFIX_SHARED)
            {
              if (!CyclicTaskModel())
                {
                  emit (depth + 1, "%s%s.InUse = 0;\n", LockPrefix,
                        p->PathName);
                }
            }
          //
          // Mark globals that were initialised here
          //
          {
            struct tGlobalAccess *g;

            g = InitialisationGlobals;
            while (g != NULL)
              {
                //printf("I>>%d %s req %d init %d varinit %d varinitglob %d varacc %d read %d\n", line, g->Var->Name,
                //              g->RequireInitialisation, g->Initialised, g->Var->Initialised, g->Var->InitialisedGlobally, g->Var->Accessed, g->Read);
                if (g->Initialised)
                  {
                    SetInitialised (g->Var);
                  }
                g = g->Next;
              }
          }

          InPackageInitialisation = FALSE;
          //----------
          emit (depth, "}\n\n");
          //----------
        }

      if (p->Start != NULL)
        {
          struct tGlobalAccess *g;

          if (VerboseBuild)
            {
              printf ("Start for %s: %s\n", p->Name, p->Start->Name);
            }
          g = p->Start->Type->TypeSpec->GlobalsAccess;
          while (g != NULL)
            {
              CheckInitBeforeUse (filename, line, g, FALSE);
              g = g->Next;
            }
        }
      //
      for (j = SavedNumVars; j < CurrentVars->NumVars; j++)
        {
          v = CurrentVars->Vars[j];
          if (IsVariable (v) && v->Referent == NULL)
            {
              // Controlled variables must be initialised during package initialisation
              if (v->Type->TypeSpec->Controlled && (!v->InitialisedGlobally))
                {
                  err_and_continue (filename, line,
                                    "Global variable '%s' in package '%s' is controlled but was not initialised in package initialisation",
                                    v->Name, v->EnclosingPackage->Name);
                }
            }
        }
      //
      finalrequired = FALSE;
      for (j = SavedNumVars; j < CurrentVars->NumVars; j++)
        {
          v = CurrentVars->Vars[j];
          if (IsVariable (v))
            {
              if (RequiresFreeing (v->Type->TypeSpec))
                {
                  finalrequired = TRUE;
                  break;
                }
            }
        }
      if (p->Finalisation != NULL)
        {
          finalrequired = TRUE;
        }
      if (finalrequired)
        {
          if (VerboseBuild)
            {
              printf ("Finalisation of %s:\n", packagename);
            }
          initrec = checked_malloc (sizeof *initrec);
          initrec->Name =
            checked_malloc (strlen (globalprefix) +
                            strlen (FinalisationSuffix) + 1);
          strcpy (initrec->Name, globalprefix);
          strcat (initrec->Name, FinalisationSuffix);
          initrec->Next = FinalisationHead;
          FinalisationHead = initrec;
          //
          //----------
          emit (0, "\n");
          if (Annotate)
            {
              emit (depth, "/*\n");
              emit (depth, " * Finalisation of package %s\n", packagename);
              emit (depth, " */\n");
            }
          emit (depth, "void %s(void)\n", initrec->Name);
          emit (depth, "{\n");
          SwitchToAppHeader ();
          emit (0, "void %s(void);\n", initrec->Name);
          SwitchToSource ();
          //----------
          InPackageFinalisation = TRUE;
          ResetLocalsCallsBuffs ();
          ResetGlobalRanges ();
          if (p->Finalisation != NULL)
            {
              Analyse (depth + 1, p->Finalisation, TRUE, FALSE);
            }
          InPackageFinalisation = FALSE;
          //----------
          for (j = SavedNumVars; j < CurrentVars->NumVars; j++)
            {
              v = CurrentVars->Vars[j];
              if (IsVariable (v))
                {
                  if (RequiresFreeing (v->Type->TypeSpec))
                    {
                      CallFreeInternal (depth + 1, line, "", v->CName,
                                        v->Type->TypeSpec);
                    }
                }
            }

          emit (depth, "}\n\n");
          //----------
        }
      //
      for (j = SavedNumVars; j < CurrentVars->NumVars; j++)
        {
          v = CurrentVars->Vars[j];
          if (IsVariable (v))
            {
              if (v->Type->TypeSpec->Controlled && (!v->Finalised))
                {
                  err_and_continue (filename, line,
                                    "Global variable '%s' has not been finalised",
                                    v->Name);
                }
            }
        }
      //
      // Add to the list of variables that were accessed in initialisation and finalisation sections
      //
      if (InitialisationGlobals != NULL)
        {
          struct tGlobalAccess *g;

          g = AllPackageAccesses;
          if (g == NULL)
            {
              AllPackageAccesses = InitialisationGlobals;
            }
          else
            {
              while (g->Next != NULL)
                {
                  g = g->Next;
                }
              g->Next = InitialisationGlobals;
            }
        }
      //
      // Any global that was defined in this package and that was not initialised globally
      // must have its initialised flag reset so that initialisation tracing works
      // in other packages that call procedures in this one.
      //
      for (j = SavedNumVars; j < CurrentVars->NumVars; j++)
        {
          v = CurrentVars->Vars[j];
          if (IsVariable (v))
            {
              if (!v->InitialisedGlobally)
                v->Initialised = FALSE;
            }
        }
      // Export publics from the package
      for (j = SavedNumVars; j < CurrentVars->NumVars; j++)
        {
          v = CurrentVars->Vars[j];
          // Adjust structure access for types (even for non-public ones because the access is
          // associated with the typespec)
          if (v->Type->TypeSpec->BaseType == TYPE_TYPE)
            {
              if (v->Type->TypeSpec->Type->TypeSpec == NULL)
                {
                  // Incomplete type
                  err(filename, line, "Definition of type %s has not been completed", v->Name);
                }
              if (v->Type->TypeSpec->Type->TypeSpec->StructureAccess ==
                  PRIVATE_ACCESS)
                v->Type->TypeSpec->Type->TypeSpec->StructureAccess =
                  HIDDEN_ACCESS;
            }
          if (v->Access == PUBLIC_ACCESS)
            {
              v->Next = packtype->PackageVars;
              packtype->PackageVars = v;
              v->PrivatePublicNext = PublicVarList;
              PublicVarList = v;
            }
          else
            {
              v->PrivatePublicNext = PrivateVarList;
              PrivateVarList = v;
            }
        }
      CurrentVars->NumVars = SavedNumVars;
      globalprefix[SavedPrefixLen] = 0;
      // already done: CurrentVars->NumVars = SavedNumVars;
      CurrentVars->NumRelations = 0;
      packtype->PackageClosed = TRUE;
      //----------
      if (Annotate)
        {
          PrintLineNumbers = FALSE;
          emit (depth, "/*\n");
          emit (depth, " * End of package %s\n", packagename);
          emit (depth, " */\n\n");
          PrintLineNumbers = TRUE;
        }
      //----------
      if (CurrentPackage->PackageType == PACKAGE_SUBSYSTEM)
        {
          if (CurrentPackage->Start == NULL)
            {
              err (filename, line,
                   "Subsystems must contain a 'main' procedure");
            }
          else
            {
              struct tStartEntry *e;

              //CheckResumable (filename, line, CurrentPackage->Start);
              e = checked_malloc (sizeof *e);
              e->Procedure = CurrentPackage->Start;
              e->ProcedureName =
                strdup (getprocname (CurrentPackage->Start->Type->TypeSpec));
              e->StackSize = CurrentPackage->StackSize;
              e->Next = StartList;
              StartList = e;
            }
        }
      else if (CurrentPackage->PackageType == PACKAGE_SYSTEM)
        {
          if (CurrentPackage->Start == NULL)
            {
              if (!NoSystemMain)
                {
                  err (filename, line,
                       "The system does not have a main procedure and pragma no_main has not been given");
                }
              SystemStart = "_no_start";
              NoSystemStart = TRUE;
            }
          else
            {
              SystemStart =
                strdup (getprocname (CurrentPackage->Start->Type->TypeSpec));
              NoSystemStart = FALSE;
            }
          SystemStackSize = CurrentPackage->StackSize;
        }

      // At the outer level, check that globals have been written at some point
      // Also emit conditionals for library usage routines
      if (CurrentPackage->PackageType == PACKAGE_SYSTEM)
        {
          if (VerboseBuild)
            {
              printf ("-- Checking all start points --\n");
            }
          SwitchToCondHeader ();
          if (CurrentPackage->Start != NULL)
            {
              CheckUsage (CurrentPackage->Start);
            }
          struct tStartEntry *StartPtr = StartList;
          while (StartPtr != NULL)
            {
              CheckUsage (StartPtr->Procedure);
              StartPtr = StartPtr->Next;
            }
          struct tCallEntry *outercall;
          outercall = InitialisationCallList;
          while (outercall != NULL)
            {
              CheckUsage (outercall->Routine);
              outercall = outercall->Next;
            }
          outercall = TargetLibraryList;
          while (outercall != NULL)
            {
              struct tGlobalAccess *g =
                outercall->Routine->Type->TypeSpec->GlobalsAccess;
              while (g != NULL)
                {
                  CheckInitBeforeUse (filename, line, g, FALSE);
                  g = g->Next;
                }
              CheckUsage (outercall->Routine);
              outercall = outercall->Next;
            }
          // Mark globals that were accessed/assigned in package initialisation and finalisation sections
          if (VerboseBuild)
            {
              printf
                ("-- Checking globals from initialisation and finalisation sections --\n");
            }
          MarkGlobalUsage (AllPackageAccesses);
          //
          if (VerboseBuild)
            {
              printf ("-- Checking usage of globals and routines --\n");
            }
          j = 0;
          while (j < CurrentVars->NumVars)
            {
              CheckEverUsed (CurrentVars->Vars[j]);
              j++;
            }
          v = PrivateVarList;
          while (v != NULL)
            {
              CheckEverUsed (v);
              CheckSharedInitialisation (v);
              CheckPackageInitialisation(v);
              v = v->PrivatePublicNext;
            }
          v = PublicVarList;
          while (v != NULL)
            {
              CheckEverUsed (v);
              CheckSharedInitialisation (v);
              CheckPackageInitialisation(v);
              v = v->PrivatePublicNext;
            }
          SwitchToSource ();
        }
    }

  // If and routine in the current package dispatches (directly or indirectly), then mark its
  // enclosing package (if any) as dispatching
  if (CurrentPackage->Dispatches
      && (CurrentPackage->EnclosingPackage != NULL))
    {
      CurrentPackage->EnclosingPackage->Dispatches = TRUE;
    }

  if (packagedef->PackageType == PACKAGE_SUBSYSTEM
      || packagedef->PackageType == PACKAGE_SYSTEM)
    {
      WithResumeThisSubsystem = SavedWithResumeThisSubsystem;
    }
  InitialisationGlobals = SavedInitialisationGlobals;
  CurrentPackage = SavedPackage;
  DefaultAllocateFunction = SavedDefaultAllocateFunction;
  DefaultFreeFunction = SavedDefaultFreeFunction;
  DefaultUsage = SavedUsage;
  if (VerboseBuild)
    {
      struct tVar *v;

      printf ("Exports from %s: [", packagename);
      v = packtype->PackageVars;
      while (v != NULL)
        {
          printf (" %s", v->Name);
          v = v->Next;
        }
      printf (" ]\n");
      printf ("Done analysing package %s as %s\n\n", p->Name, packagename);
    }
}

// The instantiation of a generic (template)
void
AnalyseGenericInstantiation (int depth, char *filename, int line,
                             struct tGenericInstantiation *g,
                             enum tAccess access, enum tPrefix prefix)
{
  struct tReference *r;
  struct tVar *generic;
  struct tGenericActualParameter *actuals;
  struct tGenericParameter *genericformals;
  bool found;
  int numformals, numactuals;
  struct tVars *savedvars, *postvars;
  struct tVar **actualvars, **avar, *newvar = NULL;
  int initialnumvars, j, firstactual;
  struct tTypeSpec *packagetypespec;
  struct tPackage *thispackage;
  char *savedactivegeneric;

  r = g->Generic;
  generic = FindVariable (filename, line, &r, 40, FALSE, TRUE);
  if (generic->Type->TypeSpec->BaseType != PACKAGE_TYPE)
    {
      errstart (filename, line);
      errcont ("Not a package: ");
      errDumpReference (g->Generic);
      errend ();
      return;
    }
  genericformals = generic->Type->TypeSpec->GenericParameters;
  if (genericformals == NULL)
    {
      err (filename, line, "Package is not generic");
    }
  if (generic->Type->TypeSpec->PackageSpec->PackageType != g->PackageType)
    {
      err (filename, line, "Package type (package/subsystem) mismatch between specification and instantiation");
    }
  if (generic->Type->TypeSpec->PackageSpec->Shared && prefix != PREFIX_SHARED)
    {
      err (filename, line, "Formal package is marked 'shared' but actual is not");
    }
  if ((!generic->Type->TypeSpec->PackageSpec->Shared) && prefix == PREFIX_SHARED)
    {
      err (filename, line, "Formal package is not marked 'shared' but actual is");
    }
  //
  if (VerboseBuild)
    {
      printf ("Instantiating generic package %s.%s as %s\n",
              generic->EnclosingPackage->Name, generic->Name, g->Name);
    }
  // Count the formals and the actuals
  numformals = 0;
  while (genericformals != NULL)
    {
      numformals++;
      genericformals = genericformals->Next;
    }
  actuals = g->Actuals;
  numactuals = 0;
  while (actuals != NULL)
    {
      numactuals++;
      actuals = actuals->Next;
    }
  if (numformals != numactuals)
    {
      err (filename, line,
           "The number of actuals %d does not equal the number of formals %d",
           numactuals, numformals);
    }
  // Create a new package spec
  thispackage = checked_malloc (sizeof *thispackage);
  *(thispackage) = *(generic->Type->TypeSpec->PackageSpec);
  thispackage->Name = g->Name;
  // Allocate space to hold the evaluated actuals
  actualvars = checked_malloc (numactuals * sizeof (struct tVar *));
  // Check that the formals and the actuals match and evaluate the actuals in the context of the
  // instantiation
  genericformals = generic->Type->TypeSpec->GenericParameters;
  avar = actualvars;
  while (genericformals != NULL)
    {
      actuals = g->Actuals;
      found = FALSE;
      while (actuals != NULL)
        {
          if (!strcmp (genericformals->Name, actuals->Name))
            {
              found = TRUE;
              break;
            }
          actuals = actuals->Next;
        }
      if (found)
        {
          bool typematch = FALSE;

          // Check the type
          if (genericformals->Type == GENERIC_RANGE_CONSTANT
              || genericformals->Type == GENERIC_DIGITS_CONSTANT
              || genericformals->Type == GENERIC_BOOLEAN_CONSTANT
              || genericformals->Type == GENERIC_CSTRING_CONSTANT
              || genericformals->Type == GENERIC_CONSTANT)
            {
              enum tBaseType exprtype;
              bool success;
              BIGINT intval;
              BIGFLOAT floatval;
              bool boolval;
              struct tReference *accessedobject;
              struct tTypeSpec *accessed;
              bool uncheckedaccess;
              struct tUnitDef *unit;
              int savedlen;

              savedlen = strlen (globalprefix);
              strcat (globalprefix, g->Name);
              strcat (globalprefix, "_");

              if (TestForCString (actuals->Expression, FALSE, TRUE))
                {
                  // This is a special case
                  exprtype = CSTRING_TYPE;
                  newvar = MallocVar (filename, line);
                  InitialiseForConstant (newvar, filename, line,
                                         genericformals->Name, PRIVATE_ACCESS,
                                         NULL, NULL, NULL, NULL);
                  SetupConstantString (depth, filename, line, newvar,
                                       actuals->Expression);
                  success = TRUE;
                  if (genericformals->Type == GENERIC_CSTRING_CONSTANT
                      || genericformals->Type == GENERIC_CONSTANT)
                    {
                      typematch = TRUE;
                    }
                  else
                    {
                      err (filename, line,
                           "Type mismatch for constant generic parameter %s",
                           genericformals->Name);
                      typematch = FALSE;
                    }
                }
              else
                {
                  EvalStaticExpression (actuals->Expression, &exprtype,
                                        &intval, &floatval, &boolval,
                                        &accessed, &uncheckedaccess, &unit,
                                        &success);
                  accessedobject = NULL;
                  if (!success)
                    {
                      err (filename, line,
                           "Actual for formal %s is not constant",
                           genericformals->Name);
                    }
                  if (genericformals->Type == GENERIC_RANGE_CONSTANT)
                    typematch = (exprtype == INTEGER_TYPE);
                  else if (genericformals->Type == GENERIC_DIGITS_CONSTANT)
                    typematch = (exprtype == FLOATING_TYPE);
                  else if (genericformals->Type == GENERIC_BOOLEAN_CONSTANT)
                    typematch = (exprtype == BOOLEAN_TYPE);
                  else if (genericformals->Type == GENERIC_ACCESS_CONSTANT)
                    typematch = (exprtype == ACCESS_TYPE);
                  else
                    typematch = TRUE;
                  if (exprtype == ACCESS_TYPE)
                    {
                      if (accessed != NULL)
                        {
                          accessedobject = actuals->Expression->Var;    // else NULL from above
                        }
                    }
                  if (typematch)
                    {
                      // Create a new constant
                      newvar = MallocVar (filename, line);
                      InitialiseForConstant (newvar, filename, line,
                                             genericformals->Name,
                                             PRIVATE_ACCESS, unit,
                                             accessedobject, accessed, NULL);
                      if (exprtype == INTEGER_TYPE)
                        {
                          newvar->High = intval;
                          newvar->Low = intval;
                          newvar->Type = &ConstantIntegerType;
                        }
                      else if (exprtype == FLOATING_TYPE)
                        {
                          newvar->FloatVal = floatval;
                          newvar->Type = &ConstantFloatType;
                        }
                      else if (exprtype == BOOLEAN_TYPE)
                        {
                          newvar->High = boolval;
                          newvar->Low = boolval;
                          newvar->Type = &ConstantBooleanType;
                        }
                      else if (exprtype == ACCESS_TYPE)
                        {
                          newvar->High = intval;
                          newvar->Low = intval;
                          newvar->Type = &ConstantAccessType;
                        }
                      WriteConstantToHeader (depth,
                                             actuals->Expression->FileName,
                                             actuals->Expression->Line,
                                             exprtype, newvar,
                                             genericformals->Name, TRUE,
                                             TRUE);
                      // cstrings are handled above as a special case
                    }
                }
              //
              globalprefix[savedlen] = 0;
            }
          else if (genericformals->Type == GENERIC_PROCEDURE
                   || genericformals->Type == GENERIC_FUNCTION)
            {
              // formal => actual or formal => actual(f1 => p1, ...)
              struct tReference *ref;
              struct tVar *var;
              struct tProcedureCall *p;

              if (actuals->Expression->Op == _VAR)
                {
                  // This could be a parameterless procedure or function
                  ref = actuals->Expression->Var;
                  var =
                    FindVariable (actuals->Expression->FileName,
                                  actuals->Expression->Line, &ref, 80, FALSE,
                                  TRUE);
                  // Create a procedure call record
                  p = MakeProcedureCall (actuals->Expression->Var, NULL);
                  typematch = TRUE;
                }
              else if (actuals->Expression->Op == _FN_CALL)
                {
                  p = actuals->Expression->Call->Ptr;
                  typematch = TRUE;
                }
              else
                {
                  p = NULL;     // Suppress spurious warnings
                  typematch = FALSE;
                }
              if (typematch)
                {
                  enum tBaseType actualbasetype;
                  struct tActualParameter *ap;
                  // The generic formal hasn't been turned into a procedure definition, so it is still
                  // a list of tElts
                  struct tElt *genericfp;
                  // The actual procedure has a formal parameter list
                  struct tFormalParameter *procfp;
                  // For individual generic formal parameters
                  struct tVariableDeclaration *genericformal = NULL;    // Initialise to avoid spurious warnings CHECK THOUGH
                  //
                  // The type of a generic formal
                  struct tType *gftype;
                  //
                  struct tVar **newptr;
                  int formalposition, actualposition;

                  // Look up the actual parameter
                  ref = p->Name;
                  var =
                    FindVariable (actuals->Expression->FileName,
                                  actuals->Expression->Line, &ref, 80, FALSE,
                                  TRUE);
                  actualbasetype = var->Type->TypeSpec->BaseType;
                  //
                  // Match the formals of the actual procedure to the formals of the formal parameter
                  if (actualbasetype == PROCEDURE_TYPE)
                    {
                      int numap, numgenericfp, numprocfp;
                      bool found;

                      // Every entry in ap must be of the form g => f, where g is a name from the
                      // formal parameter of the generic definition, and f is the name of a formal
                      // parameter of the actual procedure
                      // Check that there are the right number of terms and that the types and modes match
                      // Check the numbers
                      ap = p->ActualList;
                      numap = 0;
                      while (ap != NULL)
                        {
                          numap++;
                          ap = ap->Next;
                        }
                      genericfp = genericformals->Formals;
                      numgenericfp = 0;
                      while (genericfp != NULL)
                        {
                          numgenericfp++;
                          genericfp = genericfp->Next;
                        }
                      procfp = var->Type->TypeSpec->Formals;
                      numprocfp = 0;
                      while (procfp != NULL)
                        {
                          numprocfp++;
                          procfp = procfp->Next;
                        }
                      if (numap != numgenericfp || numap != numprocfp)
                        {
                          err (filename, line,
                               "Error in generic instantiation: routine '%s' has %d formals, the generic formal '%s' has %d, and the instantiation has %d",
                               var->Name, numprocfp, genericformals->Name,
                               numgenericfp, numap);
                        }
                      // Make sure that every entry in the generic formal occurs in the actual list
                      genericfp = genericformals->Formals;
                      while (genericfp != NULL)
                        {
                          genericformal =
                            (struct tVariableDeclaration *) genericfp->Ptr;
                          ap = p->ActualList;
                          found = FALSE;
                          while (ap != NULL)
                            {
                              if (!strcmp (genericformal->VarNames->SymbolName, ap->Name))
                                {
                                  found = TRUE;
                                  break;
                                }
                              ap = ap->Next;
                            }
                          if (!found)
                            {
                              err (filename, line,
                                   "No actual for parameter %s of generic formal",
                                   genericformal->VarNames->SymbolName);
                            }
                          genericfp = genericfp->Next;
                        }
                      // Make sure that every formal of the procedure appears once on the RHS of a parameter of the instantiation
                      procfp = var->Type->TypeSpec->Formals;
                      while (procfp != NULL)
                        {
                          found = FALSE;
                          ap = p->ActualList;
                          while (ap != NULL)
                            {
                              if (ap->Expr->Op == _VAR)
                                {
                                  if (ap->Expr->Var->Next == NULL)
                                    {
                                      if (!strcmp
                                          (ap->Expr->Var->VarName,
                                           procfp->Name))
                                        {
                                          found = TRUE;
                                          break;
                                        }
                                    }
                                  else
                                    {
                                      err (filename, line,
                                           "The RHS of the actual %s must be a simple name",
                                           ap->Name);
                                    }
                                }
                              else
                                {
                                  err (filename, line,
                                       "The RHS of the actual %s must be a name",
                                       ap->Name);
                                }
                              ap = ap->Next;
                            }
                          if (!found)
                            {
                              err (filename, line,
                                   "Formal parameter %s of procedure %s does not appear in the generic instantiation",
                                   procfp->Name, var->Name);
                            }
                          procfp = procfp->Next;
                        }
                      // Now check that the types match
                      ap = p->ActualList;
                      while (ap != NULL)
                        {
                          // find the LHS in the generic formal
                          genericfp = genericformals->Formals;
                          while (genericfp != NULL)
                            {
                              genericformal =
                                (struct tVariableDeclaration *)
                                genericfp->Ptr;
                              if (!strcmp (ap->Name, genericformal->VarNames->SymbolName))
                                break;
                              genericfp = genericfp->Next;
                            }
                          if (genericfp == NULL)
                            {
                              err (filename, line, "No formal for actual %s",
                                   ap->Name);
                            }
                          // find the RHS in the procedure formals (there will only be one match)
                          procfp = var->Type->TypeSpec->Formals;
                          while (procfp != NULL)
                            {
                              // We checked above that the RHS of the actual is of the right form
                              if (!strcmp
                                  (ap->Expr->Var->VarName, procfp->Name))
                                break;
                              procfp = procfp->Next;
                            }
                          if (procfp == NULL)
                            {
                              err (filename, line, "No match for actual RHS %s", ap->Expr->Var->VarName);       // this shouldn't happen
                            }
                          // Check genericfp against procfp
                          // First check the generic types that have been created so far
                          gftype = NULL;
                          if (genericformal->TypeName->Name->Next == NULL)
                            {
                              newptr = actualvars;
                              while (newptr < avar)
                                {
                                  if (!strcmp
                                      ((*newptr)->Name,
                                       genericformal->TypeName->
                                       Name->VarName))
                                    {
                                      if ((*newptr)->Type->
                                          TypeSpec->BaseType == TYPE_TYPE)
                                        {
                                          gftype =
                                            (*newptr)->Type->TypeSpec->Type;
                                          break;
                                        }
                                    }
                                  newptr++;
                                }
                            }
                          if (gftype == NULL)
                            {
                              gftype =
                                FindType (filename, line,
                                          genericformal->TypeName);
                            }
                          if (gftype->TypeSpec != procfp->TypeSpec)
                            {
                              err (filename, line,
                                   "The formal %s of the generic and the formal %s of routine %s are not of the same type",
                                   genericformal->VarNames->SymbolName, procfp->Name,
                                   var->Name);
                            }
                          if (genericformal->Mode != procfp->Mode)
                            {
                              err (filename, line,
                                   "The formal %s of the generic and the formal %s of routine %s have different modes",
                                   genericformal->VarNames->SymbolName, procfp->Name,
                                   var->Name);
                            }
                          //
                          ap = ap->Next;
                        }
                      if (genericformals->ReturnType == NULL)
                        {
                          if (var->Type->TypeSpec->ReturnType != NULL)
                            {
                              err (filename, line,
                                   "Generic formal %s is a procedure but the actual %s is a function",
                                   genericformal->VarNames->SymbolName, var->Name);
                            }
                        }
                      else
                        {
                          struct tType *returntype = FindType (filename, line,
                                                               ((struct
                                                                 tVariableDeclaration
                                                                 *)
                                                                (genericformals->
                                                                 ReturnType->Ptr))->
                                                               TypeName);
                          if (returntype->TypeSpec !=
                              var->Type->TypeSpec->ReturnType)
                            {
                              err (filename, line,
                                   "The return type of the generic formal function %s does not match the actual function %s",
                                   genericformal->VarNames->SymbolName, var->Name);
                            }
                        }
                      // Copy, then modify, the procedure variable
                      newvar = CopyVar (var);
                      newvar->Actual = var;
                      // Copy the type of the actual procedure and change the parameter names of the copy
                      // to match the generic formal.
                      // Keep the parameter order of the actual.
                      // The existence of the matches was verified above.
                      newvar->Type = AllocateTType ();
                      *(newvar->Type) = *(var->Type);
                      newvar->Type->TypeSpec = NewTypespec (line);
                      *(newvar->Type->TypeSpec) = *(var->Type->TypeSpec);
                      procfp = newvar->Type->TypeSpec->Formals;
                      formalposition = 0;
                      while (procfp != NULL)
                        {
                          formalposition++;
                          ap = p->ActualList;
                          actualposition = 0;
                          while (ap != NULL)
                            {
                              if (!strcmp
                                  (procfp->Name, ap->Expr->Var->VarName))
                                {
                                  procfp->Name = ap->Name;
                                  // If the positions of the formal and actual do not match,
                                  // then require named parameters
                                  if (formalposition != actualposition)
                                    {
                                      newvar->Type->
                                        TypeSpec->NamedParametersRequired =
                                        TRUE;
                                    }
                                  break;
                                }
                              actualposition++;
                              ap = ap->Next;
                            }
                          procfp = procfp->Next;
                        }
                      typematch = TRUE;
                    }
                  else
                    {
                      typematch = FALSE;
                    }
                }
              else
                {
                  typematch = FALSE;
                }
            }
          else if (genericformals->Type == GENERIC_GENERIC_PACKAGE)
            {
              if (actuals->Expression->Op == _VAR)
                {
                  struct tReference *ref;
                  struct tVar *var;
                  struct tReference *formalref;
                  struct tVar *formalvar;

                  ref = actuals->Expression->Var;
                  var =
                    FindVariable (actuals->Expression->FileName,
                                  actuals->Expression->Line, &ref, 80, FALSE,
                                  TRUE);
                  // Check that the formal is a generic package and that the actual is an
                  // instantiation of it
                  formalref = genericformals->Identifier;
                  formalvar =
                    FindVariable (actuals->Expression->FileName,
                                  actuals->Expression->Line, &formalref, 81,
                                  FALSE, TRUE);
                  if (formalvar->Type->TypeSpec->BaseType == PACKAGE_TYPE
                      && formalvar->Type->TypeSpec->GenericParameters != NULL)
                    {
                      if (var->Type->TypeSpec->BaseType == PACKAGE_TYPE)
                        {
                          struct tPackage *actualpackage;
                          actualpackage = var->Type->TypeSpec->PackageSpec;     // the actual package being used for the generic formal package
                          if (formalvar->Type->TypeSpec->PackageSpec ==
                              actualpackage->GenericFrom->PackageSpec)
                            {
                              // Check the restrictions genericformal => actual ..., if present
                              struct tGenericActualParameter *formalactuals,
                                *actualactuals;
                              struct tGenericParameter *actualformals;
                              enum tGenericParameterType actualformaltype;

                              formalactuals = genericformals->GenericActuals;   // i.e. the restrictions x => y, ...
                              typematch = TRUE;
                              while (formalactuals != NULL)
                                {
                                  if (formalactuals->Expression->Op != _VAR)
                                    {
                                      typematch = FALSE;
                                      err (filename, line,
                                           "Restrictions in a generic formal package must be of the form identifier1 => identifier2");
                                    }
                                  actualformals =
                                    actualpackage->
                                    GenericFrom->GenericParameters;
                                  while (actualformals != NULL)
                                    {
                                      if (!strcmp
                                          (actualformals->Name,
                                           formalactuals->Name))
                                        {
                                          break;
                                        }
                                      actualformals = actualformals->Next;
                                    }
                                  if (actualformals == NULL)
                                    {
                                      typematch = FALSE;
                                      err (filename, line,
                                           "Restriction on %s does not correspond to a formal parameter of the generic package %s"
                                           " from which the actual package %s was derived",
                                           formalactuals->Name,
                                           actualpackage->GenericFrom->Name,
                                           actualpackage->Name);
                                    }
                                  actualactuals =
                                    actualpackage->GenericActuals;
                                  while (actualactuals != NULL)
                                    {
                                      if (!strcmp
                                          (formalactuals->Name,
                                           actualactuals->Name))
                                        {
                                          break;
                                        }
                                      actualactuals = actualactuals->Next;
                                    }
                                  if (actualactuals == NULL)
                                    {
                                      typematch = FALSE;
                                      err (filename, line,
                                           "Unexpected missing generic actual parameter %s",
                                           formalactuals->Name);
                                    }
                                  //
                                  // actualformals points into the formals of the package from which actualpackage was derived
                                  // actualactuals points into the corresponding parameter of the derivation
                                  actualformaltype = actualformals->Type;
                                  if (actualformaltype == GENERIC_TYPE
                                      || actualformaltype ==
                                      GENERIC_RANGE_TYPE
                                      || actualformaltype ==
                                      GENERIC_DIGITS_TYPE
                                      || actualformaltype ==
                                      GENERIC_ARRAY_TYPE
                                      || actualformaltype ==
                                      GENERIC_ACCESS_TYPE)
                                    {
                                      // Find the type of the RHS of the restriction
                                      struct tVar **newptr;
                                      struct tType *gftype;
                                      struct tTypeIdentifier typeident;

                                      gftype = NULL;
                                      if (formalactuals->Expression->
                                          Var->Next == NULL)
                                        {
                                          newptr = actualvars;
                                          while (newptr < avar)
                                            {
                                              if (!strcmp
                                                  ((*newptr)->Name,
                                                   formalactuals->
                                                   Expression->Var->VarName))
                                                {
                                                  if ((*newptr)->
                                                      Type->TypeSpec->
                                                      BaseType == TYPE_TYPE)
                                                    {
                                                      gftype =
                                                        (*newptr)->
                                                        Type->TypeSpec->Type;
                                                      break;
                                                    }
                                                  else
                                                    {
                                                      err (filename, line,
                                                           "%s should be a type",
                                                           (*newptr)->Name);
                                                      break;
                                                    }
                                                }
                                              newptr++;
                                            }
                                        }
                                      if (gftype == NULL)
                                        {
                                          typeident.Name =
                                            formalactuals->Expression->Var;
                                          typeident.Attribute = NULL;
                                          gftype =
                                            FindType (filename, line,
                                                      &typeident);
                                        }
                                      // Compare with the type from the instantiation
                                      if (gftype->TypeSpec !=
                                          actualactuals->FoundType)
                                        {
                                          typematch = FALSE;
                                          err (filename, line,
                                               "Type %s does not match the restriction %s for %s",
                                               actualactuals->FoundType->Name,
                                               gftype->Name,
                                               formalactuals->Name);
                                        }
                                    }
                                  else
                                    {
                                      err (filename, line,
                                           "Restrictions on generic formal packages can only apply to types");
                                    }
                                  formalactuals = formalactuals->Next;
                                }
                            }
                          else
                            {
                              typematch = FALSE;
                              err (filename, line,
                                   "Actual package %s is not an instance of formal %s",
                                   var->Name, formalvar->Name);
                            }
                        }
                      else
                        {
                          typematch = FALSE;
                          err (filename, line, "Actual %s is not a package",
                               var->Name);
                        }
                    }
                  else
                    {
                      typematch = FALSE;
                      err (filename, line,
                           "Formal %s is not a generic package",
                           formalvar->Name);
                    }
                  if (typematch)
                    {
                      newvar = CopyVar (var);
                      newvar->Actual = var;
                    }
                }
              else
                {
                  typematch = FALSE;
                }
            }
          else if (actuals->Expression->Op == _VAR)
            {
              struct tReference *ref;
              struct tVar *var;
              enum tBaseType basetype;
              struct tTypeSpec *actualtypespec;

              ref = actuals->Expression->Var;
              var =
                FindVariable (actuals->Expression->FileName,
                              actuals->Expression->Line, &ref, 80, FALSE,
                              TRUE);
              actualtypespec = var->Type->TypeSpec->Type->TypeSpec;

              basetype = var->Type->TypeSpec->BaseType;
              if (genericformals->Type == GENERIC_RANGE_TYPE)
                {
                  if (basetype == TYPE_TYPE)
                    {
                      typematch = (actualtypespec->BaseType == INTEGER_TYPE);
                    }
                  else
                    {
                      typematch = FALSE;
                    }
                }
              else if (genericformals->Type == GENERIC_DIGITS_TYPE)
                {
                  if (basetype == TYPE_TYPE)
                    {
                      typematch = (actualtypespec->BaseType == FLOATING_TYPE);
                    }
                  else
                    {
                      typematch = FALSE;
                    }
                }
              else if (genericformals->Type == GENERIC_ARRAY_TYPE)
                {
                  if (basetype == TYPE_TYPE)
                    {
                      typematch = (actualtypespec->BaseType == ARRAY_TYPE);
                    }
                  else
                    {
                      typematch = FALSE;
                    }
                }
              else if (genericformals->Type == GENERIC_ACCESS_TYPE)
                {
                  if (basetype == TYPE_TYPE)
                    {
                      typematch = (actualtypespec->BaseType == ACCESS_TYPE);
                    }
                  else
                    {
                      typematch = FALSE;
                    }
                }
              else if (genericformals->Type == GENERIC_TYPE)
                {
                  typematch = (basetype == TYPE_TYPE);
                }
              else
                {
                  // Shouldn't get here
                  typematch = FALSE;
                }
              if (typematch)
                {
                  // Copy the entity
                  newvar = CopyVar (var);
                  newvar->Actual = var;
                  // Record the actual typespec for use in formal generic package testing
                  actuals->FoundType = actualtypespec;
                }
            }
          else
            {
              typematch = FALSE;
            }
          if (typematch)
            {
              // Change the name of the copy (but keep the actual as it is)
              newvar->Name = genericformals->Name;
              newvar->EnclosingPackage = thispackage;
              *avar = newvar;
            }
          else
            {
              err (filename, line, "Mismatch for generic formal %s",
                   genericformals->Name);
            }
        }
      else
        {
          err (filename, line, "No actual parameter for formal %s",
               genericformals->Name);
        }
      avar++;
      genericformals = genericformals->Next;
    }
  // Restore the scope environment of the generic formal
  savedvars = MallocVarSet ();
  CopyVarSet (CurrentVars, savedvars);
  CopyVarSet (generic->Type->TypeSpec->Scope, CurrentVars);
  CurrentVars->NumRelations = 0;
  // Set up actuals in the environment
  firstactual = CurrentVars->NumVars;
  for (j = 0; j < numactuals; j++)
    {
      CheckForVarOverflow (filename, line);
      CurrentVars->Vars[CurrentVars->NumVars] = actualvars[j];
      CurrentVars->Vars[CurrentVars->NumVars]->Access = PUBLIC_ACCESS;
      CurrentVars->NumVars++;
    }
  free (actualvars);
  // Number of variables including generic parameters
  initialnumvars = CurrentVars->NumVars;
  // Analyse the generic package specification in the context of the new bindings
  savedactivegeneric = ActiveGeneric;
  ActiveGeneric = g->Name;
  AnalysePackage (depth, filename, line, generic->Type->TypeSpec->PackageSpec,
                  access, FALSE, NULL, g->Name, &packagetypespec, NULL,
                  (generic->Type->TypeSpec->
                   PackageSpec->Shared ? PREFIX_SHARED : PREFIX_NONE));
  ActiveGeneric = savedactivegeneric;
  packagetypespec->PackageSpec = thispackage;
  thispackage->GenericFrom = generic->Type->TypeSpec;
  thispackage->GenericActuals = g->Actuals;
  postvars = MallocVarSet ();
  CopyVarSet (CurrentVars, postvars);
  CopyVarSet (savedvars, CurrentVars);
  // Export the result of analysing the package
  for (j = initialnumvars; j < postvars->NumVars; j++)
    {
      CheckForVarOverflow (filename, line);
      CurrentVars->Vars[CurrentVars->NumVars] = postvars->Vars[j];
      postvars->Vars[j] = NULL;
      CurrentVars->NumVars++;
    }
  // Export the generic parameters from the new package (necessary for generic formal packages)
  for (j = 0; j < numactuals; j++)
    {
      struct tVar *vn;

      vn = postvars->Vars[firstactual + j];
      vn->EnclosingPackage = packagetypespec->PackageSpec;
      vn->Next = packagetypespec->PackageVars;
      packagetypespec->PackageVars = vn;
      vn->PrivatePublicNext = PublicVarList;
      PublicVarList = vn;
    }
  FreeVarSet (postvars);
  FreeVarSet (savedvars);
}

void
TestIndexes (char *filename, int line,
             BIGINT formallow, BIGINT formalhigh, BIGINT actuallow,
             BIGINT actualhigh, bool mindefinite, bool maxdefinite)
{
  if (actuallow < formallow || actualhigh > formalhigh)
    {
      err (filename, line,
           "Index of actual %" PRINTBIG "..%" PRINTBIG " is outside formal %"
           PRINTBIG "..%" PRINTBIG "", actuallow, actualhigh, formallow,
           formalhigh);
    }
  if (mindefinite && (actuallow != formallow))
    {
      err (filename, line,
           "Actual range low %" PRINTBIG " should be equal to formal %"
           PRINTBIG "", actuallow, formallow);
    }
  if (maxdefinite && (actualhigh != formalhigh))
    {
      err (filename, line,
           "Actual range high %" PRINTBIG " should be equal to formal %"
           PRINTBIG "", actualhigh, formalhigh);
    }
}

void
CheckArrayParameter (char *filename, int line,
                     struct tTypeSpec *formaltype, struct tTypeSpec *reftype,
                     struct tTypeSpec **actualref,
                     struct tFormalParameter *formal, struct tNode *actual)
{
  // Both are arrays
  // Check for passing a slice as an actual parameter
  bool slice;
  struct tNode *lowexpr, *highexpr;
  enum tReferenceType slicetype;

  CheckForSlice (actual->Var, &slice, &lowexpr, &highexpr, &slicetype);
  if (slice)
    {
      if (highexpr == NULL)
        {
          if (!formaltype->MaxDefinite)
            {
              err (filename, line,
                   "An actual of the form a[x..] cannot be passed to a formal with an indefinite last index");
            }
          if (reftype->High != formaltype->High)
            {
              err (filename, line,
                   "When an actual of the form a[x..] is passed to a formal, the upper bound of the actual (here %"
                   PRINTBIG ") must equal that of the formal (here %" PRINTBIG
                   ")", reftype->High, formaltype->High);
            }
        }
      CheckSliceBounds (filename, line, lowexpr, highexpr, slicetype,
                        formaltype->MinDefinite, formaltype->Low,
                        formaltype->MaxDefinite, formaltype->High);
    }
  else if (formaltype != reftype)
    {
      // Different types, but if the formal is indefinite then it might be OK
      if (!(formaltype->MinDefinite && formaltype->MaxDefinite))
        {
          *actualref = reftype;
          TestIndexes (filename, line,
                       formaltype->Low, formaltype->High,
                       reftype->Low, reftype->High,
                       formaltype->MinDefinite, formaltype->MaxDefinite);
        }
      else
        {
          err (filename, line, "Array type mismatch on formal %s",
               formal->Name);
        }
    }
}

void
CheckAccessParameter (char *filename, int line,
                      struct tTypeSpec *formaltype, struct tTypeSpec *reftype,
                      struct tFormalParameter *formal)
{
  if (   formaltype->ElementTypeSpec != reftype->ElementTypeSpec
      || (!formaltype->ConstantAccess) != (!reftype->ConstantAccess)
      || (!formaltype->UnmanagedAccess) != (!reftype->UnmanagedAccess)
      || (!formaltype->PersistentAccess) != (!reftype->PersistentAccess))
    {
      err (filename, line, "Access type mismatch on formal %s", formal->Name);
    }
  if (   reftype->NewAccess && (!formaltype->NewAccess)
      && (formal->Mode == MODE_OUT || formal->Mode == MODE_IN_OUT || formal->Mode == MODE_FINAL_IN_OUT))
    {
      err (filename, line,
           "An actual parameter of type 'access new' cannot be assigned from a formal parameter (%s) that is not 'access new'",
           formal->Name);
    }
}

void
CheckFormalActualAccessModes (char *filename, int line, struct tVar *formal,
                              struct tVar *actual)
{
  if (actual->AccessVolatile)
    { // includes mapped device and shared atomic
      // If the actual is volatile then the formal should also be
      if (!(formal->AccessVolatile))
        {
          err (filename, line,
               "If an actual parameter is volatile or mapped_device then the formal parameter must be volatile");
        }
    }
}

// loopscan is used in loop scans, where the range of the actual parameter should be
// set to its full range, not the range of the formal parameter
void
MatchParameter (char *filename, int line,
                struct tFormalParameter *formal,
                struct tActualParameter *actualparameter,
                bool * finalised, bool functioncall, bool calltoclosed)
{
//      struct tType *t;
  struct tNode *actual = actualparameter->Expr;
  struct tTargetType **auxtype = &actualparameter->AuxType;
  struct tTargetType **actualtype = &actualparameter->ActualType;
  struct tTypeSpec **actualref = &actualparameter->ActualTypeSpec;
  struct tTypeSpec *formaltype;
  struct tVar *v;
  BIGINT minvalue, maxvalue, initialmin, initialmax;
  struct tUnitDef *unit;
  struct tTypeSpec *accessedtype;
  bool initialised, wholeobject;
  enum tBaseType basetype;
  struct tVar *basevar;
  struct tTypeSpec *reftype;
  enum tMode mode;
  struct tReference *r;
  int lock;
  bool constantaccess, subsystemboundary;

  *finalised = FALSE;

  // This has been retrieved from the name already (if necessary)
  formaltype = formal->TypeSpec;
  *actualref = formaltype;      // unless overridden below

  if (!LoopScan)
    {
      if (functioncall && (actual->Op == _VAR))
        {
          struct tVar *var;
          struct tReference *ref;

          ref = actual->Var;
          var = FindVariable (filename, line, &ref, 36, FALSE, TRUE);
          if (var->Mode == MODE_GLOBAL /*or SHARED */ && InFunction
                   && RoutinePrefixClosed)
            {
              err (filename, line,
                   "Closed functions cannot pass global variables as parameters");
            }
        }
    }

  if (actual->Op == _VAR)
    {
      r = actual->Var;
      v = FindVariable (filename, line, &r, 20, FALSE, FALSE);
      if (RoutinePrefixShared && v->ConstValue == NULL
          && (!IsNamedConstant (v)))
        {
          if (v->Mode == MODE_GLOBAL && (!CurrentExemptions.SideEffect))
            {
              // This does not apply within a shared package
              if (SharedAncestorPackage (CurrentPackage, v->EnclosingPackage)
                  == NULL)
                {
                  err (filename, line,
                       "Shared routines cannot pass non-constant globals, other than ones marked as shared, as parameters (%s)",
                       v->Name);
                }
            }
        }
    }
  else
    {
      r = NULL;
      v = NULL;
    }

  if (formal->Mode == MODE_IN)
    {
      if (!LoopScan)
        {
          BIGINT testminval, testmaxval;
          struct tExemptions SavedExemptions;

          SavedExemptions = CurrentExemptions;
          //
          if (actualparameter->ExemptionRange)
            {
              CurrentExemptions.UncheckedRange = TRUE;
            }

          TestAssignment (NULL, NULL, formaltype, actual, TRUE, FALSE,
                          (functioncall
                           || calltoclosed), &testminval, &testmaxval);

          CurrentExemptions = SavedExemptions;

          if (actual->Op == _VAR && actual->Attribute == NULL)
            {
              AnalyseReference (filename, line, actual->Var, &basetype,
                                &basevar, &reftype, &minvalue, &maxvalue,
                                &initialmin, &initialmax, &unit,
                                &accessedtype, &initialised, &mode, &lock,
                                TRUE, &wholeobject, actual->Attribute != NULL,
                                &constantaccess, FALSE, &subsystemboundary);
              CheckFormalActualAccessModes (filename, line, formal->Var,
                                            basevar);
              if (formaltype->BaseType == ARRAY_TYPE)
                {
                  if (reftype->BaseType != formaltype->BaseType)
                    {
                      err (filename, line, "Array expected");
                    }
                  CheckArrayParameter (filename, line, formaltype, reftype,
                                       actualref, formal, actual);
                  SetAccessed (basevar);
                }
              RecordGlobalAccess (line, GetGlobalsAccess (), basevar,
                                  TRUE, FALSE, FALSE, FALSE);
            }
        }
    }
  else if (formal->Mode == MODE_OUT || formal->Mode == MODE_IN_OUT
           || formal->Mode == MODE_FINAL_IN_OUT)
    {
      if (actual->Op == _VAR)
        {
          if (LoopScan)
            {
              //done above v = FindVariable(filename, line, &r, 20, FALSE, FALSE);
              // v can be NULL in a call within a declare block within a loop because the actual parameter
              // might be declared in the declare block declarations (which are not processed in a loop scan)
              // In that case it doesn't matter because the variable doesn't exist at the start of the loop
              // so doesn't need to have its range reset by the scan.
              if (v != NULL)
                {
                  // Just update the range
                  if (v->Type->TypeSpec->BaseType == INTEGER_TYPE
                      || v->Type->TypeSpec->BaseType == ACCESS_TYPE)
                    {
                      // Update the range of the variable to the full range for its type
                      if (r->Next == NULL)
                        {
                          // Simple variable
                          v->Low = v->Type->TypeSpec->Low;
                          v->High = v->Type->TypeSpec->High;
                          ResetRelations (v);
                        }
                    }
                }
            }
          else
            {
              //
              if (actual->Attribute != NULL)
                {
                  err (filename, line, "Actual parameter for a [FINAL][IN]OUT parameter must be a variable (not an attribute expression)");
                }
              //
              AnalyseReference (filename, line, actual->Var/*r*/, &basetype, &basevar,
                                &reftype, &minvalue, &maxvalue, &initialmin,
                                &initialmax, &unit, &accessedtype,
                                &initialised, &mode, &lock, TRUE,
                                &wholeobject, formal->Mode == MODE_OUT,
                                &constantaccess, FALSE, &subsystemboundary);
              CheckFormalActualAccessModes (filename, line, formal->Var,
                                            basevar);
              v = basevar;
              //
              if (v->AccessVirtual)
                {
                  err (filename, line,
                       "Variables with access_mode virtual cannot be passed as parameters");
                }
              //if (actual->Var->Next == NULL) {
              //      // Simple local var
              if (lock != 0)
                {
                  err (filename, line,
                       "Variable %s cannot be passed as an out or [final] in out parameter here due to alias lock",
                       actual->Var->VarName);
                }
              if (formal->Mode == MODE_OUT)
                RecordGlobalAccess (line, GetGlobalsAccess (), basevar,
                                    FALSE, TRUE, FALSE, FALSE);
              else
                RecordGlobalAccess (line, GetGlobalsAccess (), basevar,
                                    TRUE, TRUE, (!basevar->Initialised),
                                    FALSE);
              //
              if (InFunction || RoutinePrefixClosed)
                {
                  if ((v->Mode == MODE_GLOBAL || v->Mode == MODE_SHARED)
                      && (!CurrentExemptions.SideEffect))
                    {
                      err (filename, line,
                           "Functions and closed procedures cannot pass globals as out or [final] in out parameters");
                    }
                }
              if (NamedConstantType (v->Type->TypeSpec)
                  || v->ConstValue != NULL)
                {
                  err_and_continue (filename, line,
                                    "Constant %s cannot be passed as an out or [final] in out parameter",
                                    v->Name);
                }
              if (   (basevar->Mode == MODE_GLOBAL || basevar->Mode == MODE_SHARED)
            	  && reftype->BaseType == ACCESS_TYPE && reftype->UnmanagedAccess && (!reftype->PersistentAccess)) {
            	  err_and_continue(filename, line,
            			   "Non-persistent global unchecked access variables cannot be passed as out or [final] in out parameters");
              }
              if ((formal->Mode == MODE_IN_OUT
                   || formal->Mode == MODE_FINAL_IN_OUT) && (!initialised)
                  && (!v->AccessMappedDevice) && (!v->TestMappedDevice))
                {
                  err_and_continue (filename, line,
                                    "Actual for in out or final in out parameter has not been initialised");
                }
              if (formal->Mode == MODE_OUT && reftype->Controlled
                  && initialised)
                {
                  ControlledReinitError (filename, line);
                }
              if (formal->Mode == MODE_OUT)
                {
                  if (r->Next == NULL)
                    {
                      // This is a simple variable (not part of a record or array)
                      v->Finalised = FALSE;
                      if (r->ReferenceType != ARRAY_SLICE_REFERENCE
                          && r->ReferenceType != ARRAY_SLICE_LENGTH_REFERENCE)
                        {
                          // A slice does not count for initialisation (because in general it is not the whole object)
                          SetInitialised (v);
                          ResetRelations (v);
                        }
                    }
                }
              if (formal->Mode == MODE_FINAL_IN_OUT)
                {
                  if (reftype->Controlled)
                    {
                      // Only allow locals and final in out parameters to be finalised, otherwise double finalisation etc.
                      // could occur
                      if ((v->Mode == MODE_LOCAL
                           || v->Mode == MODE_FINAL_IN_OUT)
                          ||
                          ((v->Mode == MODE_GLOBAL || v->Mode == MODE_SHARED)
                           && InPackageFinalisation))
                        {       // Need a way to finalise globals
                          if (r->Next == NULL)
                            {
                              // Simple variable
                              v->Initialised = FALSE;
                              if (r->ReferenceType != ARRAY_SLICE_REFERENCE
                                  && r->ReferenceType !=
                                  ARRAY_SLICE_LENGTH_REFERENCE)
                                {
                                  // See above re slices
                                  SetFinalised (v);
                                }
                            }
                          *finalised = TRUE;
                        }
                      else
                        {
                          err_and_continue (filename, line, "Only locals and final in out actual parameters can be finalised (except in package finalisation)");        //**********CHANGE FOR PACKAGE VARS
                        }
                    }
                  else
                    {
                      err_and_continue (filename, line,
                                        "Only controlled variables can be passed as final in out parameters");
                    }
                }
              if ((reftype->BaseType == ADDRESS_TYPE
                   && formaltype->BaseType == ACCESS_TYPE
                   && formaltype->UnmanagedAccess)
                  || (formaltype->BaseType == ADDRESS_TYPE
                      && reftype->BaseType == ACCESS_TYPE
                      && reftype->UnmanagedAccess))
                {
                  ;             /* address matches any unmanaged access type */
                }
              else if (reftype->BaseType == formaltype->BaseType)
                {
                  if (reftype->BaseType == INTEGER_TYPE)
                    {
                      // Both integers
                      // Check for unit match
                      TestUnitMatch (formaltype->Unit, reftype->Unit, actual,
                                     FALSE);
                      if (formal->Mode == MODE_IN_OUT
                          || formal->Mode == MODE_FINAL_IN_OUT)
                        {
                          // Check range in
                          if (minvalue < formaltype->Low
                              || maxvalue > formaltype->High)
                            {
                              err (filename, line,
                                   "Range error on input: formal %s has range %"
                                   PRINTBIG "..%" PRINTBIG
                                   ", actual has range %" PRINTBIG "..%"
                                   PRINTBIG "", formal->Name, formaltype->Low,
                                   formaltype->High, minvalue, maxvalue);
                            }
                        }
                      // Check range out
                      if (formaltype->Low < reftype->Low
                          || formaltype->High > reftype->High)
                        {
                          err (filename, line,
                               "Range error on output: formal %s has range %"
                               PRINTBIG "..%" PRINTBIG ", actual has range %"
                               PRINTBIG "..%" PRINTBIG "", formal->Name,
                               formaltype->Low, formaltype->High,
                               reftype->Low, reftype->High);
                        }
                      // Update the range of the variable
                      if (r->Next == NULL /*zzz&& v->Referent == NULL */ )
                        {
                          // Simple variable
                          actualparameter->ActualVar = v;
                          actualparameter->ResetRange = TRUE;
                        }
                      // Check representations
                      if (formaltype->CType != reftype->CType)
                        {
                          *auxtype = formaltype->CType;
                          *actualtype = reftype->CType;
                        }
                    }
                  else if (formaltype->BaseType == ARRAY_TYPE)
                    {
                      CheckArrayParameter (filename, line, formaltype,
                                           reftype, actualref, formal,
                                           actual);
                    }
                  else if (formaltype->BaseType == ACCESS_TYPE)
                    {
                      CheckAccessParameter (filename, line, formaltype,
                                            reftype, formal);
                    }
                  else if (formaltype != reftype)
                    {
                      err (filename, line, "Type mismatch on formal %s",
                           formal->Name);
                    }
                }
              else
                {
                  err (filename, line,
                       "Base type mismatch on formal %s: formal is %s, actual is %s",
                       formal->Name, BaseTypeString[formaltype->BaseType], BaseTypeString[reftype->BaseType]);
                }
            }
        }
      else
        {
          err (filename, line,
               "Variable required for actual for out or [final] in out parameter");
        }
    }
}

void
ShowNamedUnnamedError (char *filename, int line, char *procname)
{
  err (filename, line,
       "In call to %s: unnamed parameters cannot be mixed with named parameters",
       procname);
}

void
MatchActualsToFormals (char *filename, int line,
                       struct tProcedureCall *p, struct tTypeSpec *reftype,
                       char *procname, bool functioncall,
                       bool * FinalisedThis, bool * mismatch,
                       bool * sharedcall, bool calltosharedpackage,
                       struct tVar *packagelockvar)
{
  int anum, fnum;
  struct tFormalParameter *formals;
  struct tActualParameter *actuals;
  int numformals, numactuals;
  bool finalised;
  bool matched, namedparams, unnamedparams;
  struct tShareLockElt *shares;

  actuals = p->ActualList;
  numactuals = 0;
  while (actuals != NULL)
    {
      numactuals++;
      if (!LoopScan)
        {
          struct tVar *varlist[MAX_VARS_IN_EXPR + 1];
          ScanForVars(filename, line, actuals->Expr, varlist);
          struct tVar **vp = varlist;
          struct tVar *var;
          while (*vp != NULL)
            {
              var = *vp;

              if (var->Mode == MODE_SHARED && (!var->ShareLock))
                {
                  struct tGlobalAccess *g;

                  if (functioncall)
                    {
                      err(filename, line, "Shared variables cannot be passed to functions unless already locked (%s)", (*vp)->Name);
                      // Otherwise it would be difficult to handle expressions in general e.g. if fn(a) or fn(b) then
                    }
                  *sharedcall = TRUE;
                  // Update the shared variable's subsequent share list
                  g = reftype->GlobalsAccess;
                  while (g != NULL)
                    {
                      if (g->Lock)
                        {
                          RecordLock (line, &var->ShareList, g->Var);
                        }
                      g = g->Next;
                    }
                  RecordLock (line, GetGlobalsAccess (), var);
                }
              vp++;
            }
        }
      actuals = actuals->Next;
    }
  // Record lock => var share locks
  // Check for lock => var share locks
  shares = reftype->ShareLockList;
  while (shares != NULL)
    {
      struct tGlobalAccess *g;

      if (!shares->Var->ShareLock)
        {
          *sharedcall = TRUE;
          g = reftype->GlobalsAccess;
          while (g != NULL)
            {
              if (g->Var->Mode == MODE_SHARED && g->Lock)
                {
                  RecordLock (line, &shares->Var->ShareList, g->Var);
                  //doeslock = TRUE;
                }
              g = g->Next;
            }
          RecordLock (line, GetGlobalsAccess (), shares->Var);
        }
      shares = shares->Next;
    }
  // If this is a call into a shared package, then record the lock as if for an additional parameter
  if (calltosharedpackage)
    {
      struct tGlobalAccess *g;

      g = reftype->GlobalsAccess;
      while (g != NULL)
        {
          if (g->Var->Mode == MODE_SHARED && g->Lock)
            {
              RecordInheritedGlobalAccess (line, &packagelockvar->ShareList, g->Var,
                                           TRUE, TRUE, FALSE, FALSE);
            }
          g = g->Next;
        }
      RecordLock (line, GetGlobalsAccess (), packagelockvar);
    }
  formals = reftype->Formals;
  numformals = 0;
  while (formals != NULL)
    {
      numformals++;
      formals = formals->Next;
    }
  //
  if (numactuals != numformals)
    *mismatch = TRUE;
  else
    {
      // Allocate map for formals -> actuals by position
      p->PositionNum = checked_malloc (numformals * sizeof (int));
      p->NumFormals = numformals;
      p->Formals = reftype->Formals;
      // Go through the parameters in order of formals in order to
      // match call-by-position
      formals = reftype->Formals;
      actuals = p->ActualList;
      anum = 0;
      namedparams = FALSE;
      unnamedparams = FALSE;
      fnum = 0;
      while (formals != NULL)
        {
          matched = FALSE;
          while (actuals != NULL)
            {
              if (actuals->Name == NULL)
                {
                  unnamedparams = TRUE;
                  if (reftype->NamedParametersRequired
                      && (!CurrentExemptions.AmbiguousOrder))
                    {
                      matched = FALSE;
                      err (filename, line,
                           "Named parameters are required for calls to %s",
                           procname);
                    }
                  else if (namedparams)
                    {
                      matched = FALSE;
                      ShowNamedUnnamedError (filename, line, procname);
                    }
                  else
                    {
                      matched = TRUE;
                    }
                  p->PositionNum[fnum] = anum;
                  break;
                }
              else
                {
                  namedparams = TRUE;
                  if (unnamedparams)
                    {
                      matched = FALSE;
                      ShowNamedUnnamedError (filename, line, procname);
                    }
                  if (!strcmp (actuals->Name, formals->Name))
                    {
                      matched = TRUE;
                      p->PositionNum[fnum] = anum;
                      break;
                    }
                }
              actuals = actuals->Next;
              anum++;
            }
          if (matched)
            {
              // If the mode of the actual parameter was specified, then it must be the same as the mode
              // of the formal
              if (actuals->Mode != MODE_UNSPECIFIED)
                {
                  if (actuals->Mode != formals->Mode)
                    {
                      err (filename, line,
                           "Nominated actual mode does not match mode for formal %s",
                           formals->Name);
                    }
                }
              else
                {
                  // Record the mode for use in subsequent alias testing
                  actuals->Mode = formals->Mode;
                }
              //
              MatchParameter (filename, line, formals, actuals, &finalised,
                              functioncall, reftype->PrefixClosed);
              if (actuals->IsThis && finalised)
                *FinalisedThis = TRUE;
            }
          else
            {
              *mismatch = TRUE;
              break;
            }
          formals = formals->Next;
          fnum++;
          if (namedparams)
            {
              actuals = p->ActualList;  // Set up for next time round
              anum = 0;
            }
          else
            {
              actuals = actuals->Next;
              anum++;
            }
        }
    }
}

void
ResetWrittenGlobals (struct tGlobalAccess *globals)
{
  struct tGlobalAccess *g;

  g = globals;
  while (g != NULL)
    {
      if (g->Write)
        {
          ResetVar (g->Var, FALSE);
        }
      g = g->Next;
    }
}

void
CheckAccessOfGlobals (struct tGlobalAccess *globals)
{
  struct tGlobalAccess *g;

  g = globals;
  while (g != NULL)
    {
      if (g->Initialised)
        {
          SetInitialised (g->Var);
        }
      if (g->Read)
        {
          SetAccessed (g->Var);
        }
      g = g->Next;
    }
}

void
ResetOutputParameterRanges (int line, struct tProcedureCall *p)
{
  int j, k;
  struct tFormalParameter *formal;
  struct tActualParameter *apos;

  formal = p->Formals;
  for (j = 0; j < p->NumFormals; j++)
    {
      apos = p->ActualList;
      for (k = p->PositionNum[j]; k > 0; k--)
        {
          apos = apos->Next;
        }
      if (apos->ResetRange)
        {
          apos->ActualVar->Low = formal->TypeSpec->Low;
          apos->ActualVar->High = formal->TypeSpec->High;
          if (formal->Mode != MODE_FINAL_IN_OUT && (!LoopScan))
            {
              SetInitialised (apos->ActualVar);
            }
          ResetRelations (apos->ActualVar);
        }
      formal = formal->Next;
    }
}

void
AnalyseProcedureCall (char *filename, int line,
                      struct tProcedureCall *p, struct tTypeSpec **ReturnType,
                      bool * FinalisedThis, bool * Success, bool functioncall,
                      bool closedcall, struct tShareClause **shareclause)
{
  enum tBaseType basetype;
  struct tVar *basevar;
  struct tTypeSpec *reftype;
  BIGINT minvalue, maxvalue, initialmin, initialmax;
  struct tUnitDef *unit;
  struct tTypeSpec *accessedtype;
  bool mismatch;
  enum tMode mode;
  int lock;
  bool sharedcall;
  bool initialised, wholeobject;
  bool calltosharedpackage, constantaccess, subsystemboundary;
  jmp_buf precond_error_recovery;

  *FinalisedThis = FALSE;
  *Success = TRUE;
  AnalyseReference (filename, line, p->Name, &basetype, &basevar, &reftype,
                    &minvalue, &maxvalue, &initialmin, &initialmax, &unit,
                    &accessedtype, &initialised, &mode, &lock, FALSE,
                    &wholeobject, FALSE, &constantaccess, FALSE, &subsystemboundary);
  //
  if (VerboseBuild)
    {
      printf ("-- Analysing call to %s.%s", basevar->EnclosingPackage->Name,
              basevar->Name);
      if (CurrentProc != NULL)
        printf (" from %s", CurrentProc->Name);
      printf ("\n");
    }

  // Determine if this is a call to an interface routine of a shared package from
  // outside that package
  if (basevar->Access == PUBLIC_ACCESS && basevar->EnclosingPackage->Shared     // must be the base level package - shared packages can't export subpackages
      && basevar->EnclosingPackage != CurrentPackage)
    {
      calltosharedpackage = TRUE;
    }
  else
    {
      calltosharedpackage = FALSE;
    }
  //
  // Record the fact that the current procedure calls the named one
  if (basetype == PROCEDURE_TYPE)
    {
      struct tCallEntry *p, *pp;
      bool found;

      if (CurrentProc == NULL)
        {
          p = InitialisationCallList;
        }
      else
        {
          p = CurrentProc->CallList;
        }
      pp = NULL;
      found = FALSE;
      while (p != NULL)
        {
          if (p->Routine == basevar)
            {
              found = TRUE;
              break;
            }
          pp = p;
          p = p->Next;
        }
      if (!found)
        {
          struct tCallEntry *newentry =
            checked_malloc (sizeof *newentry);

          newentry->Routine = basevar;
          newentry->Next = NULL;
          if (pp == NULL)
            {
              if (CurrentProc == NULL)
                {
                  InitialisationCallList = newentry;
                }
              else
                {
                  CurrentProc->CallList = newentry;
                }
            }
          else
            {
              pp->Next = newentry;
            }
        }
    }
  *shareclause = reftype->ShareClause;
  if (basetype != PROCEDURE_TYPE)
    {
      *ReturnType = NULL;
      *Success = FALSE;
      err (filename, line, "Cannot call a non-procedure");
    }
  else if (SharedAncestorPackage (basevar->EnclosingPackage, CurrentPackage)
           != NULL && reftype->ShareClause->Expr != NULL)
    {
      err (filename, line,
           "A routine in a shared package cannot call a guarded routine in the same package");
    }
  else if (closedcall && (!reftype->PrefixClosed)
           && (!CurrentExemptions.SideEffect))
    {
      *ReturnType = NULL;
      *Success = FALSE;
      err (filename, line, "The procedure called here must be closed");
    }
  else
    {
      if (InFunction && RoutinePrefixClosed
          && reftype->ReturnType == NULL)
        {
          *Success = FALSE;
          err (filename, line, "Closed functions cannot call procedures");
        }
      else if (InFunction && RoutinePrefixClosed
               && reftype->ReturnType != NULL
               && (!reftype->PrefixClosed))
        {
          *Success = FALSE;
          err (filename, line,
               "Closed functions cannot call functions that are not themselves closed");
        }
      else if ((InFunction || RoutinePrefixClosed)
               && (!reftype->PrefixClosed)
               && reftype->ReturnType == NULL
               && (!CurrentExemptions.SideEffect)
               && (!InUnitTestCode()))
        {
          *Success = FALSE;
          err (filename, line,
               "Functions, closed procedures and initialisers cannot call non-closed procedures");
        }
      else if (RoutinePrefixShared
               &&
               (!(reftype->PrefixShared
                  || (reftype->PrefixClosed
                      && reftype->ReturnType != NULL)))
               && (!CurrentExemptions.SideEffect))
        {
          *Success = FALSE;
          err (filename, line,
               "Shared routines can only call shared routines and closed functions");
        }

      //
      // If the called routine dispatches, then mark the caller similarly
      //
      if (reftype->Dispatches && CurrentProc != NULL)
        {
          CurrentProc->Dispatches = TRUE;
        }

      // Check that globals that the called procedure requires to have been
      // initialised have been initialised. If not, add them to the current procedure's
      // required initialisation list.
      {
        struct tGlobalAccess *g;

        g = basevar->Type->TypeSpec->GlobalsAccess;
        while (g != NULL)
          {
            if (g->RequireInitialisation)
              {
                if (!g->Var->Initialised)
                  {
                    RecordInheritedGlobalAccess (g->Line, GetGlobalsAccess (), g->Var,
                                                 FALSE, FALSE, TRUE, FALSE);
                  }
              }
            g = g->Next;
          }
      }
      //
      *ReturnType = reftype->ReturnType;
      //----------
      sharedcall = FALSE;
      mismatch = FALSE;
      if (p->ActualList == NULL)
        {
          if (reftype->Formals != NULL)
            mismatch = TRUE;
        }
      if (!mismatch)
        {
          // Always do this, even if not parameters, to handle other shares
          MatchActualsToFormals (filename, line, p, reftype, basevar->Name,
                                 functioncall, FinalisedThis, &mismatch,
                                 &sharedcall, calltosharedpackage,
                                 basevar->EnclosingPackage->LockVar);
        }
      if (mismatch)
        {
          *Success = FALSE;
          err (filename, line, "Parameter list mismatch");
        }
      //
      // Check for parameter aliasing
      struct tActualParameter *ap, *np;
      struct tReference *ar, *nr;
      struct tVar *av, *nv;
      struct tGlobalAccess *g;
      int j;

      ap = p->ActualList;
      while (ap != NULL)
        {
          if (ap->Expr->Op == _VAR && (!ap->ExemptionAliasing))
            {
              ar = ap->Expr->Var;
              av = FindVariable (filename, line, &ar, 100, FALSE, TRUE);
              np = ap->Next;
              while (np != NULL)
                {
                  if (np->Expr->Op == _VAR)
                    {
                      nr = np->Expr->Var;
                      nv =
                        FindVariable (filename, line, &nr, 100, FALSE, TRUE);
                      if (av == nv && ap->Expr->Attribute == NULL && np->Expr->Attribute == NULL)
                        {
                          enum tBaseType basetype =
                            av->Type->TypeSpec->BaseType;
                          bool aliased = TRUE;

                          if (ar->Next != NULL && nr->Next != NULL)
                            {
                              if (basetype == RECORD_TYPE)
                                {
                                  if (ar->Next->ReferenceType ==
                                      FIELD_REFERENCE
                                      && nr->Next->ReferenceType ==
                                      FIELD_REFERENCE)
                                    {
                                      if (strcmp
                                          (ar->Next->VarName,
                                           nr->Next->VarName))
                                        {
                                          aliased = FALSE;
                                        }
                                    }
                                }
                              else if (basetype == ARRAY_TYPE)
                                {
                                  aliased = FALSE;      //*****=====
                                }
                            }
                          if (aliased)
                            {
                              *Success = FALSE;
                              errstart (filename, line);
                              errcont ("Aliased parameters ");
                              errwritevariable (ap->Expr->Var);
                              errend ();
                            }
                        }
                    }
                  np = np->Next;
                }
              //
              g = basevar->Type->TypeSpec->GlobalsAccess;
              while (g != NULL)
                {
                  if (av == g->Var
                      && (av->Mode == MODE_SHARED || g->Write
                          || (ap->Mode != MODE_IN)))
                    {
                      errstart (filename, line);
                      errcont ("Variable ");
                      errwritevariable (ap->Expr->Var);
                      errcont
                        (" is indirectly aliased by global access at line %d",
                         g->Line);
                      if (av->Mode == MODE_SHARED)
                        {
                          errcont (" (with potential for self deadlock)");
                        }
                      errend ();
                    }
                  g = g->Next;
                }
            }
          //
          ap = ap->Next;
        }
      j = 0;
      while (j < CurrentVars->NumVars)
        {
          av = CurrentVars->Vars[j];
          if (av->Referent != NULL && (av->Mode == MODE_LOCAL)
              && (!av->ExemptionAliasing))
            {
              g = basevar->Type->TypeSpec->GlobalsAccess;
              while (g != NULL)
                {
                  if ((av->Referent == g->Var) && g->Write)
                    {
                      errstart (filename, line);
                      errcont ("Name %s declared at line %d", av->Name,
                               av->Line);
                      errcont (" is indirectly aliased by update at line %d",
                               g->Line);
                      errend ();
                    }
                  g = g->Next;
                }
            }
          j++;
        }

      if (basevar->Type->TypeSpec->Precondition != NULL
          || basevar->Type->TypeSpec->Postcondition != NULL)
        {
          // The called routine has a precondition or a postcondition
          //

          //PROBLEMS: MatchParameter resets ranges for out and in out parameters before this
          //        Must record possible relations for in parameters

          // Analyse actual expressions
          struct tFormalParameter *formal;
          struct tActualParameter *apos;
          int j, k;
          int SavedNumVars;
          BIGINT minvalue, maxvalue;
          struct tVars *savedvars = MallocVarSet ();

          SavedNumVars = CurrentVars->NumVars;
          formal = p->Formals;
          for (j = 0; j < p->NumFormals; j++)
            {
              apos = p->ActualList;
              for (k = p->PositionNum[j]; k > 0; k--)
                {
                  apos = apos->Next;
                }
              // Bind the actual to the formal
              if (ByReference (formal) && apos->Expr->Op == _VAR)
                {
                  struct tVar *var;
                  struct tReference *ref;

                  ref = apos->Expr->Var;
                  var = FindVariable (filename, line, &ref, 600, FALSE, TRUE);
                  CreateWholeVarAlias (filename, line, formal->Name, var);
                }
              else
                {
                  struct tVar *var;
                  struct tReference *ref;
                  struct tType *vartype;
                  BIGINT testminval, testmaxval;

                  ref = MakeNewReference ();
                  CheckForVarOverflow (filename, line);
                  var = MallocVar (filename, line);
                  var->Name = formal->Name;
                  vartype = AllocateTType ();
                  vartype->Name = formal->TypeSpec->Name;
                  vartype->NameAccess = PRIVATE_ACCESS;
                  vartype->TypeSpec = formal->TypeSpec;
                  vartype->DeferredTypespecs = NULL;
                  vartype->Next = NULL;
                  var->Type = vartype;
                  CurrentVars->Vars[CurrentVars->NumVars] = var;
                  CurrentVars->NumVars++;
                  ref->VarName = formal->Name;
                  ref->ReferenceAbbreviation = NULL;
                  ref->ReferenceType = IDENTIFIER_REFERENCE;
                  ref->Next = NULL;
                  TestAssignment (ref, var, formal->TypeSpec, apos->Expr,
                                  FALSE, FALSE, FALSE, &testminval,
                                  &testmaxval);
                }
              formal = formal->Next;
            }
          // Now test the precondition and assert the postcondition
          // This must be done in the scope environment of the called routine, not that of the caller
          CopyVarSet (CurrentVars, savedvars);

          memcpy (precond_error_recovery, error_recovery, sizeof (jmp_buf));
          if (setjmp (error_recovery) == 0)
            {
              // Restore the scope environment of the called routine
              CopyVarSet (basevar->Type->TypeSpec->Scope, CurrentVars);
              // But keep the current relations
              {
                int j;
                for (j = 0; j < savedvars->NumRelations; j++)
                  CurrentVars->Relations[j] = savedvars->Relations[j];
                CurrentVars->NumRelations = savedvars->NumRelations;
              }

              // Copy the formals that were bound above into the restored scope
              for (j = SavedNumVars; j < savedvars->NumVars; j++)
                {
                  CurrentVars->Vars[CurrentVars->NumVars] =
                    savedvars->Vars[j];
                  CurrentVars->NumVars++;
                }
              // Set actual initial values for the call, for use in the postcondition
              SetInitialValues ();
              //
              // Test the precondition
              if (basevar->Type->TypeSpec->Precondition != NULL)
                {
                  bool virtualexpr;
                  // Test as for a parameter so that the types can be seen
                  TestBooleanExpression (basevar->Type->
                                         TypeSpec->Precondition, TRUE,
                                         &minvalue, &maxvalue, FALSE,
                                         &virtualexpr);
                  if (minvalue == 1)
                    {
                      if (Annotate)
                        {
                          emit (0, "/* Precondition test line %d confirmed */\n", line);        //**********FIX FORMATTING
                        }
                    }
                  else if (maxvalue == 0)
                    {
                      if (Annotate)
                        {
                          emit (0,
                                "/* Precondition test at line %d is false */\n",
                                line);
                        }
                      err (filename, line,
                           "The precondition for the call to '%s' is false",
                           basevar->Name);
                    }
                  else
                    {
                      if (Annotate)
                        {
                          emit (0,
                                "/* Precondition test at line %d could not be confirmed */\n",
                                line);
                        }
                      err_and_continue (filename, line,
                                        "The precondition for the call to '%s' could not be confirmed:",
                                        basevar->Name);
                      TestBooleanExpression (basevar->Type->
                                             TypeSpec->Precondition, TRUE,
                                             &minvalue, &maxvalue, TRUE,
                                             &virtualexpr);
                    }
                }
              //
              ResetOutputParameterRanges (1, p);
              //
              // Reset the ranges of any globals that the called routine writes before
              // asserting the postcondition
              ResetWrittenGlobals (basevar->Type->TypeSpec->GlobalsAccess);
              //
              // Assert the postcondition, if any
              if (basevar->Type->TypeSpec->Postcondition != NULL)
                {
                  bool success;
                  struct tGlobalAccess *g;

                  // Assert that any globals that the called routine initialises are initialised before asserting
                  // the postcondition
                  g = basevar->Type->TypeSpec->GlobalsAccess;
                  while (g != NULL)
                    {
                      if (g->Initialised)
                        {
                          g->Var->Initialised = TRUE;
                        }
                      g = g->Next;
                    }
                  //
                  success = TRUE;
                  UpdateRanges (basevar->Type->TypeSpec->Postcondition, FALSE,
                                &success, FALSE, TRUE);
                  if (!success)
                    {
                      err (filename, line,
                           "The postcondition could not be analysed at the point of call");
                    }
                }
              //
              CopyVarSet (savedvars, CurrentVars);
              FreeVarSet (savedvars);
              //
              memcpy (error_recovery, precond_error_recovery,
                      sizeof (jmp_buf));
            }
          else
            {
              CopyVarSet (savedvars, CurrentVars);
              FreeVarSet (savedvars);
              memcpy (error_recovery, precond_error_recovery,
                      sizeof (jmp_buf));
              err (filename, line, "Error raised when analysing call to '%s'",
                   basevar->Name);
            }
          CurrentVars->NumVars = SavedNumVars;
        }
      else
        {
          ResetOutputParameterRanges (line, p);
          // No preconditions or postconditions; reset global ranges here because it was not done above
          ResetWrittenGlobals (basevar->Type->TypeSpec->GlobalsAccess);
        }

      if (reftype->ShareClause->Expr != NULL
          && (!sharedcall)
          && SharedAncestorPackage (basevar->EnclosingPackage,
                                    basevar->EnclosingPackage) == NULL)
        {
          // If the called procedure contains a 'when expr' clause then
          // at least one of the actual parameters must be shared
          *Success = FALSE;
          err (filename, line,
               "A call to a procedure that has a 'when' clause must include at least one shared parameter, or the procedure must be exported from a shared package");
        }
      // Add the called routine's global accesses to the current procedure's
      {
        struct tGlobalAccess *g;

        g = basevar->Type->TypeSpec->GlobalsAccess;
        while (g != NULL)
          {
            if (g->Read)
              {
                SetAccessed (g->Var);
              }
            if (g->Initialised)
              {
                // The called routine initialised this variable
                SetInitialised (g->Var);
                ResetRelations (g->Var);        //**********
              }
            RecordInheritedGlobalAccess (g->Line, GetGlobalsAccess (), g->Var,
                                         g->Read, g->Write, FALSE, FALSE);
            //
            if (g->Lock)
              {
                RecordLock(g->Line, GetGlobalsAccess (), g->Var);
              }
            //
            g = g->Next;
          }
      }
      // If this is a call to a public routine in a shared package from outside the package, then
      // record the acquisition of the lock on the package as a global access
      if (calltosharedpackage)
        {
          // Update the procedure's access list
          RecordGlobalAccess (line, GetGlobalsAccess (),
                              basevar->EnclosingPackage->LockVar, TRUE, TRUE,
                              FALSE, FALSE);
        }
      //
      // If the called procedure can restart, then so can the current procedure
      if (reftype->Restarts && (CurrentProc != NULL))
        {
          CurrentProc->Restarts = TRUE;
        }
    }
}

/* Add a target type */

void
CheckNumTargetTypes (void)
{
  if (NumTargetTypes >= MAX_TARGET_TYPES)
    {
      err ("", 0, "Too many target types (max %d)", MAX_TARGET_TYPES);
    }
}

struct tTargetType *
AddTargetIntegerType (char *cname, BIGINT min, BIGINT max,
                      bool useforarithmetic, bool preferred,
                      char *representationname, BIGINT sizeinbits,
                      char *formatstring, bool atomictype)
{
  struct tTargetType *t;

  CheckNumTargetTypes ();
  t = &TargetTypes[NumTargetTypes];
  t->BaseType = TARGET_INTEGER;
  t->CName = cname;
  t->Min = min;
  t->Max = max;
  t->UseForArithmetic = useforarithmetic;
  t->Preferred = preferred;
  t->RepresentationName = representationname;
  t->SizeInBits = (unsigned) sizeinbits;
  t->PrintfFormat = formatstring;
  t->AtomicType = atomictype;
  NumTargetTypes++;
  return t;
}

struct tTargetType *
AddTargetFloatType (char *cname, BIGINT digits, BIGINT magnitude,
                    bool useforarithmetic, char *representationname,
                    BIGINT sizeinbits, char *formatstring)
{
  struct tTargetType *t;

  CheckNumTargetTypes ();
  t = &TargetTypes[NumTargetTypes];
  t->BaseType = TARGET_FLOAT;
  t->CName = cname;
  t->Digits = digits;
  t->Magnitude = magnitude;
  t->UseForArithmetic = useforarithmetic;
  t->RepresentationName = representationname;
  t->SizeInBits = (unsigned) sizeinbits;
  t->PrintfFormat = formatstring;
  t->AtomicType = FALSE;
  NumTargetTypes++;
  return t;
}

void
PragmaSyntaxError (char *filename, int line)
{
  err (filename, line, "Syntax error in pragma");
}

void
DisplayVar (struct tVar *v, int depth)
{
  char *s;
  struct tVar *pv;

  switch (v->Access)
    {
    case PRIVATE_ACCESS:
      s = "private";
      break;
    case PUBLIC_ACCESS:
      s = "public";
      break;
    case HIDDEN_ACCESS:
      s = "hidden";
      break;
    default:
      s = "???";
      break;
    }
  emit (0, "/* ");
  emit (depth, "%s %s: ", s, v->Name);
  emit (0, "%s, line %d", v->Type->Name, v->Line);
  if (IsVariable (v))
    {
      emit (0, " init %d fin %d", v->Initialised, v->Finalised);
      if (NamedConstantType (v->Type->TypeSpec))
        {
          emit (0, " cunit ");
          EmitUnit (v->ConstUnit);
        }
      else
        {
          emit (0, " unit ");
          EmitUnit (v->Type->TypeSpec->Unit);
        }
    }
  else if (v->Type->TypeSpec->BaseType == PROCEDURE_TYPE)
    {
      emit (0, " proc shared=%d closed=%d", v->Type->TypeSpec->PrefixShared, v->Type->TypeSpec->PrefixClosed);
    }
  if (v->Type->TypeSpec->BaseType == UNIT_TYPE)
    {
      emit (0, " derived ");
      EmitUnit (v->Unit);
    }
  emit (0, " */\n");
  if (v->Type->TypeSpec->BaseType == PACKAGE_TYPE)
    {
      pv = v->Type->TypeSpec->PackageVars;
      while (pv != NULL)
        {
          DisplayVar (pv, depth + 1);
          pv = pv->Next;
        }
    }

}

char *
PragmaString (char *filename, int line, struct tActualParameter *a)
{
  if (a == NULL)
    {
      PragmaSyntaxError (filename, line);
    }
  else if (a->Expr->Op != _CONST_STRING)
    {
      PragmaSyntaxError (filename, line);
    }
  return DeQuote (a->Expr->String);
}

char *
PragmaSymbol (char *filename, int line, struct tActualParameter *a)
{
  if (a == NULL)
    {
      PragmaSyntaxError (filename, line);
    }
  else if (a->Expr->Op != _VAR)
    {
      PragmaSyntaxError (filename, line);
    }
  else if (a->Expr->Var->Next != NULL)
    {
      PragmaSyntaxError (filename, line);
    }
  return a->Expr->Var->VarName;
}

void
PragmaCheckNumargs (char *filename, int line, struct tActualParameter *params,
                    int numargs)
{
  int n;
  struct tActualParameter *a;

  a = params;
  n = 0;
  while (a != NULL)
    {
      n++;
      a = a->Next;
    }
  if (n != numargs)
    {
      err(filename, line, "Wrong number of arguments in pragma (expected %d)", numargs);
    }
}

struct tActualParameter *
PragmaFindArg (char *filename, int line, struct tActualParameter *params,
               char *name)
{
  while (params != NULL)
    {
      if (name != NULL && (!strcasecmp (params->Name, name)))
        {
          return params;
        }
      params = params->Next;
    }
  err(filename, line, "Missing argument '%s' in pragma", name);
  return NULL;                  // for the sake of -Wall
}

void
SetInlinePrefix (char *filename, int line, struct tActualParameter *a,
                 bool prefixline)
{
  if (a == NULL)
    PragmaSyntaxError (filename, line);
  if (a->Next != NULL)
    PragmaSyntaxError (filename, line);
  if (InlinePrefixSet)
    {
      err (filename, line, "The inline prefix has already been set to '%s'",
           InlinePrefix);
    }
  InlinePrefix = PragmaString (filename, line, a);
  InlinePrefixLine = prefixline;
  InlinePrefixSet = TRUE;
}

void
SetupIntegerLikeType (char *filename, int line, char *inttypename,
                      struct tTypeSpec *desttype,
                      BIGINT minrequired, BIGINT maxrequired,
                      struct tVar *destvar)
{
  struct tTargetType *inttype;

  if (desttype->CType != NULL)
    {
      err (filename, line, "The target '%s' type can only be specified once",
           desttype->Name);
    }
  inttype = GetTargetTypeByName (INTEGER_TYPE, inttypename);
  if (inttype == NULL)
    {
      err (filename, line, "Unknown target integer type iname '%s'",
           inttypename);
    }
  if (inttype->Min <= minrequired && inttype->Max >= maxrequired)
    {
      desttype->Low = minrequired;
      desttype->High = maxrequired;
      desttype->CType = inttype;
      desttype->CName = inttype->CName;
      destvar->CName = inttype->CName;
    }
  else
    {
      err (filename, line, "Integer type %s cannot represent %s", inttypename,
           desttype->Name);
    }
}

void
DescribeLocals (int depth, char *filename, int line)
{
  int j;
  char *modestr, *basetypestr, *namesstr;
  struct tVar *v;
  struct tRelation *r;

  emit (depth, "/* ----- locals at line %d of %s -----\n", line, filename);
  for (j = FirstLocalVar; j < CurrentVars->NumVars; j++)
    {
      v = CurrentVars->Vars[j];
      while (v->Referent != NULL && v->WholeObjectReference)
        {
          emit (depth, " * %s -> %s\n", v->Name, v->Referent->Name);
          v = v->Referent;
        }
      if (v->Mode != MODE_GLOBAL && v->Mode != MODE_SHARED)
        {
          switch (v->Mode)
            {
            case MODE_IN:
              modestr = "in";
              break;
            case MODE_OUT:
              modestr = "out";
              break;
            case MODE_IN_OUT:
              modestr = "in out";
              break;
            case MODE_FINAL_IN_OUT:
              modestr = "final in out";
              break;
            case MODE_LOCAL:
              modestr = "local";
              break;
            default:
              modestr = "???";
              break;
            }
          if (v->Referent != NULL)
            {
              namesstr = v->Referent->Name;
              modestr = "name";
            }
          else
            namesstr = NULL;
          switch (v->Type->TypeSpec->BaseType)
            {
            case INTEGER_TYPE:
              basetypestr = "INTEGER";
              break;
            case FLOATING_TYPE:
              basetypestr = "FLOAT";
              break;
            case BOOLEAN_TYPE:
              basetypestr = "BOOLEAN";
              break;
            case ARRAY_TYPE:
              basetypestr = "ARRAY";
              break;
            case RECORD_TYPE:
              basetypestr = "RECORD";
              break;
            case TYPE_TYPE:
              basetypestr = "TYPE";
              break;
            case CSTRING_TYPE:
              basetypestr = "STRING";
              break;
            case ACCESS_TYPE:
              basetypestr = "ACCESS";
              break;
            case ADDRESS_TYPE:
              basetypestr = "ADDRESS";
              break;
            default:
              basetypestr = "???";
              break;
            }
          emit (depth,
                " * %s %s : %s %s; cname %s; ctype %s; [%" PRINTBIG "..%"
                PRINTBIG "]\n", modestr, v->Name, basetypestr, v->Type->Name,
                v->CName, v->Type->TypeSpec->CName, v->Low, v->High);
          emit (depth, " *   init %d fin %d lock %d contr %d acc %d everacc %d\n",
                v->Initialised, v->Finalised, v->Lock,
                v->Type->TypeSpec->Controlled, v->Accessed, v->EverAccessed);
          if (namesstr != NULL)
            {
              emit (depth, " *   name for %s (or a component of it)\n",
                    namesstr);
            }
          if (v->Type->TypeSpec->BaseType == ARRAY_TYPE)
            {
              emit (depth,
                    " *   min %d [%" PRINTBIG "..%" PRINTBIG "] max %d [%"
                    PRINTBIG "..%" PRINTBIG "]\n", v->MinDefinite,
                    v->FirstLow, v->FirstHigh, v->MaxDefinite, v->LastLow,
                    v->LastHigh);
            }
        }
    }
  emit (depth, " * ----- relations\n");
  for (j = 0; j < CurrentVars->NumRelations; j++)
    {
      r = &CurrentVars->Relations[j];
      if (r->LowSet || r->HighSet)
        {
          emit (depth, " * %s", r->LHS->Name);
          if (r->LHSAttr != ATTR_NULL)
            emit (0, "'%s", AttributeNames[r->LHSAttr]);
          emit (0, " - %s", r->RHS->Name);
          if (r->RHSAttr != ATTR_NULL)
            emit (0, "'%s", AttributeNames[r->RHSAttr]);
          emit (0, " : [");
          if (r->LowSet)
            emit (0, "%" PRINTBIG "", r->Low);
          else
            emit (0, "*");
          emit (0, "..");
          if (r->HighSet)
            emit (0, "%" PRINTBIG "", r->High);
          else
            emit (0, "*");
          emit (0, "]\n");
        }
    }
  emit (depth, "  ----------------------------------- */\n");
}

void PerformTestAssertion(int depth, char *filename, int line,
                          struct tActualParameter *ActualList)
{
  int num;
  bool bf;
  BIGINT minbool, maxbool;
  bool virtualexpr;
  struct tActualParameter *a;

  if (CompilerTest)
    {
      emit (depth, "/* Runtime check from pragma */\n");
      a = ActualList;
      num = 1;
      while (a != NULL)
        {
          TestBooleanExpression (a->Expr, FALSE, &minbool, &maxbool,
                                 FALSE, &virtualexpr);
          emit (depth, "if (!(");
          bf = FALSE;
          printexpr (a->Expr, &bf, TRUE);
          emit (0, ")) __test_assertion_failure(%d, %d);\n", line,
                num);
          a = a->Next;
          num++;
        }
    }
}

void
AnalyseAssert (int depth, char *filename, int line,
                           struct tActualParameter *ActualList)
{
  int count;
  bool confirmed, disproved, virtualexpr;
  struct tActualParameter *a;
  BIGINT minvalue, maxvalue;

  confirmed = TRUE;
  disproved = FALSE;
  count = 0;
  a = ActualList;
  while (a != NULL)
    {
      count++;
      TestBooleanExpression (a->Expr, FALSE, &minvalue, &maxvalue,
                             FALSE, &virtualexpr);
      if (minvalue == 1)
        {
          if (Annotate)
            {
              emit (depth,
                    "/* Boolean test %d in assert statement at line %d confirmed */\n",
                    count, line);
            }
        }
      else if (maxvalue == 0)
        {
          if (Annotate)
            {
              emit (depth,
                    "/* Boolean test %d in assert statement at line %d is false */\n",
                    count, line);
            }
          confirmed = FALSE;
          disproved = TRUE;
        }
      else
        {
          if (Annotate)
            {
              emit (depth,
                    "/* Boolean test %d in assert statement at line %d could not be confirmed */\n",
                    count, line);
            }
          confirmed = FALSE;
        }
      a = a->Next;
    }
  if (disproved)
    {
      err_and_continue (filename, line,
                        "Test in assert statement is disproved");
    }
  else if (!confirmed)
    {
      err_and_continue (filename, line,
                        "Test in assert statement could not be proved");
    }
  //
  if (disproved || (!confirmed))
    {
      // Display the analysis
      a = ActualList;
      while (a != NULL)
        {
          TestBooleanExpression (a->Expr, FALSE, &minvalue, &maxvalue,
                                 TRUE, &virtualexpr);
          a = a->Next;
        }
    }
}

void
AnalyseAdvise (int depth, char *filename, int line,
                           struct tActualParameter *ActualList)
{
  struct tDynamicValues *savedvars;
  bool success;
  struct tActualParameter *a;

  /* Copy the current variable state */
  savedvars = StoreDynamicValues ();
  /* Update the ranges according to the restrictions */
  success = TRUE;
  a = ActualList;
  while (a != NULL)
    {
      UpdateRanges (a->Expr, FALSE, &success, FALSE, FALSE);
      a = a->Next;
    }
  /* Restore the original ranges if not every term in the test could be fully analysed */
  if (!success)
    {
      RestoreDynamicValues (savedvars);
      err (filename, line,
           "Test in pragma could not be fully analysed for range updates");
    }
  FreeDynamicValues (savedvars);
  //
  if (CompilerTest)
    {
      // Write in a runtime check if configured for consistency checks
      PerformTestAssertion(depth, filename, line, ActualList);
    }
}

void
EmitDispatch (int depth)
{
  emit (depth, "__coroutine_dispatch();\n");
}

//*****unchecked_open, unchecked_close?
void
AnalysePragma (int depth, char *filename, int line, struct tProcedureCall *p)
{
  char *name;
  struct tActualParameter *a;
  struct tReference *r;
  struct tVar *v;
  struct tDynamicValues *savedvars;
  bool success;
  BIGINT minvalue, maxvalue;
  struct tUnitDef *unit;

  if (p->Name->ReferenceType == IDENTIFIER_REFERENCE && p->Name->Next == NULL)
    {
      name = p->Name->VarName;
      if ((!strcasecmp (name, "unchecked_initialisation"))
          || (!strcasecmp (name, "unchecked_initialization")))
        {
          a = p->ActualList;
          while (a != NULL)
            {
              if (a->Expr->Op == _VAR)
                {
                  if (a->Expr->Var->Next == NULL)
                    {
                      r = a->Expr->Var;
                      v = FindVariable (filename, line, &r, 30, FALSE, TRUE);
                      if (v->Mode == MODE_LOCAL || v->Mode == MODE_GLOBAL
                          || v->Mode == MODE_SHARED || v->Mode == MODE_OUT
                          || v->Mode == MODE_IN_OUT)
                        {
                          if (v->ConstValue == NULL)
                            {
                              if ((v->Mode == MODE_GLOBAL
                                   || v->Mode == MODE_SHARED)
                                  && v->Type->TypeSpec->Controlled
                                  &&
                                  (!(InPackageInitialisation
                                     || InPackageFinalisation)))
                                {
                                  err (v->FileName, v->Line,
                                       "Unchecked_initialisation can only be declared for controlled globals inside package initialisation and finalisation sections");
                                }
                              else
                                {
                                  SetInitialRanges (v);
                                  SetInitialised (v);
                                  v->EverInitialised = TRUE;
                                  ResetRelations (v);
                                  v->Finalised = FALSE;
                                  if (!InLocalSection())
                                    {
                                      // Not in a routine or an initialisation or fincalisation section;
                                      // set global intialisation
                                      v->InitialisedGlobally = TRUE;
                                    }
                                  // Record global writes
                                  RecordGlobalAccess (line, GetGlobalsAccess (), v, FALSE, TRUE, FALSE,
                                                      FALSE);
                                }
                            }
                          else
                            {
                              err (filename, line,
                                   "%s: pragma unchecked_initialisation cannot be used on variables with constant initialisation",
                                   a->Expr->Var->VarName);
                            }
                        }
                      else
                        {
                          err (filename, line,
                               "%s: pragma unchecked_initialisation can only be applied to local and global variables and out and in out parameters",
                               a->Expr->Var->VarName);
                        }
                    }
                  else
                    {
                      PragmaSyntaxError (filename, line);
                      break;
                    }
                }
              else
                {
                  PragmaSyntaxError (filename, line);
                  break;
                }
              a = a->Next;
            }
        }
      else if ((!strcasecmp (name, "unchecked_finalisation"))
               || (!strcasecmp (name, "unchecked_finalization")))
        {
          a = p->ActualList;
          while (a != NULL)
            {
              if (a->Expr->Op == _VAR)
                {
                  if (a->Expr->Var->Next == NULL)
                    {
                      r = a->Expr->Var;
                      v = FindVariable (filename, line, &r, 30, FALSE, TRUE);
                      if (v->Mode == MODE_LOCAL || v->Mode == MODE_GLOBAL
                          || v->Mode == MODE_SHARED
                          || v->Mode == MODE_FINAL_IN_OUT)
                        {
                          if (v->Type->TypeSpec->Controlled)
                            {
                              SetFinalised (v);
                            }
                          else
                            {
                              err (filename, line,
                                   "%s: pragma unchecked_finalisation can only be used on variables of controlled types",
                                   a->Expr->Var->VarName);
                            }
                        }
                      else
                        {
                          err (filename, line,
                               "%s: pragma unchecked_finalisation can only be applied to local and global variables and final in out parameters",
                               a->Expr->Var->VarName);
                        }
                    }
                  else
                    {
                      PragmaSyntaxError (filename, line);
                      break;
                    }
                }
              else
                {
                  PragmaSyntaxError (filename, line);
                  break;
                }
              a = a->Next;
            }
        }
      else if (!strcasecmp (name, "unchecked_use"))
        {
          a = p->ActualList;
          while (a != NULL)
            {
              if (a->Expr->Op == _VAR)
                {
                  if (a->Expr->Var->Next == NULL)
                    {
                      r = a->Expr->Var;
                      v = FindVariable (filename, line, &r, 30, FALSE, TRUE);
                      if (!IsVariable(v))
                        {
                          err(filename, line, "Name of a variable expected in pragma unchecked_use (%s)", v->Name);
                        }
                      SetAccessed (v);
                      RecordGlobalAccess (line, GetGlobalsAccess (), v,
                                          TRUE, FALSE, (!v->Initialised),
                                          FALSE);
                    }
                  else
                    {
                      PragmaSyntaxError (filename, line);
                      break;
                    }
                }
              else
                {
                  PragmaSyntaxError (filename, line);
                  break;
                }
              a = a->Next;
            }
        }
      else if ((!strcasecmp (name, "finalised"))
               || (!strcasecmp (name, "finalized")))
        {
          a = p->ActualList;
          while (a != NULL)
            {
              if (a->Expr->Op == _VAR)
                {
                  if (a->Expr->Var->Next == NULL)
                    {
                      r = a->Expr->Var;
                      v = FindVariable (filename, line, &r, 30, FALSE, TRUE);
                      if (v->Mode == MODE_LOCAL || v->Mode == MODE_GLOBAL
                          || v->Mode == MODE_SHARED
                          || v->Mode == MODE_FINAL_IN_OUT)
                        {
                          if (v->Type->TypeSpec->Controlled)
                            {
                              if (v->Type->TypeSpec->BaseType == ARRAY_TYPE)
                                {
                                  if (v->Type->TypeSpec->
                                      ElementTypeSpec->Controlled)
                                    {
                                      err (filename, line,
                                           "%s: pragma finalised cannot be applied to arrays that have controlled elements",
                                           a->Expr->Var->VarName);
                                    }
                                }
                              else if (v->Type->TypeSpec->BaseType ==
                                       RECORD_TYPE)
                                {
                                  struct tFieldSpec *f;
                                  bool anycontrolled;

                                  f = v->Type->TypeSpec->FieldList;
                                  anycontrolled = FALSE;
                                  while (f != NULL)
                                    {
                                      if (f->Typespec->Controlled)
                                        anycontrolled = TRUE;
                                      f = f->Next;
                                    }
                                  if (anycontrolled)
                                    {
                                      err (filename, line,
                                           "%s: pragma finalised cannot be applied to records that have controlled fields",
                                           a->Expr->Var->VarName);
                                    }
                                }
                              SetFinalised (v);
                            }
                          else
                            {
                              err (filename, line,
                                   "%s: pragma finalised can only be used on variables of controlled types",
                                   a->Expr->Var->VarName);
                            }
                        }
                      else
                        {
                          err (filename, line,
                               "%s: pragma finalised can only be applied to local and global variables and final in out parameters",
                               a->Expr->Var->VarName);
                        }
                    }
                  else
                    {
                      PragmaSyntaxError (filename, line);
                      break;
                    }
                }
              else
                {
                  PragmaSyntaxError (filename, line);
                  break;
                }
              a = a->Next;
            }
        }
      else if (!strcasecmp (name, "hide"))
        {
          a = p->ActualList;
          while (a != NULL)
            {
              if (a->Expr->Op == _VAR)
                {
                  if (a->Expr->Var->Next == NULL)
                    {
                      r = a->Expr->Var;
                      v = FindVariable (filename, line, &r, 31, FALSE, TRUE);
                      if (!IsVariable(v))
                        {
                          err(filename, line, "Name of a variable expected in pragma hide (%s)", v->Name);
                        }
                      v->Hidden = TRUE;
                    }
                  else
                    {
                      PragmaSyntaxError (filename, line);
                      break;
                    }
                }
              else
                {
                  PragmaSyntaxError (filename, line);
                  break;
                }
              a = a->Next;
            }
        }
      else if (!strcasecmp (name, "runtime_check"))
        {
          bool bitfield;

          emit (depth, "if (");
          a = p->ActualList;
          while (a != NULL)
            {
              if (a == p->ActualList)
                {
                  emit (0, "  ");
                }
              else
                {
                  emit (depth, "    ||");
                }
              emit (0, " (!(");
              printexpr (a->Expr, &bitfield, TRUE);
              emit (0, "))\n");
              a = a->Next;
            }
          emit (depth, ") {\n");
          AnalyseRestart (depth + 1, filename, line);
          emit (depth, "}\n");
          AnalyseAdvise (depth, filename, line, p->ActualList);
        }
      else if (!strcasecmp (name, "unchecked_reset"))
        {
          a = p->ActualList;
          while (a != NULL)
            {
              if (a->Expr->Op != _VAR)
                {
                  PragmaSyntaxError (filename, line);
                }
              else if (a->Expr->Var->Next != NULL)
                {
                  PragmaSyntaxError (filename, line);
                }
              r = a->Expr->Var;
              v = FindVariable (filename, line, &r, 30, FALSE, TRUE);
              if (v->Type->TypeSpec->BaseType != PROCEDURE_TYPE)
                {
                  SetInitialRanges (v);
                  ResetRelations (v);
                }
              a = a->Next;
            }
        }
      else if (!strcasecmp (name, "unchecked_deadlock"))
        {
          struct tActualParameter *a2;
          struct tVar *v2;
          struct tVarPair *pair;

          a = p->ActualList;
          if (a == NULL)
            {
              PragmaSyntaxError (filename, line);
            }
          a2 = a->Next;
          if (a2 == NULL)
            {
              PragmaSyntaxError (filename, line);
            }
          if (a2->Next != NULL)
            {
              PragmaSyntaxError (filename, line);
            }
          if (a->Expr->Op != _VAR || a2->Expr->Op != _VAR)
            {
              PragmaSyntaxError (filename, line);
            }
          if (a->Expr->Var->Next != NULL || a2->Expr->Var->Next != NULL)
            {
              PragmaSyntaxError (filename, line);
            }
          r = a->Expr->Var;
          v = FindVariable (filename, line, &r, 31, FALSE, TRUE);
          if (!
              ((IsVariable (v) || v->Type->TypeSpec->BaseType == PACKAGE_TYPE)
               && v->Mode == MODE_SHARED))
            {
              err (filename, line,
                   "%s is not a shared variable or a shared package",
                   v->Name);
            }
          r = a2->Expr->Var;
          v2 = FindVariable (filename, line, &r, 31, FALSE, TRUE);
          if (!
              ((IsVariable (v2)
                || v2->Type->TypeSpec->BaseType == PACKAGE_TYPE)
               && v2->Mode == MODE_SHARED))
            {
              err (filename, line,
                   "%s is not a shared variable or a shared package",
                   v2->Name);
            }
          pair = checked_malloc (sizeof *pair);
          pair->Var1 = v;
          pair->Var2 = v2;
          pair->Notified = FALSE;
          pair->Next = UncheckedDeadlockList;
          UncheckedDeadlockList = pair;
        }
      else if (!strcasecmp (name, "test_assert"))
        {
          PerformTestAssertion(depth, filename, line, p->ActualList);
        }
      else if (!strcasecmp (name, "fail"))
        {
          if (!BuildingUnitTest)
            {
              err (filename, line,
                   "Pragma fail can only be used in unit test sections");
            }
          emit (depth, "return %d;\n", line);
        }
      else if (!strcasecmp (name, "succeed"))
        {
          if (!BuildingUnitTest)
            {
              err (filename, line,
                   "Pragma succeed can only be used in unit test sections");
            }
          emit (depth, "return 0;\n");
        }
      else if (!strcasecmp (name, "incomplete"))
        {
          if (CurrentProc == NULL)
            {
              err (filename, line,
                   "Pragma incomplete can only be used in the bodies of procedures and functions");
            }
          else if (GenerateOutput)
            {
              err (filename, line,
                   "Pragma incomplete cannot be used when code is to be built");
            }
          else
            {
              CurrentProc->Incomplete = TRUE;
            }
        }
      else if (!strcasecmp (name, "no_main"))
        {
          if (CurrentPackage->PackageType != PACKAGE_SYSTEM)
            {
              err (filename, line,
                   "Pragma 'no_main' can only be used in the system package");
            }
          NoSystemMain = TRUE;
        }
      else if (!strcasecmp (name, "no_c_main"))
        {
          GenerateMain = FALSE;
          if (Annotate && (TaskImplementation != CONTIKI))
            {
              emitstr ("/*\n");
              emitstr (" * User-supplied main() must call\n");
              emitstr
                (" *   __app_initialisation(), then __app_run(), then __app_finalisation()\n");
              emitstr (" */\n");
            }
        }
      else if (!strcasecmp (name, "include"))
        {
          SwitchToHeader();
          a = p->ActualList;
          while (a != NULL)
            {
              char *fname = PragmaString(filename, line, a);
              if (fname[0] == '<')
                emit (0, "#include %s\n", fname);
              else
                emit (0, "#include \"%s\"\n", fname);
              a = a->Next;
            }
            emitstr("\n");
          SwitchToSource();
        }
      else if (!strcasecmp (name, "contiki_autostart"))
        {
          a = p->ActualList;
          while (a != NULL)
            {
              char *fname = PragmaSymbol(filename, line, a);
              // current ',' '&' fname 0
              char *s = malloc(strlen(autostartlist) + 1 + 1 + strlen(fname) + 1);
              sprintf (s, "%s,&%s", autostartlist, fname);
              free (autostartlist);
              autostartlist = s;
              haveautostarts = TRUE;
              a = a->Next;
            }
        }
      else if (!strcasecmp (name, "set_default_allocate"))
        {
          DefaultAllocateFunction =
            PragmaString (filename, line, p->ActualList);
          if (p->ActualList->Next != NULL)
            PragmaSyntaxError (filename, line);
        }
      else if (!strcasecmp (name, "set_default_free"))
        {
          DefaultFreeFunction = PragmaString (filename, line, p->ActualList);
          if (p->ActualList->Next != NULL)
            PragmaSyntaxError (filename, line);
        }
      else if (!strcasecmp (name, "pre_dispatch"))
        {
          char *s = PragmaString (filename, line, p->ActualList);

          if (p->ActualList->Next != NULL)
            PragmaSyntaxError (filename, line);
          if (!UnitTest)
            {
              CoroutinePreDispatch = s;
            }
        }
      else if (!strcasecmp (name, "post_dispatch"))
        {
          char *s = PragmaString (filename, line, p->ActualList);

          if (p->ActualList->Next != NULL)
            PragmaSyntaxError (filename, line);
          if (!UnitTest)
            {
              CoroutinePostDispatch = s;
            }
        }
      else if (!strcasecmp (name, "dispatch"))
        {
          if (p->ActualList != NULL)
            PragmaSyntaxError (filename, line);
          else
            {
              if (TaskImplementation == COROUTINE)
                {
                  EmitDispatch (depth);
                  CurrentProc->Dispatches = TRUE;
                }
              else if (WithResume)
                {
                  emitcyclicdispatch (depth);
                  CurrentProc->Dispatches = TRUE;
                }
              else
                {
                  IgnoredDispatches++;
                  warning (filename, line, "Pragma dispatch has no effect");
                }
            }
        }
      else if (!strcasecmp (name, "unit_test_resume"))
        {
          if (!UnitTestWithResume)
            {
              err (filename, line,
                   "unit_test_with_resume was not specified for this routine");
            }
          if (UnitTestResumePointSet)
            {
              err (filename, line,
                   "The unit test resume point has already been set for this routine");
            }
          if (UnitTest)
            {
              emit (depth, "__unit_test_resume:\n");
            }
          UnitTestResumePointSet = TRUE;
        }
      else if (!strcasecmp (name, "unit_test_return"))
        {
          if (!UnitTestWithResume)
            {
              err (filename, line,
                   "unit_test_with_resume was not specified for this routine");
            }
          if (UnitTest)
            {
              emit (depth, "return;  /* unit test return */\n");
            }
        }
      else if (!strcasecmp (name, "stack_size"))
        {
          BIGINT stacksize;

          if (p->ActualList == NULL)
            PragmaSyntaxError (filename, line);
          if (p->ActualList->Next != NULL)
            PragmaSyntaxError (filename, line);
          stacksize =
            EvalStaticInteger (p->ActualList->Expr, &success, TRUE, &unit);
          if (success)
            {
              if (unit != NULL || stacksize <= 0)
                {
                  err (filename, line, "Unitless positive integer required");
                }
              if (CurrentPackage->PackageType != PACKAGE_SUBSYSTEM
                  && CurrentPackage->PackageType != PACKAGE_SYSTEM)
                {
                  err (filename, line,
                       "pragma stack_size must appear within the declarations of a (sub)system");
                }
              if (CurrentPackage->StackSize != 0)
                {
                  err (filename, line,
                       "Stack size has already been set for this (sub)system");
                }
              CurrentPackage->StackSize = stacksize;
            }
        }
      else if (!strcasecmp (name, "stack_overhead"))
        {
          BIGINT stackoverhead;

          if (p->ActualList->Next != NULL)
            PragmaSyntaxError (filename, line);
          stackoverhead =
            EvalStaticInteger (p->ActualList->Expr, &success, TRUE, &unit);
          if (success)
            {
              if (unit != NULL || stackoverhead < 0)
                {
                  err (filename, line,
                       "Unitless non-negative integer required");
                }
              if (CurrentProc == NULL)
                {
                  // Use the maximum value in the entire system
                  if (GlobalStackOverhead < stackoverhead)
                    GlobalStackOverhead = stackoverhead;
                  GlobalStackOverheadSet = TRUE;
                }
              else
                {
                  // Use the maximum value within a routine. This allows the pragma to be used before
                  // each call to library routines, for example.
                  if (CurrentProc->StackOverhead < stackoverhead)
                    CurrentProc->StackOverhead = stackoverhead;
                }
            }
        }
      else if (!strcasecmp (name, "call_overhead"))
        {
          BIGINT calloverhead;

          if (p->ActualList == NULL)
            PragmaSyntaxError (filename, line);
          if (p->ActualList->Next != NULL)
            PragmaSyntaxError (filename, line);
          calloverhead =
            EvalStaticInteger (p->ActualList->Expr, &success, TRUE, &unit);
          if (success)
            {
              if (unit != NULL || calloverhead < 0)
                {
                  err (filename, line,
                       "Unitless non-negative integer required");
                }
              if (FixedProcedureOverheadSet
                  && (FixedProcedureOverhead != calloverhead))
                {
                  err (filename, line,
                       "call_overhead has already been set to a different value");
                }
              FixedProcedureOverhead = calloverhead;
              FixedProcedureOverheadSet = TRUE;
            }
        }
      else if (!strcasecmp (name, "interrupt_overhead"))
        {
          BIGINT intoverhead;

          if (p->ActualList == NULL)
            PragmaSyntaxError (filename, line);
          if (p->ActualList->Next != NULL)
            PragmaSyntaxError (filename, line);
          intoverhead =
            EvalStaticInteger (p->ActualList->Expr, &success, TRUE, &unit);
          if (success)
            {
              if (unit != NULL || intoverhead < 0)
                {
                  err (filename, line,
                       "Unitless non-negative integer required");
                }
              if (InterruptOverheadSet && (InterruptOverhead != intoverhead))
                {
                  err (filename, line,
                       "interrupt_overhead has already been set to a different value");
                }
              InterruptOverhead = intoverhead;
              InterruptOverheadSet = TRUE;
            }
        }
      else if (!strcasecmp (name, "maximum_object_size"))
        {
          BIGINT maxsize;

          if (p->ActualList == NULL)
            PragmaSyntaxError (filename, line);
          if (p->ActualList->Next != NULL)
            PragmaSyntaxError (filename, line);
          maxsize =
            EvalStaticInteger (p->ActualList->Expr, &success, TRUE, &unit);
          if (success)
            {
              if (unit != NULL || maxsize < 1)
                {
                  err (filename, line,
                       "Unitless positive integer required");
                }
              if ((MaximumObjectSize != 0) && (MaximumObjectSize != maxsize))
                {
                  err (filename, line,
                       "maximum_object_size has already been set to a different value");
                }
              MaximumObjectSize = maxsize;
            }
        }
      else if (!strcasecmp (name, "target_integer_type"))
        {
          char *cname, *internalname, *formatstring;
          BIGINT minval, maxval, bits;
          bool arithmetic, preferred, atomictype;
          struct tTargetType *newtargettype;

          PragmaCheckNumargs (filename, line, p->ActualList, 9);
          a = PragmaFindArg (filename, line, p->ActualList, "cname");
          cname = PragmaString (filename, line, a);
          a = PragmaFindArg (filename, line, p->ActualList, "min_value");
          minval = EvalStaticInteger (a->Expr, &success, TRUE, &unit);
          a = PragmaFindArg (filename, line, p->ActualList, "max_value");
          maxval = EvalStaticInteger (a->Expr, &success, TRUE, &unit);
          a = PragmaFindArg (filename, line, p->ActualList, "arithmetic");
          arithmetic = EvalStaticBoolean (a->Expr, &success, TRUE);
          a = PragmaFindArg (filename, line, p->ActualList, "preferred");
          preferred = EvalStaticBoolean (a->Expr, &success, TRUE);
          a = PragmaFindArg (filename, line, p->ActualList, "iname");
          internalname = PragmaSymbol (filename, line, a);
          a = PragmaFindArg (filename, line, p->ActualList, "bits");
          bits = EvalStaticInteger (a->Expr, &success, TRUE, &unit);
          a = PragmaFindArg (filename, line, p->ActualList, "format_string");
          formatstring = PragmaString (filename, line, a);
          a = PragmaFindArg (filename, line, p->ActualList, "atomic");
          atomictype = EvalStaticBoolean (a->Expr, &success, TRUE);
          newtargettype =
            AddTargetIntegerType (cname, minval, maxval, arithmetic,
                                  preferred, internalname, bits, formatstring,
                                  atomictype);
          //
          // Add the type as a visible type in the package
          //
          struct tTypeDeclaration typedecl;
          struct tTypeSpec *typespec;
          struct tNode lowrange, highrange;
          struct tType *newtype;

          lowrange.Op = _CONST_INT;
          lowrange.Value = minval;
          highrange.Op = _CONST_INT;
          highrange.Value = maxval;
          typespec = NewTypespec (line);
          typespec->Line = line;
          typespec->BaseType = INTEGER_TYPE;
          typespec->LowExpr = &lowrange;
          typespec->HighExpr = &highrange;
          typespec->EnumList = NULL;
          typespec->ReqUnit = NULL;
          typespec->StructureAccess = PUBLIC_ACCESS;
          typedecl.Name = internalname;
          typedecl.TypeSpec = typespec;
          newtype =
            AnalyseAddType (0, filename, line, &typedecl, PUBLIC_ACCESS);
          // Make sure that it is the same one as was just defined (in case the order of the
          // definitions is not smallest range to largest range)
          newtype->TypeSpec->Type->TypeSpec->CType = newtargettype;
          newtype->TypeSpec->Type->TypeSpec->CName = newtargettype->CName;
        }
      else if (!strcasecmp (name, "boolean_type"))
        {
          char *wordtype;

          PragmaCheckNumargs (filename, line, p->ActualList, 1);
          wordtype = PragmaSymbol (filename, line, p->ActualList);
          SetupIntegerLikeType (filename, line, wordtype, BooleanTypespec, 0,
                                1, BooleanVar);

        }
      else if (!strcasecmp (name, "character_type"))
        {
          char *wordtype;

          PragmaCheckNumargs (filename, line, p->ActualList, 1);
          wordtype = PragmaSymbol (filename, line, p->ActualList);
          SetupIntegerLikeType (filename, line, wordtype, CharacterTypespec,
                                0, 127, CharacterVar);
        }
      else if (!strcasecmp (name, "universal_index_type"))
        {
          char *wordtype;
          BIGINT maxval, minval;

          if (UniversalIndexSet)
            {
              err(filename, line, "universal_index_type can only be specified once");
            }
          PragmaCheckNumargs (filename, line, p->ActualList, 3);
          a = PragmaFindArg (filename, line, p->ActualList, "word_type");
          wordtype = PragmaSymbol (filename, line, a);
          a = PragmaFindArg (filename, line, p->ActualList, "min_value");
          minval = EvalStaticInteger (a->Expr, &success, TRUE, &unit);
          a = PragmaFindArg (filename, line, p->ActualList, "max_value");
          maxval = EvalStaticInteger (a->Expr, &success, TRUE, &unit);
          SetupIntegerLikeType (filename, line, wordtype,
                                UniversalIndexTypespec, minval, maxval,
                                UniversalIndexVar);
          // Record the range
          UniversalIndexMin = minval;
          UniversalIndexMax = maxval;
          // Set the C type used for array indexes
          DefaultArrayIndex = TargetINameToCName (filename, line, wordtype);
          //
          UniversalIndexSet = TRUE;
        }
      else if (!strcasecmp (name, "target_float_type"))
        {
          char *cname, *internalname, *formatstring;
          BIGINT digits, magnitude, bits;
          bool arithmetic;
          struct tTargetType *newtargettype;

          PragmaCheckNumargs (filename, line, p->ActualList, 7);
          a = PragmaFindArg (filename, line, p->ActualList, "cname");
          cname = PragmaString (filename, line, a);
          a = PragmaFindArg (filename, line, p->ActualList, "float_digits");
          digits = EvalStaticInteger (a->Expr, &success, TRUE, &unit);
          a =
            PragmaFindArg (filename, line, p->ActualList, "float_magnitude");
          magnitude = EvalStaticInteger (a->Expr, &success, TRUE, &unit);
          a = PragmaFindArg (filename, line, p->ActualList, "arithmetic");
          arithmetic = EvalStaticBoolean (a->Expr, &success, TRUE);
          a = PragmaFindArg (filename, line, p->ActualList, "iname");
          internalname = PragmaSymbol (filename, line, a);
          a = PragmaFindArg (filename, line, p->ActualList, "bits");
          bits = EvalStaticInteger (a->Expr, &success, TRUE, &unit);
          a = PragmaFindArg (filename, line, p->ActualList, "format_string");
          formatstring = PragmaString (filename, line, a);
          newtargettype =
            AddTargetFloatType (cname, digits, magnitude, arithmetic,
                                internalname, bits, formatstring);
          //
          // Add the type as a visible type in the package
          //
          struct tTypeDeclaration typedecl;
          struct tTypeSpec *typespec;
          struct tType *newtype;

          typespec = NewTypespec (line);
          typespec->Line = line;
          typespec->BaseType = FLOATING_TYPE;
          typespec->Digits = digits;
          typespec->Magnitude = magnitude;
          typespec->ReqUnit = NULL;
          typespec->StructureAccess = PUBLIC_ACCESS;
          typedecl.Name = internalname;
          typedecl.TypeSpec = typespec;
          newtype =
            AnalyseAddType (0, filename, line, &typedecl, PUBLIC_ACCESS);
          newtype->TypeSpec->CType = newtargettype;
          newtype->TypeSpec->CName = newtargettype->CName;
        }
      else if (!strcasecmp (name, "target_stack_unit"))
        {
          char *iname;

          if (p->ActualList == NULL)
            PragmaSyntaxError (filename, line);
          if (p->ActualList->Next != NULL)
            PragmaSyntaxError (filename, line);
          if (TargetStackUnitSet)
            err (filename, line, "target_stack_unit has already been set");
          iname = PragmaSymbol (filename, line, p->ActualList);
          TargetStackUnit = TargetINameToCName (filename, line, iname);
          TargetStackUnitSet = TRUE;
        }
      else if (!strcasecmp (name, "clear_target_stack"))
        {
          if (p->ActualList != NULL)
            PragmaSyntaxError (filename, line);
          ClearTargetStack = TRUE;
        }
      else if (!strcasecmp (name, "memory_move"))
        {
          if (p->ActualList == NULL)
            PragmaSyntaxError (filename, line);
          if (p->ActualList->Next != NULL)
            PragmaSyntaxError (filename, line);
          if (ArrayToArrayCopySet)
            err (filename, line, "memory_move has already been set");
          ArrayToArrayCopy = PragmaString (filename, line, p->ActualList);
          ArrayToArrayCopySet = TRUE;
        }
      else if (!strcasecmp (name, "inline_prefix"))
        {
          SetInlinePrefix (filename, line, p->ActualList, FALSE);
        }
      else if (!strcasecmp (name, "inline_prefix_line"))
        {
          SetInlinePrefix (filename, line, p->ActualList, TRUE);
        }
      else if (!strcasecmp (name, "allow_circular_access"))
        {                       //**********
          if (p->ActualList != NULL)
            PragmaSyntaxError (filename, line);
          AllowCircularAccess = TRUE;
        }
      else if (!strcasecmp (name, "system_header"))
        {
          char *str;

          SwitchToAppHeader ();
          emitstr ("\n/* -- Lines added by pragma system_header -- */\n\n");
          a = p->ActualList;
          while (a != NULL)
            {
              str = PragmaString (filename, line, a);
              emitstr (str);
              emitstr ("\n");
              a = a->Next;
            }
          emitstr ("\n/* ----------------------------------------- */\n");
          SwitchToSource ();
        }
      else if (!strcasecmp (name, "describe"))
        {
          bool bf;
          enum tBaseType exprtype;
          int digits, magnitude;
          bool staticexpression;
          BIGINT intval;
          BIGFLOAT floatval;
          bool boolval;
          struct tTypeSpec *accessed;
          bool uncheckedaccess, accessconstant, accesspersistent, accessnew;
          bool virtualexpr;

          /* Copy the current variable state */
          savedvars = StoreDynamicValues ();
          a = p->ActualList;
          while (a != NULL)
            {
              emit (depth, "/* [%d] ", line);
              bf = FALSE;
              printexpr (a->Expr, &bf, FALSE);
              emit (0, ": ");
              AnalyseExpression (a->Expr,
                                 &exprtype,
                                 &minvalue, &maxvalue,
                                 &digits, &magnitude,
                                 &unit,
                                 &staticexpression, &intval, &floatval,
                                 &boolval, &accessed, &uncheckedaccess, FALSE,
                                 TRUE, FALSE, &virtualexpr, &accessconstant, &accesspersistent, &accessnew);
              if (staticexpression)
                {
                  emit (0, "static ");
                }
              if (exprtype == INTEGER_TYPE)
                {
                  if (staticexpression)
                    emit (0, "%" PRINTBIG " ", intval);
                  emit (0, "range %" PRINTBIG "..%" PRINTBIG "", minvalue,
                        maxvalue);
                  if (unit != NULL)
                    {
                      emit (0, " unit ");
                      EmitUnit (unit);
                    }
                }
              else if (exprtype == FLOATING_TYPE)
                {
                  if (staticexpression)
                    emit (0, "%lf ", floatval);
                  emit (0, "digits %d magnitude %d unit ", digits, magnitude);
                  EmitUnit (unit);
                }
              else if (exprtype == BOOLEAN_TYPE)
                {
                  emit (0, "range %" PRINTBIG "..%" PRINTBIG "", minvalue,
                        maxvalue);
                  if (staticexpression)
                    {
                      if (boolval)
                        emit (0, "true ");
                      else
                        emit (0, "false ");
                    }
                  emit (0, " boolean");
                }
              else if (exprtype == ACCESS_TYPE)
                {
                  if (uncheckedaccess)
                    emit (0, "unchecked ");
                  if (accessnew)
                	  emit (0, " new");
                  emit (0, "access");
                  if (accesspersistent)
                	  emit(0, " persistent");
                  if (accessconstant)
                    emit (0, " constant");
                }
              if (virtualexpr)
                {
                  emit (0, " virtual");
                }
              emit (0, " */\n");
              a = a->Next;

            }
          RestoreDynamicValues (savedvars);
        }
      else if (!strcasecmp (name, "describe_locals"))
        {
          DescribeLocals (depth, filename, line);
        }
      else if (!strcasecmp (name, "describe_scope"))
        {
          int j;
          struct tVar **v;

          emit (depth, "/* ----- current scope ----- */\n");
          v = &CurrentVars->Vars[CurrentVars->NumVars];
          for (j = 0; j < CurrentVars->NumVars; j++)
            {
              v--;
              DisplayVar (*v, depth);
            }
          emit (depth, "\n");
        }
      else if (!strcasecmp (name, "trace_on"))
        {
          if (OutputBeforeTrace == NULL)
            {
              OutputBeforeTrace = GetOutput ();
              SwitchToTrace ();
            }
        }
      else if (!strcasecmp (name, "trace_off"))
        {
          if (OutputBeforeTrace != NULL)
            {
              SwitchToOutput (OutputBeforeTrace);
              OutputBeforeTrace = NULL;
            }
        }
      else if (!strcasecmp(name, "note"))
        {
          char *s;

          printf("\n%s:%d Note:\n", filename, line);
          a = p->ActualList;
          while (a != NULL)
            {
              s = PragmaString (filename, line, a);
              printf("  %s\n", s);
              a = a->Next;
            }
          NumPragmaNotes++;
        }
      else
        {
          err (filename, line, "Unknown pragma %s\n", name);
        }
    }
  else
    {
      PragmaSyntaxError (filename, line);
    }
}

void ResetLex (char *filename);

struct StringListEntry
{
  char *String;
  struct StringListEntry *Next;
};

struct StringListEntry *SeparateFileList = NULL;

void
AnalyseSeparateDeclaration (char *filename, int line,
                            struct tSeparate *s, enum tPrefix prefix)
{
  char *separatefilename;
  bool match;
  struct tPackage *package;
  FILE *SavedSourceFile, *SavedHeaderFile;
  char *SavedSourceFileName, *SavedHeaderFileName;
  //
  int SavedLastLine;
  char *newcfile;
  //
  struct tElt *SavedProgramElts;
  bool SeparateError;

  if (s->PackageType == PACKAGE_SYSTEM)
    {
      err (filename, line, "A system cannot be separate");
    }
  if (prefix == PREFIX_UNIT_TEST && (!UnitTest))
    {
      // Omit this package
      return;
    }
  if (prefix == PREFIX_NOT_UNIT_TEST && (UnitTest))
    {
      // Omit this package
      return;
    }
  if (s->FileName == NULL)
    {
      separatefilename = checked_malloc (strlen (s->Name) + 5);
      strcpy (separatefilename, s->Name);
      strcat (separatefilename, ".rihtan");
    }
  else
    {
      separatefilename = DeQuote (s->FileName);
      if (separatefilename[0] == '@')
        {
          char **argptr = CommandArgv;
          int numargs = CommandArgc;
          char *s, *t;
          bool configargfound, mismatch;

          configargfound = FALSE;
          while (numargs > 0)
            {
              s = *argptr;
              if (!strncmp (s, "-c:", 3))
                {
                  s += 3;
                  t = separatefilename + 1;
                  mismatch = !(*s && *t);
                  while (*s && *s != '=' && *t && (!mismatch))
                    {
                      if (*s != *t)
                        {
                          mismatch = TRUE;
                        }
                      s++;
                      t++;
                    }
                  if (!(mismatch || *s != '=' || (*t)))
                    {
                      separatefilename = s + 1;
                      configargfound = TRUE;
                      break;
                    }
                }
              argptr++;
              numargs--;
            }
          if (!configargfound)
            {
              err_and_continue (filename, line,
                                "No -c:configname=filename argument matches configname '%s'",
                                separatefilename + 1);
              separatefilename = "";
            }
        }
    }
  //
  // Check for duplicate use of the file  //*****=====USE INODE etc.
  // Unless the filename starts with -
  //
  if (separatefilename[0] == '-')
    {
      separatefilename++;
    }
  else
    {
      struct StringListEntry *s;
      bool reused;

      s = SeparateFileList;
      reused = FALSE;
      while (s != NULL)
        {
          if (!strcasecmp (s->String, separatefilename))
            {
              warning (filename, line,
                    "Reuse of file %s (move package to outer scope?)",
                    separatefilename);
              info (filename, line,
                    "Prefix the file name with - to suppress this check");
              reused = TRUE;
              break;
            }
          s = s->Next;
        }
      if (!reused)
        {
          // Add the file name to the list
          s = checked_malloc (sizeof *s);
          s->String = separatefilename;
          s->Next = SeparateFileList;
          SeparateFileList = s;
        }
    }

  //
  SavedLastLine = LastLine;
  LastLine = -1;
  SeparateError = FALSE;
  //
  ResetLex (separatefilename);
  if (Annotate)
    {
      emit (OuterDepth, "/* Separate %s */\n\n", separatefilename);
    }
  SavedSourceFile = SourceFile;
  SavedSourceFileName = SourceFileName;
  SavedHeaderFile = HeaderFile;
  SavedHeaderFileName = HeaderFileName;
  SavedProgramElts = ProgramElts;
  ProgramElts = NULL;
  OpenOutput (s->Name, ".c", FALSE, &SourceFile, &SourceFileName, FALSE,
              FALSE);
  OpenOutput (s->Name, ".h", TRUE, &HeaderFile, &HeaderFileName, FALSE,
              FALSE);
  SwitchToAppHeader ();
  emit (0, "#include \"%s\"\n\n", HeaderFileName);
  SwitchToSource ();
  yyparse ();
  CloseOutput (SourceFile, FALSE);
  CloseOutput (HeaderFile, TRUE);
  newcfile = SourceFileName;
  SourceFile = SavedSourceFile;
  SourceFileName = SavedSourceFileName;
  HeaderFile = SavedHeaderFile;
  HeaderFileName = SavedHeaderFileName;
  LastLine = SavedLastLine;
  SwitchToSource ();
  //
  if (IncludeSeparateCFiles)
    {
      emit(0, "#include \"%s\"\n\n", newcfile);
    }
  //
  if (ProgramElts == NULL)
    {
      err_and_continue (filename, line, "Syntax error in separate package");
      SeparateError = TRUE;
    }
  else
    {
      if (ProgramElts->Type == PACKAGE_DECLARATION_ELT)
        {
          package = ProgramElts->Ptr;
          match = TRUE;
        }
      else if (ProgramElts->Type == GENERIC_DECLARATION_ELT)
        {
          struct tGeneric *g = ProgramElts->Ptr;

          package = g->Body->Ptr;
          match = TRUE;
        }
      else
        {
          match = FALSE;
        }
      if (match)
        {
          if (strcmp (package->Name, s->Name))
            {
              match = FALSE;
            }
          if (package->PackageType != s->PackageType)
            {
              match = FALSE;
            }
          if ((!package->Shared) != (!(prefix == PREFIX_SHARED)))
            {
              match = FALSE;
            }
          if ((!package->UnitTest) != (!(prefix == PREFIX_UNIT_TEST)))
            {
              match = FALSE;
            }
          if ((!package->NotUnitTest) != (!(prefix == PREFIX_NOT_UNIT_TEST)))
            {
              match = FALSE;
            }
          if ((!s->Generic) != (!(ProgramElts->Type == GENERIC_DECLARATION_ELT)))
            {
              match = FALSE;
            }
        }
      if (!match)
        {
          char *ptypestr, *sharestr, *genericstr;

          switch (s->PackageType)
            {
            case PACKAGE_PACKAGE:
              ptypestr = "package";
              if (prefix == PREFIX_SHARED)
                {
                  sharestr = "shared ";
                }
              else if (prefix == PREFIX_UNIT_TEST)
              {
            	  sharestr = "unit_test ";
              }
              else if (prefix == PREFIX_NOT_UNIT_TEST)
              {
            	  sharestr = "not unit_test ";
              }
              else
                {
                  if (s->Generic)
                    {
                      sharestr = "";
                    }
                  else
                    {
                      sharestr = "non-prefixed ";
                    }
                }
              break;
            case PACKAGE_SUBSYSTEM:
              ptypestr = "subsystem";
              sharestr = "";
              break;
            default:
              ptypestr = "?";
              sharestr = "?";
              break;
            }
          if (s->Generic)
            {
              genericstr = "generic ";
            }
          else
            {
              genericstr = "";
            }
          err_and_continue (filename, line,
                            "Expected definition for %s%s%s %s in file %s",
                            genericstr, sharestr, ptypestr, s->Name, separatefilename);
          SeparateError = TRUE;
        }
    }
  if (SeparateError)
    {
      PrintErrorCount ();
      printf ("\nExiting because of errors in separate package.\n\n");
      exit (1);
    }
  ProgramElts = SavedProgramElts;
}

// Record a 'separate' block
void RecordSeparateBlock(char *filename, int line, struct tSeparateBlock *s)
{
  struct tVar *v;

  CheckForDuplicateOrCaseDifference (filename, line, s->Name);
  CheckForVarOverflow (filename, line);
  v = MallocVar (filename, line);
  CurrentVars->Vars[CurrentVars->NumVars] = v;
  v->Name = s->Name;
  v->CName = NULL;
  v->ConstValue = NULL;
  v->Initialised = FALSE;
  v->Accessed = FALSE;
  v->Access = PUBLIC_ACCESS;
  v->ConstUnit = NULL;
  v->ConstAccess = NULL;
  v->ConstAccessType = NULL;
  v->Mode = MODE_GLOBAL;
  v->Unit = NULL;
  v->Type = &SeparateBlockType;
  v->Statements = s->Statements;
  CurrentVars->NumVars++;
}

void AnalyseSeparateStmt(int depth, char *filename, int line, struct tSeparateStmt *s)
{
  struct tReference *nameref = s->Name;
  struct tVar *v;

  v = FindVariable (filename, line, &nameref, 6000, FALSE, TRUE);
  if (v->Type != &SeparateBlockType)
    {
      err(filename, line, "Separate block expected");
    }
  Analyse (depth, v->Statements, TRUE, FALSE);
}

// Substitute `sym` expressions in code lines for actual C variable, type etc. names
char *
SubstituteCode (char *filename, int line, char *csource)
{
  char *t, *sym, *dest;
  char newline[1000];

  t = csource;
  dest = newline;               //**********CHECK FOR OVERFLOW
  while (*t)
    {
      if (*t == '\\' && *(t + 1) == '`')
        {
          t++;
          *dest++ = *t;
        }
      else if (*t == '`')
        {
          t++;
          sym = t;
          while (*t && *t != '`')
            t++;
          if (*t == '`' && t > sym)
            {
              struct tReference *ref, *refp = NULL, *r; // Initialise refp to avoid a warning - actually OK
              char *u, *p;
              struct tVar *v;

              ref = NULL;
              while (TRUE)
                {
                  while (sym != t && *sym <= ' ')
                    sym++;
                  if (sym == t)
                    break;
                  p = sym;
                  while (p != t && *p != '.' && *p > ' ')
                    {
                      p++;
                    }
                  r = MakeNewReference ();
                  if (ref == NULL)
                    {
                      ref = r;
                      r->ReferenceType = IDENTIFIER_REFERENCE;
                    }
                  else
                    {
                      refp->Next = r;
                      r->ReferenceType = FIELD_REFERENCE;
                    }
                  refp = r;
                  r->VarName = checked_malloc ((p - sym) + 1);  // including the 0
                  u = r->VarName;
                  while (sym != p)
                    {
                      *u++ = *sym++;
                    }
                  *u = 0;
                  r->Next = NULL;
                  r->ReferenceAbbreviation = NULL;
                  if (sym != t)
                    sym++;
                }
              if (ref == NULL)
                {
                  err (filename, line, "Empty `` term");
                }
              r = ref;
              v = FindVariable (filename, line, &r, 600, FALSE, TRUE);
              u = v->CName;
              if (u == NULL)
                {
                  err (filename, line, "%s... has no C translation",
                       ref->VarName);
                }
              while (*u)
                *dest++ = *u++;
              r = ref;
              while (r != NULL)
                {
                  free (r->VarName);
                  refp = r->Next;
                  free (r);
                  r = refp;
                }
            }
          else
            {
              err (filename, line, "Incomplete or empty `` expression");
            }
        }
      else
        {
          *dest++ = *t;
        }
      t++;
    }
  *dest = 0;
  return strdup (newline);
}

bool
IsLineElt (enum tEltType Type, bool previouswascode)
{

  return (Type != CODE_ELT) || (Type == CODE_ELT && (!previouswascode));
}

void
Analyse (int depth, struct tElt *eltlist, bool statementlist, bool commentsonlylistallowed)
{
#ifdef TRACE
  int tracenum = tracepoint;
  tracepoint++;

  printf ("Analyse %d:\n", tracenum);
  DumpStmt (elt, 0);
  printf ("------- %d\n", tracenum);
#endif
  struct tElt *elt = eltlist;
  jmp_buf saved_recovery;
  bool InCode, PreviousEltWasCode;
  // Initialise these to avoid warnings - actually OK because of setjmp
  int SavedNumVars = 0, SavedNumRelations = 0;
  bool onlycomments;

  memcpy (saved_recovery, error_recovery, sizeof (jmp_buf));

  InCode = FALSE;
  PreviousEltWasCode = FALSE;

  onlycomments = TRUE;

  while (elt != NULL)
    {

      if (elt->Type != COMMENT_ELT)
        {
          onlycomments = FALSE;
        }

      CurrentLine = elt->Line;

      if (InCode && elt->Type != CODE_ELT)
        {
          if (statementlist)
            {
              emit (depth, "}\n");
            }
          else
            {
              emitstr ("\n");
            }
          InCode = FALSE;
        }

      if (LastLine != elt->Line)
        {
          if (NumbersInline)
            {
              if (IsLineElt (elt->Type, PreviousEltWasCode))
                {
                  emit (depth, "/* Line %d */\n", elt->Line);
                }
            }
          LastLine = elt->Line;
        }
      if (setjmp (error_recovery) == 0)
        {
          SavedNumVars = CurrentVars->NumVars;
          SavedNumRelations = CurrentVars->NumRelations;

          switch (elt->Type)
            {
            case TYPE_DECLARATION_ELT:
              {
                struct tTypeDeclaration *t = elt->Ptr;
                if (elt->Access == PUBLIC_ACCESS)
                  {
                    WriteToIndex(FILE_INDEX_PUBLIC, elt->FileName, elt->Line, elt->LastLine);
                  }
                AnalyseAddType (depth, elt->FileName, elt->Line, t,
                                elt->Access);
              }
              break;
            case UNIT_DECLARATION_ELT:
              {
                struct tUnitDeclaration *u = elt->Ptr;
                if (elt->Access == PUBLIC_ACCESS)
                  {
                    WriteToIndex(FILE_INDEX_PUBLIC, elt->FileName, elt->Line, elt->LastLine);
                  }
                AnalyseAddUnit (depth, elt->FileName, elt->Line, u,
                                elt->Access);
              }
              break;
            case APPEND_REPRESENTATION_ELT:
              {
                struct tRepresentationSpec *r = elt->Ptr;
                AnalyseAddRepresentation (elt->FileName, elt->Line, r);
              }
              break;
            case VAR_DECLARATION_ELT:
              {
                struct tVariableDeclaration *v = elt->Ptr;
                if (elt->Access == PUBLIC_ACCESS)
                  {
                    WriteToIndex(FILE_INDEX_PUBLIC, elt->FileName, elt->Line, elt->LastLine);
                  }
                AnalyseVarDeclaration (depth, elt->FileName, elt->Line, v,
                                       elt->Access);
              }
              break;
            case ASSIGNMENT_ELT:
              {
                struct tAssignment *a = elt->Ptr;
                AnalyseAssignmentStmt (depth, elt->FileName, elt->Line, a,
                                       FALSE);
              }
              break;
            case WHEN_ELT:
              {
                struct tWhen *w = elt->Ptr;
                AnalyseWhen (depth, w->Cond, w->When_stmt, w->Else_stmt,
                             FALSE, w->ElseCanBeCommentsOnly);
              }
              break;
            case LOOP_ELT:
              {
                struct tLoop *p = elt->Ptr;
                AnalyseLoop (depth, p->Body);
              }
              break;
            case WHILE_ELT:
              {
                struct tWhile *p = elt->Ptr;
                AnalyseWhile (depth, p->Cond, p->Body);
              }
              break;
            case DO_ELT:
              {
                struct tDo *p = elt->Ptr;
                AnalyseDo (depth, p->Body);
              }
              break;
            case EXIT_ELT:
              AnalyseExit (depth);
              break;
            case REPEAT_ELT:
              AnalyseRepeat (depth);
              break;
            case NULL_ELT:
              if (Annotate)
                {
                  emit (depth, "/* Null statement */\n");
                }
              /* Nothing */
              break;
            case CASE_ELT:
              {
                struct tCase *c = elt->Ptr;
                AnalyseCase (depth, elt->FileName, elt->Line, c);
              }
              break;
            case FOR_ELT:
              {
                struct tFor *f = elt->Ptr;
                AnalyseFor (depth, elt->FileName, elt->Line, f);
              }
              break;
            case PROCEDURE_DECLARATION_ELT:
              {
                struct tProcedure *p = elt->Ptr;
                if (elt->Prefix == PREFIX_UNIT_TEST)
                  {
                    WriteToIndex(FILE_INDEX_UTEST, elt->FileName, elt->Line, elt->LastLine);
                  }
                else if (elt->Access == PUBLIC_ACCESS)
                  {
                    WriteToIndex(FILE_INDEX_PUBLIC, elt->FileName, elt->Line, p->LastLineOfDeclaration);
                  }
                AnalyseProcedure (depth, elt->FileName, elt->Line, p,
                                  elt->Access, elt->Prefix);
                // Then
                if (p->UnitTest != NULL && elt->Prefix != PREFIX_UNIT_TEST)
                  {
                    // Find the the end of the statement list
                    struct tElt *e;
                    e = p->UnitTest;
                    while (e->Next != NULL)
                      {
                        e = e->Next;
                      }
                    WriteToIndex(FILE_INDEX_UTEST, elt->FileName, p->UnitTest->Line, e->LastLine);
                  }
              }
              break;
            case PACKAGE_DECLARATION_ELT:
              {
                struct tPackage *p = elt->Ptr;
                struct tTypeSpec *packagetypespec;
                if (OuterDefinition)
                  {
                    if (p->PackageType != PACKAGE_SYSTEM)
                      {
                        err (elt->FileName, elt->Line,
                             "System expected at this level");
                      }
                    OuterDefinition = FALSE;
                  }
                else
                  {
                    if (p->PackageType == PACKAGE_SYSTEM)
                      {
                        err (elt->FileName, elt->Line,
                             "System not expected at this level");
                      }
                  }
                if (elt->Prefix == PREFIX_UNIT_TEST)
                  {
                    WriteToIndex(FILE_INDEX_UTEST, elt->FileName, elt->Line, elt->LastLine);
                  }
                else
                  {
                    WriteToIndex(FILE_INDEX_PSTART, elt->FileName, p->HeaderStart, p->HeaderEnd);
                  }
                AnalysePackage (depth, elt->FileName, elt->Line, p,
                                elt->Access, FALSE, NULL, p->Name,
                                &packagetypespec, p->Representation,
                                elt->Prefix);
                if (elt->Prefix != PREFIX_UNIT_TEST)
                  {
                    WriteToIndex(FILE_INDEX_PEND, elt->FileName, p->TrailerStart, p->TrailerEnd);
                  }
              }
              break;
            case GENERIC_DECLARATION_ELT:
              {
                struct tGeneric *g = elt->Ptr;
                struct tTypeSpec *packagetypespec;
                struct tPackage *body = g->Body->Ptr;
                if (elt->Prefix == PREFIX_UNIT_TEST)
                  {
                    WriteToIndex(FILE_INDEX_UTEST, elt->FileName, elt->Line, elt->LastLine);
                  }
                else
                  {
                    WriteToIndex(FILE_INDEX_GENERIC_DEF, elt->FileName, elt->Line, ((struct tPackage *)(g->Body->Ptr))->HeaderEnd);
                  }
                AnalysePackage (depth, elt->FileName, elt->Line, body,
                                elt->Access, TRUE, g->Parameters,
                                ((struct tPackage *) (g->Body->Ptr))->Name,
                                &packagetypespec, NULL, elt->Prefix);
                if (elt->Prefix != PREFIX_UNIT_TEST)
                  {
                    WriteToIndex(FILE_INDEX_PEND, elt->FileName, body->TrailerStart, body->TrailerEnd);
                  }
              }
              break;
            case GENERIC_INSTANTIATION_ELT:
              {
                struct tGenericInstantiation *g = elt->Ptr;
                if (elt->Prefix == PREFIX_UNIT_TEST)
                  {
                    WriteToIndex(FILE_INDEX_UTEST, elt->FileName, elt->Line, elt->LastLine);
                  }
                else
                  {
                    WriteToIndex(FILE_INDEX_GENERIC_INST, elt->FileName, g->FirstLine, g->LastLine);
                  }
                AnalyseGenericInstantiation (depth, elt->FileName, elt->Line,
                                             g, elt->Access, elt->Prefix);
              }
              break;
            case PROCEDURE_CALL_ELT:
              {
                struct tProcedureCall *p = elt->Ptr;
                struct tTypeSpec *rettype;
                bool tmpbool, proccallsuccess;
                struct tShareClause *shareclause;

                AnalyseProcedureCall (elt->FileName, elt->Line, p, &rettype,
                                      &tmpbool, &proccallsuccess, FALSE,
                                      FALSE, &shareclause);
                if (rettype != NULL)
                  {
                    err (elt->FileName, elt->Line,
                         "Use of a function as a procedure");
                  }
                if (proccallsuccess)
                  {
                    //----------
                    printprocedurecall (depth, elt->FileName, elt->Line, p,
                                        shareclause, TRUE);
                    //----------
                  }
              }
              break;
            case CODE_ELT:
              {
                char *s = SubstituteCode (elt->FileName, elt->Line, elt->Ptr);
                if (!InCode)
                  {
                    if (statementlist)
                      {
                        if (Annotate)
                          {
                            emit (depth, "{ /* Inline code */\n");
                          }
                        else
                          {
                            emit (depth, "{\n");
                          }
                      }
                    else
                      {
                        if (!PreviousEltWasCode)
                          {
                            emitstr ("\n");
                            if (Annotate)
                              {
                                emit (depth, "/* Inline declarations */\n");
                              }
                          }
                      }
                    InCode = TRUE;
                  }
                if (depth == 0)
                  printcode (0, s);     // special case because normally the ! is two columns to the left of the indent
                else if (statementlist)
                  printcode (depth + 1, s);
                else
                  printcode (depth, s);
                free (s);
              }
              break;
            case HEADER_CODE_ELT:
              {
                char *s = SubstituteCode (elt->FileName, elt->Line, elt->Ptr);
                SwitchToHeader ();
                printcode (depth + 1, s);
                SwitchToSource ();
                free (s);
              }
              break;
            case PRAGMA_ELT:
              {
                struct tProcedureCall *p = elt->Ptr;
                AnalysePragma (depth, elt->FileName, elt->Line, p);
              }
              break;
            case ASSERT_ELT:
              {
                 struct tActualParameter *ap = elt->Ptr;
                 AnalyseAssert(depth, elt->FileName, elt->Line, ap);
              }
              break;
            case ADVISE_ELT:
              {
                 struct tActualParameter *ap = elt->Ptr;
                 AnalyseAdvise(depth, elt->FileName, elt->Line, ap);
              }
              break;
            case CONSTANT_DECLARATION_ELT:
              {
                struct tConstant *c = elt->Ptr;
                if (elt->Access == PUBLIC_ACCESS)
                  {
                    WriteToIndex(FILE_INDEX_PUBLIC, elt->FileName, elt->Line, elt->LastLine);
                  }
                AnalyseAddConstant (depth, elt->FileName, elt->Line, c,
                                    elt->Access);
              }
              break;
            case DECLARE_BLOCK_ELT:
              {
                struct tDeclareBlock *d = elt->Ptr;
                if (d->UnitTest && (!d->NotUnitTest))
                  {
                    WriteToIndex(FILE_INDEX_UTEST, elt->FileName, elt->Line, elt->LastLine);
                  }
                AnalyseDeclareBlock (depth, elt->FileName, elt->Line, d);
              }
              break;
            case SEPARATE_BLOCK_ELT:
              {
                struct tSeparateBlock *s = elt->Ptr;

                RecordSeparateBlock(elt->FileName, elt->Line, s);
              }
              break;
            case SEPARATE_STMT_ELT:
              {
                struct tSeparateStmt *s = elt->Ptr;

                AnalyseSeparateStmt(depth, elt->FileName, elt->Line, s);
              }
              break;
            case SEPARATE_DECLARATION_ELT:
              {
                struct tSeparate *s = elt->Ptr;
                if (elt->Prefix == PREFIX_UNIT_TEST)
                  {
                    WriteToIndex(FILE_INDEX_UTEST, elt->FileName, elt->Line, elt->LastLine);
                  }
                else
                  {
                    WriteToIndex(FILE_INDEX_SEPARATE, elt->FileName, elt->Line, elt->LastLine);
                  }
                AnalyseSeparateDeclaration (elt->FileName, elt->Line, s, elt->Prefix);
              }
              break;
            case FREE_ELT:
              AnalyseFree (depth, elt->FileName, elt->Line, elt->Ptr);
              break;
            case RESTART_ELT:
              AnalyseRestart (depth, elt->FileName, elt->Line);
              break;
            case COMMENT_ELT :
              {
                struct tElt *e;
                unsigned nlines;

                nlines = 0;
                e = elt;
                while (e != NULL)
                  {
                    if (e->Type == COMMENT_ELT)
                      nlines++;
                    else
                      break;
                    e = e->Next;
                  }
                WriteToIndex(FILE_INDEX_COMMENT, elt->FileName, elt->Line, elt->Line + nlines - 1);
                //
                if (RecordComments)
                  {
                    bool toheader;
                    FILE *output;

                    toheader = FALSE;
                    if (e != NULL)
                      {
                        if (e->Type == TYPE_DECLARATION_ELT)
                          {
                            toheader = TRUE;
                          }
                      }
                    output = GetOutput();
                    if (toheader)
                      {
                        SwitchToHeader();
                      }
                    else
                      {
                        SwitchToSource();
                      }
                    if (nlines == 1)
                      {
                        char *s;

                        s = elt->Ptr;
                        while (*s && *s <= ' ') s++;
                        if (*s > ' ')
                          emit(depth, "/* %s */\n", elt->Ptr);
                      }
                    else
                      {
                        emit(depth, "/*\n");
                        e = elt;
                        for (;;)
                          {
                            emit(depth, " * %s\n", e->Ptr);
                            nlines--;
                            if (nlines == 0) break;
                            e = e->Next;
                          }
                        elt = e;
                        emit(depth, " */\n");
                      }
                    SwitchToOutput(output);
                  }
              }
              break;
            default:
              break;
            }

        }
      else
        {
          CurrentVars->NumVars = SavedNumVars;
          CurrentVars->NumRelations = SavedNumRelations;
          info (elt->FileName, elt->Line, "Resumed after error");
        }

      PreviousEltWasCode = (elt->Type == CODE_ELT);

      elt = elt->Next;
    }
  if (InCode)
    {
      if (statementlist)
        {
          emit (depth, "}\n");
        }
      else
        {
          emitstr ("\n");
        }
    }
  //
  memcpy (error_recovery, saved_recovery, sizeof (jmp_buf));
  //
  if (statementlist && onlycomments && (!commentsonlylistallowed))
    {
      err(eltlist->FileName, eltlist->Line, "A statement list that contains nothing but comments is not allowed (use a null statement)");
    }
#ifdef TRACE
  printf ("Analyse %d: done\n", tracenum);
#endif
}

void
AnalyseProgram (struct tElt *elts, bool outer, struct tFileCommentBlock *filecomment)
{
  if ((filecomment != NULL) && RecordComments)
    {
      struct tFileCommentBlock *cp;

      emitstr("/*\n");
      cp = filecomment;
      while (cp != NULL)
        {
          emit(0, " *  %s\n", cp->Comment);
          cp = cp->Next;
        }
      emitstr(" */\n\n");
    }
  emit (0, "#include \"%s\"\n\n", AppHeaderFileName);
  if (outer)
    {
      InFunction = FALSE;
    }
  OuterDefinition = outer;
  PreviousLine = -1;
  PrintLineNumbers = TRUE;
  Analyse (OuterDepth, elts, FALSE, FALSE);
  ProgramElts = elts;
  if (outer)
    {
      if (StringToArrayReqd)
        {
          emit (0, "void %s(char *dest, const char *src, int destlen)\n{\n",
                StringToArrayCopy);
          emit (0, "  if (src != NULL) {\n");
          emit (0, "    while ((*src != 0) && (destlen > 1)) {\n");
          emit (0, "      *dest++ = *src++;\n");
          emit (0, "      destlen--;\n");
          emit (0, "    }\n");
          emit (0, "  }\n");
          emit (0, "  while (destlen > 0) {\n");
          emit (0, "    *dest++ = 0;\n");
          emit (0, "    destlen--;\n");
          emit (0, "  }\n");
          emit (0, "}\n");
        }
      if (CompilerTest)
        {
          emitstr ("\n");
          emitstr ("/*\n");
          emitstr (" ***** Compiler test *****\n");
          emitstr (" */\n\n");
          emitstr ("long __allocated_objects = 0;\n\n");
          emitstr
            ("const void *__deref_check(const void *a, unsigned count, int line)\n");
          emitstr ("{\n");
          emitstr ("  if (a == NULL) {\n");
          emitstr
            ("    printf(\"***** Access through null pointer at line %d\\n\", line);\n");
          emitstr ("    exit(1);\n");
          emitstr ("  }\n");
          emitstr ("  if (count == 0) {\n");
          emitstr
            ("    printf(\"***** Access to deallocated object at line %d\\n\", line);\n");
          emitstr ("    exit(1);\n");
          emitstr ("  }\n");
          emitstr ("  return a;\n");
          emitstr ("}\n\n");
          emitstr ("void __deallocation_check(unsigned count, int line)\n");
          emitstr ("{\n");
          if (!AllowCircularAccess)
            {
              emitstr ("  if (count == 0) {\n");
              emitstr
                ("    printf(\"***** Double deallocation at line %d (circular access permission not set)\\n\", line);\n");
              emitstr ("    exit(1);\n");
              emitstr ("  }\n");
            }
          emitstr ("}\n\n");
          emit (0,
                "void __range_check_unsigned(int line, %s value, %s low, %s high)\n",
                RangeCheckUnsignedCType, RangeCheckUnsignedCType,
                RangeCheckUnsignedCType);
          emitstr ("{\n");
          emitstr ("  if (value < low || value > high) {\n");
          emit (0,
                "    printf(\"***** Range check error at line %%d, expected %s..%s, got %s\\n\", line, low, high, value);\n",
                RangeCheckUnsignedFormat, RangeCheckUnsignedFormat,
                RangeCheckUnsignedFormat);
          emitstr ("    exit(1);\n");
          emitstr ("  }\n");
          emitstr ("}\n\n");
          emit (0,
                "void __range_check_signed(int line, %s value, %s low, %s high)\n",
                RangeCheckSignedCType, RangeCheckSignedCType,
                RangeCheckSignedCType);
          emitstr ("{\n");
          emitstr ("  if (value < low || value > high) {\n");
          emit (0,
                "    printf(\"***** Range check error at line %%d, expected %s..%s, got %s\\n\", line, low, high, value);\n",
                RangeCheckSignedFormat, RangeCheckSignedFormat,
                RangeCheckSignedFormat);
          emitstr ("    exit(1);\n");
          emitstr ("  }\n");
          emitstr ("}\n\n");
          emit (0, "%s __index_check(int line, %s index, %s low, %s high)\n",
                DefaultArrayIndex, DefaultArrayIndex, DefaultArrayIndex,
                DefaultArrayIndex);
          emitstr ("{\n");
          emitstr ("  if (index < 0 || index > high - low) {\n");       // The parameter 'index' is zero-based
          emit (0, "    printf(\"***** Index check error at line %%d, expected %s..%s, got %s\\n\", line, low, high, low + index);\n", "%u", "%u", "%u");       //*****CHANGE %u to match the type
          emitstr ("    exit(1);\n");
          emitstr ("  }\n");
          emitstr ("  return index;\n");
          emitstr ("}\n\n");
          emit (0,
                "%s __length_check(int line, %s requestedlength, %s destinationlength)\n",
                DefaultArrayIndex, DefaultArrayIndex, DefaultArrayIndex);
          emitstr ("{\n");
          emitstr ("  if (requestedlength > destinationlength) {\n");
          emit (0, "    printf(\"***** Length check error at line %%d, request %s exceeds length %s\\n\", line, requestedlength, destinationlength);\n", "%u", "%u");   //*****CHANGE %u to match the type
          emitstr ("    exit(1);\n");
          emitstr ("  }\n");
          emitstr ("  return requestedlength;\n");
          emitstr ("}\n\n");
          emitstr ("void __notnull_check(int line, const void *a)\n");
          emitstr ("{\n");
          emitstr ("  if (a == NULL) {\n");
          emitstr
            ("    printf(\"***** Assignment of null to a non-null access variable or parameter at line %d\\n\", line);\n");
          emitstr ("    exit(1);\n");
          emitstr ("  }\n");
          emitstr ("}\n\n");
          emitstr ("void __case_error(int line)\n");
          emitstr ("{\n");
          emitstr
            ("  printf(\"***** Unexpected case value at line %d\\n\", line);\n");
          emitstr ("  exit(1);\n");
          emitstr ("}\n\n");
          emitstr ("void __test_assertion_failure(int line, int testnum)\n");
          emitstr ("{\n");
          emitstr
            ("  printf(\"***** Test assertion failure at line %d, test number %d\\n\", line, testnum);\n");
          emitstr ("  exit(1);\n");
          emitstr ("}\n\n");
        }
    }
}

/* ----- Miscellaneous routines ----- */

/* Write out an expression tree */
void
errDumpExpr (struct tNode *p)
{
  if (p == NULL)
    return;
  if (p->Op == _CONST_INT)
    {
      errcont ("%" PRINTBIG "", p->Value);
    }
  else if (p->Op == _CONST_FLOAT)
    {
      errcont ("%lf", p->FloatValue);
    }
  else if (p->Op == _CONST_BOOL)
    {
      if (p->Value != FALSE)
        errcont ("true");
      else
        errcont ("false");
    }
  else if (p->Op == _CONST_STRING)
    {
      errcont ("%s", p->String);
    }
  else if (p->Op == _CONST_CHARACTER)
    {
      errcont ("%s", p->String);
    }
  else if (p->Op == _CONST_NULL)
    {
      errcont ("null");
    }
  else if (p->Op == _VAR)
    {
      errDumpReference (p->Var);
      if (p->Attribute != NULL)
        errcont ("'%s", AttributeNames[GetExprAttribute (p)]);  //*****=====FIX
    }
  else if (p->Op == _FN_CALL)
    {
      errDumpProcedureCall (p->Call->Ptr);
    }
  else
    {
      errcont ("(");
      errDumpExpr (p->Left);
      errcont (" ");
      errcont ("%s", OpString[p->Op]);
      errcont (" ");
      errDumpExpr (p->Right);
      errcont (")");
    }
}

void
errDumpRange (BIGINT minvalue, BIGINT maxvalue)
{
  if (minvalue == maxvalue)
    errcont ("%" PRINTBIG "", minvalue);
  else
    errcont ("%" PRINTBIG "..%" PRINTBIG "", minvalue, maxvalue);
}

void
EmitUnit (struct tUnitDef *u)
{
  struct tUnitDef *p;

  if (u == NULL)
    {
      emit (0, "(unitless)");
      return;
    }
  p = u;
  while (p != NULL)
    {
      if (p != u)
        emit (0, "*");
      if (p->Power == 1)
        emit (0, "%s", p->Name);
      else
        emit (0, "%s^%" PRINTBIG "", p->Name, p->Power);
      p = p->Next;
    }
}

void
errDumpUnit (struct tUnitDef *u)
{
  struct tUnitDef *p;

  if (u == NULL)
    {
      errcont ("(unitless)");
      return;
    }
  p = u;
  while (p != NULL)
    {
      if (p != u)
        errcont ("*");
      if (p->Power == 1)
        errcont ("%s", p->Name);
      else
        errcont ("%s^%" PRINTBIG "", p->Name, p->Power);
      p = p->Next;
    }
}

void
errShowUnit (struct tUnitDef *u)
{
  if (u == NULL)
    errcont (" (no unit)");
  else
    {
      errcont (" unit ");
      errDumpUnit (u);
    }
}

void
errDumpTypeSpec (struct tTypeSpec *t)
{
  errcont ("%s (", t->Name);
  if (t->BaseType == INTEGER_TYPE)
    {
      errcont ("range %" PRINTBIG "..%" PRINTBIG "", t->Low, t->High);
      errShowUnit (t->Unit);
    }
  else if (t->BaseType == FLOATING_TYPE)
    {
      errcont ("digits %" PRINTBIG " magnitude %" PRINTBIG "", t->Digits,
               t->Magnitude);
      errShowUnit (t->Unit);
    }
  else if (t->BaseType == ARRAY_TYPE)
    {
      errcont ("array ");
      if (t->MinDefinite)
        errcont ("[");
      else
        errcont ("<");
      errDumpReference (t->IndexType->Name);    //*****=====
      if (t->MaxDefinite)
        errcont ("]");
      else
        errcont (">");
      errcont (" of ");
      errDumpReference (t->ArrayElementType->TypeIdentifier->Name);
    }
  else if (t->BaseType == RECORD_TYPE)
    {
      struct tFieldSpec *f;

      errcont ("record");
      f = t->FieldList;
      while (f != NULL)
        {
          errcont (" %s : ", f->Name);
          errDumpReference (f->Type->TypeIdentifier->Name);
          errcont (";");
          f = f->Next;
        }
      errcont (" end record");
    }
  else if (t->BaseType == BOOLEAN_TYPE)
    {
      errcont ("boolean");
    }
  else if (t->BaseType == CSTRING_TYPE)
    {
      errcont ("string");
    }
  else if (t->BaseType == PROCEDURE_TYPE)
    {
      struct tFormalParameter *f;

      errcont ("procedure(");
      f = t->Formals;
      while (f != NULL)
        {
          errcont ("%s : ", f->Name);
          errDumpMode (f->Mode);
          errcont (" ");
          errDumpTypeSpec (f->TypeSpec);
          f = f->Next;
        }
      errcont (")");
    }
  else
    errcont ("TypeSpec:ERROR");
  errcont (")");
}

void
errDumpReference (struct tReference *r)
{
  while (r != NULL)
    {
      switch (r->ReferenceType)
        {
        case IDENTIFIER_REFERENCE:
          errcont ("%s", r->VarName);
          break;
        case ARRAY_REFERENCE:
          errcont ("[");
          errDumpExpr (r->Index);
          errcont ("]");
          break;
        case ARRAY_SLICE_REFERENCE:
          errcont ("[");
          errDumpExpr (r->Index);
          errcont ("..");
          if (r->Index2 != NULL)
            {
              errDumpExpr (r->Index2);
            }
          errcont ("]");
          break;
        case ARRAY_SLICE_LENGTH_REFERENCE:
          errcont ("[");
          errDumpExpr (r->Index);
          errcont (" for ");
          errDumpExpr (r->Index2);
          errcont ("]");
          break;
        case FIELD_REFERENCE:
          errcont (".%s", r->VarName);
          break;
        }
      r = r->Next;
    }
}

void
errDumpActualList (struct tActualParameter *a)
{
  while (a != NULL)
    {
      if (a->Name != NULL)
        errcont ("%s => ", a->Name);
      errDumpExpr (a->Expr);
      if (a->Next != NULL)
        errcont (", ");
      a = a->Next;
    }
}

void
errDumpProcedureCall (struct tProcedureCall *p)
{
  errDumpReference (p->Name);
  errcont ("(");
  if (p->ActualList != NULL)
    {
      errDumpActualList (p->ActualList);
    }
  errcont (")\n");
}

void
errDumpMode (enum tMode mode)
{
  char *s;

  switch (mode)
    {
    case MODE_IN:
      s = "in";
      break;
    case MODE_OUT:
      s = "out";
      break;
    case MODE_IN_OUT:
      s = "in out";
      break;
    case MODE_FINAL_IN_OUT:
      s = "final in out";
      break;
    case MODE_LOCAL:
      s = "(local)";
      break;
    case MODE_GLOBAL:
      s = "(global)";
      break;
    case MODE_SHARED:
      s = "(shared)";
      break;
    default:
      s = "???";
      break;
    }
  errcont (" %s ", s);
}

extern char *lexsourcefilename;

void
yyerror (const char *str)
{
  fprintf (stderr, "[%s:%d] ERROR: %s\n", lexsourcefilename, yylineno, str);
  ErrorCount++;
}

int
yywrap (void)
{
  return 1;
}

void
GenerateCallList (struct tInitialisationRecord *init)
{
  while (init != NULL)
    {
      emit (0, "  %s();\n", init->Name);
      init = init->Next;
    }
}

char *
GetStackSize (struct tStartEntry *e)
{
  unsigned stacksize;
  char buff[200];

  sprintf (buff, "__STACK_%s", e->ProcedureName);
  emit (0, "#define %s ", buff);
  stacksize = (unsigned) (e->StackSize);
  if (stacksize == 0)
    {
      if (InterruptHandlerList == NULL)
        {
          emit (0, "(sizeof(struct __stack%s) / sizeof(%s) + 1 + %u)",
                e->ProcedureName, TargetStackUnit,
                (unsigned) GlobalStackOverhead);
        }
      else
        {
          emit (0,
                "(sizeof(struct __stack%s) / sizeof(%s) + sizeof(struct __interrupt_stack) / sizeof(%s) + 1 + %u)",
                e->ProcedureName, TargetStackUnit, TargetStackUnit,
                (unsigned) GlobalStackOverhead);
        }
    }
  else
    {
      emit (0, "%u", stacksize);
    }
  emitstr ("\n");
  return strdup (buff);
}

bool StdioIncluded = FALSE;

void
IncludeStdio (void)
{
  if (!StdioIncluded) {
    SwitchToAppHeader ();
    emitstr ("#ifndef printf\n");
    emitstr ("/* Include standard IO for test builds */\n");
    emitstr ("#include <stdio.h>\n");
    emitstr ("#endif\n\n");
  }
  StdioIncluded = TRUE;
}

bool RequireTaskingInfrastructure(void)
{
  return    (StartList != NULL)                         // have subsystems
         || (CyclicTaskModel() && (!NoSystemStart))     // a cyclic model with main procedure in the system
         || HaveRestarts;
}

void WriteContikiAutostart(char *startname)
{
  const char *autoarraydef = "struct process * const autostart_processes[] = {";
  const char *autoarrayclose = ",NULL};\n\n";
  if (startname == NULL)
    {
      // must have autostarts (see calls)
      emit(0, "%s%s%s", autoarraydef, autostartlist + 1, autoarrayclose);   // skip the inital ','
    }
  else
    {
      emit(0, "%s&%s%s%s", autoarraydef, startname, autostartlist, autoarrayclose);
    }
}

bool FinalisationRequired(void)
{
  return    FinalisationHead != NULL
         || (HaveSharedVariables && (TaskImplementation == PTHREAD))
         || CompilerTest;
}

bool IntialisationRequired(void)
{
  return    InitialisationHead != NULL
         || (HaveSharedVariables && (TaskImplementation == PTHREAD))
         || UnitTest;
}

void
GenerateInitialisation (void)
{
  unsigned numroutines;
  struct tStartEntry *e;

  if (NoSystemStart)
    {
      numroutines = 0;          // no main
    }
  else
    {
      numroutines = 1;          // including the system
    }
  e = StartList;
  while (e != NULL)
    {
      numroutines++;
      e = e->Next;
    }
  if (numroutines != 0)
    {
      if ((StartList != NULL && (TaskImplementation == COROUTINE))
          || (HaveRestarts))
        {
          SwitchToAppHeader();
          emit (0, "#include <setjmp.h>\n\n");
        }
      if (HaveRestarts)
        {
          if (TaskImplementation == NO_TASKING || CyclicTaskModel())
            {
              SwitchToAppHeader();
              emit (0, "extern jmp_buf __restartjump;\n\n");
              SwitchToSource();
              emit (0, "jmp_buf __restartjump;\n\n");
            }
          else
            {
              SwitchToAppHeader();
              emit (0, "extern jmp_buf __restartjump[%u];\n\n", numroutines);
              SwitchToSource();
              emit (0, "jmp_buf __restartjump[%u];\n\n", numroutines);
            }
        }
      if (TaskImplementation == PTHREAD)
        {
          emit (0, "pthread_t __thread_id[%u];\n\n", numroutines);
        }
    }
  //
  // Write the standard header
  SwitchToAppHeader ();
//  emit (0, "#include \"%s\"\n", PrivHeaderFileName);
  if (TaskImplementation == CONTIKI)
    {
      struct tStartEntry *s;

      emitstr("\n");
      s = StartList;
      while (s != NULL)
        {
          emit(0, "PROCESS_NAME(%s);\n", s->ProcedureName);
          s = s->Next;
        }
    }
  if (StartList != NULL)
    {
      CheckTaskImplementation ("", 0);
      if ((!GlobalStackOverheadSet) && (!CyclicTaskModel()))
        {
          err ("", 0,
               "No value for subsystem stack overhead has been set: use pragma stack_overhead at a global level");
        }
      if (TaskImplementation == PTHREAD)
        {
          emitstr ("#include <limits.h>\n");
          emitstr ("#include <pthread.h>\n");
        }
    }
  emit (0, "\n");
  if ((StartList != NULL) || HaveSharedVariables)
    {
      if (TaskImplementation == PTHREAD)
        {
          emit (0, "typedef pthread_t __THREAD_ID_TYPE;\n\n");
        }
      if (!CyclicTaskModel())
        {
          emit (0, "void __acquire_locks(unsigned numlocks, ...);\n");
          emit (0, "void __reacquire_locks(unsigned numlocks, ...);\n");
          emit (0, "void __free_locks(unsigned numlocks, ...);\n");
        }
      if (TaskImplementation == COROUTINE)
        {
          emit (0, "void __coroutine_dispatch(void);\n");
          emit (0, "void __coroutine_terminate(void);\n");
          emit (0, "extern unsigned __current_coroutine;\n\n");
        }
    }
  if (HaveRestarts)
    {
      emit (0, "void __subsystem_restart(char *filename, int line);\n");
      emit (0, "char *__get_restart_file(void);\n");
      emit (0, "int __get_restart_line(void);\n");
    }
  if (TaskImplementation == CYCLIC)
    {
      emit (0, "void __resume_terminate(void);\n");
    }
  if (StringToArrayReqd)
    {
      emit (0, "void %s(char *dest, const char *src, int destlen);\n",
            StringToArrayCopy);
    }
  if (CompilerTest)
    {
      emitstr ("\n");
      emitstr ("/*\n");
      emitstr (" ***** Compiler test *****\n");
      emitstr (" */\n\n");
      IncludeStdio ();
      emitstr ("const void *__deref_check(const void *a, unsigned count, int line);\n");
      emitstr ("void __deallocation_check(unsigned count, int line);\n");
      emit (0,
            "void __range_check_unsigned(int line, %s value, %s low, %s high);\n",
            RangeCheckUnsignedCType, RangeCheckUnsignedCType,
            RangeCheckUnsignedCType);
      emit (0,
            "void __range_check_signed(int line, %s value, %s low, %s high);\n",
            RangeCheckSignedCType, RangeCheckSignedCType,
            RangeCheckSignedCType);
      emit (0, "%s __index_check(int line, %s index, %s low, %s high);\n",
            DefaultArrayIndex, DefaultArrayIndex, DefaultArrayIndex,
            DefaultArrayIndex);
      emit (0,
            "%s __length_check(int line, %s requestedlength, %s destinationlength);\n",
            DefaultArrayIndex, DefaultArrayIndex, DefaultArrayIndex);
      emitstr ("void __notnull_check(int line, const void *a);\n");
      emitstr ("void __case_error(int line);\n");
      emitstr ("void __test_assertion_failure(int line, int testnum);\n");
      emitstr ("extern long __allocated_objects;\n");
      emitstr ("\n\n");
    }
  SwitchToSource ();
  //
  if (TaskImplementation == PTHREAD)
    {
      emit (0, "pthread_attr_t __thread_attr;\n\n");
    }
  if (HaveSharedVariables
      && (TaskImplementation == PTHREAD || TaskImplementation == COROUTINE))
    {
      //
      if (TaskImplementation == PTHREAD)
        {
          emit (0, "pthread_mutex_t %smutex;\n\n", LockPrefix);
          emit (0, "pthread_cond_t %scond;\n\n", LockPrefix);
        }
      //
      emit (0, "void __acquire_locks(unsigned numlocks, ...)\n{\n");
      emit (0, "  va_list ap;\n");
      emit (0, "  unsigned j;\n");
      emit (0, "  __LOCK_TYPE_P id;\n");
      emit (0, "  int locked;\n");
      emit (0, "  __SUBSYSTEM_ID taskid;\n\n");
      if (TaskImplementation == COROUTINE)
        {
          emit (0, "  taskid = __current_coroutine;\n");
        }
      else if (TaskImplementation == PTHREAD)
        {
          emit (0, "  taskid = pthread_self();\n");
          emit (0, "  pthread_mutex_lock(&%smutex);\n", LockPrefix);
        }
      emit (0, "  do {\n");
      emit (0, "    locked = 0;\n");
      emit (0, "    va_start(ap, numlocks);\n");
      emit (0, "    for (j = 0; j < numlocks; j++) {\n");
      emit (0, "      id = va_arg(ap, __LOCK_TYPE_P);\n");
      emit (0, "      if (id->InUse) {\n");     //**** Nested locking is precluded by design && (!pthread_equal(id->ThreadID, thisthread))) {\n");
      emit (0, "        locked = 1;\n");
      emit (0, "        break;\n");
      emit (0, "      }\n");
      emit (0, "    }\n");
      emit (0, "    if (locked) {\n");
      if (UnitTest)
        {
          emit (0,
                "      printf(\"Locking must not fail under unit test builds\\n\");\n");
          emit (0, "      exit(1);\n");

        }
      if (TaskImplementation == COROUTINE)
        EmitDispatch (3);
      else if (TaskImplementation == PTHREAD)
        emit (0, "      pthread_cond_wait(&%scond, &%smutex);\n", LockPrefix);
      emit (0, "    }\n");
      emit (0, "    va_end(ap);\n");
      emit (0, "  } while (locked);\n");
      emit (0, "  va_start(ap, numlocks);\n");
      emit (0, "  for (j = 0; j < numlocks; j++) {\n");
      emit (0, "    id = va_arg(ap, __LOCK_TYPE_P);\n");
      emit (0, "    id->InUse = 1;\n");
      emit (0, "    id->LockedBy = taskid;\n");
      emit (0, "  }\n");
      if (TaskImplementation == COROUTINE)
        ;
      else if (TaskImplementation == PTHREAD)
        emit (0, "  pthread_mutex_unlock(&%smutex);\n", LockPrefix);
      emit (0, "  va_end(ap);\n");
      emit (0, "}\n\n");
      //
      emit (0, "void __reacquire_locks(unsigned numlocks, ...)\n{\n");
      emit (0, "  va_list ap;\n");
      emit (0, "  unsigned j;\n");
      emit (0, "  __LOCK_TYPE_P id;\n");
      emit (0, "  int locked;\n");
      emit (0, "  __SUBSYSTEM_ID taskid;\n\n");
      if (TaskImplementation == COROUTINE)
        {
          emit (0, "  taskid = __current_coroutine;\n");
        }
      else if (TaskImplementation == PTHREAD)
        {
          emit (0, "  taskid = pthread_self();\n");
          emit (0, "  pthread_mutex_lock(&%smutex);\n", LockPrefix);
        }
      emit (0, "  va_start(ap, numlocks);\n");
      emit (0, "  for (j = 0; j < numlocks; j++) {\n");
      emit (0, "    id = va_arg(ap, __LOCK_TYPE_P);\n");
      emit (0, "    id->InUse = 0;\n");
      emit (0, "  }\n");
      if (TaskImplementation == COROUTINE)
        EmitDispatch (1);       // must dispatch here to give something else the chance to change things
      else if (TaskImplementation == PTHREAD)
        emit (0, "  pthread_cond_wait(&%scond, &%smutex);\n", LockPrefix, LockPrefix);  // Wait on the condition, then try to get the locks again
      emit (0, "  do {\n");
      emit (0, "    locked = 0;\n");
      emit (0, "    va_start(ap, numlocks);\n");
      emit (0, "    for (j = 0; j < numlocks; j++) {\n");
      emit (0, "      id = va_arg(ap, __LOCK_TYPE_P);\n");
      emit (0, "      if (id->InUse) {\n");     //**** Nested locking is precluded by design && (!pthread_equal(id->ThreadID, thisthread))) {\n");
      emit (0, "        locked = 1;\n");
      emit (0, "        break;\n");
      emit (0, "      }\n");
      emit (0, "    }\n");
      emit (0, "    if (locked) {\n");
      if (TaskImplementation == COROUTINE)
        EmitDispatch (3);
      else if (TaskImplementation == PTHREAD)
        emit (0, "      pthread_cond_wait(&%scond, &%smutex);\n", LockPrefix,
              LockPrefix);
      emit (0, "    }\n");
      emit (0, "    va_end(ap);\n");
      emit (0, "  } while (locked);\n");
      emit (0, "  va_start(ap, numlocks);\n");
      emit (0, "  for (j = 0; j < numlocks; j++) {\n");
      emit (0, "    id = va_arg(ap, __LOCK_TYPE_P);\n");
      emit (0, "    id->InUse = 1;\n");
      emit (0, "    id->LockedBy = taskid;\n");
      emit (0, "  }\n");
      if (TaskImplementation == COROUTINE)
        ;
      else if (TaskImplementation == PTHREAD)
        emit (0, "  pthread_mutex_unlock(&%smutex);\n", LockPrefix);
      emit (0, "  va_end(ap);\n");
      emit (0, "}\n\n");
      //
      emit (0, "void __free_locks(unsigned numlocks, ...)\n{\n");
      emit (0, "  va_list ap;\n  __LOCK_TYPE_P id;\n  unsigned j;\n\n");
      emit (0, "  va_start(ap, numlocks);\n");
      if (TaskImplementation == COROUTINE)
        ;
      else if (TaskImplementation == PTHREAD)
        emit (0, "  pthread_mutex_lock(&%smutex);\n", LockPrefix);
      emit (0, "  for (j = 0; j < numlocks; j++) {\n");
      emit (0, "    id = va_arg(ap, __LOCK_TYPE_P);\n");
      emit (0, "    id->InUse = 0;\n");
      emit (0, "  }\n");
      if (TaskImplementation == COROUTINE)
        ;
      else if (TaskImplementation == PTHREAD)
        {
          emit (0, "  pthread_mutex_unlock(&%smutex);\n", LockPrefix);
          emit (0, "  pthread_cond_broadcast(&%scond);\n", LockPrefix);
        }
      emit (0, "  va_end(ap);\n");
      //emit(0, "  sched_yield();\n");
      emit (0, "}\n\n");
    }
  emit (0, "#include \"%s\"\n\n", AuxHeaderFileName);
  //
  if (InterruptHandlerList != NULL && (!CyclicTaskModel()))
    {
      // Work out an expression for the stack requirements of interrupt handlers
      struct InterruptHandlerRecord *p;
      unsigned num;
      bool havenoninterruptable;

      SwitchToAuxHeader ();
      emit (0, "struct __interrupt_stack {\n");

      num = 1;
      havenoninterruptable = FALSE;

      p = InterruptHandlerList;
      while (p != NULL)
        {
          if (p->WithInterruptsEnabled)
            {
              emit (1, "struct __stack%s int_%u;\n", p->Name, num);
              num++;
            }
          else
            {
              havenoninterruptable = TRUE;
            }
          p = p->Next;
        }
      if (havenoninterruptable)
        {
          emit (1, "union {\n");
          p = InterruptHandlerList;
          while (p != NULL)
            {
              if (!p->WithInterruptsEnabled)
                {
                  emit (2, "struct __stack%s int_%u;\n", p->Name, num);
                  num++;
                }
              p = p->Next;
            }
          emit (1, "} __with_disabled;\n");
        }
      emit (0, "};\n\n");
      SwitchToSource ();
    }
  //
  if (RequireTaskingInfrastructure())
    {
      if (TaskImplementation == PTHREAD)
        {
          unsigned routinenum;

          routinenum = 0;
          e = StartList;
          while (e != NULL)
            {
              emit (0, "void *__run_%s(void *arg)\n{\n", e->ProcedureName);
                  emit (0, "  %s();\n", e->ProcedureName);
              emit (0, "  return NULL;\n");
              emit (0, "}\n\n");
              routinenum++;
              e = e->Next;
            }
        }
      if (TaskImplementation == COROUTINE)
        {
          unsigned routinenum;
          char *stacksize;

          e = StartList;
          while (e != NULL)
            {
              emit (0, "void __subsystem_start_%s(void);\n",
                    e->ProcedureName);
              e = e->Next;
            }
          emit (0, "void __system_start_%s(void);\n\n", SystemStart);
          // For no_start, still need a jmp_buf to receive the setjmp from the system start routine
          emit (0, "jmp_buf __taskjump[%d], __exitjump;\n",
                (NoSystemStart ? numroutines + 1 : numroutines));
          emit (0, "int __coroutine_active[%d];\n\n", numroutines);
          emit (0, "unsigned __active_coroutines;\n");
          emit (0, "unsigned __current_coroutine;\n\n");
          /*
           * NOTE: use of setjmp/longjmp
           *       longjmp is guaranteed to work under C99 only when the destination is a calling function.
           *       Jumping to a function that has already exited by returning or performing a longjmp is
           *       undefined. The main issue is preservation of local variables, due in particular to the
           *       complication of register versus stack variables.
           *       This is generally not a problem if all local variables (if any) in the corresponding
           *       routine are volatile. In this case there are no local variables.
           *
           */
          emit (0, "void __coroutine_dispatch(void)\n");
          emit (0, "{\n");
          if (UnitTest)
            {
              emit (0, "  /* Do nothing under unit test builds */\n");
            }
          else
            {
              EmitPreDispatch (1);
              emit (0,
                    "  if (setjmp(__taskjump[__current_coroutine]) == 0) {\n");
              emit (0, "    if (__active_coroutines == 0) {\n");
              emit (0, "      longjmp(__exitjump, 1);\n");
              emit (0, "    }\n");
              EmitPostDispatch (2);
              emit (0, "    do {\n");
              emit (0,
                    "      __current_coroutine = (__current_coroutine + 1) %% %d;\n",
                    numroutines);
              emit (0,
                    "    } while (!__coroutine_active[__current_coroutine]);\n");
              EmitPreDispatch (2);
              emit (0, "    longjmp(__taskjump[__current_coroutine], 1);\n");
              emit (0, "  }\n");
              EmitPostDispatch (1);
            }
          emit (0, "}\n\n");
          emit (0, "void __coroutine_terminate(void)\n");
          emit (0, "{\n");
          emit (0, "  __coroutine_active[__current_coroutine] = 0;\n");
          emit (0, "  __active_coroutines--;\n");
          EmitDispatch (1);
          emit (0, "}\n\n");
          emit (0, "void __clear_subsystem_stack(%s *p, unsigned n)\n",
                TargetStackUnit);
          emit (0, "{\n");
          if (ClearTargetStack)
            {
              emit (0, "  while (n != 0) {\n");
              emit (0, "    *p = 0;\n");
              emit (0, "    p++; n--;\n");
              emit (0, "  }\n");
            }
          else
            {
              emit (0,
                    "  /* Do something with the array in order to suppress space optimisation */\n");
              emit (0, "  p[0] = p[n - 1];\n");
            }
          emit (0, "}\n\n");
          //
          e = StartList;
          routinenum = 0;
          stacksize = 0;        // Avoid a warning - actually OK
          while (e != NULL)
            {
              emit (0, "void __subsystem_start_%s(void)\n{\n",
                    e->ProcedureName);
              if (routinenum != 0)
                {
                  emit (0, "  %s n[%s];\n\n", TargetStackUnit, stacksize);
                  emit (0, "  __clear_subsystem_stack(n, %s);\n", stacksize);
                }
              EmitPreDispatch (1);
              emit (0, "  if (setjmp(__taskjump[%d]) == 0) {\n", routinenum);
              EmitPostDispatch (2);
              if (e->Next != NULL)
                {
                  emit (0, "    __subsystem_start_%s();\n",
                        e->Next->ProcedureName);
                }
              else
                {
                  emit (0, "    __system_start_%s();\n", SystemStart);
                }
              emit (0, "  }\n");
              emit (0, "  else {\n");
              EmitPostDispatch (2);
              if (HaveRestarts)
                {
                  emit (0, "    while (1) {\n");
                  EmitPreDispatch (3);
                  emit (0, "      if (setjmp(__restartjump[%d]) == 0) {\n",
                        routinenum);
                  EmitPostDispatch (4);
                  emit (0, "        %s();\n", e->ProcedureName);
                  emit (0, "        break;\n");
                  emit (0, "      }\n");
                  EmitPostDispatch (3);
                  emit (0, "    }\n");
                }
              else
                {
                  emit (0, "    %s();\n", e->ProcedureName);
                }
              emit (0, "    __coroutine_terminate();\n");
              emit (0, "  }\n");
              emit (0, "}\n\n");
              routinenum++;
              stacksize = GetStackSize (e);
              e = e->Next;
            }
          emit (0, "void __system_start_%s(void)\n{\n", SystemStart);
          emit (0, "  %s n[%s];\n", TargetStackUnit, stacksize);
          emit (0, "  unsigned j;\n\n");
          emit (0, "  __clear_subsystem_stack(n, %s);\n", stacksize);
          emit (0, "  for (j = 0; j < %u; j++) __coroutine_active[j] = 1;\n",
                numroutines);
          emit (0, "  __active_coroutines = %u;\n", numroutines);
          emit (0, "  __current_coroutine = %d;\n",
                (NoSystemStart ? numroutines : numroutines - 1));
          EmitDispatch (1);
          if (!NoSystemStart)
            {
              if (HaveRestarts)
                {
                  emit (0, "  while (1) {\n");
                  EmitPreDispatch (2);
                  emit (0, "    if (setjmp(__restartjump[%d]) == 0) {\n",
                        numroutines - 1);
                  EmitPostDispatch (3);
                  emit (0, "      %s();\n", SystemStart);
                  emit (0, "      break;\n");
                  emit (0, "    }\n");
                  EmitPostDispatch (2);
                  emit (0, "  }\n");
                }
              else
                {
                  emit (0, "  %s();\n", SystemStart);
                }
              emit (0, "  __coroutine_terminate();\n");
            }
          emit (0, "}\n\n");
          emit (0, "unsigned __get_current_routine(void)\n");
          emit (0, "{\n");
          emit (0, "  return __current_coroutine;\n");
          emit (0, "}\n\n");
        }
      //
      if (TaskImplementation == PTHREAD)
        {
          unsigned SysNum;

          emit (0, "void __start_threads(void)\n{\n");
          emit (0, "  pthread_attr_init(&__thread_attr);\n");
          emit (0,
                "  pthread_attr_setschedpolicy(&__thread_attr, SCHED_RR);\n");
          SysNum = 0;
          e = StartList;
          while (e != NULL)
            {
              char *stacksize = GetStackSize (e);
              emit (0, "  if (%s * sizeof(%s) > PTHREAD_STACK_MIN)\n",
                    stacksize, TargetStackUnit);
              emit (0,
                    "    pthread_attr_setstacksize(&__thread_attr, %s * sizeof(%s));\n",
                    stacksize, TargetStackUnit);
              emit (0,
                    "  pthread_create(&__thread_id[%u], &__thread_attr, __run_%s, NULL);\n",
                    SysNum, e->ProcedureName);
              SysNum++;
              e = e->Next;
            }
          emit (0, "}\n\n");
          emit (0, "void __wait_threads(void)\n{\n");
          SysNum = 0;
          e = StartList;
          while (e != NULL)
            {
              emit (0, "  pthread_join(__thread_id[%u], NULL);\n", SysNum);
              SysNum++;
              e = e->Next;
            }
          emit (0, "  pthread_attr_destroy(&__thread_attr);\n");
          emit (0, "}\n\n");
          //
          emit (0, "unsigned __get_current_routine(void)\n");
          emit (0, "{ __SUBSYSTEM_ID taskid = pthread_self();\n");
          emit (0, "  unsigned j;\n");
          emit (0, "  for (j = 0; j < %u; j++) {\n", numroutines);
          emit (0, "    if (pthread_equal(__thread_id[j], taskid)) {\n");
          emit (0, "      return j;\n");
          emit (0, "    }\n");
          emit (0, "  }\n");
          emit (0, "  return 0;\n");    //*****=====ERROR
          emit (0, "}\n\n");
        }
      //
      if (TaskImplementation == CYCLIC)
        {
          emit (0, "unsigned __active_tasks;\n");
          emit (0, "unsigned __current_task;\n");
          emit (0, "enum __task_state { TASK_RUNNING, TASK_TERMINATED };\n");
          emit (0, "enum __task_state __task_states[%u];\n\n", numroutines);
          emit (0, "void __resume_terminate(void)\n");
          emit (0, "{\n");
          emit (0, "  __task_states[__current_task] = TASK_TERMINATED;\n");
          emit (0, "  __active_tasks--;\n");
          emit (0, "}\n\n");
        }
      //
      if (TaskImplementation == CONTIKI)
        {
          SwitchToAppHeader();
          emit (0, "extern struct process *__process_ids[%u];\n\n", numroutines);
          SwitchToSource();
          emit (0, "struct process *__process_ids[%u];\n\n", numroutines);
          emit (0, "unsigned __get_current_task(void)\n");
          emit (0, "{ struct process *processid = PROCESS_CURRENT();\n");
          emit (0, "  unsigned j;\n");
          emit (0, "  for (j = 0; j < %u; j++) {\n", numroutines);
          emit (0, "    if (__process_ids[j] == processid) {\n");
          emit (0, "      return j;\n");
          emit (0, "    }\n");
          emit (0, "  }\n");
          emit (0, "  return 0;\n");    //*****=====ERROR
          emit (0, "}\n\n");
        }
      //
      //
      //
      if (HaveRestarts)
        {
          if (HaveSharedVariables
              && (TaskImplementation == COROUTINE
                  || TaskImplementation == PTHREAD))
            {
              emit (0,
                    "void __subsystem_clear_lock(__LOCK_TYPE_P p, __SUBSYSTEM_ID taskid)\n");
              emit (0, "{\n");
              if (TaskImplementation == PTHREAD)
                {
                  emit (0,
                        "  if (p->InUse && pthread_equal(p->LockedBy, taskid)) p->InUse = 0;\n");
                }
              else
                {
                  emit (0,
                        "  if (p->InUse && p->LockedBy == taskid) p->InUse = 0;\n");
                }
              emit (0, "}\n\n");
            }
          //
          {
            unsigned j;
            emit (0, "char *__restart_file[%u] = {\n  ", numroutines);
            for (j = 1; j < numroutines; j++)
              {
                emit (0, "NULL, ");
              }
            emit (0, "NULL\n};\n\n");
            emit (0, "int __restart_line[%u] = {\n  ", numroutines);
            for (j = 1; j < numroutines; j++)
              {
                emit (0, "0, ");
              }
            emit (0, "0\n};\n\n");
          }
          emit (0, "void __subsystem_restart(char *filename, int line)\n");
          emit (0, "{\n");
          if (TaskImplementation == PTHREAD)
            {
              emit (0, "  __SUBSYSTEM_ID taskid = pthread_self();\n");
            }
          if (HaveSharedVariables
              && (TaskImplementation == COROUTINE
                  || TaskImplementation == PTHREAD))
            {
              if (TaskImplementation == COROUTINE)
                {
                  emit (0,
                        "  __SUBSYSTEM_ID taskid = __current_coroutine;\n");
                }
              // Clear all locks held by this (sub)system
              if (TaskImplementation == PTHREAD)
                {
                  emit (0, "  pthread_mutex_lock(&%smutex);\n", LockPrefix);
                }
              {
                struct tLockVarListEntry *p = AllLockVars;
                while (p != NULL)
                  {
                    emit (0, "  __subsystem_clear_lock(&%s%s, taskid);\n",
                          LockPrefix, p->Name);
                    p = p->Next;
                  }
              }
              if (TaskImplementation == COROUTINE)
                ;
              else if (TaskImplementation == PTHREAD)
                {
                  emit (0, "  pthread_mutex_unlock(&%smutex);\n", LockPrefix);
                  emit (0, "  pthread_cond_broadcast(&%scond);\n",
                        LockPrefix);
                }
            }
          //
          if (TaskImplementation == COROUTINE)
            {
              emit (0, "  __restart_file[__current_coroutine] = filename;\n");
              emit (0, "  __restart_line[__current_coroutine] = line;\n");
              EmitPreDispatch (1);
              emit (0, "  longjmp(__restartjump[__current_coroutine], 2);\n");
            }
          else if (TaskImplementation == PTHREAD)
            {
              emit (0,
                    "  { unsigned current_routine = __get_current_routine();\n");
              emit (0, "    __restart_file[current_routine] = filename;\n");
              emit (0, "    __restart_line[current_routine] = line;\n");
              EmitPreDispatch (2);
              emit (0, "    longjmp(__restartjump[current_routine], 2);\n");
              emit (0, "  }\n");
            }
          else if (TaskImplementation == NO_TASKING)
            {
              emit (0, "  __restart_file[0] = filename;\n");
              emit (0, "  __restart_line[0] = line;\n");
              EmitPreDispatch (1);
              emit (0, "  longjmp(__restartjump, 2);\n");
            }
          else if (CyclicTaskModel())
            {
              if (TaskImplementation == CONTIKI)
                {
                  emit(0, "  unsigned __current_task = __get_current_task();\n");
                }
              emit (0, "  __restart_file[__current_task] = filename;\n");
              emit (0, "  __restart_line[__current_task] = line;\n");
              EmitPreDispatch (1);
              emit (0, "  longjmp(__restartjump, 2);\n");
            }
          emit (0, "}\n\n");
          //
          emit (0, "char *__get_restart_file(void)\n");
          emit (0, "{\n");
          if (TaskImplementation == COROUTINE)
            {
              emit (0, "  return __restart_file[__current_coroutine];\n");
            }
          else if (TaskImplementation == PTHREAD)
            {
              emit (0, "  return __restart_file[__get_current_routine()];\n");
            }
          else if (TaskImplementation == CONTIKI)
            {
              emit (0, "  return __restart_file[__get_current_task()];\n");
            }
          else if (TaskImplementation == NO_TASKING)
            {
              emit (0, "  return __restart_file[0];\n");
            }
          else if (CyclicTaskModel())
            {
              emit (0, "  return __restart_file[__current_task];\n");
            }
          emit (0, "}\n\n");
          //
          emit (0, "int __get_restart_line(void)\n");
          emit (0, "{\n");
          if (TaskImplementation == COROUTINE)
            {
              emit (0, "  return __restart_line[__current_coroutine];\n");
            }
          else if (TaskImplementation == PTHREAD)
            {
              emit (0, "  return __restart_line[__get_current_routine()];\n");
            }
          else if (TaskImplementation == CONTIKI)
            {
              emit (0, "  return __restart_line[__get_current_task()];\n");
            }
          else if (TaskImplementation == NO_TASKING)
            {
              emit (0, "  return __restart_line[0];\n");
            }
          else if (CyclicTaskModel())
            {
              emit (0, "  return __restart_line[__current_task];\n");
            }
          emit (0, "}\n\n");
        }
    }
  if (IntialisationRequired())
    {
      emitstr ("void __app_initialisation(void)\n");
      emitstr ("{\n");
      if (UnitTest)
        {
          emitstr ("  { unsigned j;\n");
          emit (0, "    for (j = 0; j < %u; j++) __coverage[j] = 0;\n",
                BranchNumber);
          emitstr ("  }\n");
        }
      if (HaveSharedVariables)
        {
          if (TaskImplementation == PTHREAD)
            {
              emit (0, "  pthread_mutex_init(&%smutex, NULL);\n", LockPrefix);
              emit (0, "  pthread_cond_init(&%scond, NULL);\n", LockPrefix);
            }
        }                           // then...
      GenerateCallList (InitialisationHead);
      emitstr ("}\n\n");
    }
  //
  if (!UnitTest)
    {
      if (TaskImplementation != CONTIKI)
        {
          emitstr ("void __app_run(void)\n");
          emitstr ("{\n");
        }
      if (RequireTaskingInfrastructure())
        {
          if (TaskImplementation == COROUTINE)
            {
              EmitPreDispatch (1);
              emitstr ("  if (setjmp(__exitjump) == 0) {\n");
              EmitPostDispatch (2);
              emit (0, "    __subsystem_start_%s();\n",
                    StartList->ProcedureName);
              emitstr ("  }\n");
              EmitPostDispatch (1);
            }
          else if (TaskImplementation == PTHREAD)
            {
              emit (0, "  __start_threads();\n");
            }
          else if (TaskImplementation == CYCLIC)
            {
              struct tStartEntry *s;
              unsigned tasknum;
              int depth = 1;

              for (tasknum = 0; tasknum < numroutines; tasknum++)
                {
                  emit (depth, "__task_states[%u] = TASK_RUNNING;\n",
                        tasknum);
                }
              emit (depth, "__active_tasks = %u;\n", numroutines);
              emit (depth, "do {\n");
              if (!NoSystemStart)
                {
                  emit (depth + 1,
                        "if (__task_states[0] == TASK_RUNNING) {\n");
                  emit (depth + 2, "__current_task = 0;\n");
                  emit (depth + 2, "%s();\n", SystemStart);
                  emit (depth + 1, "}\n");
                }
              s = StartList;
              if (NoSystemStart)
                {
                  tasknum = 0;
                }
              else
                {
                  tasknum = 1;
                }
              while (s != NULL)
                {
                  emit (depth + 1,
                        "if (__task_states[%u] == TASK_RUNNING) {\n",
                        tasknum);
                  emit (depth + 2, "__current_task = %u;\n", tasknum);
                  emit (depth + 2, "%s();\n", s->ProcedureName);
                  emit (depth + 1, "}\n");
                  tasknum++;
                  s = s->Next;
                }
              emit (depth, "} while (__active_tasks != 0);\n");
            }
          else if (TaskImplementation == CONTIKI)
            {
              // Under Contiki the main routine is part of the platform code
              // Create a Contiki process to initialise and start up our tasks
              if (NoSystemStart)
                {
                  // No system startup routine has been written; we need to create one here
                  // to start up the subsystems
                  emitstr("PROCESS(__app_start, \"__app_start\");\n");
                  emitstr("PROCESS_THREAD(__app_start, ev, data)\n{\n");
                  WriteContikiStartupCode();
                  emitstr("  return PT_ENDED;\n");
                }
            }
        }
      if ((!CyclicTaskModel())
          && (StartList == NULL || TaskImplementation != COROUTINE))
        {
          if (!NoSystemStart)
            {
              if (TaskImplementation == PTHREAD)
                {
                  emit (0, "  __thread_id[%u] = pthread_self();\n",
                        numroutines - 1);
                }
              if (HaveRestarts)
                {
                  emit (0, "  while (1) {\n");
                  EmitPreDispatch (2);
                  if (TaskImplementation == NO_TASKING)
                    {
                      emit(0, "    if (setjmp(__restartjump) == 0) {\n");
                    }
                  else
                    {
                      emit (0, "    if (setjmp(__restartjump[%u]) == 0) {\n",
                            numroutines - 1);
                    }
                  EmitPostDispatch (3);
                  emit (0, "      %s();\n", SystemStart);
                  emit (0, "      break;\n");
                  emit (0, "    }\n");
                  EmitPostDispatch (2);
                  emit (0, "  }\n");
                }
              else
                {
                  emit (0, "  %s();\n", SystemStart);
                }
            }
        }
      //
      if (StartList != NULL)
        {
          if (TaskImplementation == PTHREAD)
            {
              emit (0, "  __wait_threads();\n");
            }
        }                       // then...

      if (   TaskImplementation != CONTIKI
          || (TaskImplementation == CONTIKI && (StartList != NULL) && NoSystemStart))
        {
          emitstr ("}\n\n");
        }
        // Otherwise we won't have written a new function here
      //
      if (TaskImplementation == CONTIKI)
        {
          // Write the autostart macro
          if ((StartList != NULL) && NoSystemStart)
            {
              WriteContikiAutostart("__app_start");
            }
          else if (!NoSystemStart)
            {
              WriteContikiAutostart(SystemStart);
            }
          else if (haveautostarts)
            {
              WriteContikiAutostart(NULL);
            }
        }
      //
    }
  //
  if (FinalisationRequired())
    {
      emitstr ("void __app_finalisation(void)\n");
      emitstr ("{\n");
      GenerateCallList (FinalisationHead);  // then...
      if (HaveSharedVariables)
        {
          if (TaskImplementation == PTHREAD)
            {
              emit (0, "  pthread_mutex_destroy(&%smutex);\n", LockPrefix);
              emit (0, "  pthread_cond_destroy(&%scond);\n", LockPrefix);
            }
        }
      if (CompilerTest)
        {
          emitstr ("  if (__allocated_objects != 0) {\n");
          emitstr
            ("    printf(\"***** Remaining allocated objects %ld is not zero\\n\", __allocated_objects);\n");
          emitstr ("    exit(1);\n");
          emitstr ("  }\n");
        }
      emitstr ("}\n\n");
    }
  //
  if (UnitTest)
    {
      struct tUnitTest *u;
      unsigned n;

      emit (0,
            "struct __tcoverage_info { char *FileName; int Line; } __coverage_info[%u] = {\n",
            BranchNumber);
      for (n = 0; n < BranchNumber - 1; n++)
        {
          emit (0, "  {\"%s\", %d},\n", CoverageMap[n].FileName,
                CoverageMap[n].Line);
        }
      emit (0, "  {\"%s\", %d}\n};\n\n",
            CoverageMap[BranchNumber - 1].FileName,
            CoverageMap[BranchNumber - 1].Line);

      emit (0, "unsigned char __coverage[%u];\n\n", BranchNumber);

      emitstr ("unsigned __unit_test_failures = 0;\n");

      IncludeStdio ();
      SwitchToSource ();

      emitstr
        ("void __unit_test_report(int testline, char *name, char *filename, int fileline, int firstbranch, int lastbranch)\n");
      emitstr ("{\n");
      emitstr ("  printf(\"%s (%s:%d) \", name, filename, fileline);\n");
      emitstr ("  if (testline != 0) {\n");
      emitstr ("    __unit_test_failures++;\n");
      emitstr ("    printf(\"failed at line %d *****\\n\", testline);\n");
      emitstr ("  }\n");
      emitstr ("  else {\n");
      emitstr ("    int numerrs = 0, j;\n");
      emitstr ("    for (j = firstbranch; j <= lastbranch; j++) {\n");
      emitstr ("      if (__coverage[j] == 0) {\n");
      emitstr ("        numerrs++;\n");
      emitstr
        ("        printf(\"\\n  Branch at %s:%d was not tested *****\", __coverage_info[j].FileName, __coverage_info[j].Line);\n");
      emitstr ("      }\n");
      emitstr ("    }\n");
      emitstr ("    if (numerrs == 0)\n");
      emitstr ("      printf(\"OK\\n\");\n");
      emitstr ("    else\n");
      emitstr ("      printf(\"\\n  Incomplete coverage\\n\");\n");
      emitstr ("  }\n");
      emitstr ("}\n\n");
      //
      emitstr ("void __app_unit_tests()\n");
      emitstr ("{\n");
      emitstr ("  unsigned j, cov = 0, nocov = 0;\n");
      emitstr ("  printf(\"Running unit tests...\\n\\n\");\n");
      u = UnitTestHead;
      while (u != NULL)
        {
          emit (0,
                "  __unit_test_report(%s%s(), \"%s\", \"%s\", %d, %u, %u);\n",
                getprocname (u->ProcType), UNIT_TEST_SUFFIX,
                u->ProcType->Name, u->FileName, u->Line,
                u->ProcType->FirstBranch, u->ProcType->LastBranch);
          u = u->Next;
        }
      emitstr ("  printf(\"\\nUnit tests completed\\n\");\n");
      emitstr ("  printf(\"\\nOverall coverage:\\n\\n\");\n");
      emit (0, "  for (j = 0; j < %u; j++) {\n", BranchNumber);
      emitstr ("    if (__coverage[j] == 0) {\n");
      emitstr ("      nocov++;\n");
      emitstr
        ("      printf(\"Branch at %s:%d was not exercised\\n\", __coverage_info[j].FileName, __coverage_info[j].Line);\n");
      emitstr ("    }\n");
      emitstr ("    else {\n");
      emitstr ("      cov++;\n");
      emitstr ("    }\n");
      emitstr ("  }\n");
      emitstr
        ("  printf(\"\\nBranches covered %u, not covered %u, percent covered %3.2f, test failures %u\\n\", cov, nocov, 100.0 * cov / (cov + nocov), __unit_test_failures);\n");
      emitstr ("}\n\n");
    }
  //
  if (GenerateMain)
    {
      emitstr ("\n");
      emitstr ("int main(void)\n");
      emitstr ("{\n");
      if (IntialisationRequired())
        {
          emitstr ("  __app_initialisation();\n");
        }
      if (UnitTest)
        {
          emitstr ("  __app_unit_tests();\n");
        }
      else
        {
          emitstr ("  __app_run();\n");
        }
      if (FinalisationRequired())
        {
          emitstr ("  __app_finalisation();\n");
        }
      emitstr ("  return 0;\n");
      emitstr ("}\n");
    }
}

void
ShowShares (struct tVar *v)
{
  struct tGlobalAccess *gs;

  if (v->Mode == MODE_SHARED)
    {
      emit (0, "/* Locks of shared variable %s are followed by locks of:",
            v->Name);
      gs = v->ShareList;
      while (gs != NULL)
        {
          emit (0, " %s", gs->Var->Name);
          gs = gs->Next;
        }
      emit (0, " */\n");
    }
}

bool
CheckDeadlock (struct tVar *v)
{
  struct tGlobalAccess *gs, *gt;
  bool lockerror;

  lockerror = FALSE;
  if (v->Mode == MODE_SHARED)
    {
      gs = v->ShareList;
      while (gs != NULL)
        {
          gt = gs->Var->ShareList;
          while (gt != NULL)
            {
              if (gt->Var == v)
                {
                  // Check for unchecked_deadlock(v, gs->var) or vice-versa
                  struct tVarPair *p;
                  bool ignore, notify;

                  p = UncheckedDeadlockList;
                  ignore = FALSE;
                  while (p != NULL)
                    {
                      if ((p->Var1 == v && p->Var2 == gs->Var)
                          || (p->Var1 == gs->Var && p->Var2 == v))
                        {
                          ignore = TRUE;
                          notify = !p->Notified;
                          p->Notified = TRUE;
                          break;
                        }
                      p = p->Next;
                    }
                  if (ignore)
                    {
                      if (notify)
                        {
                          warning ("", 0,
                                "Potential deadlock between shares of %s (declared at line %d of %s) and %s (declared at line %d of %s) ignored due to pragma",
                                v->Name, v->Line, v->FileName, gs->Var->Name,
                                gs->Var->Line, gs->Var->FileName);
                          if (Annotate)
                            {
                              emit (0,
                                    "/* Potential deadlock of %s and %s ignored due to pragma */\n",
                                    v->Name, gs->Var->Name);
                            }
                        }
                    }
                  else
                    {
                      err_and_continue ("", 0,
                                        "Potential deadlock between shares of %s (declared at line %d of %s) and %s (declared at line %d of %s)",
                                        v->Name, v->Line, v->FileName,
                                        gs->Var->Name, gs->Var->Line,
                                        gs->Var->FileName);
                      lockerror = TRUE;
                    }
                }
              gt = gt->Next;
            }
          gs = gs->Next;
        }
    }
  return lockerror;
}

// Check for shares that might deadlock
void
CheckShareLists (void)
{
  struct tVar *p;
  int j;
  bool lockerror;

  if (Annotate)
    {
      j = 0;
      while (j < CurrentVars->NumVars)
        {
          ShowShares (CurrentVars->Vars[j]);
          j++;
        }
      p = PublicVarList;
      while (p != NULL)
        {
          ShowShares (p);
          p = p->PrivatePublicNext;
        }
      p = PrivateVarList;
      while (p != NULL)
        {
          ShowShares (p);
          p = p->PrivatePublicNext;
        }
      emit (0, "\n");
    }
  // Check the current scope and public and private vars
  lockerror = FALSE;
  j = 0;
  while (j < CurrentVars->NumVars)
    {
      lockerror = lockerror || CheckDeadlock (CurrentVars->Vars[j]);
      j++;
    }
  p = PublicVarList;
  while (p != NULL)
    {
      lockerror = lockerror || CheckDeadlock (p);
      p = p->PrivatePublicNext;
    }
  p = PrivateVarList;
  while (p != NULL)
    {
      lockerror = lockerror || CheckDeadlock (p);
      p = p->PrivatePublicNext;
    }
  if (lockerror)
    {
      longjmp (error_recovery, 1);
    }
}

void
Usage (void)
{
  printf ("usage: rihtan [options] system_file\n");
  printf ("-a  analyse only - do not generate code\n");
  printf ("-g  suppress global usage errors - for intermediate builds\n");
  printf ("-v  verbose build\n");
  printf ("-i  display information about assertions\n");
  printf ("-tc translate for compiler and assertion test\n");
  printf ("-tu build for unit tests\n");
  printf ("-nl line numbers at left\n");
  printf ("-ni line numbers inline\n");
  printf ("-nc include source comments in the output (1)\n");
  printf ("-na add annotations\n");
  printf ("-ns:prefix\n");
  printf
    ("    sets the outer level prefix for generated symbols (default none)\n");
  printf ("-x  display extra information when an error is found\n");
  printf ("-c:configname=filename\n");
  printf ("    names the file that is used for separate(\"@configname\")\n");
  printf ("-fx write a source index file\n");
  printf ("-o:prefix\n");
  printf
    ("    sets a path and name prefix for output files (default none)\n");
  printf ("-oi write #include declarations for separate C files\n");
  printf ("(1) Comments that start with //- are excluded\n");
  printf
    ("\n-------------------------------------------------------------------------------\n");
  printf("Rihtan  Copyright (C) 2015  William Carney\n");
  printf("Licensed under the Apache License, Version 2.0 (the \"License\");\n");
  printf("you may not use this file except in compliance with the License.\n");
  printf("You may obtain a copy of the License at\n");
  printf("    http://www.apache.org/licenses/LICENSE-2.0\n");
  printf("Unless required by applicable law or agreed to in writing, software\n");
  printf("distributed under the License is distributed on an \"AS IS\" BASIS,\n");
  printf("WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.\n");
  printf("See the License for the specific language governing permissions and\n");
  printf("limitations under the License.\n");
  printf
    ("-------------------------------------------------------------------------------\n\n");
  exit (1);
}

char *NOT_DEFINED = "__Not_Defined__";

int
main (int argc, char **argv)
{
  //struct tType *booltype, *chartype;
  struct tTypeDeclaration typedecl;
  struct tUnitDeclaration unitdecl;
//      struct tTypeSpec *typespec;
  char *infilename = NULL;      //, *sourcefilename, *headerfilename;
  int argnum;
  char *baseoutputname, *s;

  CommandArgv = argv + 1;
  CommandArgc = argc - 1;
  for (argnum = 1; argnum < argc; argnum++)
    {
      char *arg = argv[argnum];
      
      if (arg[0] == '-')
        {
          if (!strcmp (arg, "-a"))
            {
              GenerateOutput = FALSE;
            }
          else if (!strcmp (arg, "-g"))
            {
              SuppressGlobalUsageChecks = TRUE;
            }
          else if (!strcmp (arg, "-v"))
            {
              VerboseBuild = TRUE;
            }
          else if (!strcmp (arg, "-i"))
            {
              VerboseAssertions = TRUE;
            }
          else if (!strcmp (arg, "-tc"))
            {
              CompilerTest = TRUE;
            }
          else if (!strcmp (arg, "-tu"))
            {
              UnitTest = TRUE;
            }
          else if (!strcmp (arg, "-nl"))
            {
              OuterDepth = 1;
              NumbersLeft = TRUE;
            }
          else if (!strcmp (arg, "-ni"))
            {
              OuterDepth = 0;
              NumbersInline = TRUE;
            }
          else if (!strcmp (arg, "-nc"))
            {
              RecordComments = TRUE;
            }
          else if (!strcmp (arg, "-na"))
            {
              Annotate = TRUE;
            }
          else if (!strncmp (arg, "-ns:", 4))
            {
              SystemPrefix = strdup (arg + 4);
            }
          else if (!strcmp (arg, "-x"))
            {
              ExtendedErrorInformation = TRUE;
            }
          else if (!strncmp (arg, "-c:", 3))
            {
              ;                 /* processed in separate declarations */
            }
          else if (!strcmp(arg, "-fx"))
            {
              WriteIndexFile = TRUE;
            }
          else if (!strncmp(arg, "-o:", 3))
            {
              GlobalOutputPrefix = strdup (arg + 3);
            }
          else if (!strcmp(arg, "-oi"))
            {
              IncludeSeparateCFiles = TRUE;
            }
          else if (!strcmp(arg, "--help"))
            {
              Usage ();
            }
           else
            {
              printf ("Unexpected parameter: '%s'\n\n", arg);
              Usage ();
            }
        }
      else
        {
          infilename = arg;
        }
    }
  if (infilename == NULL)
    Usage ();
  //
  localsbuff = checked_malloc (INITIAL_LOCALS_SIZE);
  callsbuff = checked_malloc (INITIAL_LOCALS_SIZE);
  //
  yyin = fopen (infilename, "r");
  if (yyin == NULL)
    {
      printf ("Cannot open %s\n", infilename);
      exit (2);
    }
  if (WriteIndexFile)
    {
      IndexFile = fopen("source_index.txt", "w");
      if (IndexFile == NULL)
        {
          printf("Cannot create index file\n");
          exit(2);
        }
    }
  s = infilename + strlen (infilename) - 1;
  while (s > infilename && *s != '/' && *s != '\\')
    s--;
  if (*s == '/' || *s == '\\')
    s++;
  lexsourcefilename = strdup (s);
  baseoutputname = strdup (s);
  s = baseoutputname + strlen (baseoutputname) - 1;
  while (s > baseoutputname && *s != '.')
    s--;
  *s = 0;
  autostartlist = strdup("");
  //
  OpenOutput (baseoutputname, ".c", FALSE, &SourceFile, &SourceFileName,
              FALSE, FALSE);
  OpenOutput (baseoutputname, ".h", TRUE, &HeaderFile, &HeaderFileName, FALSE,
              FALSE);
  OpenOutput (baseoutputname, "_aux.h", TRUE, &AuxHeaderFile,
              &AuxHeaderFileName, TRUE, TRUE);
  OpenOutput (baseoutputname, "_app.h", TRUE, &AppHeaderFile,
              &AppHeaderFileName, TRUE, TRUE);
  OpenOutput (baseoutputname, "_cond.h", TRUE, &CondHeaderFile,
              &CondHeaderFileName, TRUE, TRUE);
  SwitchToAppHeader ();
  if (UnitTest)
    {
      emitstr ("#ifndef __UNIT_TEST\n");
      emitstr ("#define __UNIT_TEST\n");
      emitstr ("#endif\n\n");
    }
  emitstr ("#include <stdlib.h>\n");
  emitstr ("#include <string.h>\n");
  emitstr ("#include <stdarg.h>\n");
  emitstr ("\n");
  emit (0, "#include \"%s\"\n\n", CondHeaderFileName);

  emitstr ("void __app_initialisation(void);\n");
  emitstr ("void __app_run(void);\n");
  emitstr ("void __app_finalisation(void);\n\n");
  SwitchToSource ();

  //
  strcpy (globalprefix, "");

  CurrentVars = MallocVarSet ();

  CurrentVars->NumVars = 0;
  CurrentVars->NumRelations = 0;

  //
  // At least one var table entry must be created here for loops above to work
  //

  NO_COMPUTATION = checked_malloc (sizeof *NO_COMPUTATION);
  NO_COMPUTATION->CName = "ERROR!";

  // Create the type 'boolean'
  BooleanTypespec = NewTypespec (0);
  BooleanTypespec->BaseType = BOOLEAN_TYPE;
  BooleanTypespec->Unit = NULL;
  BooleanTypespec->CType = NULL;        // Will be filled in when a suitable target type is defined
  BooleanTypespec->CName = NOT_DEFINED;
  BooleanTypespec->Low = 0;
  BooleanTypespec->High = 1;
  BooleanTypespec->Name = "boolean";    // Set this here for the sake of error messages
  typedecl.Name = "boolean";
  typedecl.TypeSpec = BooleanTypespec;
  AddTypeSymbol (0, "", 0, &typedecl, BooleanTypespec, PUBLIC_ACCESS,
                 &BooleanType, &BooleanVar);
  // Create the type 'character'
  CharacterTypespec = NewTypespec (0);
  CharacterTypespec->BaseType = INTEGER_TYPE;
  unitdecl.Line = 0;
  unitdecl.UnitName = MakeUniqueName ("character");
  unitdecl.unit = NULL;
  CharacterUnit = AnalyseAddUnit (1, "", 0, &unitdecl, PUBLIC_ACCESS);
  CharacterTypespec->Unit = CharacterUnit;
  typedecl.Name = "character";
  typedecl.TypeSpec = CharacterTypespec;
  CharacterTypespec->Low = 0;
  CharacterTypespec->High = 127;
  CharacterTypespec->CType = NULL;      // Will be filled in when a suitable target type is defined
  CharacterTypespec->CName = NOT_DEFINED;
  CharacterTypespec->Name = "character";
  AddTypeSymbol (0, "", 0, &typedecl, CharacterTypespec, PUBLIC_ACCESS,
                 &CharacterType, &CharacterVar);
  // Create the type 'universal_index'
  UniversalIndexTypespec = NewTypespec (0);
  UniversalIndexTypespec->BaseType = INTEGER_TYPE;
  UniversalIndexTypespec->Unit = NULL;
  typedecl.Name = "universal_index";
  typedecl.TypeSpec = UniversalIndexTypespec;
  UniversalIndexTypespec->Low = 0;
  UniversalIndexTypespec->High = 1;
  UniversalIndexTypespec->CType = NULL; // Will be filled in when a suitable target type is defined
  UniversalIndexTypespec->CName = NOT_DEFINED;
  UniversalIndexTypespec->Name = "universal_index";
  AddTypeSymbol (0, "", 0, &typedecl, UniversalIndexTypespec, PUBLIC_ACCESS,
                 &UniversalIndexType, &UniversalIndexVar);
  // Create the type 'cstring'
  CstringTypespec = NewTypespec (0);
  CstringTypespec->BaseType = CSTRING_TYPE;
  CstringTypespec->Unit = NULL;
  typedecl.Name = "string";
  typedecl.TypeSpec = CstringTypespec;
  CstringType = AnalyseAddType (0, "", 0, &typedecl, PUBLIC_ACCESS);
  // Create the type 'address'
  AddressTypespec = NewTypespec (0);
  AddressTypespec->BaseType = ADDRESS_TYPE;
  AddressTypespec->Unit = NULL;
  AddressTypespec->ElementTypeSpec = NULL;
  AddressTypespec->Low = 0;
  AddressTypespec->High = 1;
  typedecl.Name = "address";
  typedecl.TypeSpec = AddressTypespec;
  AddressType = AnalyseAddType (0, "", 0, &typedecl, PUBLIC_ACCESS);

  ConstantIntegerTypespec = NewTypespec (0);
  ConstantIntegerTypespec->BaseType = INTEGER_TYPE;
  ConstantIntegerTypespec->CName = NULL;
  ConstantIntegerTypespec->CType = NULL;
  ConstantIntegerTypespec->Controlled = FALSE;
  ConstantIntegerTypespec->High = 0;
  ConstantIntegerTypespec->HighExpr = NULL;
  ConstantIntegerTypespec->Low = 0;
  ConstantIntegerTypespec->LowExpr = NULL;
  ConstantIntegerTypespec->Line = 0;
  ConstantIntegerType.Name = "(constant integer)";
  ConstantIntegerType.NameAccess = PRIVATE_ACCESS;
  ConstantIntegerType.TypeSpec = ConstantIntegerTypespec;
  ConstantIntegerType.Next = NULL;
  //
  ConstantFloatTypespec = NewTypespec (0);
  ConstantFloatTypespec->BaseType = FLOATING_TYPE;
  ConstantFloatTypespec->CName = NULL;
  ConstantFloatTypespec->CType = NULL;
  ConstantFloatTypespec->Controlled = FALSE;
  ConstantFloatTypespec->High = 0;
  ConstantFloatTypespec->HighExpr = NULL;
  ConstantFloatTypespec->Low = 0;
  ConstantFloatTypespec->LowExpr = NULL;
  ConstantFloatTypespec->Line = 0;
  ConstantFloatType.Name = "(constant float)";
  ConstantFloatType.NameAccess = PRIVATE_ACCESS;
  ConstantFloatType.TypeSpec = ConstantFloatTypespec;
  ConstantFloatType.Next = NULL;
  //
  ConstantBooleanTypespec = NewTypespec (0);
  ConstantBooleanTypespec->BaseType = BOOLEAN_TYPE;
  ConstantBooleanTypespec->CName = NULL;
  ConstantBooleanTypespec->CType = NULL;
  ConstantBooleanTypespec->Controlled = FALSE;
  ConstantBooleanTypespec->High = 0;
  ConstantBooleanTypespec->HighExpr = NULL;
  ConstantBooleanTypespec->Low = 0;
  ConstantBooleanTypespec->LowExpr = NULL;
  ConstantBooleanTypespec->Line = 0;
  ConstantBooleanType.Name = "(constant boolean)";
  ConstantBooleanType.NameAccess = PRIVATE_ACCESS;
  ConstantBooleanType.TypeSpec = ConstantBooleanTypespec;
  ConstantBooleanType.Next = NULL;
  //
  ConstantAccessTypespec = NewTypespec (0);
  ConstantAccessTypespec->BaseType = ACCESS_TYPE;
  ConstantAccessTypespec->CName = NULL;
  ConstantAccessTypespec->CType = NULL;
  ConstantAccessTypespec->Controlled = FALSE;
  ConstantAccessTypespec->High = 0;
  ConstantAccessTypespec->HighExpr = NULL;
  ConstantAccessTypespec->Low = 0;
  ConstantAccessTypespec->LowExpr = NULL;
  ConstantAccessTypespec->Line = 0;
  ConstantAccessType.Name = "(constant access)";
  ConstantAccessType.NameAccess = PRIVATE_ACCESS;
  ConstantAccessType.TypeSpec = ConstantAccessTypespec;
  ConstantAccessType.Next = NULL;
  //
  ConstantStringTypespec = NewTypespec (0);
  ConstantStringTypespec->BaseType = CSTRING_TYPE;
  ConstantStringTypespec->CName = NULL;
  ConstantStringTypespec->CType = NULL;
  ConstantStringTypespec->Controlled = FALSE;
  ConstantStringTypespec->High = 0;
  ConstantStringTypespec->HighExpr = NULL;
  ConstantStringTypespec->Low = 0;
  ConstantStringTypespec->LowExpr = NULL;
  ConstantStringTypespec->Line = 0;
  ConstantStringType.Name = "(constant string)";
  ConstantStringType.NameAccess = PRIVATE_ACCESS;
  ConstantStringType.TypeSpec = ConstantStringTypespec;
  ConstantStringType.Next = NULL;
  //
  UnitTypespec = NewTypespec (0);
  UnitTypespec->BaseType = UNIT_TYPE;
  UnitTypespec->CName = NOT_DEFINED;
  UnitTypespec->CType = NULL;
  UnitTypespec->Controlled = FALSE;
  UnitTypespec->High = 0;
  UnitTypespec->HighExpr = NULL;
  UnitTypespec->Low = 0;
  UnitTypespec->LowExpr = NULL;
  UnitTypespec->Line = 0;
  UnitTypespec->Unit = NULL;
  UnitTypespec->Name = "unit";
  UnitType.Name = "(unit)";
  UnitType.NameAccess = PRIVATE_ACCESS;
  UnitType.TypeSpec = UnitTypespec;
  UnitType.Next = NULL;
  //
  SeparateBlockTypespec = NewTypespec (0);
  SeparateBlockTypespec->BaseType = SEPARATE_BLOCK_TYPE;
  SeparateBlockTypespec->CName = NOT_DEFINED;
  SeparateBlockTypespec->CType = NULL;
  SeparateBlockTypespec->Controlled = FALSE;
  SeparateBlockTypespec->High = 0;
  SeparateBlockTypespec->HighExpr = NULL;
  SeparateBlockTypespec->Low = 0;
  SeparateBlockTypespec->LowExpr = NULL;
  SeparateBlockTypespec->Line = 0;
  SeparateBlockTypespec->Unit = NULL;
  SeparateBlockTypespec->Name = "separate_block";
  SeparateBlockType.Name = "(separate_block)";
  SeparateBlockType.NameAccess = PRIVATE_ACCESS;
  SeparateBlockType.TypeSpec = SeparateBlockTypespec;
  SeparateBlockType.Next = NULL;
  //

  DummyVar.Access = PRIVATE_ACCESS;
  DummyVar.CName = "?";
  DummyVar.ConstValue = NULL;
  DummyVar.Initialised = FALSE;
  DummyVar.Finalised = FALSE;
  DummyVar.Accessed = FALSE;
  DummyVar.Low = 0;
  DummyVar.High = 0;
  DummyVar.MaxDefinite = TRUE;
  DummyVar.MinDefinite = TRUE;
  DummyVar.Mode = MODE_GLOBAL;
  DummyVar.Name = "???";
  DummyVar.Type = &ConstantIntegerType;
  DummyVar.Line = 0;
  DummyVar.Next = NULL;
  //
  if (setjmp (error_recovery) == 0)
    {
      yyparse ();
    }
  //
  if (ErrorCount == 0)
    {
      if (setjmp (error_recovery) == 0)
        {
          //
          CheckShareLists ();
          //
          GenerateInitialisation ();
          //
        }
    }
  //
  if (UnitTest)
    {
      SwitchToAppHeader ();
      emitstr ("extern unsigned char __coverage[];\n");
    }
  //
  CloseOutput (SourceFile, FALSE);
  CloseOutput (HeaderFile, TRUE);
  CloseOutput (AuxHeaderFile, TRUE);
  CloseOutput (AppHeaderFile, TRUE);
  CloseOutput (CondHeaderFile, TRUE);
//  CloseOutput (PrivHeaderFile, TRUE);
  //
  if (WriteIndexFile)
    {
      fclose(IndexFile);
    }
  //
  if (NumPragmaNotes != 0)
    {
      printf("\n");
    }
  PrintErrorCount ();
  printf ("\n");
  PrintNotes ();
  //
  if (SuppressGlobalUsageChecks)
    {
      printf
        ("***** %u global usage errors were suppressed by the -g option\n",
         GlobalUsageErrors);
    }
  if (IgnoredDispatches != 0)
    {
      printf ("***** %u instances of pragma dispatch had no effect\n",
              IgnoredDispatches);
    }
  if (GenerateOutput)
    {
      if (StartList != NULL && ErrorCount == 0
          && TaskImplementation == PTHREAD)
        {
          printf ("Compile with -pthread\n");
        }
      if (CompilerTest)
        {
          printf ("\n***** Built with runtime consistency tests\n\n");
        }
    }
  else
    {
      if (ErrorCount == 0)
        {
          printf
            ("\n***** Analyse only mode was requested - no code has been generated\n\n");
        }
    }

  if (NumPragmaNotes != 0)
    {
      printf("\n%u occurrence%sof pragma note\n", NumPragmaNotes, NumPragmaNotes == 1 ? " " : "s ");
    }
  if (ErrorCount == 0)
    return 0;
  else
    return 2;
}
