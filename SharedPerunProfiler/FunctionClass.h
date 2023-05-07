/*
 * Bachelor thesis: Performance Analysis of C# Programs
 *
 * Author: Hajek Vojtech (xhajek51)
 */
#pragma once

#include <string>
#include <iostream>
#include <list>
#include "FunctionInfo.h"
#include "Common.h"
#include <cor.h>
#include <corprof.h>

/**
 * Class for function callback
 */
class FunctionClass {
public:
	FunctionClass *prevFunction;
	FunctionID funcId;
	FunctionInfo *funcInfo;
	ThreadID TID;
	double cpuTimeEnter;
	double wallTimeEnter;
	double cpuTimeLeave;
	double wallTimeLeave;
	int callOrderNumber;
	int depth;
	std::list<FunctionClass*> calledFunctions;

	void Serialize();
};
