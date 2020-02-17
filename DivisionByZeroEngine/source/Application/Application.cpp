#include "Application.h"

#include "Math/Vector4.h"
#include "Math/Matrix44.h"
#include "Math/MathCommon.h"

#include "Window/WindowClass.h"
#include "Window/Window.h"

#include "Renderer/VulkanWrapper.h"

#include "Utils/Debug.h"

#include <fstream>
#include <vector>

namespace Gfx
{
	constexpr uint32_t ImageCount = 3u;
	constexpr uint32_t MaxConcurrentImages = ImageCount - 1u;

	uint32_t currentFrame = 0u;
	uint32_t checkIndex = 0u;
	uint32_t frameIndex = 0u;

	ImageView locSwapchainImageViews[ImageCount];
	RenderPass locRenderPass;
	Framebuffer locFramebuffers[ImageCount];
	CommandPool locCommandPool;
	CommandBuffer locCommandBuffers[ImageCount];
	Semaphore locAcquireSemaphores[MaxConcurrentImages];
	Fence locFences[MaxConcurrentImages];
	ShaderModule locVertexShader;
	ShaderModule locFragmentShader;
	DescriptorSetLayout locDescriptorSetLayout;
	PipelineLayout locPipelineLayout;
	Pipeline locGraphicsPipeline;

	// Vertex buffer and associated memory
	Buffer locBuffer;
	DeviceMemory locDeviceMemory;
	Buffer locUniformBuffer[MaxConcurrentImages];
	DeviceMemory locUniformDeviceMemory;
	VkDeviceSize locUniformBufferOffset = 0u;

	// Descriptors
	DescriptorPool locDescriptorPool;
	DescriptorSet locDescriptorSet[MaxConcurrentImages];
}

