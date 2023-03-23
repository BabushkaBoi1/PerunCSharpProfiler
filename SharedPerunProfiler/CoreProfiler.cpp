#include <assert.h>
#include <vector>
#include <string>
#include "CoreProfiler.h"
#include "OS.h"
#include <iostream>
using namespace std;


CComPtr<ICorProfilerInfo8> _info;
CoreProfiler* g_CoreProfiler = NULL;

CoreProfiler::CoreProfiler()
{
	
}

EXTERN_C void __stdcall EnterStub(FunctionIDOrClientID functionId, COR_PRF_ELT_INFO eltInfo)
{
	if (functionId.functionID == 0)
	{
		return;
	}
	if (g_CoreProfiler != NULL)
	{
		g_CoreProfiler->Enter(functionId.functionID, eltInfo);
	}
}

EXTERN_C void __stdcall LeaveStub(FunctionID functionId, COR_PRF_ELT_INFO eltInfo)
{
	if (functionId == 0)
	{
		return;
	}
	if (g_CoreProfiler != NULL)
	{
		g_CoreProfiler->Leave(functionId, eltInfo);
	}
}

EXTERN_C void __stdcall TailcallStub(FunctionID functionId, COR_PRF_ELT_INFO eltInfo)
{
	if (functionId == 0)
	{
		return;
	}
	if (g_CoreProfiler != NULL)
	{
		g_CoreProfiler->TailCall(functionId, eltInfo);
	}
}


UINT_PTR __stdcall Mapper2(FunctionID functionId, BOOL* pHookFunction)
{
	bool trace = false;
	if (g_CoreProfiler != NULL)
	{
		trace = g_CoreProfiler->Mapper(functionId);
	}
	return trace ? (UINT_PTR)functionId : 0;
}

bool CoreProfiler::Mapper(FunctionID functionId)
{
	auto name = GetMethodName(functionId);

	ModuleID moduleId;
	mdToken token;
	ClassID classId;
	if (FAILED(_info->GetFunctionInfo(functionId, &classId, &moduleId, &token))) {
		return false;
	}

	LPCBYTE loadAddress;
	ULONG nameLen = 0;
	AssemblyID assemblyId;

	if (SUCCEEDED(_info->GetModuleInfo(moduleId, &loadAddress, nameLen, &nameLen, NULL, &assemblyId)))
	{
		WCHAR* pszName = new WCHAR[nameLen];
		_info->GetAssemblyInfo(assemblyId, nameLen, &nameLen, pszName, NULL, NULL);

		if (OS::UnicodeToAnsi(pszName).find("System") != string::npos)
		{
			return false;
		}
		if (OS::UnicodeToAnsi(pszName).find("Microsoft") != string::npos)
		{
			return false;
		}
		if (OS::UnicodeToAnsi(pszName).find("Internal") != string::npos)
		{
			return false;
		}
		delete[] pszName;
	}

	auto* functionInfo = new FunctionInfo();

	functionInfo->funcId = functionId;
	functionInfo->name = name;

	std::map<int, FunctionInfo*> list;
	list[OS::GetTid()] = (functionInfo);

	m_functionMap.insert(std::pair<FunctionID, std::map<int, FunctionInfo*>>(functionId, list));
	
	return true;
}



#ifdef _X86_
#ifdef _WIN32
void __declspec(naked) EnterNaked(FunctionIDOrClientID functionIDOrClientID, COR_PRF_ELT_INFO eltInfo)
	{
		  __asm
		  {
			    PUSH EAX
			    PUSH ECX
			    PUSH EDX
		PUSH[ESP + 16]
		CALL EnterStub
		POP EDX
		POP ECX
		POP EAX
		RET 8
			  }
			}

void __declspec(naked) LeaveNaked(FunctionIDOrClientID functionIDOrClientID, COR_PRF_ELT_INFO eltInfo)
	{
	 __asm
		  {
			PUSH EAX
			PUSH ECX
		    PUSH EDX
		PUSH[ESP + 16]
		CALL LeaveStub
		POP EDX
		POP ECX
		POP EAX
		RET 8
	  }
		}

