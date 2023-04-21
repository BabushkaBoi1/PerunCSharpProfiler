#include <ObjectClass.h>

#include "Logger.h"


void ObjectClass::Serialize()
{
	if(this->functionNumOrder != -1)
	{
		Logger::LOG("\"ObjectAllocated\":{"
			"\"TID\":\"%d\","
			"\"objId\":\"0x%p\","
			"\"objSize\":\"%d\","
			"\"objType\":\"%s\","
			"\"untilGC\":\"%d\","
			"\"fnc\":\"%d\","
			"\"eWALLt\":\"%f\","
			"\"eCPUt\":\"%f\"}",
			this->threadId, this->objectId, this->size, this->objectTypeName.c_str(), this->gcNumber, this->functionNumOrder, this->wallTimeAllocation, this->cpuTimeAllocation);
	}
	else
	{
		Logger::LOG("\"ObjectAllocated\":{"
			"\"TID\":\"%d\","
			"\"objId\":\"0x%p\","
			"\"objSize\":\"%d\","
			"\"objType\":\"%s\","
			"\"untilGC\":\"%d\","
			"\"eWALLt\":\"%f\","
			"\"eCPUt\":\"%f\"}",
			this->threadId, this->objectId, this->size, this->objectTypeName.c_str(), this->gcNumber, this->wallTimeAllocation, this->cpuTimeAllocation);
	}
}