#include "Renderer.h"

#include "VulkanModule.h"
#include "VulkanModuleDebug.h"
#include "VulkanWrapper.h"

#include "Platform/PlatformSpecific.h"
#include "Utils/Debug.h"

#if IS_WINDOWS_PLATFORM

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "vulkan/vulkan_win32.h"

#endif // IS_WINDOWS_PLATFORM

#include <vector> // vector
#include <algorithm> // clamp

Renderer Renderer::Create(void* aWindowHandle, uint32_t aBackFramebufferCount)
{
	// Load vulkan library
	if (VulkanModule::Load() == false)
		Debug::Breakpoint();

	InitializeVulkanCommonTable();

	Renderer renderer;
	renderer.CreateInstance();
	renderer.CreateSurface(aWindowHandle);
	renderer.CreateDevice();
	renderer.CreateSwapchain(1280u, 720u, aBackFramebufferCount);
	return renderer;
}

void Renderer::Destroy(Renderer& aRenderer)
{
	aRenderer.DestroySwapchain();
	aRenderer.DestroyDevice();
	aRenderer.DestroySurface();
	aRenderer.DestroyInstance();

	// Unload vulkan library
	VulkanModule::Unload();
}

void Renderer::Create(const VkCommandPoolCreateInfo& aCommandPoolCreateInfo, CommandPool& aCommandPoolOut)
{
	const VulkanDeviceDispatchTable& table = myDeviceTable;
	VULKAN_CHECK_VALID_RESULT(table.myCreateCommandPool(myDevice, &aCommandPoolCreateInfo, nullptr, Unwrap(&aCommandPoolOut)));
}

void Renderer::Destroy(CommandPool& aCommandPool)
{
	const VulkanDeviceDispatchTable& table = myDeviceTable;
	VkCommandPool& commandPool = Unwrap(aCommandPool);
	table.myDestroyCommandPool(myDevice, commandPool, nullptr);

#if IS_DEBUG_BUILD
	commandPool = VK_NULL_HANDLE;
#endif // IS_DEBUG_BUILD
}

void Renderer::Create(const VkCommandBufferAllocateInfo& aCommandBufferAllocateInfo, CommandBuffer* someCommandBuffersOut)
{
	const VulkanDeviceDispatchTable& table = myDeviceTable;
	VULKAN_CHECK_VALID_RESULT(table.myAllocateCommandBuffers(myDevice, &aCommandBufferAllocateInfo, Unwrap(someCommandBuffersOut)));
}

void Renderer::Destroy(CommandPool& aCommandPool, CommandBuffer* someCommandBuffers, uint32_t aCommandBufferCount)
{
	const VulkanDeviceDispatchTable& table = myDeviceTable;
	VkCommandBuffer* commandBuffers = Unwrap(someCommandBuffers);
	table.myFreeCommandBuffers(myDevice, Unwrap(aCommandPool), aCommandBufferCount, commandBuffers);

#if IS_DEBUG_BUILD
	std::memset(commandBuffers, 0, sizeof(VkCommandBuffer) * aCommandBufferCount);
#endif // IS_DEBUG_BUILD
}

void Renderer::Create(const VkRenderPassCreateInfo& aRenderPassCreateInfo, RenderPass& aRenderPass)
{
	const VulkanDeviceDispatchTable& table = myDeviceTable;
	VULKAN_CHECK_VALID_RESULT(table.myCreateRenderPass(myDevice, &aRenderPassCreateInfo, nullptr, Unwrap(&aRenderPass)));
}

void Renderer::Destroy(RenderPass& aRenderPass)
{
	const VulkanDeviceDispatchTable& table = myDeviceTable;
	VkRenderPass& renderPass = Unwrap(aRenderPass);
	table.myDestroyRenderPass(myDevice, renderPass, nullptr);

#if IS_DEBUG_BUILD
	renderPass = VK_NULL_HANDLE;
#endif // IS_DEBUG_BUILD
}

void Renderer::Create(const VkImageViewCreateInfo& aImageViewCreateInfo, ImageView& aImageViewOut)
{
	const VulkanDeviceDispatchTable& table = myDeviceTable;
	VULKAN_CHECK_VALID_RESULT(table.myCreateImageView(myDevice, &aImageViewCreateInfo, nullptr, Unwrap(&aImageViewOut)));
}

void Renderer::Destroy(ImageView& aImageView)
{
	const VulkanDeviceDispatchTable& table = myDeviceTable;
	VkImageView& imageView = Unwrap(aImageView);
	table.myDestroyImageView(myDevice, imageView, nullptr);

#if IS_DEBUG_BUILD
	imageView = VK_NULL_HANDLE;
#endif // IS_DEBUG_BUILD
}

