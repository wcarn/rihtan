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

/*
 * Utility program to extract subsets of Rihtan source based on
 * an index written by the compiler using the -fx option.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int LineNumbers = 0;

FILE *indexfile;
#define BUFF_SIZE 100
char buff[BUFF_SIZE];
char linetype;
char filename[100];
unsigned firstline, lastline;
int IndentLines = 0;
int Depth = -1;

#define MAX_FILES 100
struct {
  char *Name;
  FILE *InFile;
  unsigned Line;
  unsigned PackageNum;
  char PreviousLineType;
} Files[MAX_FILES];
int NumFiles = 0;

#define FILE_BUFF_SIZE 1000
char FileBuff[FILE_BUFF_SIZE];

unsigned char LineTypesToDisplay['Z' - 'A' + 1];

void ShowLineNumber(int filenum)
{
  if (LineNumbers) {
    printf("%05u ", Files[filenum].Line);
  }
}

void Indent(void)
{
  int n;

  if (IndentLines) {
    for (n = 0; n < Depth; n++) {
      printf("  ");
    }
  }
}

void WriteLine(void)
{
  int commentonly = 0;

  if (!LineTypesToDisplay['C' - 'A']) {
    char *s;
    int instring, quote, comment;

    s = FileBuff;
    instring = 0; quote = 0;
    commentonly = 1; comment = 0;
    while (*s) {
      if (quote) {
        quote = 0;
      }
      else {
        if (*s == '\\' && instring) quote = 1;
        if (*s == '"' && (!quote)) instring = !instring;
        if (*s == '/' && *(s + 1) == '/' && (!instring)) {
          *s = '\n';
          *(s + 1) = 0;
          comment = 1;
          break;
        }
        else if (*s > ' ') {
          commentonly = 0;
        }
      }
      s++;
    }
    commentonly = commentonly && comment;
  }
  if (!commentonly) {
    fputs(FileBuff, stdout);
  }
}

void GetLine(int filenum, int InRange)
{
  if (fgets(FileBuff, FILE_BUFF_SIZE, Files[filenum].InFile) == NULL) {
    printf("Error reading line %u of %s\n", Files[filenum].Line, Files[filenum].Name);
    exit(1);
  }
  if (InRange && (linetype == 'H' || (linetype == 'G' && Files[filenum].PreviousLineType != 'G'))) {
    Files[filenum].PackageNum++;
    if (Files[filenum].PackageNum == 1) {
      Depth++;
    }
  }
  if (   (InRange && LineTypesToDisplay[linetype - 'A'])
      || ((!InRange) && LineTypesToDisplay['Z' - 'A'])
     ) {
    ShowLineNumber(filenum);
    Indent();
    WriteLine();
  }
  else if (   InRange
           && linetype == 'S'
           && (!LineTypesToDisplay['S' - 'A'])    /* Not displaying 'separate' statements */
           && (LineTypesToDisplay['H' - 'A'])     /* Displaying package headers */
          ) {
    /* Write out a comment to show that the package was from a separate file */
    ShowLineNumber(filenum);
    Indent();
    printf("  //-");
    WriteLine();
  }
  Files[filenum].Line++;
  if (InRange && linetype == 'E') {
    Files[filenum].PackageNum--;
    if (Files[filenum].PackageNum == 0) {
      Depth--;
    }
  }
  Files[filenum].PreviousLineType = linetype;
}

void SetDisplay(int negate, char *charlist)
{
  int j;
  char *s;

  for (j = 'A'; j <= 'Z'; j++) LineTypesToDisplay[j - 'A'] = negate;
  s = charlist;
  while (*s) {
    if (*s >= 'A' && *s <= 'Z') {
      LineTypesToDisplay[*s - 'A'] = !negate;
    }
    else {
      printf("Unexpected display character '%c'\n", *s);
      exit(1);
    }
    s++;
  }
}

void Usage(void)
{
  printf("Read the source code along with source_index.txt in order to\n");
  printf("extract subsets of the source.\n");
  printf("\nCompile with the -fx switch to build source_index.txt.\n\n");
  printf("usage: indexer [-nl] [-i] [display]\n");
  printf("where\n");
  printf("  display is a set of:\n");
  printf("  H package header lines\n");
  printf("  E package end lines\n");
  printf("  G generic package declarations\n");
  printf("  I generic package instantiations\n");
  printf("  S 'separate' statements\n");
  printf("  P public declarations\n");
  printf("  U unit test routines and sections\n");
  printf("  C comments\n");
  printf("  Z everything else\n");
  printf("  Prefixing this with '/' indicates the negation of the set\n");
  printf("  The default is '/S', i.e. everything except 'separate' statements\n");
  printf("\n");
  printf("-nl Add line numbers at the left\n");
  printf("-i  Indent separate packages\n");
  printf
    ("\n-------------------------------------------------------------------------------\n");
  printf("Rihtan source indexer Copyright (C) 2015  William Carney\n");
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

}

int main(int argc, char **argv)
{
  char *s;
  int j;
  int wrotenl;

  // Default - display everything except 'separate' statements (but do display
  // the separate files themselves)
  SetDisplay(1, "S");
  //
  argv++;
  while (argc > 1) {
    s = *argv;
    if (s[0] == '-') {
      if (!strcmp(s, "-nl")) {
        LineNumbers = 1;
      }
      else if (!strcmp(s, "-i")) {
        IndentLines = 1;
      }
      else if (!strcmp(s, "--help")) {
        Usage();
        exit(1);
      }
      else {
        printf("Unexpected parameter '%s'\n\n", s);
        Usage();
        exit(1);
      }
    }
    else if (s[0] == '/') {
      SetDisplay(1, s + 1);
    }
    else {
      SetDisplay(0, s);
    }
    argv++;
    argc--;
  }
  //
  indexfile = fopen("source_index.txt", "r");
  if (indexfile == NULL) {
    printf("Cannot open source_index.txt\n");
    exit(1);
  }
  wrotenl = 0;
  while (!feof(indexfile)) {
    fgets(buff, BUFF_SIZE, indexfile);
    s = buff;
    while (*s) {
      if (*s == ':' || *s == '-') *s = ' ';
      s++;
    }
    sscanf(buff, "%c %s %u %u\n", &linetype, filename, &firstline, &lastline);
//    printf(">> %c,%s,%u,%u\n", linetype, filename, firstline, lastline);
    if (!(linetype >= 'A' && linetype <= 'Z')) {
      printf("Bad line type '%c' in %s\n", linetype, filename);
      exit(2);
    }
    for (j = 0; j < NumFiles; j++) {
      if (!strcmp(Files[j].Name, filename)) break;
    }
    if (j >= NumFiles) {
      if (j >= MAX_FILES) {
        printf("Too many files\n");
        return 1;
      }
      NumFiles = j + 1;
      Files[j].Name = strdup(filename);
      Files[j].InFile = fopen(filename, "r");
      if (Files[j].InFile == NULL) {
        printf("Cannot open %s\n", Files[j].Name);
        exit(1);
      }
      Files[j].Line = 1;
      Files[j].PackageNum = 0;
      Files[j].PreviousLineType = 0;
    }
    if (Files[j].Line < firstline) {
      while (Files[j].Line < firstline) {
        GetLine(j, 0);
        wrotenl = 0;
      }
      if ((!LineTypesToDisplay['Z' - 'A']) && (!wrotenl)) {
        printf("\n");
        wrotenl = 1;
      }
    }
    while (Files[j].Line <= lastline) {
      GetLine(j, 1);
      wrotenl = 0;
    }
  }
  fclose(indexfile);
  return 0;
}
