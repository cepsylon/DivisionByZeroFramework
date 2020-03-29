#include "Window.h"

#include "WindowClass.h"

#include "Common/Debug.h"

#if IS_WINDOWS_PLATFORM

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN
#include <Windows.h>

#endif // IS_WINDOWS_PLATFORM

void Window::Create(const WindowClass& aWindowClass, int aX, int aY, unsigned aWidth, unsigned aHeight, const char* aName, Window& aWindow)
{
	aWindow.myWindowClass = &aWindowClass;
	aWindow.myName = aName;
	aWindow.myX = aX;
	aWindow.myY = aY;
	aWindow.myClientWidth = aWidth;
	aWindow.myClientHeight = aHeight;

#if IS_WINDOWS_PLATFORM
	aWindow.myStyle = WS_OVERLAPPEDWINDOW;

	// Adjust size
	RECT windowRect{
		0, 0,
		static_cast<int32_t>(aWidth), static_cast<int32_t>(aHeight)
	};
	AdjustWindowRect(&windowRect, aWindow.myStyle, false);
	aWindow.myWidth = static_cast<unsigned>(windowRect.right - windowRect.left);
	aWindow.myHeight = static_cast<unsigned>(windowRect.bottom - windowRect.top);

	// Create window
	aWindow.myWindowHandle = CreateWindow(
		aWindowClass.myName,
		aWindow.myName,
		aWindow.myStyle,
		aX, aY,
		aWindow.myWidth, aWindow.myHeight,
		nullptr, nullptr, aWindowClass.myProcessHandle, nullptr);

	if (aWindow.myWindowHandle == nullptr)
		Debug::Breakpoint();

	// Display window
	ShowWindow(aWindow.myWindowHandle, SW_SHOW);
	UpdateWindow(aWindow.myWindowHandle);
#endif // IS_WINDOWS_PLATFORM

	aWindow.myIsRunning = true;

	// Register myself to the events I care about
	aWindow.myWindowResizeCallbacks.Add(aWindow, &Window::SetClientSize);
}

void Window::Destroy(Window& aWindow)
{
#if IS_WINDOWS_PLATFORM
	DestroyWindow(aWindow.myWindowHandle);
#endif // IS_WINDOWS_PLATFORM

#if IS_DEBUG_BUILD
	memset(&aWindow, 0, sizeof(aWindow));
#endif
}

void Window::Update()
{
	WindowClass::SetThreadUpdatingWindow(this);

#if IS_WINDOWS_PLATFORM
	// Check for messages until there is none
	MSG message;
	while (PeekMessage(&message, myWindowHandle, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
#endif // IS_WINDOWS_PLATFORM

	WindowClass::SetThreadUpdatingWindow(nullptr);
}

void Window::Close()
{
	myIsRunning = false;
}

void Window::Center()
{
#if IS_WINDOWS_PLATFORM
	// Get background size
	Handle desktopHandle = GetDesktopWindow();
	RECT desktopRect;
	if (GetWindowRect(desktopHandle, &desktopRect) == false)
		Debug::Breakpoint();// TODO: Check what was the last error

	// Compute centered position
	unsigned desktopWidth = desktopRect.right - desktopRect.left;
	unsigned desktopHeight = desktopRect.bottom - desktopRect.top;
	int x = (desktopWidth - static_cast<int>(myWidth)) / 2;
	int y = (desktopHeight - static_cast<int>(myHeight)) / 2;
#endif // IS_WINDOWS_PLATFORM

	SetPosition(x, y);
}

void Window::SetPosition(int aX, int aY)
{
#if IS_WINDOWS_PLATFORM
	if (MoveWindow(myWindowHandle, aX, aY, myWidth, myHeight, true) == false)
		Debug::Breakpoint();// TODO: Check what was the last error
#endif // IS_WINDOWS_PLATFORM
}

void Window::Resize(unsigned aWidth, unsigned aHeight)
{
	// TODO: actually requrest window resize to the OS
	myClientWidth = aWidth;
	myClientHeight = aHeight;
}

void Window::SetClientSize(unsigned aWidth, unsigned aHeight)
{
	myClientWidth = aWidth;
	myClientHeight = aHeight;
}
