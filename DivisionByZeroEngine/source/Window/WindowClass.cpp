#include "WindowClass.h"

#include "Window.h"

#include "Utils/Debug.h"

#if IS_WINDOWS_PLATFORM

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace
{
	__declspec(thread) Window* locUpdatingWindow = nullptr;

	LRESULT CALLBACK locMainWindowCallback(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_CLOSE:
			if(locUpdatingWindow)
				locUpdatingWindow->Close();
			break;
		case WM_SIZE:
			if (locUpdatingWindow)
				locUpdatingWindow->GetWindowResizeCallbacks()(LOWORD(lParam), HIWORD(lParam));
			break;
		case WM_PAINT:
			if (locUpdatingWindow)
				locUpdatingWindow->GetWindowPaintCallbacks()();
			break;
		case WM_KEYDOWN:
			if (locUpdatingWindow && wParam < 256)
				locUpdatingWindow->GetWindowKeyDownCallbacks()(static_cast<unsigned char>(wParam));
			break;
		case WM_KEYUP:
			if (locUpdatingWindow && wParam < 256)
				locUpdatingWindow->GetWindowKeyUpCallbacks()(static_cast<unsigned char>(wParam));
			break;
		}

		// Default message handling
		return DefWindowProc(handle, message, wParam, lParam);
	}
}

WindowClass::WindowCallback WindowClass::ourWindowCallback = reinterpret_cast<WindowClass::WindowCallback>(locMainWindowCallback);

#endif // IS_WINDOWS_PLATFORM

#include <string.h>

void WindowClass::Create(const char* aName, WindowCallback aWindowCallback, WindowClass& aWindowClass)
{
	aWindowClass.myProcessHandle = Process::GetHandle();
	aWindowClass.myName = aName;
	aWindowClass.myWindowCallback = aWindowCallback ? aWindowCallback : WindowClass::ourWindowCallback;

#if IS_WINDOWS_PLATFORM

	aWindowClass.myStyle = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

	WNDCLASSEX window_class_extended;
	window_class_extended.cbSize = sizeof(WNDCLASSEX);
	window_class_extended.style = aWindowClass.myStyle;
	window_class_extended.lpfnWndProc = reinterpret_cast<WNDPROC>(aWindowClass.myWindowCallback);
	window_class_extended.cbClsExtra = 0;
	window_class_extended.cbWndExtra = 0;
	window_class_extended.hInstance = aWindowClass.myProcessHandle;
	window_class_extended.hIcon = nullptr;
	window_class_extended.hCursor = LoadCursor(nullptr, IDC_ARROW);
	window_class_extended.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	window_class_extended.lpszMenuName = nullptr;
	window_class_extended.lpszClassName = aWindowClass.myName;
	window_class_extended.hIconSm = nullptr;

	if (!RegisterClassEx(&window_class_extended))
		Debug::Breakpoint(); // We failed to register class

#endif // IS_WINDOWS_PLATFORM
}

void WindowClass::Destroy(WindowClass& aWindowClass)
{
#if IS_WINDOWS_PLATFORM
	UnregisterClass(aWindowClass.myName, aWindowClass.myProcessHandle);
#endif // IS_WIN32_API

#if IS_DEBUG_BUILD
	memset(&aWindowClass, 0, sizeof(aWindowClass));
#endif // IS_DEBUG_BUILD
}

void WindowClass::SetThreadUpdatingWindow(Window* aWindow)
{
	locUpdatingWindow = aWindow;
}

