#pragma once

#include <string>
#include <iostream>
#include <list>
#include <ObjectClass.h>
#include "Common.h"
#include <cor.h>
#include <corprof.h>


class FunctionClass {
public:
	FunctionClass *prevFunction;
	FunctionID funcId;
	std::string name;
	int PID;
	int TID;
	double cpuTimeEnter;
	double wallTimeEnter;
	double cpuTimeLeave;
	double wallTimeLeave;
	std::list<FunctionClass*> calledFunctions;
	std::list<ObjectClass> objectsAllocated;

	void Serilaize();
};
