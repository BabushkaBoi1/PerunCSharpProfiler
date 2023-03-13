#include "Logger.h"
using namespace std;

bool Logger::InitLogger()
{
	file.open("text.txt");
	return false;
}


void Logger::LogFunction(std::string functionName, std::string moduleName, clock_t cpuTime, clock_t wallTime)
{
	file << "Function;" << functionName << ";\n";
}
