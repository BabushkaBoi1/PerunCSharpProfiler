#include <ObjectClass.h>

#include "Logger.h"


void ObjectClass::Serialize()
{
	if(this->functionNumOrder != -1)
	{
		Logger::LOG("{"
			"\"TID\":\"0x%p\","
			"\"objId\":\"0x%p\","
			"\"objSize\":\"%d\","
			"\"objType\":\"%s\","
			"\"GC\":\"%d\","
			"\"untilGC\":\"%d\","
			"\"fnc\":\"%d\","
			"\"eWALLt\":\"%f\","
			"\"eCPUt\":\"%f\"}",
			this->threadId, this->objectId, this->size, this->typeName.c_str(), this->gcNumber, this->untilGcNumber, this->functionNumOrder, this->wallTimeAllocation, this->cpuTimeAllocation);
	}
	else
	{
		Logger::LOG("{"
			"\"TID\":\"0x%p\","
			"\"objId\":\"0x%p\","
			"\"objSize\":\"%d\","
			"\"objType\":\"%s\","
			"\"GC\":\"%d\","
			"\"untilGC\":\"%d\","
			"\"eWALLt\":\"%f\","
			"\"eCPUt\":\"%f\"}",
			this->threadId, this->objectId, this->size, this->typeName.c_str(), this->gcNumber, this->untilGcNumber, this->wallTimeAllocation, this->cpuTimeAllocation);
	}
}
