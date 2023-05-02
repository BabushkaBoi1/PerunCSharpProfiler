@echo off

set CORECLR_ENABLE_PROFILING=1
set CORECLR_PROFILER={32E2F4DA-1BEA-47ea-88F9-C5DAF691C94A}
set CORECLR_PROFILER_PATH=C:\Bakalarka\Profiler\PerunProfiler\x64\Debug\PerunNetProfilerWin.dll
set PROFILER_MODE=2
set PROFILER_LOG_FILE=C:\Bakalarka\Profiler\PerunProfiler\PerunVisualizations\Experiments\pressure


dotnet C:\Bakalarka\ExamplesApps\cacheManagerExample\cacheManagerExample\bin\Debug\net6.0\cacheManagerExample.dll