void Renderer::Create(const VkFramebufferCreateInfo& aFramebufferCreateInfo, Framebuffer& aFramebufferOut)
{
	const VulkanDeviceDispatchTable& table = myDeviceTable;
	VULKAN_CHECK_VALID_RESULT(table.myCreateFramebuffer(myDevice, &aFramebufferCreateInfo, nullptr, Unwrap(&aFramebufferOut)));
}

void Renderer::Destroy(Framebuffer& aFramebuffer)
{
	const VulkanDeviceDispatchTable& table = myDeviceTable;
	VkFramebuffer& framebuffer = Unwrap(aFramebuffer);
	table.myDestroyFramebuffer(myDevice, framebuffer, nullptr);

#if IS_DEBUG_BUILD
	framebuffer = VK_NULL_HANDLE;
#endif // IS_DEBUG_BUILD
}

void Renderer::Create(const VkPipelineLayoutCreateInfo& aPipelineLayoutCreateInfo, PipelineLayout& aPipelineLayoutOut)
{
	const VulkanDeviceDispatchTable& table = myDeviceTable;
	VULKAN_CHECK_VALID_RESULT(table.myCreatePipelineLayout(myDevice, &aPipelineLayoutCreateInfo, nullptr, Unwrap(&aPipelineLayoutOut)));
}

void Renderer::Destroy(PipelineLayout& aPipelineLayout)
{
	const VulkanDeviceDispatchTable& table = myDeviceTable;
	VkPipelineLayout& pipelineLayout = Unwrap(aPipelineLayout);
	table.myDestroyPipelineLayout(myDevice, pipelineLayout, nullptr);

#if IS_DEBUG_BUILD
	pipelineLayout = VK_NULL_HANDLE;
#endif // IS_DEBUG_BUILD
}

void Renderer::Create(const VkGraphicsPipelineCreateInfo* someGraphicsPipelineCreateInfos, Pipeline* somePipelinesOut, uint32_t aPipelineCount)
{
	const VulkanDeviceDispatchTable& table = myDeviceTable;
	VULKAN_CHECK_VALID_RESULT(table.myCreateGraphicsPipelines(myDevice, VK_NULL_HANDLE, aPipelineCount, someGraphicsPipelineCreateInfos, nullptr, Unwrap(somePipelinesOut)));
}

void Renderer::Destroy(Pipeline& aPipeline)
{
	const VulkanDeviceDispatchTable& table = myDeviceTable;
	VkPipeline& pipeline = Unwrap(aPipeline);
	table.myDestroyPipeline(myDevice, pipeline, nullptr);

#if IS_DEBUG_BUILD
	pipeline = VK_NULL_HANDLE;
#endif // IS_DEBUG_BUILD
}

void Renderer::Create(const VkSemaphoreCreateInfo& aSemaphoreCreateInfo, Semaphore& aSemaphoreOut)
{
	const VulkanDeviceDispatchTable& table = myDeviceTable;
	VULKAN_CHECK_VALID_RESULT(table.myCreateSemaphore(myDevice, &aSemaphoreCreateInfo, nullptr, Unwrap(&aSemaphoreOut)));
}

void Renderer::Destroy(Semaphore& aSemaphore)
{
	const VulkanDeviceDispatchTable& table = myDeviceTable;
	VkSemaphore& semaphore = Unwrap(aSemaphore);
	table.myDestroySemaphore(myDevice, semaphore, nullptr);

#if IS_DEBUG_BUILD
	semaphore = VK_NULL_HANDLE;
#endif // IS_DEBUG_BUILD
}

void Renderer::Create(const VkFenceCreateInfo& aFenceCreateInfo, Fence& aFenceOut)
{
	const VulkanDeviceDispatchTable& table = myDeviceTable;
	VULKAN_CHECK_VALID_RESULT(table.myCreateFence(myDevice, &aFenceCreateInfo, nullptr, Unwrap(&aFenceOut)));
}

void Renderer::Destroy(Fence& aFence)
{
	const VulkanDeviceDispatchTable& table = myDeviceTable;
	VkFence& fence = Unwrap(aFence);
	table.myDestroyFence(myDevice, fence, nullptr);

#if IS_DEBUG_BUILD
	fence = VK_NULL_HANDLE;
#endif // IS_DEBUG_BUILD
}

void Renderer::Create(const VkShaderModuleCreateInfo& aShaderModuleCreateInfo, ShaderModule& aShaderModuleOut)
{
	const VulkanDeviceDispatchTable& table = myDeviceTable;
	VULKAN_CHECK_VALID_RESULT(table.myCreateShaderModule(myDevice, &aShaderModuleCreateInfo, nullptr, Unwrap(&aShaderModuleOut)));
}

