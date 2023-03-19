#pragma once

#include <fstream>
#include "Mutex.h"
#include "AutoLock.h"
#include <list>


enum class LogLevel {
	Verbose,
	Debug,
	Info,
	Warning,
	Error,
	Fatal
};

class Logger final {
public:
	static Logger& Get();
	static void Shutdown();

	template<typename... Args>
	void Log(std::list<double> cpuTimeEnter, std::list<double> cpuTimeLeave, Args&&... args) {
		char buffer[1 << 10];
#ifdef _WINDOWS
		sprintf_s(buffer, args...);
#else
		sprintf(buffer, args...);
#endif
		DoLog(cpuTimeEnter, cpuTimeLeave, buffer);

	}
	template<typename... Args>
	__forceinline static void LOG(std::list<double> cpuTimeEnter, std::list<double> cpuTimeLeave, Args&&... args) {
		Get().Log(cpuTimeEnter, cpuTimeLeave, std::forward<Args>(args)...);
	}

private:
	Logger();
	void DoLog(std::list<double> cpuTimeEnter, std::list<double> cpuTimeLeave, const char* text);
	void Term();

private:
	Mutex _lock;
	std::ofstream _file;
	LogLevel _level = LogLevel::Debug;
};

