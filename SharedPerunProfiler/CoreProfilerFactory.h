/*
 * Bachelor thesis: Performance Analysis of C# Programs
 *
 * Author: Hajek Vojtech (xhajek51)
 * This file is based on: https://github.com/zodiacon/DotNextMoscow2019
 */
#pragma once

class CoreProfilerFactory : public IClassFactory {
public:
	// Inherited via IClassFactory
	HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObject) override;
	ULONG __stdcall AddRef(void) override;
	ULONG __stdcall Release(void) override;
	HRESULT __stdcall CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppvObject) override;
	HRESULT __stdcall LockServer(BOOL fLock) override {
		return E_NOTIMPL;
	}
};

