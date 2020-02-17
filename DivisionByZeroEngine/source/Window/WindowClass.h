#pragma once

#include "Platform/PlatformSpecific.h"

class Window;

#if IS_WINDOWS_PLATFORM
struct HWND__;
#endif // IS_WINDOWS_PLATFORM

class WindowClass
{
public:

#if IS_WINDOWS_PLATFORM
#if IS_X86_PLATFORM
	using WindowCallback = long(__stdcall*)(HWND__*, unsigned, unsigned, long);
#else
	using WindowCallback = __int64(__stdcall*)(HWND__*, unsigned, unsigned __int64, __int64);
#endif // IS_X86_PLATORM
#endif // IS_WINDOWS_PLATFORM

	
	// Must call these for lifetime of object
	// Set aWindowCallback to null for default handling
	static void Create(const char* aName, WindowCallback aWindowCallback, WindowClass& aWindowClass);
	static void Destroy(WindowClass& aWindowClass);

private:
	friend class Window;
	static void SetThreadUpdatingWindow(Window* aWindow);

	static WindowCallback ourWindowCallback;

	Process::Handle myProcessHandle;
	const char* myName;
	WindowCallback myWindowCallback;
	unsigned myStyle;
};
