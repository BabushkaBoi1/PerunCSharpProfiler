#pragma once

#include <string>
#include <iostream>
#include "Common.h"
#include <cor.h>
#include <corprof.h>

class FunctionInfo{
public:
	std::string name;
	FunctionID funcId;

	void Serialize();
};