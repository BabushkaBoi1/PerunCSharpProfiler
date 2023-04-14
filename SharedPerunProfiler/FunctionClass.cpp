#include <FunctionClass.h>

#include "Logger.h"



void FunctionClass::Serilaize()
{

	if (this->prevFunction != nullptr)
	{
		Logger::LOGInSh("{\"Function\":{"
			"\"fID\":\"%p\","
			"\"TID\":\"%d\","
			"\"fName\":\"%s\","
			"\"eWALLt\":\"%f\","
			"\"lWALLt\":\"%f\","
			"\"eCPUt\":\"%f\","
			"\"lCPUt\":\"%f\","
			"\"rFn\":\"%d\","
			"\"nOr\":\"%d\"}},",
			this->funcId,this->TID, this->name.c_str(), this->wallTimeEnter, this->wallTimeLeave,
			this->cpuTimeEnter, this->cpuTimeLeave, this->prevFunction->callOrderNumber, this->callOrderNumber);
	} else
	{
		Logger::LOGInSh("{\"Function\":{"
			"\"fID\":\"%p\","
			"\"TID\":\"%d\","
			"\"fName\":\"%s\","
			"\"eWALLt\":\"%f\","
			"\"lWALLt\":\"%f\","
			"\"eCPUt\":\"%f\","
			"\"lCPUt\":\"%f\","
			"\"rFn\":\"\","
			"\"nOr\":\"%d\"}},",
			this->funcId, this->TID, this->name.c_str(), this->wallTimeEnter, this->wallTimeLeave,
			this->cpuTimeEnter, this->cpuTimeLeave, this->callOrderNumber);
	}


	if (!this->calledFunctions.empty())
	{
		for (auto function : this->calledFunctions)
		{
			function->Serilaize();
			delete function;
		}
		calledFunctions.clear();
	}
}
