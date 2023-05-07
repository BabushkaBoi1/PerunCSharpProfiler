/*
 * Bachelor thesis: Performance Analysis of C# Programs
 *
 * Author: Hajek Vojtech (xhajek51)
 */
#pragma once

#include <list>
#include <string>
#include "Common.h"
#include <cor.h>
#include <corprof.h>

 /**
  * Class for object allocation
  */
class ObjectClass {
public:
	ObjectID objectId;
	ThreadID threadId;
	ULONG size;
	std::string typeName;
	int gcNumber;
	int untilGcNumber;
	double cpuTimeAllocation;
	double wallTimeAllocation;
	int functionNumOrder = -1;

	void Serialize();
};
