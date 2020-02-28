#include "DisplayRenderer.h"

#include "Window/Window.h"

namespace DBZ
{

void DisplayRenderer::Create(Window& aWindow, uint32_t aDesiredBackFramebufferImages, DisplayRenderer& aDisplayRenderer)
{
	aDesiredBackFramebufferImages = ourMaxDisplayImageCount < aDesiredBackFramebufferImages ? ourMaxDisplayImageCount : aDesiredBackFramebufferImages;
	VulkanWrapper::Create(aWindow.GetWindowHandle(), aDesiredBackFramebufferImages, aDisplayRenderer.myVulkanWrapper);

	aDisplayRenderer.myDisplayImageCount = aDisplayRenderer.myVulkanWrapper.GetSwapchainImageCount();
	aDisplayRenderer.myOnFlightImageCount = aDisplayRenderer.myDisplayImageCount > 1 ? aDisplayRenderer.myDisplayImageCount - 1 : 1;
	aDisplayRenderer.myDisplayImageIndex = 0u;
	aDisplayRenderer.myOnFlightImageIndex = 0u;
	aDisplayRenderer.myDisplayWidth = aWindow.GetClientWidth();
	aDisplayRenderer.myDisplayHeight = aWindow.GetClientHeight();

	// TODO: For now we can only draw to back frame buffer to draw directly to it with no depth testing

	// Create render pass
	VkFormat swapchainFormat = aDisplayRenderer.myVulkanWrapper.GetSwapchainFormat();
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

	aDisplayRenderer.myVulkanWrapper.Create(renderPassCreateInfo, aDisplayRenderer.myDisplayRenderPass);

	// Create ImageViews for later generation of the back framebuffers
	std::vector<Image> swapchainImages(aDisplayRenderer.myDisplayImageCount);
	aDisplayRenderer.myVulkanWrapper.GetSwapchainImages(swapchainImages.data());
	for (uint32_t i = 0; i < aDisplayRenderer.myDisplayImageCount; ++i)
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

		aDisplayRenderer.myVulkanWrapper.Create(imageViewCreateInfo, aDisplayRenderer.myDisplayImageViews[i]);
	}

	// Create framebuffers for the render pass
	VkFramebufferCreateInfo framebufferCreateInfo
	{
		VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		nullptr,
		0,
		Unwrap(aDisplayRenderer.myDisplayRenderPass),
		1,
		nullptr, // Filled in loop for each framebuffer
		aDisplayRenderer.myDisplayWidth,
		aDisplayRenderer.myDisplayHeight,
		1
	};

	for (uint32_t i = 0; i < aDisplayRenderer.myDisplayImageCount; ++i)
	{
		framebufferCreateInfo.pAttachments = Unwrap(&aDisplayRenderer.myDisplayImageViews[i]);
		aDisplayRenderer.myVulkanWrapper.Create(framebufferCreateInfo, aDisplayRenderer.myDisplayFramebuffers[i]);
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

	for (uint32_t i = 0; i < aDisplayRenderer.myOnFlightImageCount; ++i)
	{
		aDisplayRenderer.myVulkanWrapper.Create(semaphoreCreateInfo, aDisplayRenderer.myOnFlightImageSemaphores[i]);
		aDisplayRenderer.myVulkanWrapper.Create(fenceCreateInfo, aDisplayRenderer.myOnFlightFences[i]);
	}
}

void DisplayRenderer::Destroy(DisplayRenderer& aDisplayRenderer)
{
	// Destroy all resources

	for (uint32_t i = 0; i < aDisplayRenderer.myOnFlightImageCount; ++i)
	{
		aDisplayRenderer.myVulkanWrapper.Destroy(aDisplayRenderer.myOnFlightFences[i]);
		aDisplayRenderer.myVulkanWrapper.Destroy(aDisplayRenderer.myOnFlightImageSemaphores[i]);
	}
	aDisplayRenderer.myVulkanWrapper.Destroy(aDisplayRenderer.myDisplayRenderPass);
	for (uint32_t i = 0; i < aDisplayRenderer.myDisplayImageCount; ++i)
	{
		aDisplayRenderer.myVulkanWrapper.Destroy(aDisplayRenderer.myDisplayFramebuffers[i]);
		aDisplayRenderer.myVulkanWrapper.Destroy(aDisplayRenderer.myDisplayImageViews[i]);
	}

	VulkanWrapper::Destroy(aDisplayRenderer.myVulkanWrapper);

#if IS_DEBUG_BUILD
	memset(&aDisplayRenderer, 0, sizeof(aDisplayRenderer));
#endif
}

void DisplayRenderer::BeginFrame()
{
	// Throttle GPU if needed so we do not waste more power than needed for display frame rate
	Fence& fence = myOnFlightFences[myOnFlightImageIndex];
	myVulkanWrapper.WaitForFences(&fence, 1);
	myVulkanWrapper.ResetFences(&fence, 1);

	myVulkanWrapper.AcquireNextImage(myOnFlightImageSemaphores[myOnFlightImageIndex]);
}

void DisplayRenderer::BindDisplay(CommandBuffer& aCommandBuffer, bool aShouldClear, float aRed, float aGreen, float aBlue, float anAlpha)
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
		Unwrap(myDisplayFramebuffers[myDisplayImageIndex]),
		{ 0u, 0u, myDisplayWidth, myDisplayHeight },
		static_cast<uint32_t>(aShouldClear),
		&clearValue
	};
	myVulkanWrapper.BeginRenderPass(aCommandBuffer, renderPassBeginInfo);
}

void DisplayRenderer::UnbindDisplay(CommandBuffer& aCommandBuffer)
{
	myVulkanWrapper.EndRenderPass(aCommandBuffer);
}

void DisplayRenderer::Submit(Semaphore* someWaitSemaphores, uint32_t aWaitSemaphoreCount, CommandBuffer* someCommandBuffers, uint32_t aCommandBufferCount, Semaphore* someSignalSemaphores, uint32_t aSignalSemaphoreCount)
{

}

void DisplayRenderer::EndFrame()
{
	myVulkanWrapper.Present(nullptr, 0, myDisplayImageIndex);
	myOnFlightImageIndex = (myOnFlightImageIndex + 1) % myOnFlightImageCount;
	myDisplayImageIndex = (myDisplayImageIndex + 1) % myDisplayImageCount;
}

void DisplayRenderer::ResizeDisplayImage(uint32_t aWidth, uint32_t aHeight)
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
