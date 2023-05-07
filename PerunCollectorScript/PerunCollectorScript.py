import os
import subprocess
import argparse

if __name__ == "__main__":
    # Parse arguments
    parser = argparse.ArgumentParser(description="Argument parser collector script")
    parser.add_argument("-p", "--path", default="Data/", help="path to profile file")
    parser.add_argument("-m", "--mode", default="0", help="mode of profiling")
    parser.add_argument("-a", "--resAssembly", default="", help="restrictions on assembly, use semicolon ; between assemblies")
    parser.add_argument("-l", "--logPath", default="..\PerunVisualizations\Data", help="path to log file")
    parser.add_argument("-d", "--deallocation", default="0", help="activation of deallocation")
    parser.add_argument("-e", "--pathToExeProgram", help=".NET program with extension .exe or .dll set for profiling")
    parser.add_argument("-clrD", "--CORECLR_PROFILER_PATH", default="", help="path to DLL PerunProfiler file, if neccesary")
    args = parser.parse_args()

    if args.pathToExeProgram is None:
        print("Error: you have to set program for profiling!")
        exit

    extension = os.path.splitext(args.pathToExeProgram)[1]
    if extension.lower() == '.exe':
        command = r''+args.pathToExeProgram
    elif extension.lower() == '.dll':
        command = r'dotnet '+args.pathToExeProgram
    else:
        print("The file does not have a supported extension.")
        exit

    os.environ['CORECLR_ENABLE_PROFILING'] = '1'
    os.environ['CORECLR_PROFILER'] = '{32E2F4DA-1BEA-47ea-88F9-C5DAF691C94A}'
    os.environ['CORECLR_PROFILER_PATH'] = r'..\x64\Debug\PerunNetProfilerWin.dll'
    os.environ['PROFILER_MODE'] = args.mode
    os.environ['PROFILER_ENABLE_ASSEMBLIES'] = args.resAssembly
    os.environ['PROFILER_ENABLE_DEALLOC'] = args.deallocation
    os.environ['PROFILER_LOG_FILE'] = args.logPath

    # Run .NET program
    subprocess.run(command, shell=True)