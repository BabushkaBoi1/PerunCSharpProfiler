#include <iostream>

#include "Common.h"
#include "OS.h"
#include "CoreProfilerFactory.h"
using namespace std;

extern "C" BOOL __stdcall DllMain(HINSTANCE hInstDll, DWORD reason, PVOID) {
	switch (reason) {
	case DLL_PROCESS_ATTACH:
		cout << "Profiler DLL loaded into PID " << OS::GetPid() << "\n";
		break;

	case DLL_PROCESS_DETACH:
		cout << "Profiler DLL unloaded from PID" << OS::GetPid() << "\n";
		// debuging memory leaks
		_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
		_CrtDumpMemoryLeaks();
		break;
	}
	return TRUE;
}

class __declspec(uuid("32E2F4DA-1BEA-47ea-88F9-C5DAF691C94A")) CoreProfiler;

extern "C" HRESULT __stdcall DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppv) {

	if (rclsid == __uuidof(CoreProfiler)) {
		static CoreProfilerFactory factory;
		return factory.QueryInterface(riid, ppv);
	}
	return CLASS_E_CLASSNOTAVAILABLE;
}
