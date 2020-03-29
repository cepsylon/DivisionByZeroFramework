#include "VulkanWrapper.h"

#include "VulkanModule.h"
#include "VulkanWrapper.h"

#include "GlobalDefines.h"

#if IS_WINDOWS_PLATFORM

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "vulkan/vulkan_win32.h"

#include <stdio.h>

namespace
{

void DebugPrint(const char* format, ...)
{
	char buffer[1024];
	va_list arg;
	va_start(arg, format);
	_vsnprintf_s(buffer, sizeof(buffer), format, arg);
	va_end(arg);

	OutputDebugString(buffer);
}

}

#endif // IS_WINDOWS_PLATFORM

#include <assert.h>
#include <vector> // vector

#if IS_DEBUG_BUILD

#ifndef VULKAN_CHECK_VALID_RESULT
#define VULKAN_CHECK_VALID_RESULT(aFunctionCall) \
do \
{ \
	VkResult result = (aFunctionCall); \
	if(result != VK_SUCCESS) \
	{ \
		DebugPrint("Function call %s failed. Result value is %d\n", #aFunctionCall, static_cast<int>(result)); \
		assert(false); \
	} \
} while(false)
#endif // VULKAN_CHECK_VALID_RESULT

#else // !IS_DEBUG_BUILD

#ifndef VULKAN_CHCEK_VALID_RESULT
#define VULKAN_CHECK_VALID_RESULT(aFunctionCall) (aFunctionCall)
#endif // VULKAN_CHECK_VALID_RESULT

#endif // !IS_DEBUG_BUILD

namespace
{
	static VkBool32 ValidationLayerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT aMessageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT aMessageType,
		const VkDebugUtilsMessengerCallbackDataEXT *aCallbackData,
		void *pUserData);
}

void VulkanInstanceWrapper::Create(VulkanInstanceWrapper& aVulkanInstanceWrapper)
{
	assert(VulkanModule::Load());

	const VulkanCommonDispatchTable& table = VulkanModule::ourTable;

	// Check for needed extensions
	const char* instanceExtensions[] =
	{
		VK_KHR_SURFACE_EXTENSION_NAME,

#if IS_WINDOWS_PLATFORM
		"VK_KHR_win32_surface",
#endif // IS_WINDOWS_PLATFORM

#if IS_DEBUG_BUILD
		VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif // IS_DEBUG_BUILD
	};
	constexpr uint32_t neededExtensionCount = sizeof(instanceExtensions) / sizeof(const char*);
	bool foundExtensions[neededExtensionCount] = { false };

	// Get instance extensions
	uint32_t instanceExtensionCount = 0u;
	table.myEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, nullptr);
	std::vector<VkExtensionProperties> availableExtensions(instanceExtensionCount);
	table.myEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, availableExtensions.data());

	// Search for needed extensions
	uint32_t foundExtensionCount = 0;
	if (neededExtensionCount)
	{
		for (uint32_t i = 0; i < instanceExtensionCount; ++i)
		{
			for (uint32_t j = 0; j < neededExtensionCount; ++j)
			{
				if (foundExtensions[j] == false && std::strcmp(instanceExtensions[j], availableExtensions[i].extensionName) == 0)
				{
					foundExtensions[j] = true;
					foundExtensionCount++;
					break;
				}
			}

			if (foundExtensionCount == neededExtensionCount)
				break;
		}
	}
	assert(foundExtensionCount == neededExtensionCount);

	// Check for needed layers

	uint32_t neededLayerCount = 0u;
	const char* instanceLayers[10] = { nullptr };
	bool foundLayers[10] = { false };
#if IS_DEBUG_BUILD
	instanceLayers[0] = "VK_LAYER_KHRONOS_validation";
	neededLayerCount++;
#endif // IS_DEBUG_BUILD


	uint32_t availableLayerCount = 0u;
	table.myEnumerateInstanceLayerProperties(&availableLayerCount, nullptr);
	std::vector<VkLayerProperties> availableLayers(availableLayerCount);
	table.myEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data());

	uint32_t foundLayerCount = 0u;
	if (neededLayerCount)
	{
		for (uint32_t i = 0; i < availableLayerCount; ++i)
		{
			for (uint32_t j = 0; j < neededLayerCount; ++j)
			{
				if (foundLayers[j] == false && std::strcmp(instanceLayers[j], availableLayers[i].layerName) == 0)
				{
					foundLayers[j] = true;
					foundLayerCount++;
					break;
				}
			}

			if (foundLayerCount == neededExtensionCount)
				break;
		}
	}
	assert(foundLayerCount == neededLayerCount);

	uint32_t apiVersion;
	table.myEnumerateInstanceVersion(&apiVersion);

	// Initialize Vulkan instance for this renderer
	VkApplicationInfo applicationInfo
	{
		VK_STRUCTURE_TYPE_APPLICATION_INFO,
		nullptr,
		"VulkanBase",
		0,
		"DivisionByZero",
		0,
		apiVersion
	};

	VkInstanceCreateInfo instanceInfo{
		VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		nullptr,
		0,
		&applicationInfo,
		neededLayerCount,
		instanceLayers,
		neededExtensionCount,
		instanceExtensions
	};

#if IS_DEBUG_BUILD
	// Prepare validation layer
	VkDebugUtilsMessengerCreateInfoEXT debugMessengerInfo
	{
		VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		nullptr,
		0,
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
		reinterpret_cast<PFN_vkDebugUtilsMessengerCallbackEXT>(ValidationLayerCallback),
		nullptr
	};

	instanceInfo.pNext = &debugMessengerInfo;
#endif // IS_DEBUG_BUILD

	VkInstance& instance = Unwrap(aVulkanInstanceWrapper.myInstance);
	VkResult result = table.myCreateInstance(&instanceInfo, nullptr, &instance);
	assert(result == VK_SUCCESS);

	aVulkanInstanceWrapper.myTable.Initialize(instance);
}