void Application::Initialize()
{
	WindowClass::Create("MyFramework", nullptr, myWindowClass);
	Window::Create(myWindowClass, 0, 0, 1280u, 720u, "DivisionByZeroEngine", myMainWindow);
	myMainWindow.GetWindowResizeCallbacks().Add(*this, &Application::WindowResize);
	myMainWindow.GetWindowPaintCallbacks().Add(*this, &Application::WindowPaint);

	// Create Renderer for initialization of Instace, Surface, Device and Swapchain
	myRenderer = Renderer::Create(myMainWindow.GetWindowHandle(), Gfx::ImageCount);

	// Get images from the swapchain
	uint32_t swapchainImageCount = myRenderer.GetSwapchainImageCount();
	std::vector<Image> swapchainImages(swapchainImageCount);
	myRenderer.GetSwapchainImages(swapchainImages.data());
	
	// Create ImageViews for later generation of the back framebuffers
	VkFormat swapchainFormat = myRenderer.GetSwapchainFormat();
	for (uint32_t i = 0; i < swapchainImageCount; ++i)
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

		myRenderer.Create(imageViewCreateInfo, Gfx::locSwapchainImageViews[i]);
	}
	
	// Create render pass
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
	
	myRenderer.Create(renderPassCreateInfo, Gfx::locRenderPass);
	
	// Create framebuffers for the render pass
	VkFramebufferCreateInfo framebufferCreateInfo
	{
		VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		nullptr,
		0,
		Unwrap(Gfx::locRenderPass),
		1,
		nullptr, // Filled in loop for each framebuffer
		1280u,
		720u,
		1
	};
	
	for (uint32_t i = 0; i < swapchainImageCount; ++i)
	{
		framebufferCreateInfo.pAttachments = Unwrap(&Gfx::locSwapchainImageViews[i]);
		myRenderer.Create(framebufferCreateInfo, Gfx::locFramebuffers[i]);
	}
	
	// Create command pool
	VkCommandPoolCreateInfo commandPoolCreateInfo
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		nullptr,
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		myRenderer.GetGraphicsQueueFamilyIndex()
	};
	
	myRenderer.Create(commandPoolCreateInfo, Gfx::locCommandPool);
	
	// Create command buffers
	VkCommandBufferAllocateInfo commandBufferCreateInfo
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		nullptr,
		Unwrap(Gfx::locCommandPool),
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		Gfx::ImageCount
	};
	
	myRenderer.Create(commandBufferCreateInfo, Gfx::locCommandBuffers);
	
	// Create acquire semaphores
	VkSemaphoreCreateInfo semaphoreCreateInfo
	{
		VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		nullptr,
		0
	};
	
	for (uint32_t i = 0; i < Gfx::MaxConcurrentImages; ++i)
		myRenderer.Create(semaphoreCreateInfo, Gfx::locAcquireSemaphores[i]);
	
	// Create fences for synchronization and throttle if needed
	VkFenceCreateInfo fenceCreateInfo
	{
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		nullptr,
		VK_FENCE_CREATE_SIGNALED_BIT
	};
	
	for (uint32_t i = 0; i < Gfx::MaxConcurrentImages; ++i)
		myRenderer.Create(fenceCreateInfo, Gfx::locFences[i]);
	
	// Create shaders
	std::ifstream file{ "basic_vert.spv", std::ios::binary | std::ios::ate };
	if (file.is_open() == false)
		Debug::Breakpoint();

	std::vector<char> fileData(static_cast<uint32_t>(file.tellg()));
	file.seekg(0);
	file.read(fileData.data(), fileData.size());
	
	VkShaderModuleCreateInfo shaderCreateInfo
	{
		VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		nullptr,
		0,
		fileData.size(),
		reinterpret_cast<uint32_t*>(fileData.data())
	};
	
	myRenderer.Create(shaderCreateInfo, Gfx::locVertexShader);

	file = std::ifstream{ "basic_frag.spv", std::ios::binary | std::ios::ate };
	if (file.is_open() == false)
		Debug::Breakpoint();

	fileData.resize(static_cast<uint32_t>(file.tellg()));
	file.seekg(0);
	file.read(fileData.data(), fileData.size());
	shaderCreateInfo.codeSize = fileData.size();
	shaderCreateInfo.pCode = reinterpret_cast<uint32_t*>(fileData.data());

	myRenderer.Create(shaderCreateInfo, Gfx::locFragmentShader);

	// Create descriptors for pipeline layout
	VkDescriptorSetLayoutBinding descriptorSetLayoutBinding
	{
		0,
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		1,
		VK_SHADER_STAGE_VERTEX_BIT
	};
	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo
	{
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		nullptr,
		0,
		1,
		&descriptorSetLayoutBinding
	};
	myRenderer.Create(descriptorSetLayoutCreateInfo, Gfx::locDescriptorSetLayout);

	// Create pipeline layout
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		nullptr,
		0,
		1,
		Unwrap(&Gfx::locDescriptorSetLayout),
		0,
		nullptr
	};
	myRenderer.Create(pipelineLayoutCreateInfo, Gfx::locPipelineLayout);

	// Create pipeline
	VkPipelineShaderStageCreateInfo pipelineShaderStageInfo[2] =
	{
		// Vertex
		{
			VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			nullptr,
			0,
			VK_SHADER_STAGE_VERTEX_BIT,
			Unwrap(Gfx::locVertexShader),
			"main",
			nullptr
		},
		// Fragment
		{
			VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			nullptr,
			0,
			VK_SHADER_STAGE_FRAGMENT_BIT,
			Unwrap(Gfx::locFragmentShader),
			"main",
			nullptr
		}
	};
	
	VkVertexInputBindingDescription vertexInputBindingDescriptions[] =
	{
		{
			0,
			sizeof(float) * 7,
			VK_VERTEX_INPUT_RATE_VERTEX
		},
	};

	VkVertexInputAttributeDescription vertexInputAttributeDescriptions[] =
	{
		{
			0,
			0,
			VK_FORMAT_R32G32B32_SFLOAT,
			0
		},
		{
			1,
			0,
			VK_FORMAT_R32G32B32A32_SFLOAT,
			sizeof(float) * 3
		},
	};

	VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		nullptr,
		0,
		sizeof(vertexInputBindingDescriptions) / sizeof(VkVertexInputBindingDescription),
		vertexInputBindingDescriptions,
		sizeof(vertexInputAttributeDescriptions) / sizeof(VkVertexInputAttributeDescription),
		vertexInputAttributeDescriptions
	};
	
	VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		nullptr,
		0,
		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
		VK_FALSE
	};
	
	VkViewport viewport
	{
		0.0f,
		0.0f,
		1280.0f,
		720.0f,
		0.0f,
		1.0f
	};
	VkRect2D scissorRect
	{
		{ 0, 0 },
		{ 1280, 720 }
	};
	VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		nullptr,
		0,
		1,
		&viewport,
		1,
		&scissorRect
	};
	
	// Disable primitive clipping
	VkPipelineRasterizationDepthClipStateCreateInfoEXT pipelineRasterizationDepthClipStateCreateInfoEXT
	{
		VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_DEPTH_CLIP_STATE_CREATE_INFO_EXT,
		nullptr,
		0,
		VK_FALSE
	};

	VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		&pipelineRasterizationDepthClipStateCreateInfoEXT,
		0,
		VK_FALSE,
		VK_FALSE,
		VK_POLYGON_MODE_FILL,
		VK_CULL_MODE_BACK_BIT,
		VK_FRONT_FACE_COUNTER_CLOCKWISE,
		VK_FALSE,
		0.0f,
		0.0f,
		0.0f,
		1.0f
	};
	
	VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateInfo
	{
		
	};
	memset(&pipelineMultisampleStateInfo, 0, sizeof(pipelineMultisampleStateInfo));
	pipelineMultisampleStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	pipelineMultisampleStateInfo.pSampleMask = NULL;
	pipelineMultisampleStateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	
	VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		nullptr,
		0,
		VK_FALSE,
		VK_FALSE,
		VK_COMPARE_OP_ALWAYS,
		VK_FALSE,
		VK_FALSE,
		// 4 more values that we do not care for now
	};
	
	VkPipelineColorBlendAttachmentState blending
	{
		VK_FALSE
	};
	memset(&blending, 0, sizeof(blending));
	blending.colorWriteMask = 0xf;
	blending.blendEnable = VK_FALSE;
	VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		nullptr,
		0,
		VK_FALSE,
		VK_LOGIC_OP_NO_OP,
		1,
		&blending
	};
	
	VkDynamicState dynamicStateEnables[VK_DYNAMIC_STATE_RANGE_SIZE];
	VkPipelineDynamicStateCreateInfo pipelineDynamicStateInfo;
	memset(dynamicStateEnables, 0, sizeof dynamicStateEnables);
	memset(&pipelineDynamicStateInfo, 0, sizeof pipelineDynamicStateInfo);
	pipelineDynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	pipelineDynamicStateInfo.pDynamicStates = dynamicStateEnables;
	pipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	pipelineViewportStateCreateInfo.viewportCount = 1;
	dynamicStateEnables[pipelineDynamicStateInfo.dynamicStateCount++] = VK_DYNAMIC_STATE_VIEWPORT;
	pipelineViewportStateCreateInfo.scissorCount = 1;
	dynamicStateEnables[pipelineDynamicStateInfo.dynamicStateCount++] = VK_DYNAMIC_STATE_SCISSOR;
	
	
	VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo
	{
		VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		nullptr,
		VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT, // TODO: Check this more in depth
		2, // vertex and fragment stages
		pipelineShaderStageInfo,
		&pipelineVertexInputStateInfo,
		&pipelineInputAssemblyStateInfo,
		nullptr,
		&pipelineViewportStateCreateInfo,
		&pipelineRasterizationStateCreateInfo,
		&pipelineMultisampleStateInfo,
		&pipelineDepthStencilStateInfo,
		&pipelineColorBlendStateInfo,
		&pipelineDynamicStateInfo,
		Unwrap(Gfx::locPipelineLayout),
		Unwrap(Gfx::locRenderPass),
		0,
		VK_NULL_HANDLE,
		0
	};
	myRenderer.Create(&graphicsPipelineCreateInfo, &Gfx::locGraphicsPipeline, 1);

	// VULKAN SPACE: X to the right, Y downwards, Z forward
	// Vertex buffer
	const float vertexBufferData[] =
	{
		-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		-0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	};

	uint32_t familyQueue = myRenderer.GetGraphicsQueueFamilyIndex();
	VkBufferCreateInfo bufferCreateInfo
	{
		VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		nullptr,
		0,
		sizeof(vertexBufferData),
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_SHARING_MODE_EXCLUSIVE,
		1,
		&familyQueue
	};

	myRenderer.Create(bufferCreateInfo, Gfx::locBuffer);

	VkMemoryRequirements vertexBufferMemoryRequirements;
	myRenderer.GetMemoryRequirements(Gfx::locBuffer, vertexBufferMemoryRequirements);

	// Allocate memory to associate to vertex buffer
	VkMemoryPropertyFlags memoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	myRenderer.AllocateDeviceMemory(vertexBufferMemoryRequirements.size, vertexBufferMemoryRequirements.memoryTypeBits, memoryPropertyFlags, Gfx::locDeviceMemory);
	myRenderer.BindDeviceMemory(Gfx::locDeviceMemory, 0, Gfx::locBuffer);

	// Map memory
	void* mappedDeviceMemory = myRenderer.MapDeviceMemory(Gfx::locDeviceMemory, 0, sizeof(vertexBufferData));

	// Copy data
	std::memcpy(mappedDeviceMemory, vertexBufferData, sizeof(vertexBufferData));

	// Unmap and flush
	myRenderer.UnmapDeviceMemory(Gfx::locDeviceMemory);

	// Create uniform buffer
	float aspectRatio = static_cast<float>(myMainWindow.GetClientWidth()) / static_cast<float>(myMainWindow.GetClientHeight());
	Matrix44 mvp = Matrix44::Perspective(Math::DegreeToRadian(60.0f), aspectRatio, 0.1f, 100.0f) * Matrix44::Scale(1.0f, 1.0f, 1.0f) * Matrix44::Translate(0.0f, 0.0f, -10.0f);

	bufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	bufferCreateInfo.size = sizeof(mvp);

	for (uint32_t i = 0; i < Gfx::MaxConcurrentImages; ++i)
		myRenderer.Create(bufferCreateInfo, Gfx::locUniformBuffer[i]);

	// Allocate uniform buffers memory
	VkMemoryRequirements uniformBufferMemoryRequirements;
	myRenderer.GetMemoryRequirements(Gfx::locUniformBuffer[0], uniformBufferMemoryRequirements);
	myRenderer.AllocateDeviceMemory(uniformBufferMemoryRequirements.size * Gfx::MaxConcurrentImages, uniformBufferMemoryRequirements.memoryTypeBits, memoryPropertyFlags, Gfx::locUniformDeviceMemory);
	Gfx::locUniformBufferOffset = uniformBufferMemoryRequirements.size;

	for (uint32_t i = 0; i < Gfx::MaxConcurrentImages; ++i)
	{
		VkDeviceSize offset = Gfx::locUniformBufferOffset * i;
		myRenderer.BindDeviceMemory(Gfx::locUniformDeviceMemory, offset, Gfx::locUniformBuffer[i]);
		mappedDeviceMemory = myRenderer.MapDeviceMemory(Gfx::locUniformDeviceMemory, offset, sizeof(mvp));
		std::memcpy(mappedDeviceMemory, &mvp, sizeof(mvp));
		myRenderer.UnmapDeviceMemory(Gfx::locUniformDeviceMemory);
	}

	// Descriptor pool and sets
	VkDescriptorPoolSize descriptorPoolSize
	{
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		1
	};
	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo
	{
		VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		nullptr,
		VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
		3,
		1,
		&descriptorPoolSize
	};
	myRenderer.Create(descriptorPoolCreateInfo, Gfx::locDescriptorPool);

	for (uint32_t i = 0; i < Gfx::MaxConcurrentImages; ++i)
	{
		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo
		{
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			nullptr,
			Unwrap(Gfx::locDescriptorPool),
			1,
			Unwrap(&Gfx::locDescriptorSetLayout)
		};
		myRenderer.Create(descriptorSetAllocateInfo, &Gfx::locDescriptorSet[i]);

		// Prepare descriptor
		VkDescriptorBufferInfo descriptorBufferInfo
		{
			Unwrap(Gfx::locUniformBuffer[i]),
			0,
			sizeof(mvp)
		};
		VkWriteDescriptorSet writeDescriptorSet
		{
			VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			nullptr,
			Unwrap(Gfx::locDescriptorSet[i]),
			0,
			0,
			1,
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			nullptr,
			&descriptorBufferInfo,
			nullptr
		};
		myRenderer.UpdateDescriptorSets(&writeDescriptorSet, 1);
	}
}

