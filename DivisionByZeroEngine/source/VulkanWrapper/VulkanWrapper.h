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
class name { Vk##name myHandle; }
#endif // WRAP_VULKAN_RESOURCE

// We need to wrap around the vk versions because most of them are typedefs of the same type
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

// Renderer instance
class VulkanInstanceWrapper
{
public:
	// HWND for Windows
	// Need to research other platforms
	static void Create(VulkanInstanceWrapper& aVulkanWrapper);
	static void Destroy(VulkanInstanceWrapper& aVulkanWrapper);

	void Create(const VkCommandPoolCreateInfo& aCommandPoolCreateInfo, CommandPool& aCommandPoolOut);
	void Destroy(CommandPool& aCommandPool);

	void Create(const VkCommandBufferAllocateInfo& aCommanBufferAllocateInfo, CommandBuffer* someCommandBuffersOut);
	void Destroy(CommandPool& aCommandPool, CommandBuffer* someCommandBuffers, uint32_t aCommandBufferCount);

	void Create(const VkRenderPassCreateInfo& aRenderPassCreateInfo, RenderPass& aRenderPassOut);
	void Destroy(RenderPass& aRenderPass);

	void Create(const VkImageViewCreateInfo& aImageViewCreateInfo, ImageView& aImageViewOut);
	void Destroy(ImageView& aImageView);

	void Create(const VkFramebufferCreateInfo& aFramebufferCreateInfo, Framebuffer& aFramebufferOut);
	void Destroy(Framebuffer& aFramebuffer);

	void Create(const VkPipelineLayoutCreateInfo& aPipelineLayoutCreateInfo, PipelineLayout& aPipelineLayoutOut);
	void Destroy(PipelineLayout& aPipelineLayout);

	void Create(const VkGraphicsPipelineCreateInfo* someGraphicsPipelineCreateInfos, Pipeline* somePipelinesOut, uint32_t aPipelineCount);
	void Destroy(Pipeline& aPipeline);

	void Create(const VkSemaphoreCreateInfo& aSemaphoreCreateInfo, Semaphore& aSemaphoreOut);
	void Destroy(Semaphore& aSemaphore);

	void Create(const VkFenceCreateInfo& aFenceCreateInfo, Fence& aFenceOut);
	void Destroy(Fence& aFence);

	void Create(const VkShaderModuleCreateInfo& aShaderModuleCreateInfo, ShaderModule& aShaderModuleOut);
	void Destroy(ShaderModule& aShaderModule);

	void Create(const VkBufferCreateInfo& aBufferCreateInfo, Buffer& aBufferOut);
	void Destroy(Buffer& aBuffer);

	void Create(const VkDescriptorSetLayoutCreateInfo& aDescriptorSetLayoutCreateInfo, DescriptorSetLayout& aDescriptorSetLayoutOut);
	void Destroy(DescriptorSetLayout& aDescriptorSetLayout);

	void Create(const VkDescriptorPoolCreateInfo& aDescriptorPoolCreateInfo, DescriptorPool& aDescriptorPoolOut);
	void Destroy(DescriptorPool& aDescriptorPool);

	void Create(const VkDescriptorSetAllocateInfo& aDescriptorSetAllocate, DescriptorSet* someDescriptorSetOut);
	void Destroy(DescriptorPool& aDescriptorPool, DescriptorSet* someDescriptorSet, uint32_t aDescriptorSetCount);
	void UpdateDescriptorSets(const VkWriteDescriptorSet* someWriteDescriptorSets, uint32_t aWriteDescriptorCount);

	void GetMemoryRequirements(const Buffer& aBuffer, VkMemoryRequirements& aMemoryRequirementsOut);
	void AllocateDeviceMemory(VkDeviceSize aSize, uint32_t aMemoryTypeBits, VkMemoryPropertyFlags aMemoryProperties, DeviceMemory& aDeviceMemoryOut);
	void FreeDeviceMemory(DeviceMemory& aDeviceMemory);

	void BindDeviceMemory(DeviceMemory& aDeviceMemory, VkDeviceSize anOffset, Buffer& aBuffer);
	void* MapDeviceMemory(DeviceMemory& aDeviceMemory, VkDeviceSize anOffset, VkDeviceSize aSize);
	void UnmapDeviceMemory(DeviceMemory& aDeviceMemory);

	void ResizeSwapchain(int aWidth, int aHeight);

