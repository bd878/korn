#!/bin/bash
# static library demo

# creates static library's object file, libhello-static.o

gcc -Wall -g -c -o libhello-static.o libhello.c

# create static library

ar rcs libhello-static.a libhello-static.o

# compile demo_use program file
gcc -Wall -g -c demo_use.c -o demo_use.o

# create demo_use program

gcc -g -o demo_use_static demo_use.o -L . -lhello-static
