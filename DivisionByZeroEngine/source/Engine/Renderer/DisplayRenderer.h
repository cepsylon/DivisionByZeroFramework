#pragma once

#include "VulkanWrapper/VulkanWrapper.h"

class Window;

namespace DBZ
{

// TODO: For now this Renderer will only allows to bind the back framebuffer to render to it with no depth
class DisplayRenderer
{
public:
	static void Create(Window& aWindow, uint32_t aDesiredBackFramebufferImages, DisplayRenderer& aDisplayRendererOut);
	static void Destroy(DisplayRenderer& aDisplayRendererOut);

	// Returns frame index
	void BeginFrame();
	void BindDisplay(CommandBuffer& aCommandBuffer, bool aShouldClear, float aRed = 0.0f, float aGreen = 0.0f, float aBlue = 0.0f, float anAlpha = 0.0f);
	void UnbindDisplay(CommandBuffer& aCommandBuffer);
	void Submit(Semaphore* someWaitSemaphores, uint32_t aWaitSemaphoreCount, CommandBuffer* someCommandBuffers, uint32_t aCommandBufferCount, Semaphore* someSignalSemaphores, uint32_t aSignalSemaphoreCount);
	void EndFrame();

	void ResizeDisplayImage(uint32_t aWidth, uint32_t aHeight);

	VulkanWrapper& GetVulkanWrapper() { return myVulkanWrapper; }

	// TODO: we may not want to provide this to the user
	RenderPass GetRenderPass() { return myDisplayRenderPass; }
	uint32_t GetFrameIndex() const { return myOnFlightImageIndex; }
	uint32_t GetOnFlightImageCount() const { return myOnFlightImageCount; }
	Fence& GetFrameFence() { return myOnFlightFences[myOnFlightImageIndex]; }
	Semaphore& GetFrameSemaphore() { return myOnFlightImageSemaphores[myOnFlightImageIndex]; }

	// The engine will only support 3 maximum back frame buffers and no more
	constexpr static uint32_t ourMaxDisplayImageCount = 3u;
	constexpr static uint32_t ourMaxOnFlightImageCount = ourMaxDisplayImageCount - 1u;

private:
	uint32_t myDisplayImageCount;
	uint32_t myOnFlightImageCount;
	uint32_t myDisplayImageIndex;
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
