/*
 * Bachelor thesis: Performance Analysis of C# Programs
 *
 * Author: Hajek Vojtech (xhajek51)
 */
#pragma once

#include <string>
#include <iostream>
#include "Common.h"
#include <cor.h>
#include <corprof.h>

 /**
  * Class for function information
  */
class FunctionInfo{
public:
	std::string name;
	FunctionID funcId;

	void Serialize();
};