#pragma once

#ifndef VK_NO_PROTOTYPES
#define VK_NO_PROTOTYPES
#endif // VK_NO_PROTOTYPES
#include <vulkan/vulkan_core.h>

#define VULKAN_DISPATCH_FUNCTION(name) PFN_vk##name my##name = nullptr

struct VulkanCommonDispatchTable
{
	VULKAN_DISPATCH_FUNCTION(GetInstanceProcAddr);
	VULKAN_DISPATCH_FUNCTION(EnumerateInstanceVersion);
	VULKAN_DISPATCH_FUNCTION(EnumerateInstanceExtensionProperties);
	VULKAN_DISPATCH_FUNCTION(EnumerateInstanceLayerProperties);
	VULKAN_DISPATCH_FUNCTION(CreateInstance);
};

struct VulkanInstanceDispatchTable
{
	void Initialize(VkInstance anInstance);
	void Clear();

	VULKAN_DISPATCH_FUNCTION(GetInstanceProcAddr);
	VULKAN_DISPATCH_FUNCTION(GetDeviceProcAddr);

	VULKAN_DISPATCH_FUNCTION(DestroyInstance);

	VULKAN_DISPATCH_FUNCTION(DestroySurfaceKHR);

	VULKAN_DISPATCH_FUNCTION(EnumeratePhysicalDevices);
	VULKAN_DISPATCH_FUNCTION(GetPhysicalDeviceProperties);
	VULKAN_DISPATCH_FUNCTION(GetPhysicalDeviceFeatures);
	VULKAN_DISPATCH_FUNCTION(GetPhysicalDeviceQueueFamilyProperties);
	VULKAN_DISPATCH_FUNCTION(GetPhysicalDeviceSurfaceSupportKHR);
	VULKAN_DISPATCH_FUNCTION(GetPhysicalDeviceMemoryProperties);
	VULKAN_DISPATCH_FUNCTION(GetPhysicalDeviceSurfaceFormatsKHR);
	VULKAN_DISPATCH_FUNCTION(GetPhysicalDeviceSurfaceCapabilitiesKHR);
	VULKAN_DISPATCH_FUNCTION(EnumerateDeviceExtensionProperties);
	VULKAN_DISPATCH_FUNCTION(EnumerateDeviceLayerProperties);

	VULKAN_DISPATCH_FUNCTION(CreateDevice);
	VULKAN_DISPATCH_FUNCTION(DestroyDevice);
	VULKAN_DISPATCH_FUNCTION(CreateDebugUtilsMessengerEXT);
	VULKAN_DISPATCH_FUNCTION(DestroyDebugUtilsMessengerEXT);
	VULKAN_DISPATCH_FUNCTION(SubmitDebugUtilsMessageEXT);
};

struct VulkanDeviceDispatchTable
{
	void Initialize(VkDevice aDevice, VkInstance anInstance, PFN_vkGetDeviceProcAddr getDeviceProcAddr);
	void Clear();

	VULKAN_DISPATCH_FUNCTION(GetDeviceProcAddr);

	VULKAN_DISPATCH_FUNCTION(GetDeviceQueue);
	VULKAN_DISPATCH_FUNCTION(DeviceWaitIdle);
	VULKAN_DISPATCH_FUNCTION(AcquireNextImageKHR);
	VULKAN_DISPATCH_FUNCTION(BeginCommandBuffer);
	VULKAN_DISPATCH_FUNCTION(EndCommandBuffer);
	VULKAN_DISPATCH_FUNCTION(CmdBeginRenderPass);
	VULKAN_DISPATCH_FUNCTION(CmdEndRenderPass);
	VULKAN_DISPATCH_FUNCTION(CmdBindPipeline);
	VULKAN_DISPATCH_FUNCTION(CmdSetViewport);
	VULKAN_DISPATCH_FUNCTION(CmdSetScissor);
	VULKAN_DISPATCH_FUNCTION(CmdBindDescriptorSets);
	VULKAN_DISPATCH_FUNCTION(CmdBindVertexBuffers);
	VULKAN_DISPATCH_FUNCTION(CmdDraw);

