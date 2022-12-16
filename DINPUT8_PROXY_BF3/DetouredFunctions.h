#include <Ws2tcpip.h>
#include <shellapi.h>
#include "Logger.h"
//Typedefs
typedef int(__stdcall* FUNC3)(__in SOCKET s, __in const struct sockaddr* name, __in int namelen);
typedef hostent* (__stdcall * FUNC1)(char* name);
typedef char (__fastcall * FUNC2)(__int64 a1, __int64* a2);

const wchar_t* rhost = L"127.0.0.1";

//Trampolines
FUNC1 orggethostbyname = NULL;
FUNC2 orgGetAuthCode = NULL;
FUNC3 orgmyconnect = NULL;

//cmd params
LPWSTR cmdlineparams(const char* param)
{
	LPWSTR* szArglist;
	int nArgs;
	int i;

	szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
	if (NULL == szArglist)
	{
		Logger(lERROR, "[0000000000]", "ERROR    ::  ", "COMMAND-LINE-PARAMS ERROR!! (check start arguments)");
		return 0;
	}
	else
	{
		// Default value
		LPWSTR result = szArglist[0];

		if (param == "exename")
		{
			LPWSTR result = szArglist[0];
			return result;
		}
		if (param == "blazeip")
		{
			LPWSTR result = szArglist[4];
			return result;
		}
		return result;
	}
}


hostent* __stdcall mygethostbyname(char* name)
{
	LPWSTR blazeip = cmdlineparams("blazeip");
	char _blazeip[16];
	wcstombs(_blazeip, blazeip, 16);

	Logger(lWARN, "[1588272965]", "WARN    ::  ", "gethostbyname :: redirecting %s to %s", name, _blazeip);
	hostent* ret = orggethostbyname((char*)_blazeip);
	return ret;
}


bool doesFileExist(const char *filename) {
    return GetFileAttributesA(filename) != INVALID_FILE_ATTRIBUTES;
}


//Detouring
BYTE* DetourFunction(BYTE* address, BYTE* detour)
{
	BYTE* result = NULL;
	SYSTEM_INFO sSysInfo;
	GetSystemInfo(&sSysInfo);
	DWORD dwOldProtect;
	VirtualProtect((LPVOID)address, sSysInfo.dwPageSize, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	MH_CreateHook((LPVOID)address, (LPVOID)detour, (LPVOID*)&result);
	MH_EnableHook((LPVOID)address);
	VirtualProtect((LPVOID)address, sSysInfo.dwPageSize, dwOldProtect, &dwOldProtect);
	return result;
}

void DetourTheFunctions()
{

	LPWSTR blazeip = cmdlineparams("blazeip");
	char _blazeip[16];
	wcstombs(_blazeip, blazeip, 16);

    HMODULE h;
    FARPROC address;

	LoadLibraryA("ws2_32.dll");
	if((h = GetModuleHandleA("Ws2_32.dll")) != nullptr)
    {
        if((address = GetProcAddress(h, "gethostbyname")) != nullptr)
        { 
            orggethostbyname = (FUNC1)DetourFunction((LPBYTE)address, (LPBYTE)mygethostbyname);
			Logger(lINFO, "[1588272910]", "IMPORT  ::  ", "gethostbyname : %s",_blazeip);
        }
	}
}