	uint32_t AcquireNextImage(Semaphore& aSemaphore);
	uint32_t AcquireNextImage(Fence& aFence);
	uint32_t AcquireNextImage(Semaphore& aSemaphore, Fence& aFence);
	void BeginCommandBuffer(CommandBuffer& aCommandBuffer, const VkCommandBufferBeginInfo& aCommandBufferBeginInfo);
	void EndCommandBuffer(CommandBuffer& aCommandBuffer);
	void BeginRenderPass(CommandBuffer& aCommandBuffer, const VkRenderPassBeginInfo& aRenderPassBeginInfo);
	void EndRenderPass(CommandBuffer& aCommandBuffer);
	void BindPipeline(CommandBuffer& aCommandBuffer, Pipeline& aPipeline, bool isGraphicsPipeline);
	void SetViewport(CommandBuffer& aCommandBuffer, VkViewport* someViewports, uint32_t aViewportCount, uint32_t aFirstViewport);
	void SetScissor(CommandBuffer& aCommandBuffer, VkRect2D* someRects, uint32_t aRectCount, uint32_t aFirstRect);
	void BindVertexBuffers(CommandBuffer& aCommandBuffer, Buffer* someBuffers, const VkDeviceSize* someOffsets, uint32_t aBufferCount);
	void BindDescriptorSets(CommandBuffer& aCommandBuffer, PipelineLayout& aPipelineLayout, DescriptorSet* someDescriptorSets, uint32_t aDescriptorSetCount);
	void Draw(CommandBuffer& aCommandBuffer, uint32_t aVertexCount, uint32_t aFirstVertex, uint32_t anInstanceCount, uint32_t aFirstInstance);
	void WaitForFences(Fence* someFences, uint32_t aFenceCount);
	void ResetFences(Fence* someFences, uint32_t aFenceCount);
	void Submit(const VkSubmitInfo* aSubmitInfos, uint32_t aSubmitCount);
	void Submit(const VkSubmitInfo* aSubmitInfos, uint32_t aSubmitCount, Fence& aFence);
	void Present(Semaphore* someWaitSemaphores, uint32_t aWaitSemaphoreCount, uint32_t anImageIndex);
	void WaitForDevice() const;

	uint32_t GetGraphicsQueueFamilyIndex() const { return myQueueFamilyIndex; }
	uint32_t GetSwapchainImageCount() const { return mySwapchainImageCount; }
	VkFormat GetSwapchainFormat() const { return mySwapchainFormat; }
	void GetSwapchainImages(Image* someImagesOut);

private:

	// This resources are handled by the Renderer and won't be handled to the user
	void CreateInstance();
	void DestroyInstance();
	void CreateSurface(void* aWindowHandle);
	void DestroySurface();
	void CreateDevice();
	void DestroyDevice();
	void CreateSwapchain(uint32_t aWidth, uint32_t aHeight, uint32_t aBackFramebufferCount);
	void DestroySwapchain();

	constexpr static uint32_t ourMaxBackFramebuffers = 3u;

	VkInstance myInstance = VK_NULL_HANDLE;
	VkSurfaceKHR mySurface = VK_NULL_HANDLE;
	VkPhysicalDevice myPhysicalDevice = VK_NULL_HANDLE;
	VkDevice myDevice = VK_NULL_HANDLE;
	VkQueue myQueue = VK_NULL_HANDLE;
	VkSwapchainKHR mySwapchain = VK_NULL_HANDLE;
	VkImageView myBackFramebufferImageViews[ourMaxBackFramebuffers] = { VK_NULL_HANDLE };
	VkFramebuffer myBackFramebuffers[ourMaxBackFramebuffers] = { VK_NULL_HANDLE };
	VkDebugUtilsMessengerEXT DebugMessenger = VK_NULL_HANDLE;

	VkFormat mySwapchainFormat;

	// For now we will just stick to one graphics queue
	uint32_t myQueueFamilyIndex = 0u;
	uint32_t mySwapchainImageCount = 3u;

	VulkanInstanceDispatchTable myInstanceTable;
	VulkanDeviceDispatchTable myDeviceTable;
};

class VulkanDeviceWrapper
{
public:
	static void Create(const VulkanInstanceWrapper& aVulkanInstanceWrapper, VulkanDeviceWrapper& aVulkanDeviceWrapper);
	static void Destroy(VulkanDeviceWrapper& aVulkanDeviceWrapper);

private:
	uint32_t myQueueFamilyIndex = 0u;
	VkPhysicalDevice myPhysicalDevice = VK_NULL_HANDLE;
	VkDevice myDevice = VK_NULL_HANDLE;
	VkQueue myQueue = VK_NULL_HANDLE;

	VulkanDeviceDispatchTable myTable;
};

class VulkanDisplayWrapper
{
public:
	static void Create(const VulkanDeviceWrapper& aVulkanDeviceWrapper, void* aWindowHandle, uint32_t aBackFramebufferCount, VulkanDisplayWrapper& aVulkanDisplayWrapper);
	static void Destroy(VulkanDisplayWrapper& aVulkanDisplayWrapper);

private:
	constexpr static uint32_t ourMaxDisplayFramebuffer = 3u;

	uint32_t myDisplayImageCount = 3u;
	VkFormat mySwapchainFormat;

	VkSurfaceKHR mySurface = VK_NULL_HANDLE;
	VkSwapchainKHR mySwapchain = VK_NULL_HANDLE;
	VkImageView myDisplayFramebufferImageViews[ourMaxDisplayFramebuffer] = { VK_NULL_HANDLE };
	VkFramebuffer myDisplayFramebuffers[ourMaxDisplayFramebuffer] = { VK_NULL_HANDLE };
};

