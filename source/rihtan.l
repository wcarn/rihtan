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

/* Flex source for Rihtan */

%{
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include "ytypes.h"
#include "rihtan.tab.h"

extern unsigned ErrorCount;

#define STRING_BUFF_LENGTH 200
char stringbuff[STRING_BUFF_LENGTH];
int stringlength;
int keepaddingtostring;
int yycolumn = 1;

void addtostringbuff(unsigned char c)
{
  if (keepaddingtostring) {
    if (stringlength >= STRING_BUFF_LENGTH - 2) {    /* Allow for trailing " and null */
      printf("//[%d] String constant too long\n", yylineno);
      ErrorCount++;
    }
    else
      stringbuff[stringlength++] = c;
  }
}

void ResetLex(char *filename)
{
    YY_FLUSH_BUFFER;
    BEGIN(0);   // INITIAL is not defined yet
    yyin = fopen(filename, "r");
    if (yyin == NULL) {
        printf("Cannot open '%s'\n", filename);
        exit(1);
    }
    yytext[0] = 0;
    yylineno = 1;
    yycolumn = 1;
    lexsourcefilename = filename;
}

char *BuildCCode(char *src)
{
  char *s = src;

  /* Skip one leading space, but keep others so that indents
     will be preserved
  */
  if (*s && *s <= ' ') s++;
  return strdup(s);
}

char *BuildComment(char *src)
{
  char *s = src;
  while (*s != '/') s++;
  s++; s++;
  if (*s && *s <= ' ') s++;
  return strdup(s);
}

#define YY_USER_ACTION yylloc.filename = lexsourcefilename; \
    yylloc.first_line = yylloc.last_line = yylineno; \
    yylloc.first_column = yycolumn; yylloc.last_column = yycolumn + yyleng - 1; \
    yycolumn += yyleng;

%}

/*%option bison-bridge*/
/*%option bison-locations*/
%pointer
%option case-insensitive
%x str
DIGIT		[0-9]
HEXDIGIT	[0-9a-f]
ID		[a-zA-Z]+[a-zA-Z0-9_]*
SPACE		[ \t]+
%%
^[ \t]*\/\/-.*$    { yycolumn = 1; }
^[ \t]*\/\/.*$     { yylval.string = BuildComment(yytext);
                     yycolumn = 1;
                     return COMMENT;
                   }
\/\/.*$            { yycolumn = 1; }
-?{DIGIT}+	       { sscanf(yytext, "%"SCANBIGd"", &yylval.integer); return INTEGER; }
0x{HEXDIGIT}+      { sscanf(yytext, "%"SCANBIGx"", &yylval.integer); return INTEGER; }
-?{DIGIT}+\.{DIGIT}+ { yylval.floatval = atof(yytext); return FLOATVAL; }
\"                 { stringbuff[0] = '"'; stringlength = 1; keepaddingtostring = 1; BEGIN(str); }
<str>\"            { BEGIN(INITIAL);
                     stringbuff[stringlength++] = '"';
                     stringbuff[stringlength] = 0;
                     yylval.string = strdup(stringbuff);
                     return STRING;
                   }
<str>[\r\n]        { printf("//[%d] Unterminated string or character constant\n", yylineno);
                     ErrorCount++;
		             yylval.string = strdup("\"\"");
                     yycolumn = 1;
                     return STRING;
                   }
<str>\\.           { addtostringbuff('\\');
                     addtostringbuff(yytext[1]);
                   }
<str>[^\\\n\r\"]+  { char *yptr = yytext;
                     while (*yptr != 0) addtostringbuff(*yptr++);
                   }
\'.\'              { yylval.string = strdup(yytext); return CHARACTER; }
\'\\.\'            { yylval.string = strdup(yytext); return CHARACTER; }
\'\\[0-7]{1,3}\'   { yylval.string = strdup(yytext); return CHARACTER; }
\'\\x[0-9a-f]{1,2}\'   { yylval.string = strdup(yytext); return CHARACTER; }
:=                 { return ASSIGN; }
[\+\-\*\/\;\(\)\:\|\^\.\[\]\,\'] { return (int)yytext[0]; }
"<"                { return LT; }
">"                { return GT; }
"<="               { return LEQ; }
">="               { return GEQ; }
"="                { return EQ; }
"/="               { return NEQ; }
"<["               { return OPENLT; }
"]>"               { return OPENGT; }
mod                { return MOD; }
type               { return TYPE; }
is                 { return IS; }
range              { return RANGE; }
digits             { return DIGITS; }
magnitude          { return MAGNITUDE; }
\.\.               { return DOTDOT; }
when               { return WHEN; }
if                 { return IF; }
then               { return THEN; }
elsif              { return ELSIF; }
else               { return ELSE; }
and                { return AND; }
do                 { return DO; }
end                { return END; }
begin              { return BEGIN_SYM; }
loop               { return LOOP; }
exit               { return EXIT; }
repeat             { return REPEAT; }
while              { return WHILE; }
or                 { return OR; }
not                { return NOT; }
true               { return TRUE_SYM; }
false              { return FALSE_SYM; }
case               { return CASE_SYM; }
null               { return NULL_SYM; }
for                { return FOR_SYM; }
in                 { return IN_SYM; }
out                { return OUT_SYM; }
unit               { return UNIT_SYM; }
array              { return ARRAY_SYM; }
of                 { return OF_SYM; }
record             { return RECORD_SYM; }
union              { return UNION_SYM; }
unchecked          { return UNCHECKED_SYM; }
procedure          { return PROCEDURE; }
function           { return FUNCTION; }
package            { return PACKAGE; }
subsystem          { return SUBSYSTEM; }
system             { return SYSTEM; }
public             { return PUBLIC_SYM; }
"=>"               { return ARROW; }
controlled         { return CONTROLLED; }
main               { return MAIN; }
restart            { return RESTART; }
final              { return FINAL; }
pragma             { return PRAGMA; }
closed             { return CLOSED; }
generic            { return GENERIC; }
new                { return NEW; }
constant           { return CONSTANT; }
persistent         { return PERSISTENT; }
managed            { return MANAGED_SYM; }
declare            { return DECLARE; }
renames            { return RENAMES; }
separate           { return SEPARATE; }
using              { return USING; }
access             { return ACCESS_SYM; }
shared             { return SHARED_SYM; }
with               { return WITH_SYM; }
iand               { return IAND; }
ior                { return IOR; }
ixor               { return IXOR; }
free               { return FREE_SYM; }
unit_test          { return UNIT_TEST; }
use                { return USE_SYM; }
assert             { return ASSERT; }
advise             { return ADVISE; }
!c.*$              { yylval.string = BuildCCode(&yytext[2]);
                     yycolumn = 1;
                     return CODE;
                   }
!h.*$              { yylval.string = BuildCCode(&yytext[2]);
                     yycolumn = 1;
                     return HEADER_CODE;
                   }
!.*$               { yylval.string = BuildCCode(&yytext[1]);
                     yycolumn = 1;
                     return CODE;
                   }
{ID}		           { yylval.string = strdup(yytext); return IDENTIFIER; }
{DIGIT}+{ID}       { printf("SCAN_ERROR:%s\n", yytext); }
{SPACE}		         ;
[\r\n]             { yycolumn = 1; }
.                  { return SCAN_ERROR; }
%%
