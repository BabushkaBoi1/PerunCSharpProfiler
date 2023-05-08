# PerunProfiler

This is repository of Bachelor thesis: Performance Analysis of C# Programs.

Author: Hájek Vojtěch, xhajek51 

This is the implementation of module extension for Perun: Lightweight Performance Version System (https://github.com/tfiedor/perun).
The module is right now based as Visual Studio solution. The module is capable of profiling any .NET program with trace
of functions, memory allocation and garbage collecting.

## Requirements
- Installed .NET Core 1.0 or later
- Installed Visual Studio 2022
- Installed Python 3.10
- Windows 10

## Build Collector
###Windows build for architecture x64

1. open soltuion PerunProfiler.sln in Visual Studio 2022
2. build project PerunNetProfilerWIn
3. succesfull build should be in file .\x64\Debug\

Other architectures for Windows were not tested.


## Build python scripts
For Python Visualizations script install these packages:
- pip install pandas
- pip install seaborn 
- pip install json 
- pip install matplotlib 
- pip install plotly
- pip install numpy
- pip install argparse
- pip install dataframe_image


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
     - [-d DEALLOCATION, --deallocation DEALLOCATION] -- activates logic to profile deallocation of objects
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

1. Run script .\PerunVizualizations\main.py with these arguments
  - [-h, --help] -- show this help message and exit
  - [-p PATH, --path PATH] -- Set path to profile data file (extension .json).
  - [-gs {wallTime,cpuTime}, --gScatterPlot {wallTime,cpuTime}] -- Scatter plot in wall/cpu time, second value is number of types of objets.
  - [-gt {functions,functionsWithAlloc}, --gTreemap {functions,functionsWithAlloc}] -- Treemap mode for only functions or functions with allocation (default functions).
  - [-t THREAD, --thread THREAD] --  Set name of thread to be shown at treeMap graph or tables.
  - [-n NUMBER, --number NUMBER] -- Show this number of objects, can be use with any graph/table (default 10)
  - [-s SAVEPATH, --savePath SAVEPATH] -- path to save plot
  - [-m {0,1,2}, --mode {0,1,2}] -- Set mode for profile data(default 0)
  - [-tabF {count,wallTime,cpuTime}, --tableFunctions {count,wallTime,cpuTime}] -- Make table plot for functions.
  - [-tabO {count,objSize}, --tableObjects {count,objSize}] -- Make table plot for types of objects by count/size.
  - [-tabT {functions, objects}, --tableThreads {functions,objects}] -- Make plot table of threads with count of functions or objects.

Example 
> $ python3 main.py -p Data\example.json -m 0 -tabF count -gs wallTime -tabO objSize -gt functionsWithAlloc -n 8


