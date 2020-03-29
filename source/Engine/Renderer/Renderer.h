#pragma once

#include "VulkanWrapper/VulkanWrapper.h"

class Window;

namespace DBZ
{

class DisplayRenderer;

class Renderer
{
public:
	static void Create(Renderer& aRendererOut);
	static void Destroy(Renderer& aRenderer);

	void BeginFrame(DisplayRenderer* someDisplayRenderers, uint32_t aDisplayRendererCount);
	void EndFrame(Semaphore* someWaitSemaphores, uint32_t aWaitSemaphoreCount, DisplayRenderer* someDisplayRenderers, uint32_t aDisplayRendererCount);
	void Submit(uint32_t aQueueIndex, const VkSubmitInfo* aSubmitInfos, uint32_t aSubmitCount, const Fence* aFence) const;
	void WaitForDevice() const;

	// TODO: Gather some statistics from this
	// TODO: Store all data somehow in the Renderer
	void Create(const Window& aWindow, uint32_t aDesiredImageCount, const VkRenderPassCreateInfo& aRenderPassCreateInfo, DisplayRenderer& aDisplayRendererOut);
	void Destroy(DisplayRenderer& aDisplayRenderer);
	void Resize(uint32_t aWidth, uint32_t aHeight, DisplayRenderer& aDisplayRenderer);

	void Create(const VkCommandPoolCreateInfo& aCommandPoolCreateInfo, CommandPool& aCommandPoolOut);
	void Destroy(CommandPool& aCommandPool);

	void Create(const VkCommandBufferAllocateInfo& aCommandBufferAllocateInfo, VulkanCommandBufferWrapper* someVulkanCommandBufferWrappers);
	void Destroy(CommandPool& aCommandPool, VulkanCommandBufferWrapper* someCommandBuffers, uint32_t aCommandBufferCount);

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

	uint32_t GetQueueFamilyIndex(uint32_t anIndex) const { return myVulkanDeviceWrapper.GetQueueFamilyIndex(anIndex); }

	// Allow triple frame buffering at most
	constexpr static uint32_t ourMaxDisplayImagesPerDisplay = 3u;
	// One presented and at maximum another 2 being processed
	constexpr static uint32_t ourMaxOnFlightImagesPerDisplay = 2u;

private:
	// Device management helpers
	void CreateDevice(const DisplayRenderer* someDisplays, uint32_t aDisplayCount);
	void DestroyDevice();

	// DisplayRenderer object management helpers
	void CreateDisplaySurface(const Window& aWindow, DisplayRenderer& aDisplayRendererOut) const;
	void CreateDisplaySwapchain(uint32_t aWidth, uint32_t aHeight, uint32_t aDesiredImageCount, DisplayRenderer& aDisplayRendererOut) const;
	void CreateDisplayRenderPass(const VkRenderPassCreateInfo& aRenderPassCreateInfo, DisplayRenderer& aDisplayRendererOut) const;
	void CreateDisplayImageViews(DisplayRenderer& aDisplayRendererOut) const;
	void CreateDisplayFramebuffers(DisplayRenderer& aDisplayRendererOut) const;
	void DestroyDisplaySwapchainResources(DisplayRenderer& aDisplayRenderer) const;

	VulkanInstanceWrapper myVulkanInstanceWrapper;
	VulkanDeviceWrapper myVulkanDeviceWrapper;
};

}
