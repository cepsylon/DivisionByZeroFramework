#include "Renderer.h"

#include "DisplayRenderer.h"

#include "Memory/StackAllocation.h"
#include "Window/Window.h"

#include "Common/Debug.h"

namespace DBZ
{

void Renderer::Create(Renderer& aRendererOut)
{
	VulkanInstanceWrapper::Create(aRendererOut.myVulkanInstanceWrapper);
}

void Renderer::Destroy(Renderer& aRenderer)
{
	aRenderer.DestroyDevice();
	VulkanInstanceWrapper::Destroy(aRenderer.myVulkanInstanceWrapper);
}

void Renderer::BeginFrame(DisplayRenderer* someDisplayRenderers, uint32_t aDisplayRendererCount)
{
	Fence* fencesToWaitFor = static_cast<Fence*>(DBZ_ALLOCATE_STACK_MEMORY(sizeof(Fence) * aDisplayRendererCount));
	for (uint32_t i = 0; i < aDisplayRendererCount; ++i)
		fencesToWaitFor[i] = someDisplayRenderers[i].myOnFlightFences[someDisplayRenderers[i].myOnFlightImageIndex];

	// Throttle GPU if needed so we do not waste more power than needed for display frame rate
	myVulkanDeviceWrapper.WaitForFences(fencesToWaitFor, aDisplayRendererCount);
	myVulkanDeviceWrapper.ResetFences(fencesToWaitFor, aDisplayRendererCount);

	// Acquire next image
	for (uint32_t i = 0; i < aDisplayRendererCount; ++i)
	{
		DisplayRenderer& display = someDisplayRenderers[i];
		myVulkanDeviceWrapper.AcquireNextImage(display.mySwapchain, &display.myDisplayImageAcquireSemaphores[display.myOnFlightImageIndex], nullptr);
	}
}

void Renderer::EndFrame(Semaphore* someWaitSemaphores, uint32_t aWaitSemaphoreCount, DisplayRenderer* someDisplayRenderers, uint32_t aDisplayRendererCount)
{
	SwapchainKHR* displaySwapchains = static_cast<SwapchainKHR*>(DBZ_ALLOCATE_STACK_MEMORY(sizeof(SwapchainKHR) * aDisplayRendererCount));
	uint32_t* displayImageIndices = static_cast<uint32_t*>(DBZ_ALLOCATE_STACK_MEMORY(sizeof(uint32_t) * aDisplayRendererCount));

	for (uint32_t i = 0; i < aDisplayRendererCount; ++i)
	{
		DisplayRenderer& display = someDisplayRenderers[i];
		displaySwapchains[i] = display.mySwapchain;
		displayImageIndices[i] = display.myDisplayImageIndex;
	}

	VkPresentInfoKHR presentInfoKHR
	{
		VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		nullptr,
		aWaitSemaphoreCount,
		Unwrap(someWaitSemaphores),
		1,
		Unwrap(displaySwapchains),
		displayImageIndices,
		nullptr
	};

	// TODO: queue is hardcoded
	myVulkanDeviceWrapper.Present(presentInfoKHR, 0);

	for (uint32_t i = 0; i < aDisplayRendererCount; ++i)
	{
		DisplayRenderer& display = someDisplayRenderers[i];
		display.myOnFlightImageIndex = (display.myOnFlightImageIndex + 1) % display.myOnFlightImageCount;
		display.myDisplayImageIndex = (display.myDisplayImageIndex + 1) % display.myDisplayImageCount;
	}
}

void Renderer::Submit(uint32_t aQueueIndex, const VkSubmitInfo* aSubmitInfos, uint32_t aSubmitCount, const Fence* aFence) const
{
	myVulkanDeviceWrapper.Submit(aQueueIndex, aSubmitInfos, aSubmitCount, aFence);
}

void Renderer::WaitForDevice() const
{
	myVulkanDeviceWrapper.WaitForDevice();
}

void Renderer::Create(const Window& aWindow, uint32_t aDesiredImageCount, const VkRenderPassCreateInfo& aRenderPassCreateInfo, DisplayRenderer& aDisplayRendererOut)
{
	CreateDisplaySurface(aWindow, aDisplayRendererOut);

	if (myVulkanDeviceWrapper.IsValid() == false)
		CreateDevice(&aDisplayRendererOut, 1);

	CreateDisplaySwapchain(aWindow.GetClientWidth(), aWindow.GetClientHeight(), aDesiredImageCount, aDisplayRendererOut);
	CreateDisplayRenderPass(aRenderPassCreateInfo, aDisplayRendererOut);
	CreateDisplayImageViews(aDisplayRendererOut);
	CreateDisplayFramebuffers(aDisplayRendererOut);
	
	// Fences and semaphores
	VkFenceCreateInfo fenceCreateInfo
	{
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			nullptr,
			VK_FENCE_CREATE_SIGNALED_BIT
	};

	VkSemaphoreCreateInfo semaphoreCreateInfo
	{
		VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		nullptr,
		0
	};

	for (uint32_t i = 0; i < aDisplayRendererOut.myOnFlightImageCount; ++i)
	{
		myVulkanDeviceWrapper.Create(semaphoreCreateInfo, aDisplayRendererOut.myDisplayImageAcquireSemaphores[i]);
		myVulkanDeviceWrapper.Create(fenceCreateInfo, aDisplayRendererOut.myOnFlightFences[i]);
	}
}

void Renderer::Destroy(DisplayRenderer& aDisplayRendererOut)
{
	for (uint32_t i = 0; i < aDisplayRendererOut.myOnFlightImageCount; ++i)
	{
		myVulkanDeviceWrapper.Destroy(aDisplayRendererOut.myDisplayImageAcquireSemaphores[i]);
		myVulkanDeviceWrapper.Destroy(aDisplayRendererOut.myOnFlightFences[i]);
	}

	DestroyDisplaySwapchainResources(aDisplayRendererOut);
	myVulkanDeviceWrapper.Destroy(aDisplayRendererOut.myDisplayRenderPass);
	myVulkanInstanceWrapper.Destroy(aDisplayRendererOut.mySurface);

#if IS_DEVELOPMENT_BUILD
	memset(&aDisplayRendererOut, 0, sizeof(aDisplayRendererOut));
#endif // IS_DEVELOPMENT_BUILD
}

void Renderer::Resize(uint32_t aWidth, uint32_t aHeight, DisplayRenderer& aDisplayRendererOut)
{
	// Destroy only swapchain related resources without destroying swapchain to be able to reuse if possible
	uint32_t swapchainImageCount = aDisplayRendererOut.myDisplayImageCount;
	for (uint32_t i = 0; i < swapchainImageCount; ++i)
	{
		myVulkanDeviceWrapper.Destroy(aDisplayRendererOut.myDisplayFramebuffers[i]);
		myVulkanDeviceWrapper.Destroy(aDisplayRendererOut.myDisplayImageViews[i]);
	}

	CreateDisplaySwapchain(aWidth, aHeight, aDisplayRendererOut.myDisplayImageCount, aDisplayRendererOut);
	CreateDisplayImageViews(aDisplayRendererOut);
	CreateDisplayFramebuffers(aDisplayRendererOut);
}

void Renderer::Create(const VkCommandPoolCreateInfo& aCommandPoolCreateInfo, CommandPool& aCommandPoolOut)
{
	myVulkanDeviceWrapper.Create(aCommandPoolCreateInfo, aCommandPoolOut);
}

void Renderer::Destroy(CommandPool& aCommandPool)
{
	myVulkanDeviceWrapper.Destroy(aCommandPool);
}

void Renderer::Create(const VkCommandBufferAllocateInfo& aCommandBufferAllocateInfo, VulkanCommandBufferWrapper* someVulkanCommandBufferWrappers)
{
	myVulkanDeviceWrapper.Create(aCommandBufferAllocateInfo, someVulkanCommandBufferWrappers);
}

void Renderer::Destroy(CommandPool& aCommandPool, VulkanCommandBufferWrapper* someCommandBuffers, uint32_t aCommandBufferCount)
{
	myVulkanDeviceWrapper.Destroy(aCommandPool, someCommandBuffers, aCommandBufferCount);
}

void Renderer::Create(const VkRenderPassCreateInfo& aRenderPassCreateInfo, RenderPass& aRenderPassOut)
{
	myVulkanDeviceWrapper.Create(aRenderPassCreateInfo, aRenderPassOut);
}

void Renderer::Destroy(RenderPass& aRenderPass)
{
	myVulkanDeviceWrapper.Destroy(aRenderPass);
}

void Renderer::Create(const VkImageViewCreateInfo& aImageViewCreateInfo, ImageView& aImageViewOut)
{
	myVulkanDeviceWrapper.Create(aImageViewCreateInfo, aImageViewOut);
}

void Renderer::Destroy(ImageView& aImageView)
{
	myVulkanDeviceWrapper.Destroy(aImageView);
}

void Renderer::Create(const VkFramebufferCreateInfo& aFramebufferCreateInfo, Framebuffer& aFramebufferOut)
{
	myVulkanDeviceWrapper.Create(aFramebufferCreateInfo, aFramebufferOut);
}

void Renderer::Destroy(Framebuffer& aFramebuffer)
{
	myVulkanDeviceWrapper.Destroy(aFramebuffer);
}

void Renderer::Create(const VkPipelineLayoutCreateInfo& aPipelineLayoutCreateInfo, PipelineLayout& aPipelineLayoutOut)
{
	myVulkanDeviceWrapper.Create(aPipelineLayoutCreateInfo, aPipelineLayoutOut);
}

void Renderer::Destroy(PipelineLayout& aPipelineLayout)
{
	myVulkanDeviceWrapper.Destroy(aPipelineLayout);
}

void Renderer::Create(const VkGraphicsPipelineCreateInfo* someGraphicsPipelineCreateInfos, Pipeline* somePipelinesOut, uint32_t aPipelineCount)
{
	myVulkanDeviceWrapper.Create(someGraphicsPipelineCreateInfos, somePipelinesOut, aPipelineCount);
}

void Renderer::Destroy(Pipeline& aPipeline)
{
	myVulkanDeviceWrapper.Destroy(aPipeline);
}

void Renderer::Create(const VkSemaphoreCreateInfo& aSemaphoreCreateInfo, Semaphore& aSemaphoreOut)
{
	myVulkanDeviceWrapper.Create(aSemaphoreCreateInfo, aSemaphoreOut);
}

void Renderer::Destroy(Semaphore& aSemaphore)
{
	myVulkanDeviceWrapper.Destroy(aSemaphore);
}

void Renderer::Create(const VkFenceCreateInfo& aFenceCreateInfo, Fence& aFenceOut)
{
	myVulkanDeviceWrapper.Create(aFenceCreateInfo, aFenceOut);
}

void Renderer::Destroy(Fence& aFence)
{
	myVulkanDeviceWrapper.Destroy(aFence);
}

void Renderer::Create(const VkShaderModuleCreateInfo& aShaderModuleCreateInfo, ShaderModule& aShaderModuleOut)
{
	myVulkanDeviceWrapper.Create(aShaderModuleCreateInfo, aShaderModuleOut);
}

void Renderer::Destroy(ShaderModule& aShaderModule)
{
	myVulkanDeviceWrapper.Destroy(aShaderModule);
}

void Renderer::Create(const VkBufferCreateInfo& aBufferCreateInfo, Buffer& aBufferOut)
{
	myVulkanDeviceWrapper.Create(aBufferCreateInfo, aBufferOut);
}

void Renderer::Destroy(Buffer& aBuffer)
{
	myVulkanDeviceWrapper.Destroy(aBuffer);
}

void Renderer::Create(const VkDescriptorSetLayoutCreateInfo& aDescriptorSetLayoutCreateInfo, DescriptorSetLayout& aDescriptorSetLayoutOut)
{
	myVulkanDeviceWrapper.Create(aDescriptorSetLayoutCreateInfo, aDescriptorSetLayoutOut);
}

void Renderer::Destroy(DescriptorSetLayout& aDescriptorSetLayout)
{
	myVulkanDeviceWrapper.Destroy(aDescriptorSetLayout);
}

void Renderer::Create(const VkDescriptorPoolCreateInfo& aDescriptorPoolCreateInfo, DescriptorPool& aDescriptorPoolOut)
{
	myVulkanDeviceWrapper.Create(aDescriptorPoolCreateInfo, aDescriptorPoolOut);
}

void Renderer::Destroy(DescriptorPool& aDescriptorPool)
{
	myVulkanDeviceWrapper.Destroy(aDescriptorPool);
}

void Renderer::Create(const VkDescriptorSetAllocateInfo& aDescriptorSetAllocate, DescriptorSet* someDescriptorSetOut)
{
	myVulkanDeviceWrapper.Create(aDescriptorSetAllocate, someDescriptorSetOut);
}

void Renderer::Destroy(DescriptorPool& aDescriptorPool, DescriptorSet* someDescriptorSet, uint32_t aDescriptorSetCount)
{
	myVulkanDeviceWrapper.Destroy(aDescriptorPool, someDescriptorSet, aDescriptorSetCount);
}

void Renderer::UpdateDescriptorSets(const VkWriteDescriptorSet* someWriteDescriptorSets, uint32_t aWriteDescriptorCount)
{
	myVulkanDeviceWrapper.UpdateDescriptorSets(someWriteDescriptorSets, aWriteDescriptorCount);
}

void Renderer::GetMemoryRequirements(const Buffer& aBuffer, VkMemoryRequirements& aMemoryRequirementsOut)
{
	myVulkanDeviceWrapper.GetMemoryRequirements(aBuffer, aMemoryRequirementsOut);
}

void Renderer::AllocateDeviceMemory(VkDeviceSize aSize, uint32_t aMemoryTypeBits, VkMemoryPropertyFlags aMemoryProperties, DeviceMemory& aDeviceMemoryOut)
{
	myVulkanDeviceWrapper.AllocateDeviceMemory(aSize, aMemoryTypeBits, aMemoryProperties, aDeviceMemoryOut);
}

void Renderer::FreeDeviceMemory(DeviceMemory& aDeviceMemory)
{
	myVulkanDeviceWrapper.FreeDeviceMemory(aDeviceMemory);
}

void Renderer::BindDeviceMemory(DeviceMemory& aDeviceMemory, VkDeviceSize anOffset, Buffer& aBuffer)
{
	myVulkanDeviceWrapper.BindDeviceMemory(aDeviceMemory, anOffset, aBuffer);
}

void* Renderer::MapDeviceMemory(DeviceMemory& aDeviceMemory, VkDeviceSize anOffset, VkDeviceSize aSize)
{
	return myVulkanDeviceWrapper.MapDeviceMemory(aDeviceMemory, anOffset, aSize);
}

void Renderer::UnmapDeviceMemory(DeviceMemory& aDeviceMemory)
{
	myVulkanDeviceWrapper.UnmapDeviceMemory(aDeviceMemory);
}

void Renderer::CreateDevice(const DisplayRenderer* someDisplays, uint32_t aDisplayCount)
{
	uint32_t physicalDeviceCount = 0u;
	myVulkanInstanceWrapper.EnumeratePhysicalDevices(physicalDeviceCount, nullptr);

	if (physicalDeviceCount == 0u)
		Debug::Breakpoint();

	PhysicalDevice* physicalDevices = static_cast<PhysicalDevice*>(DBZ_ALLOCATE_STACK_MEMORY(physicalDeviceCount * sizeof(PhysicalDevice)));
	myVulkanInstanceWrapper.EnumeratePhysicalDevices(physicalDeviceCount, physicalDevices);

	// Choose best physical device to create device with
	uint32_t bestPhysicalDeviceIndex = 0u;
	uint32_t queueFamilyIndex = 0u;
	uint32_t maxScore = 0u;
	for (uint32_t i = 0; i < physicalDeviceCount; ++i)
	{
		// Give a score to the physical device. (Somehow)
		PhysicalDevice device = physicalDevices[i];
		uint32_t score = 0u;
		uint32_t graphicsQueueFamilyIndex = 0u;

		// Device properties
		VkPhysicalDeviceProperties properties;
		myVulkanInstanceWrapper.GetPhysicalDeviceProperties(device, properties);
		score += properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? 100u : 0u;

		// Device features
		VkPhysicalDeviceFeatures features;
		myVulkanInstanceWrapper.GetPhysicalDeviceFeatures(device, features);

		// Device queue family properties
		uint32_t familyPropertyCount = 0u;
		myVulkanInstanceWrapper.GetPhysicalDeviceQueueFamilyProperties(device, familyPropertyCount, nullptr);

		if (familyPropertyCount == 0u)
			continue;

		VkQueueFamilyProperties* familyProperties = static_cast<VkQueueFamilyProperties*>(DBZ_ALLOCATE_STACK_MEMORY(familyPropertyCount * sizeof(VkQueueFamilyProperties)));
		myVulkanInstanceWrapper.GetPhysicalDeviceQueueFamilyProperties(device, familyPropertyCount, familyProperties);

		for (uint32_t j = 0; j < familyPropertyCount; ++j)
		{
			const VkQueueFamilyProperties& properties = familyProperties[j];

			// Check if the queue family has presentation support for the displays we want
			VkBool32 hasSupportForDisplays = VK_TRUE;
			for (uint32_t k = 0; k < aDisplayCount; ++k)
				hasSupportForDisplays |= static_cast<VkBool32>(myVulkanInstanceWrapper.GetPhysicalDeviceSurfaceSupportKHR(device, j, someDisplays[k].mySurface));

			// Check if it's a valid queue
			if (hasSupportForDisplays && properties.queueCount > 0 && properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				graphicsQueueFamilyIndex = j;
				break;
			}
		}

		// Device memory properties
		VkPhysicalDeviceMemoryProperties memoryProperties;
		myVulkanInstanceWrapper.GetPhysicalDeviceMemoryProperties(device, memoryProperties);

		if (maxScore < score)
		{
			maxScore = score;
			bestPhysicalDeviceIndex = i;
			queueFamilyIndex = graphicsQueueFamilyIndex;
		}
	}

	// Create device
	float queuePriority = 1.0f;
	VkDeviceQueueCreateInfo deviceQueueCreateInfo{
		VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		nullptr,
		0,
		queueFamilyIndex,
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
	myVulkanInstanceWrapper.EnumerateDeviceExtensionProperties(physicalDevices[bestPhysicalDeviceIndex], nullptr, physicalDeviceExtensionCount, nullptr);

	VkDeviceCreateInfo deviceCreateInfo{
		VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		nullptr,
		0,
		1,
		&deviceQueueCreateInfo,
		0,
		nullptr,
		deviceExtensionCount,
		deviceExtensions,
		nullptr
	};

	myVulkanInstanceWrapper.Create(physicalDevices[bestPhysicalDeviceIndex], deviceCreateInfo, myVulkanDeviceWrapper);
}

void Renderer::DestroyDevice()
{
	myVulkanInstanceWrapper.Destroy(myVulkanDeviceWrapper);
}

void Renderer::CreateDisplaySurface(const Window& aWindow, DisplayRenderer& aDisplayRendererOut) const
{
	myVulkanInstanceWrapper.Create(aWindow.GetWindowHandle(), aDisplayRendererOut.mySurface);
}

void Renderer::CreateDisplaySwapchain(uint32_t aWidth, uint32_t aHeight, uint32_t aDesiredImageCount, DisplayRenderer& aDisplayRendererOut) const
{
	aDisplayRendererOut.myDisplayImageIndex = 0u;
	aDisplayRendererOut.myOnFlightImageIndex = 0u;
	aDisplayRendererOut.myDisplayWidth = aWidth;
	aDisplayRendererOut.myDisplayHeight = aHeight;

	PhysicalDevice physicalDevice = myVulkanDeviceWrapper.GetPhysicalDevice();
	SurfaceKHR surface = aDisplayRendererOut.mySurface;

	// Choose format for the swapchain
	uint32_t surfaceFormatCount = 0u;
	myVulkanInstanceWrapper.GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, surfaceFormatCount, nullptr);

	if (surfaceFormatCount == 0)
		Debug::Breakpoint();

	VkSurfaceFormatKHR* surfaceFormats = static_cast<VkSurfaceFormatKHR*>(DBZ_ALLOCATE_STACK_MEMORY(sizeof(VkSurfaceFormatKHR) * surfaceFormatCount));
	myVulkanInstanceWrapper.GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, surfaceFormatCount, surfaceFormats);
	aDisplayRendererOut.mySwapchainFormat = surfaceFormats[0].format;

