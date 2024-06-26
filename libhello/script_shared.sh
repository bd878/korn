#!/bin/bash
# shared library demo

# create shared library's object file, libhello.o

gcc -fPIC -Wall -g -c libhello.c

# create shared library
# use -lc to link against C lib, since
# libhello depends on C library

gcc -g -shared -Wl,-soname,libhello.so.0 \
    -o libhello.so.0.0 libhello.o -lc

# setup the soname.
# we could just execute:
# ln -sf libhello.so.0.0 libhello.so.0,
# but let's ldconfig figure it out

/sbin/ldconfig -n .

# setup the linker name

ln -sf libhello.so.0 libhello.so

# compile demo_use program file

gcc -Wall -g -c demo_use.c -o demo_use.o

# create program demo_use
# The -L. causes "." to be searched during creation
# of the program.
# this does NOT mean that "." will be searched when the program
# is executed!

gcc -g -o demo_use demo_use.o -L. -lhello

# execute the program.
# We need to tell the program where the shared lib is,
# using LD_LIBRARY_PATH

LD_LIBRARY_PATH="." ./demo_use
