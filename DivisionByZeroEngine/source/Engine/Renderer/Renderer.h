#pragma once

#include "VulkanWrapper/VulkanWrapper.h"

class Window;

namespace DBZ
{

// TODO: For now this Renderer will only allows to bind the back framebuffer to render to it with no depth
class Renderer
{
public:
	static void Create(Window& aWindow, uint32_t aDesiredBackFramebufferImages, Renderer& aRendererOut);
	static void Destroy(Renderer& aRendererOut);

	// Returns frame index
	uint32_t BeginFrame();
	void BindDisplay(CommandBuffer& aCommandBuffer, uint32_t aFrameIndex, bool aShouldClear, float aRed = 0.0f, float aGreen = 0.0f, float aBlue = 0.0f, float anAlpha = 0.0f);
	void EndDisplayRender(Semaphore* someWaitSemaphores, uint32_t aWaitSemaphoreCount, CommandBuffer* someCommandBuffers, uint32_t aCommandBufferCount, Semaphore* someSignalSemaphores, uint32_t aSignalSemaphoreCount);
	void EndFrame(uint32_t aFrameIndex);

	void ResizeDisplayImage(uint32_t aWidth, uint32_t aHeight);

	VulkanWrapper& GetVulkanWrapper() { return myVulkanWrapper; }
	uint32_t GetDisplayImageCount() { return myDisplayImageCount; }
	uint32_t GetMaxOnFlightImageCount() { return myMaxOnFlightImageCount; }

	// TODO: we may not want to provide this to the user
	RenderPass GetRenderPass() { return myDisplayRenderPass; }
	Fence& GetFence(uint32_t anIndex) { return myOnFlightFences[anIndex]; }
	Semaphore& GetSemaphore(uint32_t anIndex) { return myOnFlightImageSemaphores[anIndex]; }
	Framebuffer& GetFramebuffer(uint32_t anIndex) { return myDisplayFramebuffers[anIndex]; }

	// The engine will only support 3 maximum back frame buffers and no more
	constexpr static uint32_t ourMaxDisplayImageCount = 3u;
	constexpr static uint32_t ourMaxOnFlightImageCount = ourMaxDisplayImageCount - 1u;

private:
	uint32_t myDisplayImageCount;
	uint32_t myMaxOnFlightImageCount;
	uint32_t myOnFlightImageIndex;
	uint32_t myDisplayWidth;
	uint32_t myDisplayHeight;
	ImageView myDisplayImageViews[ourMaxDisplayImageCount];
	RenderPass myDisplayRenderPass;
	Framebuffer myDisplayFramebuffers[ourMaxDisplayImageCount];
	Semaphore myOnFlightImageSemaphores[ourMaxOnFlightImageCount];
	Fence myOnFlightFences[ourMaxOnFlightImageCount];
	VulkanWrapper myVulkanWrapper;
};

} // namespace DBZ