void __declspec(naked) TailcallNaked(FunctionIDOrClientID functionIDOrClientID, COR_PRF_ELT_INFO eltInfo)
{
    __asm
    {
		PUSH EAX
		PUSH ECX
		PUSH EDX
		PUSH[ESP + 16]
		CALL TailcallStub
		POP EDX
		POP ECX
		POP EAX
		RET 8
	}
	}
#endif
#elif defined(_AMD64_)
EXTERN_C void EnterNaked(FunctionIDOrClientID functionIDOrClientID, COR_PRF_ELT_INFO eltInfo);
EXTERN_C void LeaveNaked(FunctionIDOrClientID functionIDOrClientID, COR_PRF_ELT_INFO eltInfo);
EXTERN_C void TailcallNaked(FunctionIDOrClientID functionIDOrClientID, COR_PRF_ELT_INFO eltInfo);
#endif


HRESULT __stdcall CoreProfiler::QueryInterface(REFIID riid, void** ppvObject) {
	if (ppvObject == nullptr)
		return E_POINTER;

	if (riid == __uuidof(IUnknown) ||
		riid == __uuidof(ICorProfilerCallback) ||
		riid == __uuidof(ICorProfilerCallback2) ||
		riid == __uuidof(ICorProfilerCallback3) ||
		riid == __uuidof(ICorProfilerCallback4) ||
		riid == __uuidof(ICorProfilerCallback5) ||
		riid == __uuidof(ICorProfilerCallback6) ||
		riid == __uuidof(ICorProfilerCallback7) ||
		riid == __uuidof(ICorProfilerCallback8)) {
		AddRef();
		*ppvObject = static_cast<ICorProfilerCallback8*>(this);
		return S_OK;
	}

	return E_NOINTERFACE;
}

ULONG __stdcall CoreProfiler::AddRef(void) {
	return ++_refCount;
}

ULONG __stdcall CoreProfiler::Release(void) {
	auto count = --_refCount;
	if (count == 0)
		delete this;

	return count;
}

HRESULT CoreProfiler::Initialize(IUnknown* pICorProfilerInfoUnk) {
	g_CoreProfiler = this;
	std::cout << "Profiler initialize, cpu time:" << OS::GetCpuTime() << ", wall time:" << OS::GetWallTime() << ", pid: " << OS::GetPid() << "\n";

	Logger::LOGInSh("[{\"Profiler\":"
			"{\"action\":\"initialize\","
			"\"PID\":\"%d\","
			"\"TID\":\"0x%p\","
			"\"enterWALLt\":\"%f\","
			"\"enterCPUt\":\"%f\"}},",
			OS::GetPid(), OS::GetTid() , OS::GetWallTime(), OS::GetCpuTime());

	pICorProfilerInfoUnk->QueryInterface(&_info);
	assert(_info);

	auto modeStr = stoi(OS::ReadEnvironmentVariable("PROFILER_MODE"));

	if (modeStr == 0)
	{
		_info->SetEventMask(
			COR_PRF_MONITOR_MODULE_LOADS |
			COR_PRF_MONITOR_ASSEMBLY_LOADS |
			COR_PRF_MONITOR_GC |
			COR_PRF_MONITOR_CLASS_LOADS |
			COR_PRF_MONITOR_THREADS |
			COR_PRF_MONITOR_EXCEPTIONS |
			COR_PRF_MONITOR_JIT_COMPILATION |	
			COR_PRF_MONITOR_OBJECT_ALLOCATED | 
			COR_PRF_ENABLE_OBJECT_ALLOCATED);
	} else
	{
		_info->SetEventMask(
			COR_PRF_MONITOR_THREADS |
			COR_PRF_MONITOR_ENTERLEAVE |
			COR_PRF_ENABLE_FUNCTION_ARGS |
			COR_PRF_ENABLE_FUNCTION_RETVAL |
			COR_PRF_ENABLE_FRAME_INFO);
	}
	
	_info->SetEnterLeaveFunctionHooks3WithInfo(EnterNaked, LeaveNaked, TailcallNaked);

	_info->SetFunctionIDMapper(Mapper2);

	return S_OK;
}