void Renderer::Destroy(ShaderModule& aShaderModule)
{
	const VulkanDeviceDispatchTable& table = myDeviceTable;
	VkShaderModule& shaderModule = Unwrap(aShaderModule);
	table.myDestroyShaderModule(myDevice, shaderModule, nullptr);

#if IS_DEBUG_BUILD
	shaderModule = VK_NULL_HANDLE;
#endif // IS_DEBUG_BUILD
}

void Renderer::Create(const VkBufferCreateInfo& aBufferCreateInfo, Buffer& aBufferOut)
{
	const VulkanDeviceDispatchTable& table = myDeviceTable;
	VULKAN_CHECK_VALID_RESULT(table.myCreateBuffer(myDevice, &aBufferCreateInfo, nullptr, Unwrap(&aBufferOut)));
}

void Renderer::Destroy(Buffer& aBuffer)
{
	const VulkanDeviceDispatchTable& table = myDeviceTable;
	VkBuffer& buffer = Unwrap(aBuffer);
	table.myDestroyBuffer(myDevice, buffer, nullptr);

#if IS_DEBUG_BUILD
	buffer = VK_NULL_HANDLE;
#endif // IS_DEBUG_BUILD
}

void Renderer::Create(const VkDescriptorSetLayoutCreateInfo& aDescriptorSetLayoutCreateInfo, DescriptorSetLayout& aDescriptorSetLayoutOut)
{
	const VulkanDeviceDispatchTable& table = myDeviceTable;
	VULKAN_CHECK_VALID_RESULT(table.myCreateDescriptorSetLayout(myDevice, &aDescriptorSetLayoutCreateInfo, nullptr, Unwrap(&aDescriptorSetLayoutOut)));
}

void Renderer::Destroy(DescriptorSetLayout& aDescriptorSetLayout)
{
	const VulkanDeviceDispatchTable& table = myDeviceTable;
	VkDescriptorSetLayout& descriptorSetLayout = Unwrap(aDescriptorSetLayout);
	table.myDestroyDescriptorSetLayout(myDevice, descriptorSetLayout, nullptr);

#if IS_DEBUG_BUILD
	descriptorSetLayout = VK_NULL_HANDLE;
#endif
}

void Renderer::Create(const VkDescriptorPoolCreateInfo& aDescriptorPoolCreateInfo, DescriptorPool& aDescriptorPoolOut)
{
	const VulkanDeviceDispatchTable& table = myDeviceTable;
	VULKAN_CHECK_VALID_RESULT(table.myCreateDescriptorPool(myDevice, &aDescriptorPoolCreateInfo, nullptr, Unwrap(&aDescriptorPoolOut)));
}

void Renderer::Destroy(DescriptorPool& aDescriptorPool)
{
	const VulkanDeviceDispatchTable& table = myDeviceTable;
	VkDescriptorPool& descriptorPool = Unwrap(aDescriptorPool);
	table.myDestroyDescriptorPool(myDevice, descriptorPool, nullptr);

#if IS_DEBUG_BUILD
	descriptorPool = VK_NULL_HANDLE;
#endif
}

void Renderer::Create(const VkDescriptorSetAllocateInfo& aDescriptorSetAllocateInfo, DescriptorSet* someDescriptorSets)
{
	const VulkanDeviceDispatchTable& table = myDeviceTable;
	VULKAN_CHECK_VALID_RESULT(table.myAllocateDescriptorSets(myDevice, &aDescriptorSetAllocateInfo, Unwrap(someDescriptorSets)));
}

void Renderer::Destroy(DescriptorPool& aDescriptorPool, DescriptorSet* someDescriptorSet, uint32_t aDescriptorSetCount)
{
	const VulkanDeviceDispatchTable& table = myDeviceTable;
	VkDescriptorSet* descriptorSet = Unwrap(someDescriptorSet);
	table.myFreeDescriptorSets(myDevice, Unwrap(aDescriptorPool), aDescriptorSetCount, descriptorSet);
#if IS_DEBUG_BUILD
	std::memset(descriptorSet, 0, sizeof(VkDescriptorSet) * aDescriptorSetCount);
#endif // IS_DEBUG_BUILD
}

void Renderer::UpdateDescriptorSets(const VkWriteDescriptorSet* someWriteDescriptorSets, uint32_t aWriteDescriptorCount)
{
	const VulkanDeviceDispatchTable& table = myDeviceTable;
	table.myUpdateDescriptorSets(myDevice, aWriteDescriptorCount, someWriteDescriptorSets, 0, nullptr);
}

