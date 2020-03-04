#pragma once

#include "Renderer.h"

namespace DBZ
{

// TODO: For now this Renderer will only allows to bind the back framebuffer to render to it with no depth
class DisplayRenderer
{
public:

	// TODO: we may not want to provide this to the user
	RenderPass GetRenderPass() { return myDisplayRenderPass; }
	uint32_t GetFrameIndex() const { return myOnFlightImageIndex; }
	uint32_t GetOnFlightImageCount() const { return myOnFlightImageCount; }
	Fence& GetFrameFence() { return myOnFlightFences[myOnFlightImageIndex]; }
	Semaphore& GetFrameSemaphore() { return myDisplayImageAcquireSemaphores[myOnFlightImageIndex]; }
	Framebuffer& GetFramebuffer() { return myDisplayFramebuffers[myDisplayImageIndex]; }
	uint32_t GetWidth() const { return myDisplayWidth; }
	uint32_t GetHeight() const { return myDisplayHeight; }

private:
	friend class Renderer;

	uint32_t myDisplayImageCount;
	uint32_t myOnFlightImageCount;
	uint32_t myDisplayImageIndex;
	uint32_t myOnFlightImageIndex;
	uint32_t myDisplayWidth;
	uint32_t myDisplayHeight;
	VkFormat mySwapchainFormat;
	SurfaceKHR mySurface;
	SwapchainKHR mySwapchain;
	ImageView myDisplayImageViews[Renderer::ourMaxDisplayImagesPerDisplay];
	RenderPass myDisplayRenderPass;
	Framebuffer myDisplayFramebuffers[Renderer::ourMaxDisplayImagesPerDisplay];
	Semaphore myDisplayImageAcquireSemaphores[Renderer::ourMaxOnFlightImagesPerDisplay];
	Fence myOnFlightFences[Renderer::ourMaxOnFlightImagesPerDisplay];
};

} // namespace DBZ
