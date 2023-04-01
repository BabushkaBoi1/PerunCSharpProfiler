#pragma once
#include <string>
#include <iostream>
#include <list>
#include <ObjectClass.h>


class FunctionClass {
public:
	FunctionClass *prevFunction;
	int funcId;
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