void Renderer::GetMemoryRequirements(const Buffer& aBuffer, VkMemoryRequirements& aMemoryRequirementsOut)
{
	const VulkanDeviceDispatchTable& deviceTable = myDeviceTable;
	deviceTable.myGetBufferMemoryRequirements(myDevice, Unwrap(aBuffer), &aMemoryRequirementsOut);
}

void Renderer::AllocateDeviceMemory(VkDeviceSize aSize, uint32_t aMemoryTypeBits, VkMemoryPropertyFlags aMemoryProperties, DeviceMemory& aDeviceMemoryOut)
{
	// Get physical device memory properties
	const VulkanInstanceDispatchTable& instanceTable = myInstanceTable;
	const VulkanDeviceDispatchTable& deviceTable = myDeviceTable;
	VkPhysicalDeviceMemoryProperties memoryProperties;
	instanceTable.myGetPhysicalDeviceMemoryProperties(myPhysicalDevice, &memoryProperties);

	// Search for memory
	uint32_t memoryIndex = UINT32_MAX;
	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i)
	{
		uint32_t memoryTypeBits = (1 << i);
	
		bool validMemoryType = memoryTypeBits & aMemoryTypeBits;
		bool hasWantedMemoryProperties = memoryProperties.memoryTypes[i].propertyFlags & aMemoryProperties;
		if (validMemoryType && hasWantedMemoryProperties)
		{
			memoryIndex = i;
			break;
		}
	}
	
	if (memoryIndex == UINT32_MAX)
		Debug::Breakpoint();
	
	// Allocate memory
	VkMemoryAllocateInfo memoryAllocateInfo
	{
		VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		nullptr,
		aSize,
		memoryIndex
	};
	
	deviceTable.myAllocateMemory(myDevice, &memoryAllocateInfo, nullptr, Unwrap(&aDeviceMemoryOut));
}

void Renderer::FreeDeviceMemory(DeviceMemory& aDeviceMemory)
{
	const VulkanDeviceDispatchTable& table = myDeviceTable;
	VkDeviceMemory& deviceMemory = Unwrap(aDeviceMemory);
	table.myFreeMemory(myDevice, deviceMemory, nullptr);

#if IS_DEBUG_BUILD
	deviceMemory = VK_NULL_HANDLE;
#endif // IS_DEBUG_BUILD
}

void Renderer::BindDeviceMemory(DeviceMemory& aDeviceMemory, VkDeviceSize anOffset, Buffer& aBuffer)
{
	const VulkanDeviceDispatchTable& deviceTable = myDeviceTable;
	deviceTable.myBindBufferMemory(myDevice, Unwrap(aBuffer), Unwrap(aDeviceMemory), anOffset);
}

void* Renderer::MapDeviceMemory(DeviceMemory& aDeviceMemory, VkDeviceSize anOffset, VkDeviceSize aSize)
{
	const VulkanDeviceDispatchTable& table = myDeviceTable;

	void* mappedMemory = nullptr;
	VULKAN_CHECK_VALID_RESULT(table.myMapMemory(myDevice, Unwrap(aDeviceMemory), anOffset, aSize, 0, &mappedMemory));
	return mappedMemory;
}

void Renderer::UnmapDeviceMemory(DeviceMemory& aDeviceMemory)
{
	const VulkanDeviceDispatchTable& table = myDeviceTable;
	table.myUnmapMemory(myDevice, Unwrap(aDeviceMemory));
}

void Renderer::ResizeSwapchain(int aWidth, int aHeight)
{
	CreateSwapchain(aWidth, aHeight, mySwapchainImageCount);
}

uint32_t Renderer::AcquireNextImage(Semaphore& aSemaphore)
{
	uint32_t imageIndex;
	VULKAN_CHECK_VALID_RESULT(myDeviceTable.myAcquireNextImageKHR(myDevice, mySwapchain, UINT64_MAX, Unwrap(aSemaphore), VK_NULL_HANDLE, &imageIndex));
	return imageIndex;
}

uint32_t Renderer::AcquireNextImage(Fence& aFence)
{
	uint32_t imageIndex;
	VULKAN_CHECK_VALID_RESULT(myDeviceTable.myAcquireNextImageKHR(myDevice, mySwapchain, UINT64_MAX, VK_NULL_HANDLE, Unwrap(aFence), &imageIndex));
	return imageIndex;
}

uint32_t Renderer::AcquireNextImage(Semaphore& aSemaphore, Fence& aFence)
{
	uint32_t imageIndex;
	VULKAN_CHECK_VALID_RESULT(myDeviceTable.myAcquireNextImageKHR(myDevice, mySwapchain, UINT64_MAX, Unwrap(aSemaphore), Unwrap(aFence), &imageIndex));
	return imageIndex;
}

