#pragma once

#include "VulkanWrapper/VulkanWrapper.h"

class Renderer
{
public:
	void Initialize();

private:
	VulkanWrapper myVulkanWrapper;
};