HRESULT CoreProfiler::Shutdown() {
	cout << "Profiler shutdown, cpu time:" << OS::GetCpuTime() << ", wall time:" << OS::GetWallTime() << ", pid: " << OS::GetPid() << "\n";
	Logger::LOGInSh("{\"Profiler\":"
		"{\"action\":\"shutdown\","
		"\"PID\":\"%d\","
		"\"TID\":\"0x%p\","
		"\"enterWALLt\":\"%f\","
		"\"enterCPUt\":\"%f\"}}]",
		OS::GetPid(), OS::GetTid(), OS::GetWallTime(), OS::GetCpuTime());

	for (auto map : m_functionMap)
	{
		for (auto &function : map.second)
		{
			function.second->cpuTimeEnter.clear();
			function.second->wallTimeEnter.clear();
		}
	}

	m_functionMap.clear();
	g_CoreProfiler = NULL;
	
	return S_OK;
}


void CoreProfiler::Enter(FunctionID functionID, COR_PRF_ELT_INFO eltInfo)
{
	if (!m_functionMap[functionID].empty())
	{
		auto tmp = m_functionMap[functionID];
		if (tmp[OS::GetTid()] != nullptr)
		{
			tmp[OS::GetTid()]->cpuTimeEnter.push_back(OS::GetCpuTime());
			tmp[OS::GetTid()]->wallTimeEnter.push_back(OS::GetWallTime());
		} else
		{
			auto tmp2 = m_functionMap[functionID].begin();
			auto* functionInfo = new FunctionInfo();

			functionInfo->funcId = tmp2->first;
			functionInfo->name = tmp2->second->name;
			functionInfo->cpuTimeEnter.push_back(OS::GetCpuTime());
			functionInfo->wallTimeEnter.push_back(OS::GetWallTime());

			m_functionMap[functionID][OS::GetTid()] = functionInfo;
		}
	}
}

void CoreProfiler::Leave(FunctionID functionID, COR_PRF_ELT_INFO eltInfo)
{
	if (!m_functionMap[functionID].empty())
	{
		auto* tmp = m_functionMap[functionID][OS::GetTid()];
		if (tmp != nullptr) {

			Logger::LOG("\"Function\":{"
				"\"fID\":\"%p\","
				"\"fName\":\"%s\","
				"\"PID\":\"%d\","
				"\"TID\":\"%d\","
				"\"enterWALLt\":\"%f\","
				"\"leaveWALLt\":\"%f\","
				"\"enterCPUt\":\"%f\","
				"\"leaveCPUt\":\"%f\"}",
				tmp->funcId, tmp->name.c_str(), OS::GetPid(), OS::GetTid(), tmp->wallTimeEnter.back(), OS::GetWallTime(),
				tmp->cpuTimeEnter.back(), OS::GetCpuTime());

			tmp->cpuTimeEnter.pop_back();
		}
	}
}

void CoreProfiler::TailCall(FunctionID functionID, COR_PRF_ELT_INFO eltInfo)
{
	if (!m_functionMap[functionID].empty())
	{
		auto* tmp = m_functionMap[functionID][OS::GetTid()];
		if (tmp != nullptr) {
			Logger::LOG("\"Function\":{"
				"\"fID\":\"%p\","
				"\"fName\":\"%s\","
				"\"PID\":\"%d\","
				"\"TID\":\"%d\","
				"\"enterWALLt\":\"%f\","
				"\"leaveWALLt\":\"%f\","
				"\"enterCPUt\":\"%f\","
				"\"leaveCPUt\":\"%f\"}",
				tmp->funcId, tmp->name.c_str(), OS::GetPid(), OS::GetTid(), tmp->wallTimeEnter.back(), OS::GetWallTime(),
				tmp->cpuTimeEnter.back(), OS::GetCpuTime());

			tmp->cpuTimeEnter.pop_back();
		}
	}
}

HRESULT CoreProfiler::AppDomainCreationStarted(AppDomainID appDomainId) {
	return S_OK;
}

HRESULT CoreProfiler::AppDomainCreationFinished(AppDomainID appDomainId, HRESULT hrStatus) {
	return S_OK;
}