void VulkanInstanceWrapper::Destroy(VulkanInstanceWrapper& aVulkanInstanceWrapper)
{
	VulkanInstanceDispatchTable& table = aVulkanInstanceWrapper.myTable;
	VkInstance& instance = Unwrap(aVulkanInstanceWrapper.myInstance);
	table.myDestroyInstance(instance, nullptr);

#if IS_DEBUG_BUILD
	memset(&aVulkanInstanceWrapper, 0, sizeof(aVulkanInstanceWrapper));
#endif // IS_DEBUG_BUILD

	VulkanModule::Unload();
}

void VulkanInstanceWrapper::EnumeratePhysicalDevices(uint32_t& aPhysicalDeviceCount, PhysicalDevice* somePhysicalDevicesOut) const
{
	VULKAN_CHECK_VALID_RESULT(myTable.myEnumeratePhysicalDevices(Unwrap(myInstance), &aPhysicalDeviceCount, Unwrap(somePhysicalDevicesOut)));
}

void VulkanInstanceWrapper::GetPhysicalDeviceProperties(const PhysicalDevice& aPhysicalDevice, VkPhysicalDeviceProperties& aPhysicalDevicePropertiesOut) const
{
	myTable.myGetPhysicalDeviceProperties(Unwrap(aPhysicalDevice), &aPhysicalDevicePropertiesOut);
}

void VulkanInstanceWrapper::GetPhysicalDeviceFeatures(const PhysicalDevice& aPhysicalDevice, VkPhysicalDeviceFeatures& aPhysicalDeviceFeaturesOut) const
{
	myTable.myGetPhysicalDeviceFeatures(Unwrap(aPhysicalDevice), &aPhysicalDeviceFeaturesOut);
}

void VulkanInstanceWrapper::GetPhysicalDeviceQueueFamilyProperties(const PhysicalDevice& aPhysicalDevice, uint32_t& aQueueFamilyPropertyCount, VkQueueFamilyProperties* someQueueFamilyPropertiesOut) const
{
	myTable.myGetPhysicalDeviceQueueFamilyProperties(Unwrap(aPhysicalDevice), &aQueueFamilyPropertyCount, someQueueFamilyPropertiesOut);
}

bool VulkanInstanceWrapper::GetPhysicalDeviceSurfaceSupportKHR(const PhysicalDevice& aPhysicalDevice, uint32_t aQueueFamilyIndex, const SurfaceKHR& aSurface) const
{
	VkBool32 hasSupportForSurface = false;
	VULKAN_CHECK_VALID_RESULT(myTable.myGetPhysicalDeviceSurfaceSupportKHR(Unwrap(aPhysicalDevice), aQueueFamilyIndex, Unwrap(aSurface), &hasSupportForSurface));
	return hasSupportForSurface;
}

void VulkanInstanceWrapper::GetPhysicalDeviceSurfaceFormatsKHR(const PhysicalDevice& aPhysicalDevice, const SurfaceKHR& aSurface, uint32_t& aSurfaceFormatCount, VkSurfaceFormatKHR* someSurfaceFormatsOut) const
{
	VULKAN_CHECK_VALID_RESULT(myTable.myGetPhysicalDeviceSurfaceFormatsKHR(Unwrap(aPhysicalDevice), Unwrap(aSurface), &aSurfaceFormatCount, someSurfaceFormatsOut));
}

void VulkanInstanceWrapper::GetPhysicalDeviceSurfaceCapabilitiesKHR(const PhysicalDevice& aPhysicalDevice, const SurfaceKHR& aSurface, VkSurfaceCapabilitiesKHR& aSurfaceCapabilitiesKHROut) const
{
	VULKAN_CHECK_VALID_RESULT(myTable.myGetPhysicalDeviceSurfaceCapabilitiesKHR(Unwrap(aPhysicalDevice), Unwrap(aSurface), &aSurfaceCapabilitiesKHROut));
}

void VulkanInstanceWrapper::GetPhysicalDeviceMemoryProperties(const PhysicalDevice& aPhysicalDevice, VkPhysicalDeviceMemoryProperties& aPhysicalDeviceMemoryPropertiesOut) const
{
	myTable.myGetPhysicalDeviceMemoryProperties(Unwrap(aPhysicalDevice), &aPhysicalDeviceMemoryPropertiesOut);
}

void VulkanInstanceWrapper::EnumerateDeviceExtensionProperties(const PhysicalDevice& aPhysicalDevice, const char* aLayerName, uint32_t& aPropertyCount, VkExtensionProperties* someExtensionPropertiesOut) const
{
	VULKAN_CHECK_VALID_RESULT(myTable.myEnumerateDeviceExtensionProperties(Unwrap(aPhysicalDevice), aLayerName, &aPropertyCount, someExtensionPropertiesOut));
}

void VulkanInstanceWrapper::Create(const PhysicalDevice& aPhysicalDevice, const VkDeviceCreateInfo& aDeviceCreateInfo, VulkanDeviceWrapper& aVulkanDeviceWrapperOut) const
{
	aVulkanDeviceWrapperOut.myQueueFamilyIndices = static_cast<uint32_t*>(malloc(sizeof(uint32_t) * aDeviceCreateInfo.queueCreateInfoCount));
	aVulkanDeviceWrapperOut.myQueues = static_cast<Queue*>(malloc(sizeof(Queue) * aDeviceCreateInfo.queueCreateInfoCount));
	aVulkanDeviceWrapperOut.myPhysicalDevice = aPhysicalDevice;
	VULKAN_CHECK_VALID_RESULT(myTable.myCreateDevice(Unwrap(aPhysicalDevice), &aDeviceCreateInfo, nullptr, Unwrap(&aVulkanDeviceWrapperOut.myDevice)));
	GetPhysicalDeviceMemoryProperties(aPhysicalDevice, aVulkanDeviceWrapperOut.myMemoryProperties);
	aVulkanDeviceWrapperOut.myTable.Initialize(Unwrap(aVulkanDeviceWrapperOut.myDevice), myTable.myGetDeviceProcAddr);

	for (uint32_t i = 0; i < aDeviceCreateInfo.queueCreateInfoCount; ++i)
	{
		uint32_t queueFamilyIndex = aDeviceCreateInfo.pQueueCreateInfos[i].queueFamilyIndex;
		aVulkanDeviceWrapperOut.myQueueFamilyIndices[i] = queueFamilyIndex;
		aVulkanDeviceWrapperOut.myTable.myGetDeviceQueue(Unwrap(aVulkanDeviceWrapperOut.myDevice), queueFamilyIndex, 0, Unwrap(&aVulkanDeviceWrapperOut.myQueues[i]));
	}
}

