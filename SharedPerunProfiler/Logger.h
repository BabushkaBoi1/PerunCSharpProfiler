#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include "Logger.h"
#include "OS.h"
#include <assert.h>
#include <time.h>
#include <iomanip>
#include "AutoLock.h"
#include "Mutex.h"

class Logger
{
public:
	static Logger& Get();
	static void Shutdown();
	
	__forceinline static void LogFunction(std::string functionName, std::string moduleName, clock_t cpuTime, clock_t wallTime);


private:
	Logger();
	void Term();
	Mutex _lock;
	std::ofstream file;
};

