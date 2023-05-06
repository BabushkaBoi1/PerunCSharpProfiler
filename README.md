# PerunProfiler

This is repository of Perun module for profiling C#/.NET programs.

Project SharedPerunProfiler is the main source of implementation, other two projects are for builds on Windows and Linux.
Linux build is not working because of dependencies on nakedcallbacks.asm. Whole project is made in Visual studio 2022 Enterprsie.

## Requirements
- Installed .NET Core 1.0 or later
- Installed Visual Studio 2022
- Installed Python 3.10

## Build
###Windows build for architecture x64

1. open soltuion PerunProfiler.sln in Visual Studio 2022
2. build project PerunNetProfilerWIn
3. succesfull build should be in file .\x64\Debug\

Other architectures for Windows were not tested

###Linux build
Linux build is not working in this moment, because of unresolved dependencies to nakedcallbacks.asm file.

## Modes for profiling
- Mode = 0 -- tracing allocations with functions
- Mode = 1 -- tracing functions
- Mode = 2 -- tracing object allocations 
- Enable_dealloc = 1 -- tracing with object deallocations (usage for Mode 0 or 2)

## Manual for profiling
####First option:

  1. Run Python script .\PerunCollectorScript\PerunCollectorScript.py with these arguments 
     - [-h, --help] -- show this help message and exit
     - [-p PATH, --path PATH]  path to profile file        
     - [-m MODE, --mode MODE]  mode of profiling
     - [-a RESASSEMBLY, --resAssembly RESASSEMBLY] restrictions on assembly, use semicolon ; between assemblies
     - [-l LOGPATH, --logPath LOGPATH] -- path to log file
     - [-d DEALLOCATION, --deallocation DEALLOCATION] -- activation of deallocation
     - [-e PATHTOEXEPROGRAM, --pathToExeProgram PATHTOEXEPROGRAM] -- .NET program with extension .exe or .dll set for profiling
     - [-clrD CORECLR_PROFILER_PATH, --CORECLR_PROFILER_PATH CORECLR_PROFILER_PATH] -- path to DLL PerunProfiler file, if neccesary
     
  Example
> $ python3 PerunCollectorScript.py -m 1 -a CacheManager;CacheManagerExample -e ..\CacheManagerExample\bin\Debug\net6.0\CacheManagerExample.dll
   
 ####Second option:
  1. Set these environment variables
     - CORECLR_ENABLE_PROFILING=1 -- enables profiling in CLR
     - CORECLR_PROFILER_PATH -- path to .dll/.so file build file of profiler
     - CORECLR_PROFILER={32E2F4DA-1BEA-47ea-88F9-C5DAF691C94A}
     - PROFILER_MODE -- mode which will be profiling set to
     - PROFILER_ENABLE_ASSEMBLIES -- name of assemblies you want to profile seperated by ; (example: cacheManager;cacheManagerExample)
     - PROFILER_ENABLE_DEALLOC -- activates logic to profile deallocation of objects
     - PROFILER_LOG_FILE -- sets path to log file
  2. Run any .NET program with these extension
     - .dll -- file must be run with dotnet command (example: "dotnet CacheManagerExample.dll)
     - .exe
  
## Manual for vizualization


## Known issues