void Application::Run()
{
	while (myMainWindow.IsRunning())
	{
		myMainWindow.Update();
		WindowPaint();
	}
}

void Application::Shutdown()
{
	// Wait for device to finish before deleting anything
	myRenderer.WaitForDevice();

	// Destroy all resources
	myRenderer.Destroy(Gfx::locDescriptorPool, Gfx::locDescriptorSet, Gfx::MaxConcurrentImages);
	myRenderer.Destroy(Gfx::locDescriptorPool);
	myRenderer.FreeDeviceMemory(Gfx::locUniformDeviceMemory);
	for (uint32_t i = 0; i < Gfx::MaxConcurrentImages; ++i)
		myRenderer.Destroy(Gfx::locUniformBuffer[i]);
	myRenderer.FreeDeviceMemory(Gfx::locDeviceMemory);
	myRenderer.Destroy(Gfx::locBuffer);
	myRenderer.Destroy(Gfx::locGraphicsPipeline);
	myRenderer.Destroy(Gfx::locPipelineLayout);
	myRenderer.Destroy(Gfx::locDescriptorSetLayout);
	myRenderer.Destroy(Gfx::locFragmentShader);
	myRenderer.Destroy(Gfx::locVertexShader);
	for (uint32_t i = 0; i < Gfx::MaxConcurrentImages; ++i)
	{
		myRenderer.Destroy(Gfx::locAcquireSemaphores[i]);
		myRenderer.Destroy(Gfx::locFences[i]);
	}
	myRenderer.Destroy(Gfx::locCommandPool, Gfx::locCommandBuffers, Gfx::ImageCount);
	myRenderer.Destroy(Gfx::locCommandPool);
	for (uint32_t i = 0; i < Gfx::ImageCount; ++i)
		myRenderer.Destroy(Gfx::locFramebuffers[i]);
	for(uint32_t i = 0; i < Gfx::ImageCount; ++i)
		myRenderer.Destroy(Gfx::locSwapchainImageViews[i]);
	myRenderer.Destroy(Gfx::locRenderPass);

	// Release memory
	Renderer::Destroy(myRenderer);
	Window::Destroy(myMainWindow);
	WindowClass::Destroy(myWindowClass);
}

