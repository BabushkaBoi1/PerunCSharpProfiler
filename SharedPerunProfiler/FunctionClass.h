#pragma once

#include <string>
#include <iostream>
#include <list>
#include "FunctionInfo.h"
#include "Common.h"
#include <cor.h>
#include <corprof.h>


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
	std::list<FunctionClass*> calledFunctions;

	void Serialize();
};