void Renderer::BeginCommandBuffer(CommandBuffer& aCommandBuffer, const VkCommandBufferBeginInfo& aCommandBufferBeginInfo)
{
	VULKAN_CHECK_VALID_RESULT(myDeviceTable.myBeginCommandBuffer(Unwrap(aCommandBuffer), &aCommandBufferBeginInfo));
}

void Renderer::EndCommandBuffer(CommandBuffer& aCommandBuffer)
{
	VULKAN_CHECK_VALID_RESULT(myDeviceTable.myEndCommandBuffer(Unwrap(aCommandBuffer)));
}

void Renderer::BeginRenderPass(CommandBuffer& aCommandBuffer, const VkRenderPassBeginInfo& aRenderPassBeginInfo)
{
	// TODO: Check what third parameter is for
	myDeviceTable.myCmdBeginRenderPass(Unwrap(aCommandBuffer), &aRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void Renderer::EndRenderPass(CommandBuffer& aCommandBuffer)
{
	myDeviceTable.myCmdEndRenderPass(Unwrap(aCommandBuffer));
}

void Renderer::BindPipeline(CommandBuffer& aCommandBuffer, Pipeline& aPipeline, bool isGraphicsPipeline)
{
	VkPipelineBindPoint pipelineBindPoint = isGraphicsPipeline ? VK_PIPELINE_BIND_POINT_GRAPHICS : VK_PIPELINE_BIND_POINT_COMPUTE;
	myDeviceTable.myCmdBindPipeline(Unwrap(aCommandBuffer), pipelineBindPoint, Unwrap(aPipeline));
}

void Renderer::SetViewport(CommandBuffer& aCommandBuffer, VkViewport* someViewports, uint32_t aViewportCount, uint32_t aFirstViewport)
{
	myDeviceTable.myCmdSetViewport(Unwrap(aCommandBuffer), aFirstViewport, aViewportCount, someViewports);
}

void Renderer::SetScissor(CommandBuffer& aCommandBuffer, VkRect2D* someRects, uint32_t aRectCount, uint32_t aFirstRect)
{
	myDeviceTable.myCmdSetScissor(Unwrap(aCommandBuffer), aFirstRect, aRectCount, someRects);
}

void Renderer::BindVertexBuffers(CommandBuffer& aCommandBuffer, Buffer* someBuffers, const VkDeviceSize* someOffsets, uint32_t aBufferCount)
{
	myDeviceTable.myCmdBindVertexBuffers(Unwrap(aCommandBuffer), 0, aBufferCount, Unwrap(someBuffers), someOffsets);
}

void Renderer::BindDescriptorSets(CommandBuffer& aCommandBuffer, PipelineLayout& aPipelineLayout, DescriptorSet* someDescriptorSets, uint32_t aDescriptorSetCount)
{
	myDeviceTable.myCmdBindDescriptorSets(Unwrap(aCommandBuffer), VK_PIPELINE_BIND_POINT_GRAPHICS, Unwrap(aPipelineLayout), 0, aDescriptorSetCount, Unwrap(someDescriptorSets), 0, nullptr);
}

void Renderer::Draw(CommandBuffer& aCommandBuffer, uint32_t aVertexCount, uint32_t aFirstVertex, uint32_t anInstanceCount, uint32_t aFirstInstance)
{
	myDeviceTable.myCmdDraw(Unwrap(aCommandBuffer), aVertexCount, anInstanceCount, aFirstVertex, aFirstInstance);
}

void Renderer::WaitForFences(Fence* someFences, uint32_t aFenceCount)
{
	VULKAN_CHECK_VALID_RESULT(myDeviceTable.myWaitForFences(myDevice, aFenceCount, Unwrap(someFences), VK_TRUE, UINT64_MAX));
}

void Renderer::ResetFences(Fence* someFences, uint32_t aFenceCount)
{
	VULKAN_CHECK_VALID_RESULT(myDeviceTable.myResetFences(myDevice, aFenceCount, Unwrap(someFences)));
}

void Renderer::Submit(const VkSubmitInfo* someSubmitInfos, uint32_t aSubmitCount)
{
	VULKAN_CHECK_VALID_RESULT(myDeviceTable.myQueueSubmit(myQueue, aSubmitCount, someSubmitInfos, VK_NULL_HANDLE));
}

void Renderer::Submit(const VkSubmitInfo* someSubmitInfos, uint32_t aSubmitCount, Fence& aFence)
{
	VULKAN_CHECK_VALID_RESULT(myDeviceTable.myQueueSubmit(myQueue, aSubmitCount, someSubmitInfos, Unwrap(aFence)));
}

void Renderer::Present(Semaphore* someWaitSemaphores, uint32_t aWaitSemaphoreCount, uint32_t anImageIndex)
{
	VkPresentInfoKHR presentInfoKHR
	{
		VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		nullptr,
		aWaitSemaphoreCount,
		Unwrap(someWaitSemaphores),
		1,
		&mySwapchain,
		&anImageIndex,
		nullptr
	};

	VkResult result = myDeviceTable.myQueuePresentKHR(myQueue, &presentInfoKHR);
	if (result != VK_SUCCESS)
	{
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
			Debug::PrintToOutput("ERROR: OUT_OF_DATE_KHR\n");
		else if (result == VK_ERROR_SURFACE_LOST_KHR)
			Debug::PrintToOutput("ERROR: SURFACE_LOST_KHR\n");
		else
			Debug::Breakpoint();
	}
}

void Renderer::WaitForDevice() const
{
	VULKAN_CHECK_VALID_RESULT(myDeviceTable.myDeviceWaitIdle(myDevice));
}

void Renderer::GetSwapchainImages(Image* someImagesOut)
{
	const VulkanDeviceDispatchTable& table = myDeviceTable;
	VULKAN_CHECK_VALID_RESULT(table.myGetSwapchainImagesKHR(myDevice, mySwapchain, &mySwapchainImageCount, Unwrap(someImagesOut)));
}

namespace
{
	static VkBool32 ValidationLayerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT aMessageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT aMessageType,
		const VkDebugUtilsMessengerCallbackDataEXT *aCallbackData,
		void *pUserData);
}

void Renderer::InitializeVulkanCommonTable()
{
	VulkanCommonDispatchTable& table = VulkanCommonDispatchTable::ourTable;
	if (table.myGetInstanceProcAddr == nullptr)
		table.myGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(VulkanModule::GetProcedureAddress("vkGetInstanceProcAddr"));

	// Something went wrong, we cannot continue since that's the function used to retrieve others
	if (table.myGetInstanceProcAddr == nullptr)
		Debug::Breakpoint();

	// Initialize functions needed for instance creation (available layers and extensions)
	table.myEnumerateInstanceVersion = reinterpret_cast<PFN_vkEnumerateInstanceVersion>(VulkanCommonDispatchTable::ourTable.myGetInstanceProcAddr(nullptr, "vkEnumerateInstanceVersion"));
	table.myEnumerateInstanceExtensionProperties = reinterpret_cast<PFN_vkEnumerateInstanceExtensionProperties>(VulkanCommonDispatchTable::ourTable.myGetInstanceProcAddr(nullptr, "vkEnumerateInstanceExtensionProperties"));
	table.myEnumerateInstanceLayerProperties = reinterpret_cast<PFN_vkEnumerateInstanceLayerProperties>(VulkanCommonDispatchTable::ourTable.myGetInstanceProcAddr(nullptr, "vkEnumerateInstanceLayerProperties"));
	table.myCreateInstance = reinterpret_cast<PFN_vkCreateInstance>(VulkanCommonDispatchTable::ourTable.myGetInstanceProcAddr(nullptr, "vkCreateInstance"));
}

void Renderer::CreateInstance()
{
	const VulkanCommonDispatchTable& table = VulkanCommonDispatchTable::ourTable;

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
	}

	if (foundExtensionCount != neededExtensionCount)
		Debug::Breakpoint();

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
	}

	if (foundLayerCount != neededLayerCount)
		Debug::Breakpoint();

	uint32_t apiVersion;
	table.myEnumerateInstanceVersion(&apiVersion);

	// Initialize Vulkan instance for this renderer
	VkApplicationInfo applicationInfo
	{
		VK_STRUCTURE_TYPE_APPLICATION_INFO,
		nullptr,
		"VulkanBase",
		0,
		"CepsyEngine",
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

	VkResult result = table.myCreateInstance(&instanceInfo, nullptr, &myInstance);
	if (result != VK_SUCCESS)
		Debug::Breakpoint();

	myInstanceTable.Initialize(myInstance);
}