void VulkanInstanceWrapper::Destroy(VulkanDeviceWrapper& aVulkanDeviceWrapper) const
{
	free(aVulkanDeviceWrapper.myQueueFamilyIndices);
	free(aVulkanDeviceWrapper.myQueues);
	myTable.myDestroyDevice(Unwrap(aVulkanDeviceWrapper.myDevice), nullptr);

#if IS_DEBUG_BUILD
	memset(&aVulkanDeviceWrapper, 0, sizeof(aVulkanDeviceWrapper));
#endif // IS_DEBUG_BUILD
}

void VulkanInstanceWrapper::Create(void* aWindowHandle, SurfaceKHR& aSurface) const
{
#if IS_WINDOWS_PLATFORM
	// Create presentation surface
	VkWin32SurfaceCreateInfoKHR surfaceInfo{
		VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
		nullptr,
		0,
		GetModuleHandle(nullptr),
		reinterpret_cast<HWND>(aWindowHandle)
	};
	PFN_vkCreateWin32SurfaceKHR createSurface = reinterpret_cast<PFN_vkCreateWin32SurfaceKHR>(myTable.myGetInstanceProcAddr(Unwrap(myInstance), "vkCreateWin32SurfaceKHR"));
	VULKAN_CHECK_VALID_RESULT(createSurface(Unwrap(myInstance), &surfaceInfo, nullptr, Unwrap(&aSurface)));
#endif // IS_WINDOWS_PLATFORM
}

void VulkanInstanceWrapper::Destroy(SurfaceKHR& aSurface) const
{
	const VulkanInstanceDispatchTable& table = myTable;
	VkSurfaceKHR& surface = Unwrap(aSurface);
	table.myDestroySurfaceKHR(Unwrap(myInstance), surface, nullptr);

#if IS_DEBUG_BUILD
	surface = VK_NULL_HANDLE;
#endif // IS_DEBUG_BUILD
}

uint32_t VulkanDeviceWrapper::AcquireNextImage(const SwapchainKHR& aSwapchain, const Semaphore* aSemaphore, const Fence* aFence) const
{
	VkSemaphore semaphore = aSemaphore ? Unwrap(*aSemaphore) : VK_NULL_HANDLE;
	VkFence fence = aFence ? Unwrap(*aFence) : VK_NULL_HANDLE;
	uint32_t imageIndex;
	VULKAN_CHECK_VALID_RESULT(myTable.myAcquireNextImageKHR(Unwrap(myDevice), Unwrap(aSwapchain), UINT64_MAX, semaphore, fence, &imageIndex));
	return imageIndex;
}

void VulkanDeviceWrapper::GetSwapchainImagesKHR(const SwapchainKHR& aSwapchain, uint32_t& aSwapchainImageCount, Image* someSwapchainImagesOut) const
{
	VULKAN_CHECK_VALID_RESULT(myTable.myGetSwapchainImagesKHR(Unwrap(myDevice), Unwrap(aSwapchain), &aSwapchainImageCount, Unwrap(someSwapchainImagesOut)));
}

void VulkanDeviceWrapper::WaitForFences(Fence* someFences, uint32_t aFenceCount) const
{
	VULKAN_CHECK_VALID_RESULT(myTable.myWaitForFences(Unwrap(myDevice), aFenceCount, Unwrap(someFences), VK_TRUE, UINT64_MAX));
}

void VulkanDeviceWrapper::ResetFences(Fence* someFences, uint32_t aFenceCount) const
{
	VULKAN_CHECK_VALID_RESULT(myTable.myResetFences(Unwrap(myDevice), aFenceCount, Unwrap(someFences)));
}

void VulkanDeviceWrapper::Submit(uint32_t aQueueIndex, const VkSubmitInfo* someSubmitInfos, uint32_t aSubmitCount, const Fence* aFence) const
{
	VkFence fence = aFence ? Unwrap(*aFence) : VK_NULL_HANDLE;
	VULKAN_CHECK_VALID_RESULT(myTable.myQueueSubmit(Unwrap(myQueues[aQueueIndex]), aSubmitCount, someSubmitInfos, fence));
}

void VulkanDeviceWrapper::Present(const VkPresentInfoKHR& aPresentInfoKHR, uint32_t aQueueIndex) const
{
	VkResult result = myTable.myQueuePresentKHR(Unwrap(myQueues[aQueueIndex]), &aPresentInfoKHR);
	if (result != VK_SUCCESS)
	{
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
			DebugPrint("ERROR: OUT_OF_DATE_KHR\n");
		else if (result == VK_ERROR_SURFACE_LOST_KHR)
			DebugPrint("ERROR: SURFACE_LOST_KHR\n");
		else
			assert(false);
	}
}

void VulkanDeviceWrapper::WaitForDevice() const
{
	VULKAN_CHECK_VALID_RESULT(myTable.myDeviceWaitIdle(Unwrap(myDevice)));
}

void VulkanDeviceWrapper::Create(const VkSwapchainCreateInfoKHR& aSwapchainCreateInfoKHR, SwapchainKHR& aSwapchainOut) const
{
	VULKAN_CHECK_VALID_RESULT(myTable.myCreateSwapchainKHR(Unwrap(myDevice), &aSwapchainCreateInfoKHR, nullptr, Unwrap(&aSwapchainOut)));
}

