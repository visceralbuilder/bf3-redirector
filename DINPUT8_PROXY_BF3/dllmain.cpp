#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <tchar.h>
#include <stdlib.h>
#include "MinHook.h"
#include <WinSock2.h> 
#include <direct.h>
#include <io.h>
#include <fcntl.h>
#include <iostream>
#include <fstream>
#pragma comment(lib,"ws2_32.lib")
#pragma pack(1)

#include "DetouredFunctions.h"
#include "Helper.h"
#include "dinput.h"


DirectInput8Create_t OriginalFunction = nullptr;
HMODULE DInput8DLL = nullptr;

DINPUT8_API HRESULT DirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter)
{
	if (OriginalFunction)
	{
		return OriginalFunction(hinst, dwVersion, riidltf, ppvOut, punkOuter);
	}
	return S_FALSE;
}

DWORD orgDecrypt = NULL;

char* cmdline = GetCommandLineA();

DWORD WINAPI Start(LPVOID lpParam)
{
	Logger(lINFO, "[1588272910]", "IMPORT  ::  ", "VENICE Console Init.");
	Logger(lINFO, "[1588272910]", "IMPORT  ::  ", "Version: 1");
	MH_Initialize();
	if (strstr(cmdline, "-patchSSL") != 0)
	{
		SearchOffsets();
		DetourTheFunctions();
	}
	else Logger(lERROR, "[0000000000]", "ERROR    ::  ", "WARNING DONT USE THIS DLL ON OFFICIAL BATTLEFIELD SERVER'S!       USE  '-patchSSL'  PARAMETER TO CONNECT TO EMULATOR");
	return 1;
}

void Init()
{
	DWORD dwThreadId, dwThrdParam = 1;
	HANDLE hThread;
	if (strstr(cmdline, "-noconsole") == 0)
	{
		OpenConsole();
	}
	char DInputDllName[MAX_PATH];
	GetSystemDirectoryA(DInputDllName, MAX_PATH);
	strcat_s(DInputDllName, "\\dinput8.dll");
	DInput8DLL = LoadLibraryA(DInputDllName);
	if (DInput8DLL > (HMODULE)31)
	{
		OriginalFunction = (DirectInput8Create_t)GetProcAddress(DInput8DLL, "DirectInput8Create");
	}
	hThread = CreateThread(NULL, 0, Start, &dwThrdParam, 0, &dwThreadId);
}

BOOL APIENTRY DllMain(HMODULE Module,DWORD  ReasonForCall,LPVOID Reserved)
{
	switch (ReasonForCall)
	{
	case DLL_PROCESS_ATTACH:
	Init();
	break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
    break;
	}
	return TRUE;
}