void Application::WindowResize(uint32_t aWidth, uint32_t aHeight)
{
	// Wait for device and destroy Swapchain specific resources
	myRenderer.WaitForDevice();
	uint32_t swapchainImageCount = myRenderer.GetSwapchainImageCount();
	for (uint32_t i = 0; i < swapchainImageCount; ++i)
	{
		myRenderer.Destroy(Gfx::locSwapchainImageViews[i]);
		myRenderer.Destroy(Gfx::locFramebuffers[i]);
	}
	myRenderer.ResizeSwapchain(aWidth, aHeight);

	// Create resources again
	// Get images from the swapchain
	swapchainImageCount = myRenderer.GetSwapchainImageCount();
	std::vector<Image> swapchainImages(swapchainImageCount);
	myRenderer.GetSwapchainImages(swapchainImages.data());

	// Create ImageViews for later generation of the back framebuffers
	VkFormat swapchainFormat = myRenderer.GetSwapchainFormat();
	for (uint32_t i = 0; i < swapchainImageCount; ++i)
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

		myRenderer.Create(imageViewCreateInfo, Gfx::locSwapchainImageViews[i]);
	}


	// Create framebuffers for the render pass
	VkFramebufferCreateInfo framebufferCreateInfo
	{
		VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		nullptr,
		0,
		Unwrap(Gfx::locRenderPass),
		1,
		nullptr, // Filled in loop for each framebuffer
		aWidth,
		aHeight,
		1
	};

	for (uint32_t i = 0; i < swapchainImageCount; ++i)
	{
		framebufferCreateInfo.pAttachments = Unwrap(&Gfx::locSwapchainImageViews[i]);
		myRenderer.Create(framebufferCreateInfo, Gfx::locFramebuffers[i]);
	}
}

