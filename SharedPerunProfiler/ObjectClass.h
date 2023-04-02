#pragma once
#include <string>
#include <list>

#include "Common.h"

class ObjectClass {
public:
	int objectId;
	ULONG size;

	double cpuTimeAllocation;
	double wallTimeAllocation;
};
