#include "Renderer.h"

#include "Window/Window.h"

namespace DBZ
{

void Renderer::Create(Window& aWindow, uint32_t aDesiredBackFramebufferImages, Renderer& aRenderer)
{
	aDesiredBackFramebufferImages = ourMaxDisplayImageCount < aDesiredBackFramebufferImages ? ourMaxDisplayImageCount : aDesiredBackFramebufferImages;
	VulkanWrapper::Create(aWindow.GetWindowHandle(), aDesiredBackFramebufferImages, aRenderer.myVulkanWrapper);

	aRenderer.myDisplayImageCount = aRenderer.myVulkanWrapper.GetSwapchainImageCount();
	aRenderer.myMaxOnFlightImageCount = aRenderer.myDisplayImageCount > 1 ? aRenderer.myDisplayImageCount - 1 : 1;
	aRenderer.myOnFlightImageIndex = 0u;
	aRenderer.myDisplayWidth = aWindow.GetClientWidth();
	aRenderer.myDisplayHeight = aWindow.GetClientHeight();

	// TODO: For now we can only draw to back frame buffer to draw directly to it with no depth testing

	// Create render pass
	VkFormat swapchainFormat = aRenderer.myVulkanWrapper.GetSwapchainFormat();
	VkAttachmentDescription attachmentDescription
	{
		0,
		swapchainFormat,
		VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_CLEAR,
		VK_ATTACHMENT_STORE_OP_STORE,
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		VK_ATTACHMENT_STORE_OP_DONT_CARE,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
	};

	VkAttachmentReference colorAttachmentReference
	{
		0,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};

	VkSubpassDescription subpassDescription
	{
		0,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		0,
		nullptr,
		1,
		&colorAttachmentReference,
		nullptr,
		nullptr,
		0,
		nullptr
	};

	VkSubpassDependency attachmentDependency
	{
		VK_SUBPASS_EXTERNAL,
		0,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		0,
		VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
		0
	};

	VkRenderPassCreateInfo renderPassCreateInfo
	{
		VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		nullptr,
		0,
		1,
		&attachmentDescription,
		1,
		&subpassDescription,
		1,
		&attachmentDependency
	};

	aRenderer.myVulkanWrapper.Create(renderPassCreateInfo, aRenderer.myDisplayRenderPass);

	// Create ImageViews for later generation of the back framebuffers
	std::vector<Image> swapchainImages(aRenderer.myDisplayImageCount);
	aRenderer.myVulkanWrapper.GetSwapchainImages(swapchainImages.data());
	for (uint32_t i = 0; i < aRenderer.myDisplayImageCount; ++i)
	{
		VkImageViewCreateInfo imageViewCreateInfo =
		{
			VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			nullptr,
			0,
			Unwrap(swapchainImages[i]),
			VK_IMAGE_VIEW_TYPE_2D,
			swapchainFormat,
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

		aRenderer.myVulkanWrapper.Create(imageViewCreateInfo, aRenderer.myDisplayImageViews[i]);
	}

	// Create framebuffers for the render pass
	VkFramebufferCreateInfo framebufferCreateInfo
	{
		VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		nullptr,
		0,
		Unwrap(aRenderer.myDisplayRenderPass),
		1,
		nullptr, // Filled in loop for each framebuffer
		aRenderer.myDisplayWidth,
		aRenderer.myDisplayHeight,
		1
	};

	for (uint32_t i = 0; i < aRenderer.myDisplayImageCount; ++i)
	{
		framebufferCreateInfo.pAttachments = Unwrap(&aRenderer.myDisplayImageViews[i]);
		aRenderer.myVulkanWrapper.Create(framebufferCreateInfo, aRenderer.myDisplayFramebuffers[i]);
	}

	// Create acquire semaphores
	// Create fences for synchronization and throttle if needed
	VkSemaphoreCreateInfo semaphoreCreateInfo
	{
		VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		nullptr,
		0
	};

	VkFenceCreateInfo fenceCreateInfo
	{
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		nullptr,
		VK_FENCE_CREATE_SIGNALED_BIT
	};

	for (uint32_t i = 0; i < aRenderer.myMaxOnFlightImageCount; ++i)
	{
		aRenderer.myVulkanWrapper.Create(semaphoreCreateInfo, aRenderer.myOnFlightImageSemaphores[i]);
		aRenderer.myVulkanWrapper.Create(fenceCreateInfo, aRenderer.myOnFlightFences[i]);
	}
}

void Renderer::Destroy(Renderer& aRenderer)
{
	// Destroy all resources

	for (uint32_t i = 0; i < aRenderer.myMaxOnFlightImageCount; ++i)
	{
		aRenderer.myVulkanWrapper.Destroy(aRenderer.myOnFlightFences[i]);
		aRenderer.myVulkanWrapper.Destroy(aRenderer.myOnFlightImageSemaphores[i]);
	}
	aRenderer.myVulkanWrapper.Destroy(aRenderer.myDisplayRenderPass);
	for (uint32_t i = 0; i < aRenderer.myDisplayImageCount; ++i)
	{
		aRenderer.myVulkanWrapper.Destroy(aRenderer.myDisplayFramebuffers[i]);
		aRenderer.myVulkanWrapper.Destroy(aRenderer.myDisplayImageViews[i]);
	}

	VulkanWrapper::Destroy(aRenderer.myVulkanWrapper);

#if IS_DEBUG_BUILD
	memset(&aRenderer, 0, sizeof(aRenderer));
#endif
}

uint32_t Renderer::BeginFrame()
{
	// Throttle GPU if needed so we do not waste more power than needed for display frame rate
	Fence& fence = myOnFlightFences[myOnFlightImageIndex];
	myVulkanWrapper.WaitForFences(&fence, 1);
	myVulkanWrapper.ResetFences(&fence, 1);

	return myVulkanWrapper.AcquireNextImage(myOnFlightImageSemaphores[myOnFlightImageIndex]);
}

void Renderer::BindDisplay(CommandBuffer& aCommandBuffer, uint32_t aFrameIndex, bool aShouldClear, float aRed, float aGreen, float aBlue, float anAlpha)
{
	VkClearValue clearValue;
	clearValue.color.float32[0] = aRed;
	clearValue.color.float32[1] = aGreen;
	clearValue.color.float32[2] = aBlue;
	clearValue.color.float32[3] = anAlpha;
	clearValue.depthStencil.depth = 0.0f;
	clearValue.depthStencil.stencil = 0u;

	VkRenderPassBeginInfo renderPassBeginInfo
	{
		VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		nullptr,
		Unwrap(myDisplayRenderPass),
		Unwrap(myDisplayFramebuffers[aFrameIndex]),
		{ 0u, 0u, myDisplayWidth, myDisplayHeight },
		static_cast<uint32_t>(aShouldClear),
		&clearValue
	};
	myVulkanWrapper.BeginRenderPass(aCommandBuffer, renderPassBeginInfo);
}

void Renderer::EndDisplayRender(Semaphore* someWaitSemaphores, uint32_t aWaitSemaphoreCount, CommandBuffer* someCommandBuffers, uint32_t aCommandBufferCount, Semaphore* someSignalSemaphores, uint32_t aSignalSemaphoreCount)
{

}

void Renderer::EndFrame(uint32_t aFrameIndex)
{
	myVulkanWrapper.Present(nullptr, 0, aFrameIndex);
	myOnFlightImageIndex = myMaxOnFlightImageCount == 2u ? myOnFlightImageIndex ^ 1u : 0u;
}

void Renderer::ResizeDisplayImage(uint32_t aWidth, uint32_t aHeight)
{
	myDisplayWidth = aWidth;
	myDisplayHeight = aHeight;

	// Wait for device and destroy swapchain specific resources
	myVulkanWrapper.WaitForDevice();
	for (uint32_t i = 0; i < myDisplayImageCount; ++i)
	{
		myVulkanWrapper.Destroy(myDisplayFramebuffers[i]);
		myVulkanWrapper.Destroy(myDisplayImageViews[i]);
	}
	myVulkanWrapper.ResizeSwapchain(myDisplayWidth, myDisplayHeight);

	// Create resources again
	// Get images from the swapchain
	myDisplayImageCount = myVulkanWrapper.GetSwapchainImageCount();
	std::vector<Image> swapchainImages(myDisplayImageCount);
	myVulkanWrapper.GetSwapchainImages(swapchainImages.data());

	// Create ImageViews for later generation of the back framebuffers
	VkFormat swapchainFormat = myVulkanWrapper.GetSwapchainFormat();
	for (uint32_t i = 0; i < myDisplayImageCount; ++i)
	{
		VkImageViewCreateInfo imageViewCreateInfo =
		{
			VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			nullptr,
			0,
			Unwrap(swapchainImages[i]),
			VK_IMAGE_VIEW_TYPE_2D,
			swapchainFormat,
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

		myVulkanWrapper.Create(imageViewCreateInfo, myDisplayImageViews[i]);
	}


	// Create framebuffers for the render pass
	VkFramebufferCreateInfo framebufferCreateInfo
	{
		VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		nullptr,
		0,
		Unwrap(myDisplayRenderPass),
		1,
		nullptr, // Filled in loop for each framebuffer
		myDisplayWidth,
		myDisplayHeight,
		1
	};

	for (uint32_t i = 0; i < myDisplayImageCount; ++i)
	{
		framebufferCreateInfo.pAttachments = Unwrap(&myDisplayImageViews[i]);
		myVulkanWrapper.Create(framebufferCreateInfo, myDisplayFramebuffers[i]);
	}
}

} // namespace DBZ