HRESULT CoreProfiler::AppDomainShutdownStarted(AppDomainID appDomainId) {
	return S_OK;
}

HRESULT CoreProfiler::AppDomainShutdownFinished(AppDomainID appDomainId, HRESULT hrStatus) {
	return S_OK;
}

HRESULT CoreProfiler::AssemblyLoadStarted(AssemblyID assemblyId) {
	return S_OK;
}

HRESULT CoreProfiler::AssemblyLoadFinished(AssemblyID assemblyId, HRESULT hrStatus) {
	return S_OK;
}

HRESULT CoreProfiler::AssemblyUnloadStarted(AssemblyID assemblyId) {
	return S_OK;
}

HRESULT CoreProfiler::AssemblyUnloadFinished(AssemblyID assemblyId, HRESULT hrStatus) {
	return S_OK;
}

HRESULT CoreProfiler::ModuleLoadStarted(ModuleID moduleId) {
	return S_OK;
}

HRESULT CoreProfiler::ModuleLoadFinished(ModuleID moduleId, HRESULT hrStatus) {

	return S_OK;
}

HRESULT CoreProfiler::ModuleUnloadStarted(ModuleID moduleId) {
	return S_OK;
}

HRESULT CoreProfiler::ModuleUnloadFinished(ModuleID moduleId, HRESULT hrStatus) {
	return S_OK;
}

HRESULT CoreProfiler::ModuleAttachedToAssembly(ModuleID moduleId, AssemblyID AssemblyId) {
	return S_OK;
}

HRESULT CoreProfiler::ClassLoadStarted(ClassID classId) {
	return S_OK;
}

HRESULT CoreProfiler::ClassLoadFinished(ClassID classId, HRESULT hrStatus) {
	return S_OK;
}

HRESULT CoreProfiler::ClassUnloadStarted(ClassID classId) {
	return S_OK;
}

HRESULT CoreProfiler::ClassUnloadFinished(ClassID classId, HRESULT hrStatus) {
	return S_OK;
}

HRESULT CoreProfiler::FunctionUnloadStarted(FunctionID functionId) {
	return S_OK;
}

HRESULT CoreProfiler::JITCompilationStarted(FunctionID functionId, BOOL fIsSafeToBlock) {
	return S_OK;
}

HRESULT CoreProfiler::JITCompilationFinished(FunctionID functionId, HRESULT hrStatus, BOOL fIsSafeToBlock) {
	return S_OK;
}

HRESULT CoreProfiler::JITCachedFunctionSearchStarted(FunctionID functionId, BOOL* pbUseCachedFunction) {
	return S_OK;
}

HRESULT CoreProfiler::JITCachedFunctionSearchFinished(FunctionID functionId, COR_PRF_JIT_CACHE result) {
	return S_OK;
}

HRESULT CoreProfiler::JITFunctionPitched(FunctionID functionId) {
	return S_OK;
}

HRESULT CoreProfiler::JITInlining(FunctionID callerId, FunctionID calleeId, BOOL* pfShouldInline) {
	return S_OK;
}

HRESULT CoreProfiler::ThreadCreated(ThreadID threadId) {
	Logger::LOG("\"Thread\":{"
		"\"action\":\"created\","
		"\"PID\":\"%d\","
		"\"TID\":\"0x%p\","
		"\"enterWALLt\":\"%f\","
		"\"enterCPUt\":\"%f\"}",
		OS::GetPid(), threadId, OS::GetWallTime(), OS::GetCpuTime());

	return S_OK;
}

HRESULT CoreProfiler::ThreadDestroyed(ThreadID threadId) {
	Logger::LOG("\"Thread\":{"
		"\"action\":\"destroyed\","
		"\"PID\":\"%d\","
		"\"TID\":\"0x%p\","
		"\"enterWALLt\":\"%f\","
		"\"enterCPUt\":\"%f\"}",
		OS::GetPid(), threadId, OS::GetWallTime(), OS::GetCpuTime());

	return S_OK;
}

HRESULT CoreProfiler::ThreadAssignedToOSThread(ThreadID managedThreadId, DWORD osThreadId) {
	return S_OK;
}

