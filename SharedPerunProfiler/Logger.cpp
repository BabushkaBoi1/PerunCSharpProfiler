#include "Logger.h"


using namespace std;

Logger& Logger::Get() {
	static Logger logger;
	return logger;
}

void Logger::Shutdown() {
	Get().Term();
}

Logger::Logger()
{
	auto fileDir = OS::ReadEnvironmentVariable("PROFILER_LOG_FILE");

	file.open(fileDir);
}

void Logger::Term() {
	if (file.is_open()) {
		file.flush();
		file.close();
	}
}


void Logger::LogFunction(std::string functionName, std::string moduleName, clock_t cpuTime, clock_t wallTime)
{
}
