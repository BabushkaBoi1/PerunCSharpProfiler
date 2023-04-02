#include "Logger.h"
#include "OS.h"
#include <sstream>
#include <assert.h>
#include <time.h>
#include <iomanip>
#include <fstream>



Logger& Logger::Get() {
	static Logger logger;
	return logger;
}

void Logger::Shutdown() {
	Get().Term();
}

void Logger::Term() {
	if (_file.is_open()) {
		_file.flush();
		_file.close();
	}
}

#ifdef _WINDOWS
#include <Windows.h>
#endif

void Logger::DoLog(const char* text) {

	std::stringstream message;

	message << "{"
		<< text << "},";

	auto smessage = message.str();
	{
		AutoLock locker(_lock);
		_file << smessage;
	}
}

void Logger::DoInitLog(const char* text)
{
	std::stringstream message;

	message
		<< text;

	auto smessage = message.str();
	{
		AutoLock locker(_lock);
		_file << smessage;
	}
}

Logger::Logger() {
	auto logDir = OS::ReadEnvironmentVariable("PROFILER_LOG_FILE");
	
	if (logDir.empty())
		logDir = OS::GetCurrentDir();

	// build log file path based on current date and time
	auto now = ::time(nullptr);
	char time[64];
#ifdef _WINDOWS
	tm local;
	localtime_s(&local, &now);
	auto tlocal = &local;
#else
	auto tlocal = localtime(&now);
#endif
	::strftime(time, sizeof(time), "PerunCSharpProfiler_%F_%H%M.json", tlocal);

	_file.open(logDir + "/" + time, std::ios::out);
}
