#pragma once

#include "VulkanWrapper/VulkanWrapper.h"

class Renderer
{
public:
	static void Create(Renderer& aRendererOut);
	static void Destroy(Renderer& aRenderer);

private:
	VulkanInstanceWrapper myVulkanInstanceWrapper;
	VulkanDeviceWrapper myVulkanDeviceWrapper;
};
