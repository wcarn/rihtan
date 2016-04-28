# The Rihtan Programming Language

Rihtan is a programming language that is oriented towards small to medium
sized microprocessor-based embedded systems, which would otherwise typically
be programmed in C, and system programming.
The Rihtan translator outputs formatted, readable C source code.

Rihtan is designed to be used in C-based toolchains, to be integrated readily into
existing C code, to add no significant runtime overhead compared to a C program,
and to be learnt easily by C programmers.

Its features include
- clean, unambiguous syntax
- detailed treatment of basic types
- extensive static checking including avoidance of out-of-range errors, array overflows, unintended aliasing and data access race conditions
- packages
- generics
- subsystems
- direct support for embedded system concepts, and
- integrated unit testing.

Rihtan syntax is closer to the language family that includes Ada than it is to C, but
it should present no particular difficulty for C programmers.

## Status

Currently Rihtan is suitable for experimenting with the ideas that it implements, but the generated C code should be checked
and tested within the C development environment.

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