HRESULT CoreProfiler::RemotingClientInvocationStarted() {
	return S_OK;
}

HRESULT CoreProfiler::RemotingClientSendingMessage(GUID* pCookie, BOOL fIsAsync) {
	return S_OK;
}

HRESULT CoreProfiler::RemotingClientReceivingReply(GUID* pCookie, BOOL fIsAsync) {
	return S_OK;
}

HRESULT CoreProfiler::RemotingClientInvocationFinished() {
	return S_OK;
}

HRESULT CoreProfiler::RemotingServerReceivingMessage(GUID* pCookie, BOOL fIsAsync) {
	return S_OK;
}

HRESULT CoreProfiler::RemotingServerInvocationStarted() {
	return S_OK;
}

HRESULT CoreProfiler::RemotingServerInvocationReturned() {
	return S_OK;
}

HRESULT CoreProfiler::RemotingServerSendingReply(GUID* pCookie, BOOL fIsAsync) {
	return S_OK;
}

HRESULT CoreProfiler::UnmanagedToManagedTransition(FunctionID functionId, COR_PRF_TRANSITION_REASON reason) {
	return S_OK;
}

HRESULT CoreProfiler::ManagedToUnmanagedTransition(FunctionID functionId, COR_PRF_TRANSITION_REASON reason) {
	return S_OK;
}

HRESULT CoreProfiler::RuntimeSuspendStarted(COR_PRF_SUSPEND_REASON suspendReason) {
	return S_OK;
}

HRESULT CoreProfiler::RuntimeSuspendFinished() {
	return S_OK;
}

HRESULT CoreProfiler::RuntimeSuspendAborted() {
	return S_OK;
}

HRESULT CoreProfiler::RuntimeResumeStarted() {
	return S_OK;
}

HRESULT CoreProfiler::RuntimeResumeFinished() {
	return S_OK;
}

HRESULT CoreProfiler::RuntimeThreadSuspended(ThreadID threadId) {
	return S_OK;
}

HRESULT CoreProfiler::RuntimeThreadResumed(ThreadID threadId) {
	return S_OK;
}

HRESULT CoreProfiler::MovedReferences(ULONG cMovedObjectIDRanges, ObjectID* oldObjectIDRangeStart, ObjectID* newObjectIDRangeStart, ULONG* cObjectIDRangeLength) {
	return S_OK;
}

HRESULT CoreProfiler::ObjectAllocated(ObjectID objectId, ClassID classId) {
	ModuleID module;
	mdTypeDef type;
	if (SUCCEEDED(_info->GetClassIDInfo(classId, &module, &type))) {
		auto name = GetTypeName(type, module);

		if (!name.empty())
		{
			Logger::LOG("\"ObjectAllocated\":{"
				"\"PID\":\"%d\","
				"\"TID\":\"0x%p\","
				"\"objectId\":\"0x%p\","
				"\"objectType\":\"%s\","
				"\"enterWALLt\":\"%f\","
				"\"enterCPUt\":\"%f\"}",
				OS::GetPid(), OS::GetTid(), objectId, name.c_str(), OS::GetWallTime(), OS::GetCpuTime());
		}
	}
	return S_OK;
}

HRESULT CoreProfiler::ObjectsAllocatedByClass(ULONG cClassCount, ClassID* classIds, ULONG* cObjects) {

	return S_OK;
}

HRESULT CoreProfiler::ObjectReferences(ObjectID objectId, ClassID classId, ULONG cObjectRefs, ObjectID* objectRefIds) {
	return S_OK;
}

HRESULT CoreProfiler::RootReferences(ULONG cRootRefs, ObjectID* rootRefIds) {
	return S_OK;
}

HRESULT CoreProfiler::ExceptionThrown(ObjectID thrownObjectId) {
	return S_OK;
}

HRESULT CoreProfiler::ExceptionSearchFunctionEnter(FunctionID functionId) {
	return S_OK;
}

HRESULT CoreProfiler::ExceptionSearchFunctionLeave() {
	return S_OK;
}

HRESULT CoreProfiler::ExceptionSearchFilterEnter(FunctionID functionId) {
	return S_OK;
}

