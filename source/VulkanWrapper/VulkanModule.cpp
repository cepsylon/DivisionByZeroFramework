#include "VulkanModule.h"

#if IS_WINDOWS_PLATFORM

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN
#include <Windows.h>

#endif // IS_WINDOWS_PLATFORM

#include <assert.h>

VulkanModule::Handle VulkanModule::ourHandle = nullptr;
VulkanCommonDispatchTable VulkanModule::ourTable;
int VulkanModule::ourReferenceCount = 0;

bool VulkanModule::Load()
{
	// We previously loaded the library
	if (ourReferenceCount)
		return ourReferenceCount++;

#if IS_WINDOWS_PLATFORM
	ourHandle = reinterpret_cast<VulkanModule::Handle>(LoadLibrary("vulkan-1.dll"));
#endif // IS_WINDOWS_PLATFORM

	if (ourHandle)
		ourReferenceCount++;

	VulkanCommonDispatchTable& table = ourTable;
	table.myGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(VulkanModule::GetProcedureAddress("vkGetInstanceProcAddr"));

	// Something went wrong, we cannot continue since that's the function used to retrieve others
	assert(table.myGetInstanceProcAddr);

	// Initialize functions needed for instance creation (available layers and extensions)
	table.myEnumerateInstanceVersion = reinterpret_cast<PFN_vkEnumerateInstanceVersion>(ourTable.myGetInstanceProcAddr(nullptr, "vkEnumerateInstanceVersion"));
	table.myEnumerateInstanceExtensionProperties = reinterpret_cast<PFN_vkEnumerateInstanceExtensionProperties>(ourTable.myGetInstanceProcAddr(nullptr, "vkEnumerateInstanceExtensionProperties"));
	table.myEnumerateInstanceLayerProperties = reinterpret_cast<PFN_vkEnumerateInstanceLayerProperties>(ourTable.myGetInstanceProcAddr(nullptr, "vkEnumerateInstanceLayerProperties"));
	table.myCreateInstance = reinterpret_cast<PFN_vkCreateInstance>(ourTable.myGetInstanceProcAddr(nullptr, "vkCreateInstance"));

	return ourReferenceCount;
}

void VulkanModule::Unload()
{
	if (ourReferenceCount == 1)
	{
#if IS_WINDOWS_PLATFORM
		FreeLibrary(reinterpret_cast<HMODULE>(ourHandle));
#endif // IS_WINDOWS_PLATFORM

		memset(&ourTable, 0, sizeof(ourTable));
		ourHandle = nullptr;
	}

	if(ourReferenceCount > 0)
		ourReferenceCount--;
}

void* VulkanModule::GetProcedureAddress(const char* aProcedureAddress)
{
	if (ourHandle == nullptr)
		return nullptr;

#if IS_WINDOWS_PLATFORM
	return GetProcAddress(reinterpret_cast<HMODULE>(ourHandle), aProcedureAddress);
#endif // IS_WINDOWS_PLATFORM
}