	// Get surface capabilities
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	myVulkanInstanceWrapper.GetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, surfaceCapabilities);

	// Check if current surface allows for the amount of images we want. If maxImageCount is 0, then there's unlimited (until we run out of memory)
	aDisplayRendererOut.myDisplayImageCount = ourMaxDisplayImagesPerDisplay < aDesiredImageCount ? ourMaxDisplayImagesPerDisplay : aDesiredImageCount;;
	if (surfaceCapabilities.maxImageCount)
	{
		// Clamp to allowed range
		aDisplayRendererOut.myDisplayImageCount = surfaceCapabilities.minImageCount < aDisplayRendererOut.myDisplayImageCount ? aDisplayRendererOut.myDisplayImageCount : surfaceCapabilities.minImageCount;
		aDisplayRendererOut.myDisplayImageCount = aDisplayRendererOut.myDisplayImageCount < surfaceCapabilities.maxImageCount ? aDisplayRendererOut.myDisplayImageCount : surfaceCapabilities.maxImageCount;
	}
	aDisplayRendererOut.myOnFlightImageCount = aDisplayRendererOut.myDisplayImageCount > 1 ? aDisplayRendererOut.myDisplayImageCount - 1 : 1;

	VkSwapchainCreateInfoKHR swapchainInfo{
		VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		nullptr,
		0,
		Unwrap(surface),
		aDisplayRendererOut.myDisplayImageCount,
		aDisplayRendererOut.mySwapchainFormat,
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
		Unwrap(aDisplayRendererOut.mySwapchain) // TODO: When creating the swapchain again due to resize, pass the previous swapchain here
	};

	SwapchainKHR newSwapchain;
	myVulkanDeviceWrapper.Create(swapchainInfo, newSwapchain);
	myVulkanDeviceWrapper.Destroy(aDisplayRendererOut.mySwapchain);
	aDisplayRendererOut.mySwapchain = newSwapchain;

	// Request number of images in swapchain so it does not complain later on when we use mySwapchainImageCount
	myVulkanDeviceWrapper.GetSwapchainImagesKHR(aDisplayRendererOut.mySwapchain, aDisplayRendererOut.myDisplayImageCount, nullptr);
}

