#include "stdafx.h"
#include <xlocinfo>

#define GREEN 10
#define DARKGREEN 2
#define RED 4
#define DARKGREY 8
#define LIGHTCYAN 11
#define YELLOW 14
#define WHITE 15


bool isLogging = false;

void setColor(unsigned int color)
{
	HANDLE screen = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(screen, color);
}


void Logger(unsigned int lvl, char* caller, char* logline, char* log, ...)
{
	while (isLogging) Sleep(10);
	isLogging = true;
	char logOut[1024];
	char logOut2[2024];
	setColor(DARKGREY);
	setColor(LIGHTCYAN);
	printf("%s ", caller);
	if (lvl == lINFO) setColor(GREEN);
	else if (lvl == lWARN) setColor(YELLOW);
	else if (lvl == lERROR) setColor(RED);
	else if (lvl == lDEBUG) setColor(DARKGREEN);
	va_list argList;
	va_start(argList, logline);
	vsnprintf(logOut, 1024, logline, argList);
	va_end(argList);
	va_list argList2;
	va_start(argList2, log);
	vsnprintf(logOut2, 2024, log, argList2);
	va_end(argList);
	printf("%s", logOut );
	if (log) setColor(WHITE);
	printf("%s\n",  logOut2);
	isLogging = false;
}
