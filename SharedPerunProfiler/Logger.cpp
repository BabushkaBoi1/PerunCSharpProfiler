#include "Logger.h"
#include "OS.h"
#include <sstream>
#include <assert.h>
#include <time.h>
#include <iomanip>


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

void Logger::DoLog(std::list<double> cpuTimeEnter, std::list<double> cpuTimeLeave, const char* text) {
	char time[48];
	const auto now = ::time(nullptr);
#ifdef _WINDOWS
	tm lt;
	localtime_s(&lt, &now);
	auto plt = &lt;
#else
	auto plt = localtime(&now);
#endif
	timespec ts;
	timespec_get(&ts, TIME_UTC);

	strftime(time, sizeof(time), "%D %T", plt);

	std::stringstream message;

	message
		<< "[" << time << "." << std::setw(6) << std::setfill('0') << (ts.tv_nsec / 1000) << "]"
		<< " [" << OS::GetPid() << ","
		<< OS::GetTid() << "] Enter cpu time:";
	for (auto cpuTime : cpuTimeEnter)
	{
		message << "[" << cpuTime << "]";
	}
	message << "enter func";
	for (auto cpuTime : cpuTimeLeave)
	{
		message << "[" << cpuTime << "]";
	}
	message
		<< text << std::endl;

	auto smessage = message.str();

	{
		AutoLock locker(_lock);
		_file << smessage;
	}

#if defined(_WINDOWS) && defined(_DEBUG)
	OutputDebugStringA(smessage.c_str());
#endif
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
	::strftime(time, sizeof(time), "PerunCSharpProfiler_%F_%H%M.log", tlocal);

	_file.open(logDir + "/" + time, std::ios::out);

}
