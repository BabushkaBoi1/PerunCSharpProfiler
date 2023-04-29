#include "OS.h"

#ifdef _WINDOWS
#include <Windows.h>
#else
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#endif

std::string OS::ReadEnvironmentVariable(const char* name) {
#ifdef _WINDOWS
	char value[1024];
	DWORD result = GetEnvironmentVariableA(name, value, sizeof(value));
	if (result == 0)
	{
		return "";
	}
	return value;
#else
	return ::getenv(name);
#endif
}

int OS::GetPid() {
#ifdef _WINDOWS
	return ::GetCurrentProcessId();
#else
	return getpid();
#endif
}

int OS::GetTid() {
#ifdef _WINDOWS
	return ::GetCurrentThreadId();
#else
	return pthread_self();
#endif
}

std::string OS::GetCurrentDir() {
	char buffer[512] = { 0 };
#ifdef _WINDOWS
	::GetCurrentDirectoryA(sizeof(buffer), buffer);
#else
	getcwd(buffer, sizeof(buffer));
#endif
	return buffer;
}

std::string OS::UnicodeToAnsi(const WCHAR* str) {
#ifdef _WINDOWS
	std::wstring ws(str);
#else
	std::basic_string<WCHAR> ws(str);
#endif
	return std::string(ws.begin(), ws.end());
}

double OS::GetCpuTime()
{
#ifdef _WINDOWS
	FILETIME a, b, c, d;
	if (GetProcessTimes(GetCurrentProcess(), &a, &b, &c, &d) != 0) {
		return (double)(d.dwLowDateTime | ((unsigned long long)d.dwHighDateTime << 32)) * 0.0000001;
	}
	else {
		return 0;
	}
#else
	return (double)clock() / CLOCKS_PER_SEC;
#endif
}

double OS::GetWallTime()
{
#ifdef _WINDOWS
	LARGE_INTEGER time, freq;
	if (!QueryPerformanceFrequency(&freq)) {
		return 0;
	}
	if (!QueryPerformanceCounter(&time)) {
		return 0;
	}
	return (double)time.QuadPart / freq.QuadPart;
#else
	struct timeval time;
	if (gettimeofday(&time, NULL)) {
		return 0;
	}
	return (double)time.tv_sec + (double)time.tv_usec * .000001;
#endif
}