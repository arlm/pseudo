// Copyright (c) Microsoft Corporation. All rights reserved.

#pragma once

#include <Pseudo\ComHelp.hpp>

// Include this header once in your project to declare the standard COM DLL exports

#ifdef _WIN64
#pragma comment(linker, "/EXPORT:DllCanUnloadNow,PRIVATE")
#else
#pragma comment(linker, "/EXPORT:DllCanUnloadNow=_DllCanUnloadNow@0,PRIVATE")
#endif
STDAPI DllCanUnloadNow(void)
{
    return S_OK;
}

// Returns a class factory to create an object of the requested type
#ifdef _WIN64
#pragma comment(linker, "/EXPORT:DllGetClassObject,PRIVATE")
#else
#pragma comment(linker, "/EXPORT:DllGetClassObject=_DllGetClassObject@12,PRIVATE")
#endif
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return Pseudo::GetComHelp().DllGetClassObject(rclsid, riid, ppv);
}

// DllRegisterServer - Adds entries to the system registry
#ifdef _WIN64
#pragma comment(linker, "/EXPORT:DllRegisterServer,PRIVATE")
#else
#pragma comment(linker, "/EXPORT:DllRegisterServer=_DllRegisterServer@0,PRIVATE")
#endif
STDAPI DllRegisterServer(void)
{
	return Pseudo::GetComHelp().DllRegisterServer();
}

// DllUnregisterServer - Removes entries from the system registry
#ifdef _WIN64
#pragma comment(linker, "/EXPORT:DllUnregisterServer,PRIVATE")
#else
#pragma comment(linker, "/EXPORT:DllUnregisterServer=_DllUnregisterServer@0,PRIVATE")
#endif
STDAPI DllUnregisterServer(void)
{
	return Pseudo::GetComHelp().DllUnregisterServer();;
}

// DllInstall - Does general product setup
#ifdef _WIN64
#pragma comment(linker, "/EXPORT:DllInstall,PRIVATE")
#else
#pragma comment(linker, "/EXPORT:DllInstall=_DllInstall@8,PRIVATE")
#endif
STDAPI DllInstall(
    BOOL bInstall,
    LPCWSTR p_cmd_line)
{
	(void)p_cmd_line;
	(void)bInstall;
	
    return S_OK;
}
