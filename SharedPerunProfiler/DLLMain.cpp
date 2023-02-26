#include "Common.h"
#include "OS.h"
#include "CoreProfilerFactory.h"
#include <iostream>
using namespace std;

extern "C" BOOL __stdcall DllMain(HINSTANCE hInstDll, DWORD reason, PVOID) {
	switch (reason) {
	case DLL_PROCESS_ATTACH:
		cout << "Profiler DLL loaded into PID %d", OS::GetPid();
		break;

	case DLL_PROCESS_DETACH:
		cout << "Profiler DLL unloaded from PID %d", OS::GetPid();
		break;
	}
	return TRUE;
}

class __declspec(uuid("805A308B-061C-47F3-9B30-F785C3186E81")) CoreProfiler;

extern "C" HRESULT __stdcall DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppv) {
	if (rclsid == __uuidof(CoreProfiler)) {
		static CoreProfilerFactory factory;
		return factory.QueryInterface(riid, ppv);
	}
	return CLASS_E_CLASSNOTAVAILABLE;
}
