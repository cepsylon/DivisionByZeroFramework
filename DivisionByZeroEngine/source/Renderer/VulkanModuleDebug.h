#pragma once

#include "Platform/PlatformDefines.h"

#if IS_DEBUG_BUILD

#include "Utils/Debug.h"

#ifndef VULKAN_CHECK_VALID_RESULT
#define VULKAN_CHECK_VALID_RESULT(aFunctionCall) \
do \
{ \
	VkResult result = (aFunctionCall); \
	if(result != VK_SUCCESS) \
	{ \
		Debug::PrintToOutput("Result value is %d\n", static_cast<int>(result)); \
		Debug::Breakpoint(); \
	} \
} while(false)
#endif // VULKAN_CHECK_VALID_RESULT

#else // !IS_DEBUG_BUILD

#ifndef VULKAN_CHCEK_VALID_RESULT
#define VULKAN_CHECK_VALID_RESULT(aFunctionCall) (aFunctionCall)
#endif // VULKAN_CHECK_VALID_RESULT

#endif // !IS_DEBUG_BUILD