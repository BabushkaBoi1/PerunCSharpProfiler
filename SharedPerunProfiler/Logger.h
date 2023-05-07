/*
 * Bachelor thesis: Performance Analysis of C# Programs
 *
 * Author: Hajek Vojtech (xhajek51)
 */
#pragma once

#include <fstream>
#include "Mutex.h"
#include "AutoLock.h"
#include <list>
#include <CoreProfiler.h>

 /**
  * Class for Logger
  */
class Logger final {
public:
	static Logger& Get();
	static void Shutdown();

	template<typename... Args>
	void Log(bool doInit, Args&&... args) {
		char buffer[1 << 10];
#ifdef _WINDOWS
		sprintf_s(buffer, args...);
#else
		sprintf(buffer, args...);
#endif
		if (doInit)
		{
			DoInitLog(buffer);
		} else
		{
			DoLog(buffer);
		}
	}

	template<typename... Args>
	__forceinline static void LOG(Args&&... args) {
		Get().Log(false, std::forward<Args>(args)...);
	}

	template<typename... Args>
	__forceinline static void LOGInSh(Args&&... args) {
		Get().Log(true, std::forward<Args>(args)...);
	}

private:
	Logger();
	void DoLog(const char* text);
	void DoInitLog(const char* text);
	void Term();

private:
	Mutex _lock;
	std::ofstream _file;
};

