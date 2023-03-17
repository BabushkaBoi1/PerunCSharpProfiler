#pragma once

#include <fstream>
#include "Mutex.h"
#include "AutoLock.h"


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

	LogLevel GetLevel() const;
	void SetLevel(LogLevel level);

	template<typename... Args>
	void Log(Args&&... args) {
		char buffer[1 << 10];
#ifdef _WINDOWS
		sprintf_s(buffer, args...);
#else
		sprintf(buffer, args...);
#endif
		DoLog(buffer);

	}
	template<typename... Args>
	__forceinline static void LOG(Args&&... args) {
		Get().Log(std::forward<Args>(args)...);
	}

private:
	Logger();
	void DoLog(const char* text);
	void Term();

private:
	Mutex _lock;
	std::ofstream _file;
	LogLevel _level = LogLevel::Debug;
};

