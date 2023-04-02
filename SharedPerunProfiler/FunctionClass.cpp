#include <FunctionClass.h>

#include "Logger.h"



void FunctionClass::Serilaize()
{
	Logger::LOGInSh("{\"Function\":{"
		"\"fID\":\"%p\","
		"\"PID\":\"%d\","
		"\"TID\":\"%d\","
		"\"eWALLt\":\"%f\","
		"\"lWALLt\":\"%f\","
		"\"eCPUt\":\"%f\","
		"\"lCPUt\":\"%f\"",
		this->funcId, this->PID, this->TID, this->wallTimeEnter, this->wallTimeLeave,
		this->cpuTimeEnter, this->cpuTimeLeave);

	if (!this->calledFunctions.empty())
	{
		Logger::LOGInSh(",\"cFns\":[");

		//auto firstId = this->calledFunctions.front()->funcId;
		for (auto function : this->calledFunctions)
		{
			
			if (function != this->calledFunctions.front())
			{
				Logger::LOGInSh(",");
			}
			function->Serilaize();
			delete function;
		}
		calledFunctions.clear();
		Logger::LOGInSh("]}}");
	}
	else
	{
		Logger::LOGInSh("}}");
	}
}
