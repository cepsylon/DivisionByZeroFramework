#include "Process.h"

#if IS_WINDOWS_PLATFORM

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

namespace Process
{
	Handle GetHandle() { return GetModuleHandle(nullptr); }
}

#endif
