#pragma once
#include <iostream>
#include <string>
#include "Common.h"

class OS final {
public:
	static std::string ReadEnvironmentVariable(const char* name);
	static int GetPid();
	static int GetTid();
	static std::string GetCurrentDir();
	static std::string UnicodeToAnsi(const WCHAR* str);
	static double GetCpuTime();
	static double GetWallTime();
};