void Renderer::CreateDisplayRenderPass(const VkRenderPassCreateInfo& aRenderPassCreateInfo, DisplayRenderer& aDisplayRendererOut) const
{
	// TODO: Check that the renderpass fulfill a bunch of settings we need to enforce
	// Force format to only attachment (back buffer)
	const_cast<VkAttachmentDescription*>(aRenderPassCreateInfo.pAttachments)->format = aDisplayRendererOut.mySwapchainFormat;
	myVulkanDeviceWrapper.Create(aRenderPassCreateInfo, aDisplayRendererOut.myDisplayRenderPass);
}

void Renderer::CreateDisplayImageViews(DisplayRenderer& aDisplayRendererOut) const
{
	uint32_t swapchainImageCount = aDisplayRendererOut.myDisplayImageCount;
	Image* swapchainImages = static_cast<Image*>(DBZ_ALLOCATE_STACK_MEMORY(sizeof(Image) * swapchainImageCount));
	myVulkanDeviceWrapper.GetSwapchainImagesKHR(aDisplayRendererOut.mySwapchain, swapchainImageCount, swapchainImages);

	for (uint32_t i = 0; i < swapchainImageCount; ++i)
	{
		VkImageViewCreateInfo imageViewCreateInfo =
		{
			VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			nullptr,
			0,
			Unwrap(swapchainImages[i]),
			VK_IMAGE_VIEW_TYPE_2D,
			aDisplayRendererOut.mySwapchainFormat,
			{
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY,
			},
			{
				VK_IMAGE_ASPECT_COLOR_BIT,
				0,
				1,
				0,
				1
			}
		};

		myVulkanDeviceWrapper.Create(imageViewCreateInfo, aDisplayRendererOut.myDisplayImageViews[i]);
	}
}

