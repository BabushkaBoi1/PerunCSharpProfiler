#!/bin/bash

# Set up variables for compiler and flags
CFLAGS="-fpic -Wswitch -Wno-deprecated-declarations -Wempty-body -Wconversion -Wreturn-type -Wparentheses -Wno-pointer-sign -Wno-format -Wuninitialized -Wunreachable-code -Wunused-function -Wunused-value -Wunused-variable -std=c++17 -Wall -fno-strict-aliasing -I../coreclr/pal/inc/rt -I../coreclr/pal/src/include -I../coreclr/pal/prebuilt/inc -I../coreclr/pal/inc -I../coreclr/inc -IC:/Bakalarka/Profiler/PerunProfiler/SharedPerunProfiler -IC:/Bakalarka/Profiler/PerunProfiler/PerunNetProfilerLin -IC:/Bakalarka/Profiler/PerunProfiler/SharedPerunProfiler/asm/systemv -g2 -gdwarf-2 -frtti -fno-omit-frame-pointer -std=c11 -fexceptions"

# Build the object file
clang++ $CFLAGS -c $1.cpp -o obj/$(basename $1).o

# Build the shared object file
clang++ -shared -o lib$(basename $1).so obj/$(basename $1).o