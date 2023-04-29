#include <FunctionInfo.h>

#include "Logger.h"


void FunctionInfo::Serialize()
{
	Logger::LOGInSh("\"%p\": \"%s\",", this->funcId, this->name.c_str());
}