void Renderer::DestroyInstance()
{
	VulkanInstanceDispatchTable& table = myInstanceTable;
	table.myDestroyInstance(myInstance, nullptr);

#if IS_DEBUG_BUILD
	myInstance = VK_NULL_HANDLE;
	table.Clear();
#endif // IS_DEBUG_BUILD
}

void Renderer::CreateSurface(void* aWindowHandle)
{
#if IS_WINDOWS_PLATFORM
	// Create presentation surface
	VkWin32SurfaceCreateInfoKHR surfaceInfo{
		VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
		nullptr,
		0,
		Process::GetHandle(),
		reinterpret_cast<HWND>(aWindowHandle)
	};
	PFN_vkCreateWin32SurfaceKHR createSurface = reinterpret_cast<PFN_vkCreateWin32SurfaceKHR>(myInstanceTable.myGetInstanceProcAddr(myInstance, "vkCreateWin32SurfaceKHR"));
	VkResult result = createSurface(myInstance, &surfaceInfo, nullptr, &mySurface);
	if (result != VK_SUCCESS)
		Debug::Breakpoint();
#endif // IS_WINDOWS_PLATFORM
}

void Renderer::DestroySurface()
{
	const VulkanInstanceDispatchTable& table = myInstanceTable;
	table.myDestroySurfaceKHR(myInstance, mySurface, nullptr);

#if IS_DEBUG_BUILD
	mySurface = VK_NULL_HANDLE;
#endif // IS_DEBUG_BUILD
}