void VulkanDeviceWrapper::Destroy(SwapchainKHR& aSwapchain) const
{
	VkSwapchainKHR& swapchain = Unwrap(aSwapchain);
	myTable.myDestroySwapchainKHR(Unwrap(myDevice), swapchain, nullptr);

#if IS_DEBUG_BUILD
	swapchain = VK_NULL_HANDLE;
#endif
}

void VulkanDeviceWrapper::Create(const VkCommandPoolCreateInfo& aCommandPoolCreateInfo, CommandPool& aCommandPoolOut) const
{
	const VulkanDeviceDispatchTable& table = myTable;
	VULKAN_CHECK_VALID_RESULT(table.myCreateCommandPool(Unwrap(myDevice), &aCommandPoolCreateInfo, nullptr, Unwrap(&aCommandPoolOut)));
}

void VulkanDeviceWrapper::Destroy(CommandPool& aCommandPool) const
{
	const VulkanDeviceDispatchTable& table = myTable;
	VkCommandPool& commandPool = Unwrap(aCommandPool);
	table.myDestroyCommandPool(Unwrap(myDevice), commandPool, nullptr);

#if IS_DEBUG_BUILD
	commandPool = VK_NULL_HANDLE;
#endif // IS_DEBUG_BUILD
}

void VulkanDeviceWrapper::Create(const VkCommandBufferAllocateInfo& aCommandBufferAllocateInfo, VulkanCommandBufferWrapper* someVulkanCommandBufferWrappers) const
{
	const VulkanDeviceDispatchTable& table = myTable;
	CommandBuffer* commandBuffers = static_cast<CommandBuffer*>(alloca(sizeof(CommandBuffer) * aCommandBufferAllocateInfo.commandBufferCount));
	VULKAN_CHECK_VALID_RESULT(table.myAllocateCommandBuffers(Unwrap(myDevice), &aCommandBufferAllocateInfo, Unwrap(commandBuffers)));

	VulkanCommandBufferDispatchTable commandBufferTable;
	commandBufferTable.Initialize(Unwrap(myDevice), myTable.myGetDeviceProcAddr);
	for (uint32_t i = 0; i < aCommandBufferAllocateInfo.commandBufferCount; ++i)
	{
		someVulkanCommandBufferWrappers[i].myCommandBuffer = commandBuffers[i];
		someVulkanCommandBufferWrappers[i].myTable = commandBufferTable;
	}
}

void VulkanDeviceWrapper::Destroy(CommandPool& aCommandPool, VulkanCommandBufferWrapper* someCommandBuffers, uint32_t aCommandBufferCount) const
{
	CommandBuffer* commandBuffers = static_cast<CommandBuffer*>(alloca(sizeof(CommandBuffer) * aCommandBufferCount));
	for (uint32_t i = 0; i < aCommandBufferCount; ++i)
		commandBuffers[i] = someCommandBuffers[i].myCommandBuffer;

	const VulkanDeviceDispatchTable& table = myTable;
	table.myFreeCommandBuffers(Unwrap(myDevice), Unwrap(aCommandPool), aCommandBufferCount, Unwrap(commandBuffers));

#if IS_DEBUG_BUILD
	std::memset(someCommandBuffers, 0, sizeof(VulkanCommandBufferWrapper) * aCommandBufferCount);
#endif // IS_DEBUG_BUILD
}

void VulkanDeviceWrapper::Create(const VkRenderPassCreateInfo& aRenderPassCreateInfo, RenderPass& aRenderPass) const
{
	const VulkanDeviceDispatchTable& table = myTable;
	VULKAN_CHECK_VALID_RESULT(table.myCreateRenderPass(Unwrap(myDevice), &aRenderPassCreateInfo, nullptr, Unwrap(&aRenderPass)));
}

void VulkanDeviceWrapper::Destroy(RenderPass& aRenderPass) const
{
	const VulkanDeviceDispatchTable& table = myTable;
	VkRenderPass& renderPass = Unwrap(aRenderPass);
	table.myDestroyRenderPass(Unwrap(myDevice), renderPass, nullptr);

#if IS_DEBUG_BUILD
	renderPass = VK_NULL_HANDLE;
#endif // IS_DEBUG_BUILD
}

void VulkanDeviceWrapper::Create(const VkImageViewCreateInfo& aImageViewCreateInfo, ImageView& aImageViewOut) const
{
	const VulkanDeviceDispatchTable& table = myTable;
	VULKAN_CHECK_VALID_RESULT(table.myCreateImageView(Unwrap(myDevice), &aImageViewCreateInfo, nullptr, Unwrap(&aImageViewOut)));
}

void VulkanDeviceWrapper::Destroy(ImageView& aImageView) const
{
	const VulkanDeviceDispatchTable& table = myTable;
	VkImageView& imageView = Unwrap(aImageView);
	table.myDestroyImageView(Unwrap(myDevice), imageView, nullptr);

#if IS_DEBUG_BUILD
	imageView = VK_NULL_HANDLE;
#endif // IS_DEBUG_BUILD
}

void VulkanDeviceWrapper::Create(const VkFramebufferCreateInfo& aFramebufferCreateInfo, Framebuffer& aFramebufferOut) const
{
	const VulkanDeviceDispatchTable& table = myTable;
	VULKAN_CHECK_VALID_RESULT(table.myCreateFramebuffer(Unwrap(myDevice), &aFramebufferCreateInfo, nullptr, Unwrap(&aFramebufferOut)));
}

void VulkanDeviceWrapper::Destroy(Framebuffer& aFramebuffer) const
{
	const VulkanDeviceDispatchTable& table = myTable;
	VkFramebuffer& framebuffer = Unwrap(aFramebuffer);
	table.myDestroyFramebuffer(Unwrap(myDevice), framebuffer, nullptr);

#if IS_DEBUG_BUILD
	framebuffer = VK_NULL_HANDLE;
#endif // IS_DEBUG_BUILD
}

