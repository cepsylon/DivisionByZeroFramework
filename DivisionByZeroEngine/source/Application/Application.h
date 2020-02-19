#pragma once

#include "Window/WindowClass.h"
#include "Window/Window.h"
#include "VulkanWrapper/VulkanWrapper.h"

class Application
{
public:
	void Initialize();
	void Run();
	void Shutdown();

	void WindowResize(uint32_t aWidth, uint32_t aHeight);
	void WindowPaint();

private:
	friend Application & App();

	Application() = default;
	Application(const Application &) = delete;
	Application & operator=(const Application &) = delete;

	WindowClass myWindowClass;
	Window myMainWindow;
	VulkanWrapper myVulkanWrapper;
};

Application & App();
