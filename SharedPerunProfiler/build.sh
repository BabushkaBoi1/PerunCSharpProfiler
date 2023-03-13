#!/bin/sh

CXX_FLAGS="$CXX_FLAGS --no-undefined -Wno-invalid-noreturn -fPIC -fms-extensions -DHOST_64BIT -DBIT64 -DPAL_STDCPP_COMPAT -DPLATFORM_UNIX -std=c++17"
INCLUDES="-I /mnt/c/Bakalarka/Profiler/PerunProfiler/CoreClr/pal/inc/rt -I /mnt/c/Bakalarka/Profiler/PerunProfiler/CoreClr/pal/prebuilt/inc -I /mnt/c/Bakalarka/Profiler/PerunProfiler/CoreClr/pal/inc -I /mnt/c/Bakalarka/Profiler/PerunProfiler/CoreClr/inc"

clang++ -shared -o PerunNetProfilerLin.so $CXX_FLAGS $INCLUDES CoreProfilerFactory.cpp CoreProfiler.cpp DLLMain.cpp Mutex.cpp Logger.cpp OS.cpp asmhelpers.S

printf 'Done.\n'