void VulkanDeviceWrapper::Create(const VkPipelineLayoutCreateInfo& aPipelineLayoutCreateInfo, PipelineLayout& aPipelineLayoutOut) const
{
	const VulkanDeviceDispatchTable& table = myTable;
	VULKAN_CHECK_VALID_RESULT(table.myCreatePipelineLayout(Unwrap(myDevice), &aPipelineLayoutCreateInfo, nullptr, Unwrap(&aPipelineLayoutOut)));
}

void VulkanDeviceWrapper::Destroy(PipelineLayout& aPipelineLayout) const
{
	const VulkanDeviceDispatchTable& table = myTable;
	VkPipelineLayout& pipelineLayout = Unwrap(aPipelineLayout);
	table.myDestroyPipelineLayout(Unwrap(myDevice), pipelineLayout, nullptr);

#if IS_DEBUG_BUILD
	pipelineLayout = VK_NULL_HANDLE;
#endif // IS_DEBUG_BUILD
}

void VulkanDeviceWrapper::Create(const VkGraphicsPipelineCreateInfo* someGraphicsPipelineCreateInfos, Pipeline* somePipelinesOut, uint32_t aPipelineCount) const
{
	const VulkanDeviceDispatchTable& table = myTable;
	VULKAN_CHECK_VALID_RESULT(table.myCreateGraphicsPipelines(Unwrap(myDevice), VK_NULL_HANDLE, aPipelineCount, someGraphicsPipelineCreateInfos, nullptr, Unwrap(somePipelinesOut)));
}

void VulkanDeviceWrapper::Destroy(Pipeline& aPipeline) const
{
	const VulkanDeviceDispatchTable& table = myTable;
	VkPipeline& pipeline = Unwrap(aPipeline);
	table.myDestroyPipeline(Unwrap(myDevice), pipeline, nullptr);

#if IS_DEBUG_BUILD
	pipeline = VK_NULL_HANDLE;
#endif // IS_DEBUG_BUILD
}

void VulkanDeviceWrapper::Create(const VkSemaphoreCreateInfo& aSemaphoreCreateInfo, Semaphore& aSemaphoreOut) const
{
	const VulkanDeviceDispatchTable& table = myTable;
	VULKAN_CHECK_VALID_RESULT(table.myCreateSemaphore(Unwrap(myDevice), &aSemaphoreCreateInfo, nullptr, Unwrap(&aSemaphoreOut)));
}

void VulkanDeviceWrapper::Destroy(Semaphore& aSemaphore) const
{
	const VulkanDeviceDispatchTable& table = myTable;
	VkSemaphore& semaphore = Unwrap(aSemaphore);
	table.myDestroySemaphore(Unwrap(myDevice), semaphore, nullptr);

#if IS_DEBUG_BUILD
	semaphore = VK_NULL_HANDLE;
#endif // IS_DEBUG_BUILD
}

void VulkanDeviceWrapper::Create(const VkFenceCreateInfo& aFenceCreateInfo, Fence& aFenceOut) const
{
	const VulkanDeviceDispatchTable& table = myTable;
	VULKAN_CHECK_VALID_RESULT(table.myCreateFence(Unwrap(myDevice), &aFenceCreateInfo, nullptr, Unwrap(&aFenceOut)));
}

void VulkanDeviceWrapper::Destroy(Fence& aFence) const
{
	const VulkanDeviceDispatchTable& table = myTable;
	VkFence& fence = Unwrap(aFence);
	table.myDestroyFence(Unwrap(myDevice), fence, nullptr);

#if IS_DEBUG_BUILD
	fence = VK_NULL_HANDLE;
#endif // IS_DEBUG_BUILD
}

void VulkanDeviceWrapper::Create(const VkShaderModuleCreateInfo& aShaderModuleCreateInfo, ShaderModule& aShaderModuleOut) const
{
	const VulkanDeviceDispatchTable& table = myTable;
	VULKAN_CHECK_VALID_RESULT(table.myCreateShaderModule(Unwrap(myDevice), &aShaderModuleCreateInfo, nullptr, Unwrap(&aShaderModuleOut)));
}

void VulkanDeviceWrapper::Destroy(ShaderModule& aShaderModule) const
{
	const VulkanDeviceDispatchTable& table = myTable;
	VkShaderModule& shaderModule = Unwrap(aShaderModule);
	table.myDestroyShaderModule(Unwrap(myDevice), shaderModule, nullptr);

#if IS_DEBUG_BUILD
	shaderModule = VK_NULL_HANDLE;
#endif // IS_DEBUG_BUILD
}

void VulkanDeviceWrapper::Create(const VkBufferCreateInfo& aBufferCreateInfo, Buffer& aBufferOut) const
{
	const VulkanDeviceDispatchTable& table = myTable;
	VULKAN_CHECK_VALID_RESULT(table.myCreateBuffer(Unwrap(myDevice), &aBufferCreateInfo, nullptr, Unwrap(&aBufferOut)));
}

void VulkanDeviceWrapper::Destroy(Buffer& aBuffer) const
{
	const VulkanDeviceDispatchTable& table = myTable;
	VkBuffer& buffer = Unwrap(aBuffer);
	table.myDestroyBuffer(Unwrap(myDevice), buffer, nullptr);

#if IS_DEBUG_BUILD
	buffer = VK_NULL_HANDLE;
#endif // IS_DEBUG_BUILD
}

void VulkanDeviceWrapper::Create(const VkDescriptorSetLayoutCreateInfo& aDescriptorSetLayoutCreateInfo, DescriptorSetLayout& aDescriptorSetLayoutOut) const
{
	const VulkanDeviceDispatchTable& table = myTable;
	VULKAN_CHECK_VALID_RESULT(table.myCreateDescriptorSetLayout(Unwrap(myDevice), &aDescriptorSetLayoutCreateInfo, nullptr, Unwrap(&aDescriptorSetLayoutOut)));
}

void VulkanDeviceWrapper::Destroy(DescriptorSetLayout& aDescriptorSetLayout) const
{
	const VulkanDeviceDispatchTable& table = myTable;
	VkDescriptorSetLayout& descriptorSetLayout = Unwrap(aDescriptorSetLayout);
	table.myDestroyDescriptorSetLayout(Unwrap(myDevice), descriptorSetLayout, nullptr);

#if IS_DEBUG_BUILD
	descriptorSetLayout = VK_NULL_HANDLE;
#endif
}