HRESULT CoreProfiler::ExceptionSearchFilterLeave() {
	return S_OK;
}

HRESULT CoreProfiler::ExceptionSearchCatcherFound(FunctionID functionId) {
	return S_OK;
}

HRESULT CoreProfiler::ExceptionOSHandlerEnter(UINT_PTR __unused) {
	return S_OK;
}

HRESULT CoreProfiler::ExceptionOSHandlerLeave(UINT_PTR __unused) {
	return S_OK;
}

HRESULT CoreProfiler::ExceptionUnwindFunctionEnter(FunctionID functionId) {
	return S_OK;
}

HRESULT CoreProfiler::ExceptionUnwindFunctionLeave() {
	return S_OK;
}

HRESULT CoreProfiler::ExceptionUnwindFinallyEnter(FunctionID functionId) {
	return S_OK;
}

HRESULT CoreProfiler::ExceptionUnwindFinallyLeave() {
	return S_OK;
}

HRESULT CoreProfiler::ExceptionCatcherEnter(FunctionID functionId, ObjectID objectId) {
	return S_OK;
}

HRESULT CoreProfiler::ExceptionCatcherLeave() {
	return S_OK;
}

HRESULT CoreProfiler::COMClassicVTableCreated(ClassID wrappedClassId, const GUID& implementedIID, void* pVTable, ULONG cSlots) {
	return S_OK;
}

HRESULT CoreProfiler::COMClassicVTableDestroyed(ClassID wrappedClassId, const GUID& implementedIID, void* pVTable) {
	return S_OK;
}

HRESULT CoreProfiler::ExceptionCLRCatcherFound() {
	return S_OK;
}

HRESULT CoreProfiler::ExceptionCLRCatcherExecute() {
	return S_OK;
}

HRESULT CoreProfiler::ThreadNameChanged(ThreadID threadId, ULONG cchName, WCHAR* name) {
	return S_OK;
}

HRESULT CoreProfiler::GarbageCollectionStarted(int cGenerations, BOOL* generationCollected, COR_PRF_GC_REASON reason) {

	Logger::LOG("\"GC\":{"
		"\"action\":\"started\","
		"\"PID\":\"%d\","
		"\"TID\":\"0x%p\","
		"\"Gen0\":\"0x%s\","
		"\"Gen1\":\"0x%s\","
		"\"Gen2\":\"0x%s\","
		"\"enterWALLt\":\"%f\","
		"\"enterCPUt\":\"%f\"}",
		OS::GetPid(), OS::GetTid(), generationCollected[0] ? "Yes" : "No", generationCollected[1] ? "Yes" : "No",
		generationCollected[2] ? "Yes" : "No", OS::GetWallTime(), OS::GetCpuTime());

	return S_OK;
}

HRESULT CoreProfiler::SurvivingReferences(ULONG cSurvivingObjectIDRanges, ObjectID* objectIDRangeStart, ULONG* cObjectIDRangeLength) {
	return S_OK;
}

HRESULT CoreProfiler::GarbageCollectionFinished() {
	Logger::LOG("\"GC\":{"
		"\"action\":\"finished\","
		"\"PID\":\"%d\","
		"\"TID\":\"0x%p\","
		"\"enterWALLt\":\"%f\","
		"\"enterCPUt\":\"%f\"}",
		OS::GetPid(), OS::GetTid(), OS::GetWallTime(), OS::GetCpuTime());
	return S_OK;
}

HRESULT CoreProfiler::FinalizeableObjectQueued(DWORD finalizerFlags, ObjectID objectID) {
	return S_OK;
}

HRESULT CoreProfiler::RootReferences2(ULONG cRootRefs, ObjectID* rootRefIds, COR_PRF_GC_ROOT_KIND* rootKinds, COR_PRF_GC_ROOT_FLAGS* rootFlags, UINT_PTR* rootIds) {
	return S_OK;
}

HRESULT CoreProfiler::HandleCreated(GCHandleID handleId, ObjectID initialObjectId) {
	return S_OK;
}

HRESULT CoreProfiler::HandleDestroyed(GCHandleID handleId) {
	return S_OK;
}