	VULKAN_DISPATCH_FUNCTION(CreateSwapchainKHR);
	VULKAN_DISPATCH_FUNCTION(DestroySwapchainKHR);
	VULKAN_DISPATCH_FUNCTION(GetSwapchainImagesKHR);

	VULKAN_DISPATCH_FUNCTION(CreateImageView);
	VULKAN_DISPATCH_FUNCTION(DestroyImageView);

	VULKAN_DISPATCH_FUNCTION(CreateRenderPass);
	VULKAN_DISPATCH_FUNCTION(DestroyRenderPass);

	VULKAN_DISPATCH_FUNCTION(CreateFramebuffer);
	VULKAN_DISPATCH_FUNCTION(DestroyFramebuffer);

	VULKAN_DISPATCH_FUNCTION(CreateCommandPool);
	VULKAN_DISPATCH_FUNCTION(DestroyCommandPool);
	VULKAN_DISPATCH_FUNCTION(AllocateCommandBuffers);
	VULKAN_DISPATCH_FUNCTION(FreeCommandBuffers);

	VULKAN_DISPATCH_FUNCTION(CreateSemaphore);
	VULKAN_DISPATCH_FUNCTION(DestroySemaphore);

	VULKAN_DISPATCH_FUNCTION(CreateFence);
	VULKAN_DISPATCH_FUNCTION(DestroyFence);
	VULKAN_DISPATCH_FUNCTION(WaitForFences);
	VULKAN_DISPATCH_FUNCTION(ResetFences);

	VULKAN_DISPATCH_FUNCTION(CreateShaderModule);
	VULKAN_DISPATCH_FUNCTION(DestroyShaderModule);

	VULKAN_DISPATCH_FUNCTION(CreatePipelineLayout);
	VULKAN_DISPATCH_FUNCTION(DestroyPipelineLayout);

	VULKAN_DISPATCH_FUNCTION(CreateGraphicsPipelines);
	VULKAN_DISPATCH_FUNCTION(DestroyPipeline);

	VULKAN_DISPATCH_FUNCTION(CreateBuffer);
	VULKAN_DISPATCH_FUNCTION(DestroyBuffer);
	VULKAN_DISPATCH_FUNCTION(GetBufferMemoryRequirements);
	VULKAN_DISPATCH_FUNCTION(BindBufferMemory);

	VULKAN_DISPATCH_FUNCTION(CreateDescriptorSetLayout);
	VULKAN_DISPATCH_FUNCTION(DestroyDescriptorSetLayout);

	VULKAN_DISPATCH_FUNCTION(CreateDescriptorPool);
	VULKAN_DISPATCH_FUNCTION(DestroyDescriptorPool);

	VULKAN_DISPATCH_FUNCTION(AllocateDescriptorSets);
	VULKAN_DISPATCH_FUNCTION(FreeDescriptorSets);
	VULKAN_DISPATCH_FUNCTION(UpdateDescriptorSets);

	VULKAN_DISPATCH_FUNCTION(AllocateMemory);
	VULKAN_DISPATCH_FUNCTION(FreeMemory);
	VULKAN_DISPATCH_FUNCTION(MapMemory);
	VULKAN_DISPATCH_FUNCTION(UnmapMemory);

	VULKAN_DISPATCH_FUNCTION(CmdBeginDebugUtilsLabelEXT);
	VULKAN_DISPATCH_FUNCTION(CmdEndDebugUtilsLabelEXT);
	VULKAN_DISPATCH_FUNCTION(CmdInsertDebugUtilsLabelEXT);
	VULKAN_DISPATCH_FUNCTION(SetDebugUtilsObjectNameEXT);

	VULKAN_DISPATCH_FUNCTION(QueueSubmit);
	VULKAN_DISPATCH_FUNCTION(QueuePresentKHR);
};

#undef DispatchFunction