void VulkanDeviceWrapper::Create(const VkDescriptorPoolCreateInfo& aDescriptorPoolCreateInfo, DescriptorPool& aDescriptorPoolOut) const
{
	const VulkanDeviceDispatchTable& table = myTable;
	VULKAN_CHECK_VALID_RESULT(table.myCreateDescriptorPool(Unwrap(myDevice), &aDescriptorPoolCreateInfo, nullptr, Unwrap(&aDescriptorPoolOut)));
}

void VulkanDeviceWrapper::Destroy(DescriptorPool& aDescriptorPool) const
{
	const VulkanDeviceDispatchTable& table = myTable;
	VkDescriptorPool& descriptorPool = Unwrap(aDescriptorPool);
	table.myDestroyDescriptorPool(Unwrap(myDevice), descriptorPool, nullptr);

#if IS_DEBUG_BUILD
	descriptorPool = VK_NULL_HANDLE;
#endif
}

void VulkanDeviceWrapper::Create(const VkDescriptorSetAllocateInfo& aDescriptorSetAllocateInfo, DescriptorSet* someDescriptorSets) const
{
	const VulkanDeviceDispatchTable& table = myTable;
	VULKAN_CHECK_VALID_RESULT(table.myAllocateDescriptorSets(Unwrap(myDevice), &aDescriptorSetAllocateInfo, Unwrap(someDescriptorSets)));
}

void VulkanDeviceWrapper::Destroy(DescriptorPool& aDescriptorPool, DescriptorSet* someDescriptorSet, uint32_t aDescriptorSetCount) const
{
	const VulkanDeviceDispatchTable& table = myTable;
	VkDescriptorSet* descriptorSet = Unwrap(someDescriptorSet);
	table.myFreeDescriptorSets(Unwrap(myDevice), Unwrap(aDescriptorPool), aDescriptorSetCount, descriptorSet);
#if IS_DEBUG_BUILD
	std::memset(descriptorSet, 0, sizeof(VkDescriptorSet) * aDescriptorSetCount);
#endif // IS_DEBUG_BUILD
}

void VulkanDeviceWrapper::UpdateDescriptorSets(const VkWriteDescriptorSet* someWriteDescriptorSets, uint32_t aWriteDescriptorCount) const
{
	const VulkanDeviceDispatchTable& table = myTable;
	table.myUpdateDescriptorSets(Unwrap(myDevice), aWriteDescriptorCount, someWriteDescriptorSets, 0, nullptr);
}

void VulkanDeviceWrapper::GetMemoryRequirements(const Buffer& aBuffer, VkMemoryRequirements& aMemoryRequirementsOut) const
{
	const VulkanDeviceDispatchTable& deviceTable = myTable;
	deviceTable.myGetBufferMemoryRequirements(Unwrap(myDevice), Unwrap(aBuffer), &aMemoryRequirementsOut);
}

void VulkanDeviceWrapper::AllocateDeviceMemory(VkDeviceSize aSize, uint32_t aMemoryTypeBits, VkMemoryPropertyFlags aMemoryProperties, DeviceMemory& aDeviceMemoryOut) const
{
	// Get physical device memory properties
	const VulkanDeviceDispatchTable& deviceTable = myTable;

	// Search for memory
	uint32_t memoryIndex = UINT32_MAX;
	for (uint32_t i = 0; i < myMemoryProperties.memoryTypeCount; ++i)
	{
		uint32_t memoryTypeBits = (1 << i);

		bool validMemoryType = memoryTypeBits & aMemoryTypeBits;
		bool hasWantedMemoryProperties = myMemoryProperties.memoryTypes[i].propertyFlags & aMemoryProperties;
		if (validMemoryType && hasWantedMemoryProperties)
		{
			memoryIndex = i;
			break;
		}
	}

	if (memoryIndex == UINT32_MAX)
		assert(false);

	// Allocate memory
	VkMemoryAllocateInfo memoryAllocateInfo
	{
		VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		nullptr,
		aSize,
		memoryIndex
	};

	deviceTable.myAllocateMemory(Unwrap(myDevice), &memoryAllocateInfo, nullptr, Unwrap(&aDeviceMemoryOut));
}

void VulkanDeviceWrapper::FreeDeviceMemory(DeviceMemory& aDeviceMemory) const
{
	const VulkanDeviceDispatchTable& table = myTable;
	VkDeviceMemory& deviceMemory = Unwrap(aDeviceMemory);
	table.myFreeMemory(Unwrap(myDevice), deviceMemory, nullptr);

#if IS_DEBUG_BUILD
	deviceMemory = VK_NULL_HANDLE;
#endif // IS_DEBUG_BUILD
}

void VulkanDeviceWrapper::BindDeviceMemory(DeviceMemory& aDeviceMemory, VkDeviceSize anOffset, Buffer& aBuffer) const
{
	const VulkanDeviceDispatchTable& deviceTable = myTable;
	deviceTable.myBindBufferMemory(Unwrap(myDevice), Unwrap(aBuffer), Unwrap(aDeviceMemory), anOffset);
}

void* VulkanDeviceWrapper::MapDeviceMemory(DeviceMemory& aDeviceMemory, VkDeviceSize anOffset, VkDeviceSize aSize) const
{
	const VulkanDeviceDispatchTable& table = myTable;

	void* mappedMemory = nullptr;
	VULKAN_CHECK_VALID_RESULT(table.myMapMemory(Unwrap(myDevice), Unwrap(aDeviceMemory), anOffset, aSize, 0, &mappedMemory));
	return mappedMemory;
}

void VulkanDeviceWrapper::UnmapDeviceMemory(DeviceMemory& aDeviceMemory) const
{
	const VulkanDeviceDispatchTable& table = myTable;
	table.myUnmapMemory(Unwrap(myDevice), Unwrap(aDeviceMemory));
}