void Renderer::CreateDevice()
{
	// For now we will only create a graphics queue
	const VulkanInstanceDispatchTable& table = myInstanceTable;

	uint32_t physicalDeviceCount = 0u;
	VkResult result = table.myEnumeratePhysicalDevices(myInstance, &physicalDeviceCount, nullptr);
	if (result != VK_SUCCESS)
		Debug::Breakpoint();
	std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
	result = table.myEnumeratePhysicalDevices(myInstance, &physicalDeviceCount, physicalDevices.data());
	if (result != VK_SUCCESS)
		Debug::Breakpoint();

	if (physicalDevices.empty())
		Debug::Breakpoint();

	// Choose best physical device to create device with
	uint32_t bestPhysicalDeviceIndex = 0u;
	uint32_t maxScore = 0u;
	for (uint32_t i = 0; i < physicalDeviceCount; ++i)
	{
		// Give a score to the physical device. (Somehow)
		VkPhysicalDevice device = physicalDevices[i];
		uint32_t score = 0u;
		uint32_t graphicsQueueFamilyIndex = 0u;
	
		// Device properties
		VkPhysicalDeviceProperties properties;
		table.myGetPhysicalDeviceProperties(device, &properties);
		score += properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? 100u : 0u;
	
		// Device features
		VkPhysicalDeviceFeatures features;
		table.myGetPhysicalDeviceFeatures(device, &features);
	
		// Device queue family properties
		uint32_t familyPropertyCount = 0u;
		std::vector<VkQueueFamilyProperties> familyProperties;
		table.myGetPhysicalDeviceQueueFamilyProperties(device, &familyPropertyCount, nullptr);
		familyProperties.resize(familyPropertyCount);
		table.myGetPhysicalDeviceQueueFamilyProperties(device, &familyPropertyCount, familyProperties.data());
	
		for (uint32_t j = 0; j < familyPropertyCount; ++j)
		{
			const VkQueueFamilyProperties& properties = familyProperties[j];
	
			// Check if the queue family has presentation support for KHR
			VkBool32 hasPresentationSupportKHR;
			result = table.myGetPhysicalDeviceSurfaceSupportKHR(device, j, mySurface, &hasPresentationSupportKHR);
			if (result != VK_SUCCESS)
				Debug::Breakpoint();
			if (hasPresentationSupportKHR == VK_FALSE)
				continue;
	
			// Check if the queue family has a graphics queue
			if (properties.queueCount > 0 && properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				graphicsQueueFamilyIndex = j;
				break;
			}
		}
	
		// Device memory properties
		VkPhysicalDeviceMemoryProperties memoryProperties;
		table.myGetPhysicalDeviceMemoryProperties(device, &memoryProperties);
	
		if (maxScore < score)
		{
			maxScore = score;
			bestPhysicalDeviceIndex = i;
			myQueueFamilyIndex = graphicsQueueFamilyIndex;
		}
	}


	// Create device
	float queuePriority = 1.0f;
	VkDeviceQueueCreateInfo queueInfo{
		VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		nullptr,
		0,
		myQueueFamilyIndex,
		1,
		&queuePriority
	};
	
	const char* deviceExtensions[] =
	{
		"VK_KHR_swapchain"
	};
	uint32_t deviceExtensionCount = sizeof(deviceExtensions) / sizeof(const char*);

	// TODO: Check for wanted extensions
	uint32_t physicalDeviceExtensionCount = 0u;
	myPhysicalDevice = physicalDevices[bestPhysicalDeviceIndex];
	result = table.myEnumerateDeviceExtensionProperties(myPhysicalDevice, nullptr, &physicalDeviceExtensionCount, nullptr);
	if (result != VK_SUCCESS)
		Debug::Breakpoint();

	VkDeviceCreateInfo deviceInfo{
		VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		nullptr,
		0,
		1,
		&queueInfo,
		0,
		nullptr,
		deviceExtensionCount,
		deviceExtensions,
		nullptr
	};
	result = table.myCreateDevice(myPhysicalDevice, &deviceInfo, nullptr, &myDevice);
	if (result != VK_SUCCESS)
		Debug::Breakpoint();
	myDeviceTable.Initialize(myDevice, myInstance, table.myGetDeviceProcAddr);

	myDeviceTable.myGetDeviceQueue(myDevice, myQueueFamilyIndex, 0, &myQueue);
}

