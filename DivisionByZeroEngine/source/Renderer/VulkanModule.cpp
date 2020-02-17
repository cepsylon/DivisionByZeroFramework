#include "VulkanModule.h"

#include "Platform/PlatformIncludes.h"

VulkanModule::Handle VulkanModule::ourHandle = nullptr;
int VulkanModule::ourReferenceCount = 0;

bool VulkanModule::Load()
{
	// We previously loaded the library
	if (ourReferenceCount)
		return ourReferenceCount++;

#if IS_WINDOWS_PLATFORM
	ourHandle = reinterpret_cast<VulkanModule::Handle>(LoadLibrary("vulkan-1.dll"));
#endif

	if (ourHandle)
		ourReferenceCount++;

	return ourReferenceCount;
}

void VulkanModule::Unload()
{
	if (ourReferenceCount == 1)
	{
#if IS_WINDOWS_PLATFORM
		FreeLibrary(reinterpret_cast<HMODULE>(ourHandle));
#endif

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
#endif
}