void Renderer::CreateDisplayFramebuffers(DisplayRenderer& aDisplayRendererOut) const
{
	VkFramebufferCreateInfo framebufferCreateInfo
	{
		VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		nullptr,
		0,
		Unwrap(aDisplayRendererOut.myDisplayRenderPass),
		1,
		nullptr, // Filled in loop for each framebuffer
		aDisplayRendererOut.myDisplayWidth,
		aDisplayRendererOut.myDisplayHeight,
		1
	};

	uint32_t swapchainImageCount = aDisplayRendererOut.myDisplayImageCount;
	for (uint32_t i = 0; i < swapchainImageCount; ++i)
	{
		framebufferCreateInfo.pAttachments = Unwrap(&aDisplayRendererOut.myDisplayImageViews[i]);
		myVulkanDeviceWrapper.Create(framebufferCreateInfo, aDisplayRendererOut.myDisplayFramebuffers[i]);
	}
}

void Renderer::DestroyDisplaySwapchainResources(DisplayRenderer& aDisplayRendererOut) const
{
	uint32_t swapchainImageCount = aDisplayRendererOut.myDisplayImageCount;
	for (uint32_t i = 0; i < swapchainImageCount; ++i)
	{
		myVulkanDeviceWrapper.Destroy(aDisplayRendererOut.myDisplayFramebuffers[i]);
		myVulkanDeviceWrapper.Destroy(aDisplayRendererOut.myDisplayImageViews[i]);
	}

	myVulkanDeviceWrapper.Destroy(aDisplayRendererOut.mySwapchain);
}

}