void Application::WindowPaint()
{
	// Render
	Fence& fence = Gfx::locFences[Gfx::checkIndex];

	// Ensure no more than Gfx::MaxConcurrentImages are being drawn
	myRenderer.WaitForFences(&fence, 1);
	myRenderer.ResetFences(&fence, 1);

	Gfx::frameIndex = myRenderer.AcquireNextImage(Gfx::locAcquireSemaphores[Gfx::checkIndex]);

	// Update uniform buffer
	static float xPosition = 0.0f;
	xPosition += 0.16f;
	xPosition = xPosition < 5.0f ? xPosition : -5.0f;
	float aspectRatio = static_cast<float>(myMainWindow.GetClientWidth()) / static_cast<float>(myMainWindow.GetClientHeight());
	Matrix44 mvp = Matrix44::Perspective(Math::DegreeToRadian(60.0f), aspectRatio, 0.1f, 100.0f) * Matrix44::Scale(1.0f, 1.0f, 1.0f) * Matrix44::Translate(xPosition, 0.0f, -10.0f);
	VkDeviceSize offset = Gfx::locUniformBufferOffset * Gfx::checkIndex;
	void* mappedDeviceMemory = myRenderer.MapDeviceMemory(Gfx::locUniformDeviceMemory, offset, sizeof(mvp));
	std::memcpy(mappedDeviceMemory, &mvp, sizeof(mvp));
	myRenderer.UnmapDeviceMemory(Gfx::locUniformDeviceMemory);

	CommandBuffer& cmd = Gfx::locCommandBuffers[Gfx::frameIndex];
	Framebuffer& framebuffer = Gfx::locFramebuffers[Gfx::frameIndex];

	// Start recording
	VkCommandBufferBeginInfo beginInfo
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		nullptr,
		VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
		nullptr
	};

	myRenderer.BeginCommandBuffer(cmd, beginInfo);

	// Start render pass
	unsigned width = myMainWindow.GetClientWidth();
	unsigned height = myMainWindow.GetClientHeight();
	VkClearValue clearColor;
	memset(&clearColor, 0u, sizeof(VkClearValue));
	VkRenderPassBeginInfo renderPassBeginInfo
	{
		VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		nullptr,
		Unwrap(Gfx::locRenderPass),
		Unwrap(framebuffer),
		{ 0u, 0u, width, height },
		1,
		&clearColor
	};
	myRenderer.BeginRenderPass(cmd, renderPassBeginInfo);

	// Bind pipeline
	myRenderer.BindPipeline(cmd, Gfx::locGraphicsPipeline, true);

	VkViewport viewport;
	memset(&viewport, 0, sizeof(viewport));
	viewport.width = static_cast<float>(width);
	viewport.height = static_cast<float>(height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	myRenderer.SetViewport(cmd, &viewport, 1, 0);

	VkRect2D scissor;
	memset(&scissor, 0, sizeof(scissor));
	scissor.extent.width = width;
	scissor.extent.height = height;
	scissor.offset.x = 0;
	scissor.offset.y = 0;
	myRenderer.SetScissor(cmd, &scissor, 1, 0);

	// Draw
	myRenderer.BindDescriptorSets(cmd, Gfx::locPipelineLayout, &Gfx::locDescriptorSet[Gfx::checkIndex], 1);
	VkDeviceSize vertexBufferOffset = 0u;
	myRenderer.BindVertexBuffers(cmd, &Gfx::locBuffer, &vertexBufferOffset, 1);
	myRenderer.Draw(cmd, 4, 0, 1, 0);

	myRenderer.EndRenderPass(cmd);
	myRenderer.EndCommandBuffer(cmd);

	VkPipelineStageFlags pipelineStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	VkSubmitInfo submitInfo;
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = NULL;
	submitInfo.pWaitDstStageMask = &pipelineStageFlags;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = Unwrap(&Gfx::locAcquireSemaphores[Gfx::checkIndex]);
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = Unwrap(&cmd);
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores = nullptr;

	myRenderer.Submit(&submitInfo, 1, fence);
	myRenderer.Present(nullptr, 0, Gfx::frameIndex);

	// Update values for next frame
	Gfx::currentFrame++;
	Gfx::checkIndex = Gfx::currentFrame & 1;
}

Application & App()
{
	static Application ourApp;
	return ourApp;
}
