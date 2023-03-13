#pragma once
#include <string>
#include <time.h>
#include <iostream>
#include <fstream>
#include "OS.h"

class Logger
{
public:
	bool InitLogger();
	void LogFunction(std::string functionName, std::string moduleName, clock_t cpuTime, clock_t wallTime);


private:
	std::ofstream file;
};

