#!/bin/sh

CXX_FLAGS="$CXX_FLAGS --no-undefined -Wno-invalid-noreturn -fPIC -fms-extensions -HOST_64BIT -BIT64 -UNICODE -PLATFORM_UNIX -PAL_STDCPP_COMPAT -DPLATFORM_UNIX -std=c++17"
INCLUDES="-I ../coreclr/pal/inc/rt -I ../coreclr/pal/src/include -I ../coreclr/pal/prebuilt/inc -I ../coreclr/inc"

clang++ -shared -o PerunNetProfilerLin.so $CXX_FLAGS $INCLUDES CoreProfilerFactory.cpp CoreProfiler.cpp DLLMain.cpp Mutex.cpp Logger.cpp OS.cpp