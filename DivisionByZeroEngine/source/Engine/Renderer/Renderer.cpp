#include "Renderer.h"

void Renderer::Create(Renderer& aRendererOut)
{
	VulkanInstanceWrapper::Create(aRendererOut.myVulkanInstanceWrapper);
	VulkanDeviceWrapper::Create(aRendererOut.myVulkanDeviceWrapper);
}

void Renderer::Destroy(Renderer& aRenderer)
{
	VulkanDeviceWrapper::Destroy(myVulkanDeviceWrapper);
	VulkanInstanceWrapper::Destroy(myVulkanInstanceWrapper);
}
