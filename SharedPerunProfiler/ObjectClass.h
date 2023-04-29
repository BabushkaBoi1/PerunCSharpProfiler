#pragma once

#include <list>
#include <string>
#include "Common.h"
#include <cor.h>
#include <corprof.h>

class ObjectClass {
public:
	ObjectID objectId;
	ThreadID threadId;
	ULONG size;
	std::string typeName;
	int gcNumber;
	double cpuTimeAllocation;
	double wallTimeAllocation;
	int functionNumOrder = -1;

	void Serialize();
};
