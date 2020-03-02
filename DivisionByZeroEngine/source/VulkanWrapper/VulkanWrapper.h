#pragma once

#ifndef VK_NO_PROTOTYPES
#define VK_NO_PROTOTYPES
#endif // VK_NO_PROTOTYPES
#include <vulkan/vulkan_core.h>

#ifndef WRAP_VULKAN_RESOURCE
#define WRAP_VULKAN_RESOURCE(name) class name;\
inline const Vk##name& Unwrap(const name& aResource) { return reinterpret_cast<const Vk##name&>(aResource); } \
inline const Vk##name* Unwrap(const name* aResource) { return reinterpret_cast<const Vk##name*>(aResource); } \
inline Vk##name& Unwrap(name& aResource) { return reinterpret_cast<Vk##name&>(aResource); } \
inline Vk##name* Unwrap(name* aResource) { return reinterpret_cast<Vk##name*>(aResource); } \
class name { Vk##name myHandle = VK_NULL_HANDLE; }; \
static_assert(sizeof(Vk##name) == sizeof(name), "Resource wrapper size does not match original size");
#endif // WRAP_VULKAN_RESOURCE

// We need to wrap around the vk versions because most of them are typedefs of the same type
WRAP_VULKAN_RESOURCE(Instance);
WRAP_VULKAN_RESOURCE(DebugUtilsMessengerEXT);
WRAP_VULKAN_RESOURCE(PhysicalDevice);
WRAP_VULKAN_RESOURCE(SurfaceKHR);
WRAP_VULKAN_RESOURCE(Device);
WRAP_VULKAN_RESOURCE(SwapchainKHR);
WRAP_VULKAN_RESOURCE(Queue);
WRAP_VULKAN_RESOURCE(CommandPool);
WRAP_VULKAN_RESOURCE(CommandBuffer);
WRAP_VULKAN_RESOURCE(RenderPass);
WRAP_VULKAN_RESOURCE(Image);
WRAP_VULKAN_RESOURCE(ImageView);
WRAP_VULKAN_RESOURCE(Framebuffer);
WRAP_VULKAN_RESOURCE(DescriptorSetLayout);
WRAP_VULKAN_RESOURCE(PipelineLayout);
WRAP_VULKAN_RESOURCE(Pipeline);
WRAP_VULKAN_RESOURCE(Semaphore);
WRAP_VULKAN_RESOURCE(Fence);
WRAP_VULKAN_RESOURCE(ShaderModule);
WRAP_VULKAN_RESOURCE(DeviceMemory);
WRAP_VULKAN_RESOURCE(Buffer);
WRAP_VULKAN_RESOURCE(DescriptorPool);
WRAP_VULKAN_RESOURCE(DescriptorSet);

#undef WRAP_VULKAN_RESOURCE


#include "VulkanDispatchTable.h"

class VulkanDeviceWrapper;
class VulkanCommandBufferWrapper;

// Renderer instance
class VulkanInstanceWrapper
{
public:
	static void Create(VulkanInstanceWrapper& aVulkanInstanceWrapper);
	static void Destroy(VulkanInstanceWrapper& aVulkanInstanceWrapper);

	void EnumeratePhysicalDevices(uint32_t& aPhysicalDeviceCount, PhysicalDevice* somePhysicalDevicesOut) const;
	void GetPhysicalDeviceProperties(const PhysicalDevice& aPhysicalDevice, VkPhysicalDeviceProperties& aPhysicalDevicePropertiesOut) const;
	void GetPhysicalDeviceFeatures(const PhysicalDevice& aPhysicalDevice, VkPhysicalDeviceFeatures& aPhysicalDeviceFeaturesOut) const;
	void GetPhysicalDeviceQueueFamilyProperties(const PhysicalDevice& aPhysicalDevice, uint32_t& aQueueFamilyPropertyCount, VkQueueFamilyProperties* someQueueFamilyPropertiesOut) const;
	bool GetPhysicalDeviceSurfaceSupportKHR(const PhysicalDevice& aPhysicalDevice, uint32_t aQueueFamilyIndex, const SurfaceKHR& aSurface) const;
	void GetPhysicalDeviceSurfaceFormatsKHR(const PhysicalDevice& aPhysicalDevice, const SurfaceKHR& aSurface, uint32_t& aSurfaceFormatCount, VkSurfaceFormatKHR* someSurfaceFormatsOut) const;
	void GetPhysicalDeviceSurfaceCapabilitiesKHR(const PhysicalDevice& aPhysicalDevice, const SurfaceKHR& aSurface, VkSurfaceCapabilitiesKHR& aSurfaceCapabilitiesKHROut) const;
	void GetPhysicalDeviceMemoryProperties(const PhysicalDevice& aPhysicalDevice, VkPhysicalDeviceMemoryProperties& aPhysicalDeviceMemoryPropertiesOut) const;
	void EnumerateDeviceExtensionProperties(const PhysicalDevice& aPhysicalDevice, const char* aLayerName, uint32_t& aPropertyCount, VkExtensionProperties* someExtensionPropertiesOut) const;

	void Create(const PhysicalDevice& aPhysicalDevice, const VkDeviceCreateInfo& aDeviceCreateInfo, VulkanDeviceWrapper& aVulkanDeviceWrapperOut) const;
	void Destroy(VulkanDeviceWrapper& aVulkanDeviceWrapper) const;

	// HWND for Windows
	// Need to research other platforms
	void Create(void* aWindowHandle, SurfaceKHR& aSurface) const;
	void Destroy(SurfaceKHR& aSurface) const;

	Instance GetInstance() const { return myInstance; }
	const VulkanInstanceDispatchTable& GetTable() const { return myTable; }

private:
	Instance myInstance;
	//DebugUtilsMessengerEXT DebugMessenger; Not used for the moment
	VulkanInstanceDispatchTable myTable;
};

class VulkanDeviceWrapper
{
public:
	uint32_t AcquireNextImage(const SwapchainKHR& aSwapchain, const Semaphore* aSemaphore, const Fence* aFence) const;
	void GetSwapchainImagesKHR(const SwapchainKHR& aSwapchain, uint32_t& aSwapchainImageCount, Image* someSwapchainImagesOut) const;
	void WaitForFences(Fence* someFences, uint32_t aFenceCount) const;
	void ResetFences(Fence* someFences, uint32_t aFenceCount) const;
	void Submit(uint32_t aQueueIndex, const VkSubmitInfo* aSubmitInfos, uint32_t aSubmitCount, const Fence* aFence) const;
	void Present(const VkPresentInfoKHR& aPresentInfoKHR, uint32_t aQueueIndex) const;
	void WaitForDevice() const;

	void Create(const VkSwapchainCreateInfoKHR& aSwapchainCreateInfoKHR, SwapchainKHR& aSwapchainOut) const;
	void Destroy(SwapchainKHR& aSwapchain) const;

	void Create(const VkCommandPoolCreateInfo& aCommandPoolCreateInfo, CommandPool& aCommandPoolOut) const;
	void Destroy(CommandPool& aCommandPool) const;

	void Create(const VkCommandBufferAllocateInfo& aCommanBufferAllocateInfo, VulkanCommandBufferWrapper* someVulkanCommandBufferWrappers) const;
	void Destroy(CommandPool& aCommandPool, VulkanCommandBufferWrapper* someCommandBuffers, uint32_t aCommandBufferCount) const;

	void Create(const VkRenderPassCreateInfo& aRenderPassCreateInfo, RenderPass& aRenderPassOut) const;
	void Destroy(RenderPass& aRenderPass) const;

	void Create(const VkImageViewCreateInfo& aImageViewCreateInfo, ImageView& aImageViewOut) const;
	void Destroy(ImageView& aImageView) const;

	void Create(const VkFramebufferCreateInfo& aFramebufferCreateInfo, Framebuffer& aFramebufferOut) const;
	void Destroy(Framebuffer& aFramebuffer) const;

	void Create(const VkPipelineLayoutCreateInfo& aPipelineLayoutCreateInfo, PipelineLayout& aPipelineLayoutOut) const;
	void Destroy(PipelineLayout& aPipelineLayout) const;

	void Create(const VkGraphicsPipelineCreateInfo* someGraphicsPipelineCreateInfos, Pipeline* somePipelinesOut, uint32_t aPipelineCount) const;
	void Destroy(Pipeline& aPipeline) const;

	void Create(const VkSemaphoreCreateInfo& aSemaphoreCreateInfo, Semaphore& aSemaphoreOut) const;
	void Destroy(Semaphore& aSemaphore) const;

	void Create(const VkFenceCreateInfo& aFenceCreateInfo, Fence& aFenceOut) const;
	void Destroy(Fence& aFence) const;

	void Create(const VkShaderModuleCreateInfo& aShaderModuleCreateInfo, ShaderModule& aShaderModuleOut) const;
	void Destroy(ShaderModule& aShaderModule) const;

	void Create(const VkBufferCreateInfo& aBufferCreateInfo, Buffer& aBufferOut) const;
	void Destroy(Buffer& aBuffer) const;

	void Create(const VkDescriptorSetLayoutCreateInfo& aDescriptorSetLayoutCreateInfo, DescriptorSetLayout& aDescriptorSetLayoutOut) const;
	void Destroy(DescriptorSetLayout& aDescriptorSetLayout) const;

	void Create(const VkDescriptorPoolCreateInfo& aDescriptorPoolCreateInfo, DescriptorPool& aDescriptorPoolOut) const;
	void Destroy(DescriptorPool& aDescriptorPool) const;

	void Create(const VkDescriptorSetAllocateInfo& aDescriptorSetAllocate, DescriptorSet* someDescriptorSetOut) const;
	void Destroy(DescriptorPool& aDescriptorPool, DescriptorSet* someDescriptorSet, uint32_t aDescriptorSetCount) const;
	void UpdateDescriptorSets(const VkWriteDescriptorSet* someWriteDescriptorSets, uint32_t aWriteDescriptorCount) const;

	void GetMemoryRequirements(const Buffer& aBuffer, VkMemoryRequirements& aMemoryRequirementsOut) const;
	void AllocateDeviceMemory(VkDeviceSize aSize, uint32_t aMemoryTypeBits, VkMemoryPropertyFlags aMemoryProperties, DeviceMemory& aDeviceMemoryOut) const;
	void FreeDeviceMemory(DeviceMemory& aDeviceMemory) const;

	void BindDeviceMemory(DeviceMemory& aDeviceMemory, VkDeviceSize anOffset, Buffer& aBuffer) const;
	void* MapDeviceMemory(DeviceMemory& aDeviceMemory, VkDeviceSize anOffset, VkDeviceSize aSize) const;
	void UnmapDeviceMemory(DeviceMemory& aDeviceMemory) const;

	uint32_t GetQueueFamilyIndex(uint32_t anIndex) const { return myQueueFamilyIndices[anIndex]; }
	Queue GetQueue(uint32_t anIndex) const { return myQueues[anIndex]; }
	PhysicalDevice GetPhysicalDevice() const { return myPhysicalDevice; }
	Device GetDevice() const { return myDevice; }
	const VulkanDeviceDispatchTable& GetTable() const { return myTable; }

private:
	friend class VulkanInstanceWrapper;

	uint32_t* myQueueFamilyIndices = nullptr;
	Queue* myQueues = nullptr;
	PhysicalDevice myPhysicalDevice;
	Device myDevice;
	VkPhysicalDeviceMemoryProperties myMemoryProperties;
	VulkanDeviceDispatchTable myTable;
};

class VulkanCommandBufferWrapper
{
public:
	void BeginCommandBuffer(const VkCommandBufferBeginInfo& aCommandBufferBeginInfo) const;
	void EndCommandBuffer() const;
	void BeginRenderPass(const VkRenderPassBeginInfo& aRenderPassBeginInfo) const;
	void EndRenderPass() const;
	void BindPipeline(Pipeline& aPipeline, bool isGraphicsPipeline) const;
	void SetViewport(VkViewport* someViewports, uint32_t aViewportCount, uint32_t aFirstViewport) const;
	void SetScissor(VkRect2D* someRects, uint32_t aRectCount, uint32_t aFirstRect) const;
	void BindVertexBuffers(Buffer* someBuffers, const VkDeviceSize* someOffsets, uint32_t aBufferCount) const;
	void BindDescriptorSets(PipelineLayout& aPipelineLayout, DescriptorSet* someDescriptorSets, uint32_t aDescriptorSetCount) const;
	void Draw(uint32_t aVertexCount, uint32_t aFirstVertex, uint32_t anInstanceCount, uint32_t aFirstInstance) const;

private:
	friend class VulkanDeviceWrapper;

	CommandBuffer myCommandBuffer;
	VulkanCommandBufferDispatchTable myTable;
};

