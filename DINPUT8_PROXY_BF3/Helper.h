#include<stdio.h>

bool Compare(const BYTE* pData, const BYTE* bMask, const char* szMask)
{
    for (; *szMask; ++szMask, ++pData, ++bMask)
        if (*szMask == 'x' && *pData != *bMask)   return 0;
    return (*szMask) == NULL;
}
BYTE* FindPattern(BYTE* dwAddress, DWORD dwLen, BYTE *bMask, char * szMask)
{
    for (DWORD i = 0; i < dwLen; i++)
        if (Compare((BYTE*)(dwAddress + i), bMask, szMask))  return (BYTE*)(dwAddress + i);
    return 0;
}

BOOL DoRtlAdjustPrivilege()
{
    #define SE_DEBUG_PRIVILEGE  20L
    #define AdjustCurrentProcess    0
    BOOL bPrev = FALSE;
    LONG (WINAPI *RtlAdjustPrivilege)(DWORD, BOOL, INT, PBOOL);
    *(FARPROC *)&RtlAdjustPrivilege = GetProcAddress(GetModuleHandle(_T("ntdll.dll")), "RtlAdjustPrivilege");
    if(!RtlAdjustPrivilege) return FALSE;
    RtlAdjustPrivilege(SE_DEBUG_PRIVILEGE, TRUE, AdjustCurrentProcess, &bPrev);
    return TRUE;
}
typedef BOOL (CALLBACK *LPENUMADDRESSES)(LPBYTE lpAddress, DWORD dwSize, DWORD dwState, DWORD dwType, DWORD dwProtect);

BOOL EnumProcessAddresses(HANDLE hProcess, LPENUMADDRESSES lpCallback)
{
    MEMORY_BASIC_INFORMATION mbi;
    SYSTEM_INFO msi;
    ZeroMemory(&mbi, sizeof(mbi));
    GetSystemInfo(&msi);
    for(LPBYTE lpAddress = (LPBYTE)msi.lpMinimumApplicationAddress;
        lpAddress <= (LPBYTE)msi.lpMaximumApplicationAddress;
        lpAddress += mbi.RegionSize){
        if(VirtualQueryEx(hProcess, lpAddress, &mbi, sizeof(mbi))){
            if(lpCallback && !lpCallback((LPBYTE)mbi.BaseAddress, mbi.RegionSize,
                mbi.State, mbi.Type, mbi.Protect)) return FALSE;
        } else break;
    }
    return TRUE;
}
LPBYTE baseAddress = NULL;

int CertCheck()
{
	return 0;
}

void PatchSSLCertCheck(BYTE* address)
{
	DetourFunction(address, (LPBYTE)CertCheck);
}

unsigned char patternServer[] = { 0x81, 0xEC, 0x04, 0x04, 0x00, 0x00, 0x80, 0xBC, 0x24, 0x10, 0x04, 0x00, 0x00, 0x01, 0x53, 0x55 };
BOOL CALLBACK ScanAddressRange(LPBYTE lpAddress, DWORD dwSize, DWORD dwState, DWORD dwType, DWORD dwProtect)
{
	char* cmdline = GetCommandLineA();
	FILE* bf3;
	if ((dwProtect & PAGE_EXECUTE_READ) == 0)
		return TRUE;
	BYTE* offset;
	offset = FindPattern(lpAddress, dwSize, patternServer, "xxxxxxxxxxxxxxx");
	if (offset != 0)
	{
		PatchSSLCertCheck(offset);
		Logger(lWARN, "[1588272910]", "WARN    ::  ", "Patched SSL Valiadtion");
		Logger(lWARN, "[1588272910]", "WARN    ::  ", "BF3: Server");
		return FALSE;
	}
	if (strstr(cmdline, "-client") != 0)
	{
		PatchSSLCertCheck((PBYTE)0xB2BEA0);
		Logger(lWARN, "[1588272910]", "WARN    ::  ", "Patched SSL Valiadtion");
		Logger(lWARN, "[1588272910]", "WARN    ::  ", "BF3: Client");
		return FALSE;
	}
	return TRUE;
}
void SearchOffsets()
{
	if (DoRtlAdjustPrivilege())
		EnumProcessAddresses(GetCurrentProcess(), ScanAddressRange);
	else;
}

void hexdump(void *pAddressIn, long  lSize, char* source)
{
	FILE* fp = fopen("C:\\packets\\log.txt","a");
	fprintf(fp, "%s\r\n", source);
	char szBuf[100];
	long lIndent = 1;
	long lOutLen, lIndex, lIndex2, lOutLen2;
	long lRelPos;
	struct { char *pData; unsigned long lSize; } buf;
	unsigned char *pTmp,ucTmp;
	unsigned char *pAddress = (unsigned char *)pAddressIn;
	buf.pData   = (char *)pAddress;
	buf.lSize   = lSize;
	while (buf.lSize > 0)
	{
		pTmp     = (unsigned char *)buf.pData;
		lOutLen  = (int)buf.lSize;
		if (lOutLen > 16)
			lOutLen = 16;
		sprintf(szBuf, " >                            "
						"                      "
						"    %08lX", pTmp-pAddress);
		lOutLen2 = lOutLen;
		for(lIndex = 1+lIndent, lIndex2 = 53-15+lIndent, lRelPos = 0;
			lOutLen2;
			lOutLen2--, lIndex += 2, lIndex2++
			)
		{
			ucTmp = *pTmp++;
			sprintf(szBuf + lIndex, "%02X ", (unsigned short)ucTmp);
			if(!isprint(ucTmp))  ucTmp = '.';
			szBuf[lIndex2] = ucTmp;
			if (!(++lRelPos & 3))
			{  lIndex++; szBuf[lIndex+2] = ' '; }
		}
		if (!(lRelPos & 3)) lIndex--;
		szBuf[lIndex  ]   = '<';
		szBuf[lIndex+1]   = ' ';
		fprintf(fp,"%s\r\n", szBuf);
		buf.pData   += lOutLen;
		buf.lSize   -= lOutLen;
	}
	fprintf(fp,"\r\n");
	fclose(fp);
}

void logMessage(char* source, char* message)
{  
    FILE* fp = fopen("C:\\packets\\log.txt","a");
    fprintf(fp, "%s - %s\r\n", source, message);
    fclose(fp);
}

void OpenConsole()
{
	AllocConsole();
	freopen("conin$","r", stdin);
	freopen("conout$","w", stdout);
	freopen("conout$","w", stderr);
	HWND consoleHandle = GetConsoleWindow();
	MoveWindow(consoleHandle,1,1,1280,480,1);
}

void DeleteAllFiles(char* folderPath)
{
	wchar_t fileFound[256];
	WIN32_FIND_DATA info;
	HANDLE hp; 
	wsprintf(fileFound, _T("%S\\*.*"), folderPath);
	hp = FindFirstFile(fileFound, &info);
	do
	{
		wsprintf(fileFound,_T("%S\\%s"), folderPath, info.cFileName);
		DeleteFile(fileFound);
	}
	while(FindNextFile(hp, &info)); 
	FindClose(hp);
}

void SetupLogs()
{
	char* buff = (char*)calloc(1024, 1);
	if (CreateDirectoryA("C:\\packets", NULL) || GetLastError() == ERROR_ALREADY_EXISTS)
	{
		DeleteAllFiles("C:\\packets");
		FILE* fp = fopen("C:\\packets\\log.txt","w");
		fclose(fp);
	}

}