void Renderer::DestroyDevice()
{
	VulkanDeviceDispatchTable& table = myDeviceTable;
	table.myDestroyDevice(myDevice, nullptr);

#if IS_DEBUG_BUILD
	myDevice = VK_NULL_HANDLE;
	myQueue = VK_NULL_HANDLE;
	table.Clear();
#endif // IS_DEBUG_BUILD
}

void Renderer::CreateSwapchain(uint32_t aWidth, uint32_t aHeight, uint32_t aBackFramebufferCount)
{
	const VulkanInstanceDispatchTable& instanceTable = myInstanceTable;
	const VulkanDeviceDispatchTable& deviceTable = myDeviceTable;

	// Choose format for the swapchain
	uint32_t surfaceFormatCount = 0u;
	VULKAN_CHECK_VALID_RESULT(instanceTable.myGetPhysicalDeviceSurfaceFormatsKHR(myPhysicalDevice, mySurface, &surfaceFormatCount, nullptr));

	std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
	VULKAN_CHECK_VALID_RESULT(instanceTable.myGetPhysicalDeviceSurfaceFormatsKHR(myPhysicalDevice, mySurface, &surfaceFormatCount, surfaceFormats.data()));
	mySwapchainFormat = surfaceFormats[0].format;

	// Get surface capabilities
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	VULKAN_CHECK_VALID_RESULT(instanceTable.myGetPhysicalDeviceSurfaceCapabilitiesKHR(myPhysicalDevice, mySurface, &surfaceCapabilities));

	// Check if current surface allows for the amount of images we want. If maxImageCount is 0, then there's unlimited (until we run out of memory)
	mySwapchainImageCount = aBackFramebufferCount;
	if (surfaceCapabilities.maxImageCount)
	{
		// Clamp to allowed range
		mySwapchainImageCount = surfaceCapabilities.minImageCount < mySwapchainImageCount ? mySwapchainImageCount : surfaceCapabilities.minImageCount;
		mySwapchainImageCount = mySwapchainImageCount < surfaceCapabilities.maxImageCount ? mySwapchainImageCount : surfaceCapabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR swapchainInfo{
		VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		nullptr,
		0,
		mySurface,
		mySwapchainImageCount,
		mySwapchainFormat,
		VK_COLOR_SPACE_SRGB_NONLINEAR_KHR, // TODO: check what is this
		VkExtent2D{ aWidth, aHeight },
		1u, // TODO: what is an stereoscopic-3D application? If so, it needs to be something else than non 1
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		VK_SHARING_MODE_EXCLUSIVE, // TODO: Investigate this, and its uses
		0u, // TODO: Same as previous
		nullptr,
		VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR, // TODO: Same as previous
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, // TODO: Same as previous
		VK_PRESENT_MODE_FIFO_KHR, // TODO: Choose right one, or give the user the ability to choose
		VK_FALSE, // TODO: Investigate this
		mySwapchain // TODO: When creating the swapchain again due to resize, pass the previous swapchain here
	};
	VkSwapchainKHR newSwapchain = VK_NULL_HANDLE;
	VULKAN_CHECK_VALID_RESULT(deviceTable.myCreateSwapchainKHR(myDevice, &swapchainInfo, nullptr, &newSwapchain));
	deviceTable.myDestroySwapchainKHR(myDevice, mySwapchain, nullptr);
	mySwapchain = newSwapchain;

	// Request number of images in swapchain so it does not complain later on when we use mySwapchainImageCount
	VULKAN_CHECK_VALID_RESULT(deviceTable.myGetSwapchainImagesKHR(myDevice, mySwapchain, &mySwapchainImageCount, nullptr));
}

void Renderer::DestroySwapchain()
{
	const VulkanDeviceDispatchTable& table = myDeviceTable;
	table.myDestroySwapchainKHR(myDevice, mySwapchain, nullptr);

#if IS_DEBUG_BUILD
	mySwapchain = VK_NULL_HANDLE;
#endif
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

		Debug::PrintToOutput(message);

		// Don't bail out, but keep going.
		return false;
	}
}

#endif
