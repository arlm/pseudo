// Copyright (c) John Lyon-Smith 2002.  All rights reserved.

#include "Precomp.h"
#include "MultiFruit.h"
#include "SingleFruit.h"

ComClassRegister g_pCoClasses[] = 
{
	{ &CLSID_MultiFruit, ComThreadingModel::ModelFree, L"MultiFruit", 1, L"Pseudo Fruit Class", MultiFruit::CreateObject, false},
	{ &CLSID_SingleFruit, ComThreadingModel::ModelFree, L"SingleFruit", 1, L"Pseudo Singleton Fruit Class", SingleFruit::CreateObject, true},
	{0}
};

HINSTANCE g_hInstance;

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(lpReserved);
	
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		GetComHelp().Initialize(hInstance, L"Pseudo.TestComLib", g_pCoClasses);

		g_hInstance = hInstance;
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
	}

	return TRUE;
}

#include "Pseudo\ComExports.cpp"