HRESULT CoreProfiler::InitializeForAttach(IUnknown* pCorProfilerInfoUnk, void* pvClientData, UINT cbClientData) {
	return S_OK;
}

HRESULT CoreProfiler::ProfilerAttachComplete() {
	return S_OK;
}

HRESULT CoreProfiler::ProfilerDetachSucceeded() {
	return S_OK;
}

HRESULT CoreProfiler::ReJITCompilationStarted(FunctionID functionId, ReJITID rejitId, BOOL fIsSafeToBlock) {
	return S_OK;
}

HRESULT CoreProfiler::GetReJITParameters(ModuleID moduleId, mdMethodDef methodId, ICorProfilerFunctionControl* pFunctionControl) {
	return S_OK;
}

HRESULT CoreProfiler::ReJITCompilationFinished(FunctionID functionId, ReJITID rejitId, HRESULT hrStatus, BOOL fIsSafeToBlock) {
	return S_OK;
}

HRESULT CoreProfiler::ReJITError(ModuleID moduleId, mdMethodDef methodId, FunctionID functionId, HRESULT hrStatus) {
	return S_OK;
}

HRESULT CoreProfiler::MovedReferences2(ULONG cMovedObjectIDRanges, ObjectID* oldObjectIDRangeStart, ObjectID* newObjectIDRangeStart, SIZE_T* cObjectIDRangeLength) {
	return S_OK;
}

HRESULT CoreProfiler::SurvivingReferences2(ULONG cSurvivingObjectIDRanges, ObjectID* objectIDRangeStart, SIZE_T* cObjectIDRangeLength) {
	return S_OK;
}

HRESULT CoreProfiler::ConditionalWeakTableElementReferences(ULONG cRootRefs, ObjectID* keyRefIds, ObjectID* valueRefIds, GCHandleID* rootIds) {
	return S_OK;
}

HRESULT CoreProfiler::GetAssemblyReferences(const WCHAR* wszAssemblyPath, ICorProfilerAssemblyReferenceProvider* pAsmRefProvider) {
	return S_OK;
}

HRESULT CoreProfiler::ModuleInMemorySymbolsUpdated(ModuleID moduleId) {
	return S_OK;
}

HRESULT CoreProfiler::DynamicMethodJITCompilationStarted(FunctionID functionId, BOOL fIsSafeToBlock, LPCBYTE pILHeader, ULONG cbILHeader) {
	return S_OK;
}

HRESULT CoreProfiler::DynamicMethodJITCompilationFinished(FunctionID functionId, HRESULT hrStatus, BOOL fIsSafeToBlock) {
	return S_OK;
}


std::string GetTypeName(mdTypeDef type, ModuleID module) {
	CComPtr<IMetaDataImport> spMetadata;
	if (SUCCEEDED(_info->GetModuleMetaData(module, ofRead, IID_IMetaDataImport, reinterpret_cast<IUnknown**>(&spMetadata)))) {
		WCHAR name[256];
		ULONG nameSize = 256;
		DWORD flags;
		mdTypeDef baseType;
		if (SUCCEEDED(spMetadata->GetTypeDefProps(type, name, 256, &nameSize, &flags, &baseType))) {
			return OS::UnicodeToAnsi(name);
		}
	}
	return "";
}

std::string GetMethodName(FunctionID function) {
	ModuleID module;
	mdToken token;
	mdTypeDef type;
	ClassID classId;
	if (FAILED(_info->GetFunctionInfo(function, &classId, &module, &token)))
		return "";

	CComPtr<IMetaDataImport> spMetadata;
	if (FAILED(_info->GetModuleMetaData(module, ofRead, IID_IMetaDataImport, reinterpret_cast<IUnknown**>(&spMetadata))))
		return "";
	PCCOR_SIGNATURE sig;
	ULONG blobSize, size, attributes;
	WCHAR name[256];
	DWORD flags;
	ULONG codeRva;
	if (FAILED(spMetadata->GetMethodProps(token, &type, name, 256, &size, &attributes, &sig, &blobSize, &codeRva, &flags)))
		return "";

	return GetTypeName(type, module) + "::" + OS::UnicodeToAnsi(name) + "()";
}
