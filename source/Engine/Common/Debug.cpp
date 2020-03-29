#include "Debug.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <debugapi.h>

#include <stdio.h>

namespace Debug
{

void PrintToOutput(const char* format, ...)
{
	char buffer[1024];
	va_list arg;
	va_start(arg, format);
	_vsnprintf_s(buffer, sizeof(buffer), format, arg);
	va_end(arg);
	
	OutputDebugString(buffer);
}

void Breakpoint()
{
	__debugbreak();
}

}
