// stdafx.h : Includedatei für Standardsystem-Includedateien
// oder häufig verwendete projektspezifische Includedateien,
// die nur in unregelmäßigen Abständen geändert werden.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Selten verwendete Teile der Windows-Header nicht einbinden.
// Windows-Headerdateien:
#include <windows.h>
#include <winternl.h>
#include <imagehlp.h>


//////////////     dinput8      /////////////////////////////
#ifdef DLL_EXPORT
#define DINPUT8_API __declspec(dllexport)
#else
#define DINPUT8_API __declspec(dllexport)
#endif

//////////////     Logger       /////////////////////////////
#define lINFO 0
#define lWARN 1
#define lERROR 2
#define lDEBUG 4
void Logger(unsigned int lvl, char* caller, char* logline, char* log, ...);
// TODO: Hier auf zusätzliche Header, die das Programm erfordert, verweisen.
