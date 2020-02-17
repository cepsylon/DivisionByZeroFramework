#pragma once

#ifndef VK_NO_PROTOTYPES
#define VK_NO_PROTOTYPES
#endif // VK_NO_PROTOTYPES
#include <vulkan/vulkan_core.h>

#ifndef WRAP_VULKAN_RESOURCE
#define WRAP_VULKAN_RESOURCE(name) class name;\
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
