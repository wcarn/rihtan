# The Rihtan Programming Language

Rihtan is a programming language that is oriented towards small to medium
sized microprocessor-based systems that might otherwise typically
be programmed in C.

Rihtan is intended for relatively simple programs that
have a static structure. For large, complex programs, programs with
a dynamic task structure, and programs that have sophisticated timing, scheduling
or integrity requirements, other languages like Ada or Spark Ada, for example, will be more appropriate.

The Rihtan translator outputs formatted, readable C source code. It is designed to be used in C-based toolchains, to be integrated readily into C code, to add no significant runtime overhead compared to a C program, and to be learnt easily by C programmers. Thus it offers C programmers an easy path to a different style of
programming that some might find to be of interest.

Rihtan's features include
- clean, unambiguous syntax
- more detailed treatment of basic types
- static checking intended to detect out-of-range errors, array overflows, unintended aliasing and data access race conditions
- packages (modules) as a first-class language feature
- generics
- subsystems
- direct support for embedded system concepts, and
- integrated unit testing.

## Status

Currently Rihtan is suitable for experimenting with the ideas that it implements, but the generated C code should be checked and tested within the C development environment.

## Building the compiler

The Rihtan compiler can be built from the C source, e.g. using gcc:

```
gcc -Wall -O rihtan.c parser_support.c expressions.c lex.yy.c rihtan.tab.c -o ../bin/rihtan.exe
```

The files lex.yy.c and rihtan.tab.c can be rebuilt with:

```
bison -d rihtan.y
flex --yylineno rihtan.l
```
Bison and flex are only needed if the syntax is to be changed.

## Running Rihtan

Run rihtan with no arguments for a usage summary.

Samples can be found in the `samples` directory. See `index.txt` for
instructions on how to build them.
This assumes that gcc in the path in order to compile the generated C files, although a
different compiler could be used.

Under Windows all of the samples can be built by going into the samples directory and running `build_samples.bat`.

## Documentation

The main language documentation is 'Rihtan-reference-manual.pdf' in the doc directory.

## License

Rihtan is licensed under the Apache License version 2.0. See LICENSE.
