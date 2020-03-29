#pragma once

#include "VulkanWrapper/VulkanWrapper.h"

#include <list>

class VRamManager
{
public:

private:

	struct VPage
	{
		struct Block
		{
			uint32_t mySize = 0u;
			uint32_t myOffset = 0u;
		};

		VPage* myNext = nullptr;
		DeviceMemory myDeviceMemory;
		std::list<Block> myFreeBlocks;
		std::list<Block> myInUseBlocks;
	};

	VPage* myHead = nullptr;
};
