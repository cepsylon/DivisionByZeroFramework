#pragma once

#include "GlobalDefines.h"

#include "Common/Callback.h"

#include <stdint.h>

class WindowClass;

#if IS_WINDOWS_PLATFORM
struct HWND__;
#endif // IS_WINDOWS_PLATFORM

class Window
{
public:

#if IS_WINDOWS_PLATFORM
	using Handle = HWND__*;
#endif // IS_WINDOWS_PLATFORM

	static void Create(const WindowClass& aWindowClass, int x, int y, unsigned width, unsigned height, const char* aName, Window& aWindowOut);
	static void Destroy(Window& aWindow);

	void Update();
	void Close();
	void Center();
	void SetPosition(int aX, int aY);
	void Resize(unsigned aWidth, unsigned aHeight);

	// Getters
	Callback<uint32_t, uint32_t>& GetWindowResizeCallbacks() { return myWindowResizeCallbacks; }
	const Callback<uint32_t, uint32_t>& GetWindowResizeCallbacks() const { return myWindowResizeCallbacks; }
	Callback<>& GetWindowPaintCallbacks() { return myWindowPaintCallbacks; }
	const Callback<>& GetWindowPaintCallbacks() const { return myWindowPaintCallbacks; }
	Callback<unsigned char>& GetWindowKeyDownCallbacks() { return myWindowKeyDownCallbacks; }
	const Callback<unsigned char>& GetWindowKeyDownCallbacks() const { return myWindowKeyDownCallbacks; }
	Callback<unsigned char>& GetWindowKeyUpCallbacks() { return myWindowKeyUpCallbacks; }
	const Callback<unsigned char>& GetWindowKeyUpCallbacks() const { return myWindowKeyUpCallbacks; }
	Handle GetWindowHandle() const { return myWindowHandle; }
	int GetX() const { return myX; }
	int GetY() const { return myY; }
	unsigned GetWidth() const { return myWidth; }
	unsigned GetHeight() const { return myHeight; }
	unsigned GetClientWidth() const { return myClientWidth; }
	unsigned GetClientHeight() const { return myClientHeight; }
	bool IsRunning() const { return myIsRunning; }

private:

	void SetClientSize(unsigned aWidth, unsigned aHeight);

	Callback<uint32_t, uint32_t> myWindowResizeCallbacks;
	Callback<> myWindowPaintCallbacks;
	Callback<unsigned char> myWindowKeyDownCallbacks;
	Callback<unsigned char> myWindowKeyUpCallbacks;
	Handle myWindowHandle;
	const WindowClass* myWindowClass;
	const char* myName;
	int myX;
	int myY;
	unsigned myWidth;
	unsigned myHeight;
	int myClientX;
	int myClientY;
	unsigned myClientWidth;
	unsigned myClientHeight;
	unsigned myStyle;
	bool myIsRunning;
};
