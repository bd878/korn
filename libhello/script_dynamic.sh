#!/bin/bash
# dynamically loaded library demo

# presume that libhello.so has been created

# compile demo_dynamic file into an .o file

gcc -Wall -g -c demo_dynamic.c

# create program demo_use

gcc -g -o demo_dynamic demo_dynamic.o -ldl

# execute the program

LD_LIBRARY_PATH="." ./demo_dynamic