void VulkanCommandBufferWrapper::BeginCommandBuffer(const VkCommandBufferBeginInfo& aCommandBufferBeginInfo) const
{
	VULKAN_CHECK_VALID_RESULT(myTable.myBeginCommandBuffer(Unwrap(myCommandBuffer), &aCommandBufferBeginInfo));
}

void VulkanCommandBufferWrapper::EndCommandBuffer() const
{
	VULKAN_CHECK_VALID_RESULT(myTable.myEndCommandBuffer(Unwrap(myCommandBuffer)));
}

void VulkanCommandBufferWrapper::BeginRenderPass(const VkRenderPassBeginInfo& aRenderPassBeginInfo) const
{
	// TODO: Check what third parameter is for
	myTable.myCmdBeginRenderPass(Unwrap(myCommandBuffer), &aRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanCommandBufferWrapper::EndRenderPass() const
{
	myTable.myCmdEndRenderPass(Unwrap(myCommandBuffer));
}

void VulkanCommandBufferWrapper::BindPipeline(Pipeline& aPipeline, bool isGraphicsPipeline) const
{
	VkPipelineBindPoint pipelineBindPoint = isGraphicsPipeline ? VK_PIPELINE_BIND_POINT_GRAPHICS : VK_PIPELINE_BIND_POINT_COMPUTE;
	myTable.myCmdBindPipeline(Unwrap(myCommandBuffer), pipelineBindPoint, Unwrap(aPipeline));
}

void VulkanCommandBufferWrapper::SetViewport(VkViewport* someViewports, uint32_t aViewportCount, uint32_t aFirstViewport) const
{
	myTable.myCmdSetViewport(Unwrap(myCommandBuffer), aFirstViewport, aViewportCount, someViewports);
}

void VulkanCommandBufferWrapper::SetScissor(VkRect2D* someRects, uint32_t aRectCount, uint32_t aFirstRect) const
{
	myTable.myCmdSetScissor(Unwrap(myCommandBuffer), aFirstRect, aRectCount, someRects);
}

void VulkanCommandBufferWrapper::BindVertexBuffers(Buffer* someBuffers, const VkDeviceSize* someOffsets, uint32_t aBufferCount) const
{
	myTable.myCmdBindVertexBuffers(Unwrap(myCommandBuffer), 0, aBufferCount, Unwrap(someBuffers), someOffsets);
}

void VulkanCommandBufferWrapper::BindDescriptorSets(PipelineLayout& aPipelineLayout, DescriptorSet* someDescriptorSets, uint32_t aDescriptorSetCount) const
{
	myTable.myCmdBindDescriptorSets(Unwrap(myCommandBuffer), VK_PIPELINE_BIND_POINT_GRAPHICS, Unwrap(aPipelineLayout), 0, aDescriptorSetCount, Unwrap(someDescriptorSets), 0, nullptr);
}

void VulkanCommandBufferWrapper::Draw(uint32_t aVertexCount, uint32_t aFirstVertex, uint32_t anInstanceCount, uint32_t aFirstInstance) const
{
	myTable.myCmdDraw(Unwrap(myCommandBuffer), aVertexCount, anInstanceCount, aFirstVertex, aFirstInstance);
}

#if IS_DEBUG_BUILD

#include <stdio.h>
#include <stdlib.h>

namespace
{
	static const char* toString(VkObjectType anObjectType)
	{
		switch (anObjectType)
		{
		case VK_OBJECT_TYPE_QUERY_POOL:
			return "VK_OBJECT_TYPE_QUERY_POOL";
		case VK_OBJECT_TYPE_OBJECT_TABLE_NVX:
			return "VK_OBJECT_TYPE_OBJECT_TABLE_NVX";
		case VK_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION:
			return "VK_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION";
		case VK_OBJECT_TYPE_SEMAPHORE:
			return "VK_OBJECT_TYPE_SEMAPHORE";
		case VK_OBJECT_TYPE_SHADER_MODULE:
			return "VK_OBJECT_TYPE_SHADER_MODULE";
		case VK_OBJECT_TYPE_SWAPCHAIN_KHR:
			return "VK_OBJECT_TYPE_SWAPCHAIN_KHR";
		case VK_OBJECT_TYPE_SAMPLER:
			return "VK_OBJECT_TYPE_SAMPLER";
		case VK_OBJECT_TYPE_INDIRECT_COMMANDS_LAYOUT_NVX:
			return "VK_OBJECT_TYPE_INDIRECT_COMMANDS_LAYOUT_NVX";
		case VK_OBJECT_TYPE_DEBUG_REPORT_CALLBACK_EXT:
			return "VK_OBJECT_TYPE_DEBUG_REPORT_CALLBACK_EXT";
		case VK_OBJECT_TYPE_IMAGE:
			return "VK_OBJECT_TYPE_IMAGE";
		case VK_OBJECT_TYPE_UNKNOWN:
			return "VK_OBJECT_TYPE_UNKNOWN";
		case VK_OBJECT_TYPE_DESCRIPTOR_POOL:
			return "VK_OBJECT_TYPE_DESCRIPTOR_POOL";
		case VK_OBJECT_TYPE_COMMAND_BUFFER:
			return "VK_OBJECT_TYPE_COMMAND_BUFFER";
		case VK_OBJECT_TYPE_BUFFER:
			return "VK_OBJECT_TYPE_BUFFER";
		case VK_OBJECT_TYPE_SURFACE_KHR:
			return "VK_OBJECT_TYPE_SURFACE_KHR";
		case VK_OBJECT_TYPE_INSTANCE:
			return "VK_OBJECT_TYPE_INSTANCE";
		case VK_OBJECT_TYPE_VALIDATION_CACHE_EXT:
			return "VK_OBJECT_TYPE_VALIDATION_CACHE_EXT";
		case VK_OBJECT_TYPE_IMAGE_VIEW:
			return "VK_OBJECT_TYPE_IMAGE_VIEW";
		case VK_OBJECT_TYPE_DESCRIPTOR_SET:
			return "VK_OBJECT_TYPE_DESCRIPTOR_SET";
		case VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT:
			return "VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT";
		case VK_OBJECT_TYPE_COMMAND_POOL:
			return "VK_OBJECT_TYPE_COMMAND_POOL";
		case VK_OBJECT_TYPE_PHYSICAL_DEVICE:
			return "VK_OBJECT_TYPE_PHYSICAL_DEVICE";
		case VK_OBJECT_TYPE_DISPLAY_KHR:
			return "VK_OBJECT_TYPE_DISPLAY_KHR";
		case VK_OBJECT_TYPE_BUFFER_VIEW:
			return "VK_OBJECT_TYPE_BUFFER_VIEW";
		case VK_OBJECT_TYPE_DEBUG_UTILS_MESSENGER_EXT:
			return "VK_OBJECT_TYPE_DEBUG_UTILS_MESSENGER_EXT";
		case VK_OBJECT_TYPE_FRAMEBUFFER:
			return "VK_OBJECT_TYPE_FRAMEBUFFER";
		case VK_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE:
			return "VK_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE";
		case VK_OBJECT_TYPE_PIPELINE_CACHE:
			return "VK_OBJECT_TYPE_PIPELINE_CACHE";
		case VK_OBJECT_TYPE_PIPELINE_LAYOUT:
			return "VK_OBJECT_TYPE_PIPELINE_LAYOUT";
		case VK_OBJECT_TYPE_DEVICE_MEMORY:
			return "VK_OBJECT_TYPE_DEVICE_MEMORY";
		case VK_OBJECT_TYPE_FENCE:
			return "VK_OBJECT_TYPE_FENCE";
		case VK_OBJECT_TYPE_QUEUE:
			return "VK_OBJECT_TYPE_QUEUE";
		case VK_OBJECT_TYPE_DEVICE:
			return "VK_OBJECT_TYPE_DEVICE";
		case VK_OBJECT_TYPE_RENDER_PASS:
			return "VK_OBJECT_TYPE_RENDER_PASS";
		case VK_OBJECT_TYPE_DISPLAY_MODE_KHR:
			return "VK_OBJECT_TYPE_DISPLAY_MODE_KHR";
		case VK_OBJECT_TYPE_EVENT:
			return "VK_OBJECT_TYPE_EVENT";
		case VK_OBJECT_TYPE_PIPELINE:
			return "VK_OBJECT_TYPE_PIPELINE";
		default:
			return "Unhandled VkObjectType";
		}
	}

	static VkBool32 ValidationLayerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT aMessageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT aMessageType,
		const VkDebugUtilsMessengerCallbackDataEXT *aCallbackData,
		void *pUserData)
	{
		char prefix[64] = "";
		const size_t messageLength = 7000;
		char message[messageLength] = { 0 };

		if (aMessageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
			strcat_s(prefix, "VERBOSE : ");
		}
		else if (aMessageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
			strcat_s(prefix, "INFO : ");
		}
		else if (aMessageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
			strcat_s(prefix, "WARNING : ");
		}
		else if (aMessageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
			strcat_s(prefix, "ERROR : ");
		}

		if (aMessageType & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) {
			strcat_s(prefix, "GENERAL");
		}
		else {
			if (aMessageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
				strcat_s(prefix, "VALIDATION");
			}
			if (aMessageType & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) {
				if (aMessageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
					strcat_s(prefix, "|");
				}
				strcat_s(prefix, "PERFORMANCE");
			}
		}

		sprintf_s(message, messageLength, "%s - Message Id Number: %d | Message Id Name: %s\n\t%s\n", prefix, aCallbackData->messageIdNumber,
			aCallbackData->pMessageIdName, aCallbackData->pMessage);
		if (aCallbackData->objectCount > 0)
		{
			char objectMessage[500];
			sprintf_s(objectMessage, "\n\tObjects - %d\n", aCallbackData->objectCount);
			strcat_s(message, messageLength, objectMessage);
			for (uint32_t object = 0; object < aCallbackData->objectCount; ++object)
			{
				if (NULL != aCallbackData->pObjects[object].pObjectName && strlen(aCallbackData->pObjects[object].pObjectName) > 0)
				{
					sprintf_s(objectMessage, "\t\tObject[%d] - %s, Handle %p, Name \"%s\"\n", object,
						toString(aCallbackData->pObjects[object].objectType),
						(void *)(aCallbackData->pObjects[object].objectHandle), aCallbackData->pObjects[object].pObjectName);
				}
				else
				{
					sprintf_s(objectMessage, "\t\tObject[%d] - %s, Handle %p\n", object,
						toString(aCallbackData->pObjects[object].objectType),
						(void *)(aCallbackData->pObjects[object].objectHandle));
				}
				strcat_s(message, messageLength, objectMessage);
			}
		}
		if (aCallbackData->cmdBufLabelCount > 0)
		{
			char labelMessage[500];
			sprintf_s(labelMessage, "\n\tCommand Buffer Labels - %d\n", aCallbackData->cmdBufLabelCount);
			strcat_s(message, messageLength, labelMessage);
			for (uint32_t cmdBufferLabel = 0; cmdBufferLabel < aCallbackData->cmdBufLabelCount; ++cmdBufferLabel)
			{
				sprintf_s(labelMessage, "\t\tLabel[%d] - %s { %f, %f, %f, %f}\n", cmdBufferLabel,
					aCallbackData->pCmdBufLabels[cmdBufferLabel].pLabelName, aCallbackData->pCmdBufLabels[cmdBufferLabel].color[0],
					aCallbackData->pCmdBufLabels[cmdBufferLabel].color[1], aCallbackData->pCmdBufLabels[cmdBufferLabel].color[2],
					aCallbackData->pCmdBufLabels[cmdBufferLabel].color[3]);
				strcat_s(message, messageLength, labelMessage);
			}
		}

		DebugPrint(message);

		// Don't bail out, but keep going.
		return false;
	}
}

#endif
