#include <FunctionClass.h>

#include "Logger.h"



void FunctionClass::Serilaize()
{
	Logger::LOGInSh("{\"Function\":{"
		"\"fID\":\"%p\","
		"\"fName\":\"%s\","
		"\"PID\":\"%d\","
		"\"TID\":\"%d\","
		"\"enterWALLt\":\"%f\","
		"\"leaveWALLt\":\"%f\","
		"\"enterCPUt\":\"%f\","
		"\"leaveCPUt\":\"%f\"",
		this->funcId, this->name.c_str(), this->PID, this->TID, this->wallTimeEnter, this->wallTimeLeave,
		this->cpuTimeEnter, this->cpuTimeLeave);

	if (!this->calledFunctions.empty())
	{
		Logger::LOGInSh("\"calledFunctions\":[");

		for (auto function : this->calledFunctions)
		{
			function->Serilaize();
			delete function;
		}
		calledFunctions.clear();
		Logger::LOGInSh("]}},");
	}
}
