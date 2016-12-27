/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in LuaUI2.h
*/

#include "stdafx.h"
#include "Common.h"
#include "ResourceManager.h"

HINSTANCE g_hInstance;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	g_hInstance = hModule;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		OutputDebugStringA("DLL_PROCESS_ATTACH\r\n");
		break;
	case DLL_THREAD_ATTACH:
		OutputDebugStringA("DLL_THREAD_ATTACH\r\n");
		break;
	case DLL_THREAD_DETACH:
		OutputDebugStringA("DLL_THREAD_DETACH\r\n");
		break;
	case DLL_PROCESS_DETACH:
		OutputDebugStringA("DLL_PROCESS_DETACH\r\n");
		ResourceManager::Destroy();
		break;
	}
	return TRUE;
}

