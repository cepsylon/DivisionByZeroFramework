#pragma once

#include "GlobalDefines.h"

#include "VulkanDispatchTable.h"

struct VulkanModule
{
	static bool Load();
	static void Unload();

	static void* GetProcedureAddress(const char* aProcedureName);

#if IS_WINDOWS_PLATFORM
	using Handle = struct HMODULE__*;
#endif // IS_WINDOWS_PLATFORM

	static Handle ourHandle;
	static VulkanCommonDispatchTable ourTable;
	static int ourReferenceCount;
};
