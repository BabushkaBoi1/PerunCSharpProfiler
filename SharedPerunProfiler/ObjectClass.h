#pragma once
#include <string>
#include <list>

#include "Common.h"

class ObjectClass {
public:
	int objectId;
	ULONG size;
	bool survived;
	int gc;
	double cpuTimeAllocation;
	double wallTimeAllocation;
};
