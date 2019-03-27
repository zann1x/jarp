// Define NOMINMAX to prevent the min and max function of the windows.h header to be defined
#define NOMINMAX

#include <vulkan/vulkan.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <set>
#include <algorithm>
#include <limits>
#include <chrono>

#include <string>
#include <fstream>

#include "CrossPlatformWindow.h"
#include "VulkanRHI/VulkanCommandBuffer.h"
#include "VulkanRHI/VulkanCommandPool.h"
#include "VulkanRHI/VulkanDebug.h"
#include "VulkanRHI/VulkanDescriptorPool.h"
#include "VulkanRHI/VulkanDescriptorSet.h"
#include "VulkanRHI/VulkanDescriptorSetLayout.h"
#include "VulkanRHI/VulkanDevice.h"
#include "VulkanRHI/VulkanFramebuffer.h"
#include "VulkanRHI/VulkanGraphicsPipeline.h"
#include "VulkanRHI/VulkanInstance.h"
#include "VulkanRHI/VulkanQueue.h"
#include "VulkanRHI/VulkanRenderPass.h"
#include "VulkanRHI/VulkanShader.h"
#include "VulkanRHI/VulkanSwapchain.h"
#include "VulkanRHI/VulkanUtils.hpp"

#include "Model.h"
#include "Utils.hpp"

///////////////// VULKAN APPLICATION /////////////////

Model* pModel;

VkBuffer VertexBuffer;
VkDeviceMemory VertexBufferDeviceMemory;

VkBuffer IndexBuffer;
VkDeviceMemory IndexBufferDeviceMemory;

VulkanDescriptorPool* pDescriptorPool;
VulkanDescriptorSetLayout* pDescriptorSetLayout;
VulkanDescriptorSet* pDescriptorSet;

struct SUniformBufferObject
{
	alignas(16) glm::mat4 MVP;
};

SUniformBufferObject MVP;

std::vector<VkBuffer> UniformBuffers;
std::vector<VkDeviceMemory> UniformBufferMemories;

///////////////// VULKAN /////////////////

struct
{
	bool VSync = true;
} Settings;

VulkanInstance* pInstance;
CrossPlatformWindow Window;

VulkanDevice* pLogicalDevice;
VulkanSwapchain* pSwapchain;

VulkanShader* pShader;
VulkanRenderPass* pRenderPass;
VulkanGraphicsPipeline* pGraphicsPipeline;
std::vector<VulkanFramebuffer*> pFramebuffers;

VulkanCommandPool* pCommandPool;
std::vector<VulkanCommandBuffer*> pCommandBuffers;

VkSemaphore SignalSemaphore;
VkSemaphore WaitSemaphore;

/* Depends on:
 *	- Instance
 *  - SurfaceKHR
 */
VkPhysicalDevice PickPhysicalDevice()
{
	// Find the physical device to use
	VkPhysicalDevice PhysicalDevice;
	uint32_t PhysicalDeviceCount;
	VK_ASSERT(vkEnumeratePhysicalDevices(pInstance->GetHandle(), &PhysicalDeviceCount, nullptr));
	std::vector<VkPhysicalDevice> PhysicalDevices(PhysicalDeviceCount);
	VK_ASSERT(vkEnumeratePhysicalDevices(pInstance->GetHandle(), &PhysicalDeviceCount, PhysicalDevices.data()));

	// Get info about the available physical devices and pick one for use
	for (size_t i = 0; i < PhysicalDevices.size(); ++i)
	{
		VkPhysicalDeviceProperties PhysicalDeviceProperties;
		vkGetPhysicalDeviceProperties(PhysicalDevices[i], &PhysicalDeviceProperties);

		uint32_t QueueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevices[i], &QueueFamilyCount, nullptr);
		if (QueueFamilyCount < 1)
			continue;

		PhysicalDevice = PhysicalDevices[i];

		if (PhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			break;
	}

	return PhysicalDevice;
}

/* Depends on:
 *	- Device
 */
void CreateBuffer(VkDeviceSize Size, VkBufferUsageFlags Usage, VkBuffer& Buffer, VkMemoryPropertyFlags MemoryProperties, VkDeviceMemory& DeviceMemory)
{
	// Create the buffer
	VkBufferCreateInfo BufferCreateInfo = {};
	BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	BufferCreateInfo.pNext = nullptr;
	BufferCreateInfo.flags = 0;
	BufferCreateInfo.size = Size;
	BufferCreateInfo.usage = Usage;
	BufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	BufferCreateInfo.queueFamilyIndexCount = 0;
	BufferCreateInfo.pQueueFamilyIndices = nullptr;

	VK_ASSERT(vkCreateBuffer(pLogicalDevice->GetInstanceHandle(), &BufferCreateInfo, nullptr, &Buffer));

	// Allocate the buffer's memory
	VkMemoryRequirements MemoryRequirements;
	vkGetBufferMemoryRequirements(pLogicalDevice->GetInstanceHandle(), Buffer, &MemoryRequirements);

	uint32_t MemoryTypeIndex = ~0;
	VkPhysicalDeviceMemoryProperties PhysicalDeviceMemoryProperties;
	vkGetPhysicalDeviceMemoryProperties(pLogicalDevice->GetPhysicalHandle(), &PhysicalDeviceMemoryProperties);

	for (uint32_t i = 0; i < PhysicalDeviceMemoryProperties.memoryTypeCount; ++i)
	{
		if ((MemoryRequirements.memoryTypeBits & (1 << i) && (PhysicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & MemoryProperties) == MemoryProperties))
		{
			MemoryTypeIndex = i;
			break;
		}
	}

	if (MemoryTypeIndex == ~0)
		throw std::runtime_error("Failed to find suitable memory type for buffer!");

	VkMemoryAllocateInfo MemoryAllocateInfo = {};
	MemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	MemoryAllocateInfo.pNext = nullptr;
	MemoryAllocateInfo.allocationSize = MemoryRequirements.size;
	MemoryAllocateInfo.memoryTypeIndex = MemoryTypeIndex;

	VK_ASSERT(vkAllocateMemory(pLogicalDevice->GetInstanceHandle(), &MemoryAllocateInfo, nullptr, &DeviceMemory));

	// Bind the buffer to the allocated memory
	vkBindBufferMemory(pLogicalDevice->GetInstanceHandle(), Buffer, DeviceMemory, 0);
}

/* Depends on:
 *	- Device
 *  - CommandPool
 */
void CopyBuffer(VkBuffer SrcBuffer, VkBuffer DstBuffer, VkDeviceSize Size)
{
	VkCommandBufferAllocateInfo CommandBufferAllocateInfo = {};
	CommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	CommandBufferAllocateInfo.pNext = nullptr;
	CommandBufferAllocateInfo.commandPool = pCommandPool->GetHandle();
	CommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	CommandBufferAllocateInfo.commandBufferCount = 1;

	VkCommandBuffer CommandBuffer;
	VK_ASSERT(vkAllocateCommandBuffers(pLogicalDevice->GetInstanceHandle(), &CommandBufferAllocateInfo, &CommandBuffer));

	VkCommandBufferBeginInfo CommandBufferBeginInfo = {};
	CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	CommandBufferBeginInfo.pNext = nullptr;
	CommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	CommandBufferBeginInfo.pInheritanceInfo = nullptr;

	// START OF RECORD //
	VK_ASSERT(vkBeginCommandBuffer(CommandBuffer, &CommandBufferBeginInfo));

	VkBufferCopy BufferCopy = {};
	BufferCopy.srcOffset = 0;
	BufferCopy.dstOffset = 0;
	BufferCopy.size = Size;

	vkCmdCopyBuffer(CommandBuffer, SrcBuffer, DstBuffer, 1, &BufferCopy);

	VK_ASSERT(vkEndCommandBuffer(CommandBuffer));
	// END OF RECORD //

	VkSubmitInfo SubmitInfo = {};
	SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	SubmitInfo.pNext = nullptr;
	SubmitInfo.waitSemaphoreCount = 0;
	SubmitInfo.pWaitSemaphores = nullptr;
	SubmitInfo.pWaitDstStageMask = nullptr;
	SubmitInfo.commandBufferCount = 1;
	SubmitInfo.pCommandBuffers = &CommandBuffer;
	SubmitInfo.signalSemaphoreCount = 0;
	SubmitInfo.pSignalSemaphores = nullptr;

	pLogicalDevice->GetGraphicsQueue().QueueSubmit({ CommandBuffer }, 0, {}, {});
	pLogicalDevice->GetGraphicsQueue().WaitUntilIdle();

	vkFreeCommandBuffers(pLogicalDevice->GetInstanceHandle(), pCommandPool->GetHandle(), 1, &CommandBuffer);
}

/* Depends on:
 *	- Device
 */
template <typename T>
void CreateAndUploadBuffer(VkBufferUsageFlags Usage, VkBuffer& Buffer, VkDeviceMemory& DeviceMemory, const std::vector<T>& Data)
{
	// Create the staging buffer
	VkDeviceSize BufferSize = sizeof(Data[0]) * Data.size();
	VkBuffer StagingBuffer;
	VkDeviceMemory StagingBufferMemory;
	CreateBuffer(BufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, StagingBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, StagingBufferMemory);

	void* RawData;
	vkMapMemory(pLogicalDevice->GetInstanceHandle(), StagingBufferMemory, 0, BufferSize, 0, &RawData);
	memcpy(RawData, Data.data(), static_cast<size_t>(BufferSize));
	vkUnmapMemory(pLogicalDevice->GetInstanceHandle(), StagingBufferMemory);

	// Create the actual vertex buffer
	CreateBuffer(BufferSize, Usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT, Buffer, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, DeviceMemory);

	CopyBuffer(StagingBuffer, Buffer, BufferSize);

	vkFreeMemory(pLogicalDevice->GetInstanceHandle(), StagingBufferMemory, nullptr);
	vkDestroyBuffer(pLogicalDevice->GetInstanceHandle(), StagingBuffer, nullptr);
}

void CreateVertexBuffer()
{
	CreateAndUploadBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VertexBuffer, VertexBufferDeviceMemory, pModel->GetVertices());
}

void CreateIndexBuffer()
{
	CreateAndUploadBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, IndexBuffer, IndexBufferDeviceMemory, pModel->GetIndices());
}

void CreateUniformBuffer()
{
	VkDeviceSize BufferSize = sizeof(MVP);
	
	UniformBuffers.resize(pSwapchain->GetImages().size());
	UniformBufferMemories.resize(pSwapchain->GetImages().size());

	for (size_t i = 0; i < pSwapchain->GetImages().size(); ++i)
	{
		CreateBuffer(BufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, UniformBuffers[i], VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, UniformBufferMemories[i]);
	}
}

/* Depends on:
 *	- CommandBuffer
 *  - RenderPass
 *  - SwapchainKHR
 */
void RecordCommandBuffers()
{
	// Record on the command buffers
	for (size_t i = 0; i < pCommandBuffers.size(); ++i)
	{
		const VkCommandBuffer& CommandBuffer = pCommandBuffers[i]->GetHandle();

		VkCommandBufferBeginInfo CommandBufferBeginInfo = {};
		CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		CommandBufferBeginInfo.pNext = nullptr;
		CommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		CommandBufferBeginInfo.pInheritanceInfo = nullptr; // This is a primary command buffer, so the value can be ignored

		// START OF RECORD //
		VK_ASSERT(vkBeginCommandBuffer(CommandBuffer, &CommandBufferBeginInfo));

		VkClearValue ClearValue = { 0.0f, 0.0f, 0.0f, 1.0f };

		VkRenderPassBeginInfo RenderPassBeginInfo = {};
		RenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		RenderPassBeginInfo.pNext = nullptr;
		RenderPassBeginInfo.renderPass = pRenderPass->GetHandle();
		RenderPassBeginInfo.framebuffer = pFramebuffers[i]->GetHandle();
		RenderPassBeginInfo.renderArea.extent = pSwapchain->GetDetails().Extent;
		RenderPassBeginInfo.renderArea.offset = { 0, 0 };
		RenderPassBeginInfo.clearValueCount = 1;
		RenderPassBeginInfo.pClearValues = &ClearValue;

		vkCmdBeginRenderPass(CommandBuffer, &RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE); // Only primary command buffers, so inline subpass suffices

		vkCmdBindPipeline(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pGraphicsPipeline->GetHandle());

		VkBuffer VertexBuffers[] = { VertexBuffer };
		VkDeviceSize Offsets[] = { 0 };
		vkCmdBindVertexBuffers(CommandBuffer, 0, 1, VertexBuffers, Offsets);
		vkCmdBindIndexBuffer(CommandBuffer, IndexBuffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pGraphicsPipeline->GetLayoutHandle(), 0, 1, &pDescriptorSet->At(i), 0, nullptr);

		vkCmdDrawIndexed(CommandBuffer, static_cast<uint32_t>(pModel->GetIndices().size()), 1, 0, 0, 0);
		
		vkCmdEndRenderPass(CommandBuffer);

		VK_ASSERT(vkEndCommandBuffer(CommandBuffer));
		// END OF RECORD //
	}
}

/* Depends on:
 *	- Device
 */
void CreateSyncObjects()
{
	// TODO add fences

	VkSemaphoreCreateInfo SemaphoreCreateInfo = {};
	SemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	SemaphoreCreateInfo.flags = 0;
	SemaphoreCreateInfo.pNext = nullptr;

	VK_ASSERT(vkCreateSemaphore(pLogicalDevice->GetInstanceHandle(), &SemaphoreCreateInfo, nullptr, &SignalSemaphore));
	VK_ASSERT(vkCreateSemaphore(pLogicalDevice->GetInstanceHandle(), &SemaphoreCreateInfo, nullptr, &WaitSemaphore));
}

void StartVulkan()
{
	pInstance = new VulkanInstance();
	pInstance->CreateInstance();
#if defined(_DEBUG)
	VulkanDebug::SetupDebugCallback(pInstance->GetHandle());
#endif

	VkPhysicalDevice PhysicalDevice = PickPhysicalDevice();
	pLogicalDevice = new VulkanDevice(PhysicalDevice);
	pLogicalDevice->CreateLogicalDevice();
	pSwapchain = new VulkanSwapchain(Window, pInstance->GetHandle(), *pLogicalDevice);
	pSwapchain->CreateSwapchain(Window.GetWidth(), Window.GetHeight(), Settings.VSync);

	pDescriptorSetLayout = new VulkanDescriptorSetLayout(*pLogicalDevice);
	pDescriptorSetLayout->CreateDescriptorSetLayout(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);

	pShader = new VulkanShader(*pLogicalDevice);
	pShader->AddDescriptorSetLayout(*pDescriptorSetLayout);
	pShader->CreateShaderModule(VK_SHADER_STAGE_VERTEX_BIT, "Shaders/Shader.vert.spv");
	pShader->CreateShaderModule(VK_SHADER_STAGE_FRAGMENT_BIT, "Shaders/Shader.frag.spv");

	pModel = new Model(*pShader);

	pRenderPass = new VulkanRenderPass(*pLogicalDevice, *pSwapchain);
	pRenderPass->CreateRenderPass();
	pGraphicsPipeline = new VulkanGraphicsPipeline(*pLogicalDevice, *pRenderPass, *pShader);
	pGraphicsPipeline->CreateGraphicsPipeline(pModel->GetPipelineVertexInputStateCreateInfo(), pSwapchain->GetDetails().Extent);
	pFramebuffers.resize(pSwapchain->GetImages().size());
	for (size_t i = 0; i < pSwapchain->GetImages().size(); ++i)
	{
		pFramebuffers[i] = new VulkanFramebuffer(*pLogicalDevice, *pRenderPass);
		pFramebuffers[i]->CreateFramebuffer(pSwapchain->GetImageViews()[i], pSwapchain->GetDetails().Extent);
	}

	pDescriptorPool = new VulkanDescriptorPool(*pLogicalDevice, *pSwapchain);
	pDescriptorPool->CreateDescriptorPool();
	pCommandPool = new VulkanCommandPool(*pLogicalDevice);
	pCommandPool->CreateCommandPool();

	pCommandBuffers.resize(pFramebuffers.size());
	for (size_t i = 0; i < pFramebuffers.size(); ++i)
	{
		pCommandBuffers[i] = new VulkanCommandBuffer(*pLogicalDevice, *pCommandPool);
		pCommandBuffers[i]->CreateCommandBuffer();
	}

	CreateVertexBuffer();
	CreateIndexBuffer();
	CreateUniformBuffer();
	pDescriptorSet = new VulkanDescriptorSet(*pLogicalDevice);
	pDescriptorSet->CreateDescriptorSets(*pDescriptorSetLayout, *pDescriptorPool, pSwapchain->GetImages().size(), sizeof(MVP), UniformBuffers);

	CreateSyncObjects();

	RecordCommandBuffers();
}

void CleanupSwapchain()
{
	for (size_t i = 0; i < pCommandBuffers.size(); ++i)
	{
		pCommandBuffers[i]->Destroy();
	}

	for (auto& Framebuffer : pFramebuffers)
		Framebuffer->Destroy();
	pGraphicsPipeline->Destroy();
	pRenderPass->Destroy();
	pSwapchain->Destroy();
}

void RecreateSwapchain()
{
	// If the window is minimized, the framebuffer's size will be 0
	std::pair<int, int> FramebufferSize;
	do
	{
		FramebufferSize = Window.GetFramebufferSize();
		glfwWaitEvents();
	} while (FramebufferSize.first == 0 || FramebufferSize.second == 0);

	pLogicalDevice->WaitUntilIdle();
	CleanupSwapchain();

	pSwapchain->CreateSwapchain(FramebufferSize.first, FramebufferSize.second, Settings.VSync);
	pRenderPass->CreateRenderPass();
	pGraphicsPipeline->CreateGraphicsPipeline(pModel->GetPipelineVertexInputStateCreateInfo(), pSwapchain->GetDetails().Extent);

	for (size_t i = 0; i < pSwapchain->GetImages().size(); ++i)
	{
		pFramebuffers[i]->CreateFramebuffer(pSwapchain->GetImageViews()[i], pSwapchain->GetDetails().Extent);
	}

	for (size_t i = 0; i < pFramebuffers.size(); ++i)
	{
		pCommandBuffers[i]->CreateCommandBuffer();
	}

	RecordCommandBuffers();
}

void ShutdownVulkan()
{
	// Free all resources
	pLogicalDevice->WaitUntilIdle();

	CleanupSwapchain();
	delete pRenderPass;
	delete pSwapchain;
	for (auto& CommandBuffer : pCommandBuffers)
	{
		delete CommandBuffer;
	}

	delete pModel;

	pShader->Destroy();
	delete pShader;

	vkDestroySemaphore(pLogicalDevice->GetInstanceHandle(), WaitSemaphore, nullptr);
	vkDestroySemaphore(pLogicalDevice->GetInstanceHandle(), SignalSemaphore, nullptr);
	
	// Even though the uniform buffer depends on the number of swapchain images, it seems that it doesn't need to be recreated with the swapchain
	for (size_t i = 0; i < UniformBuffers.size(); ++i)
	{
		vkFreeMemory(pLogicalDevice->GetInstanceHandle(), UniformBufferMemories[i], nullptr);
		vkDestroyBuffer(pLogicalDevice->GetInstanceHandle(), UniformBuffers[i], nullptr);
	}
	vkFreeMemory(pLogicalDevice->GetInstanceHandle(), IndexBufferDeviceMemory, nullptr);
	vkDestroyBuffer(pLogicalDevice->GetInstanceHandle(), IndexBuffer, nullptr);
	vkFreeMemory(pLogicalDevice->GetInstanceHandle(), VertexBufferDeviceMemory, nullptr);
	vkDestroyBuffer(pLogicalDevice->GetInstanceHandle(), VertexBuffer, nullptr);

	pCommandPool->Destroy();
	delete pCommandPool;
	pDescriptorPool->Destroy();
	delete pDescriptorPool;
	delete pDescriptorSet;
	pDescriptorSetLayout->Destroy();
	delete pDescriptorSetLayout;
	
	delete pLogicalDevice;
#if defined(_DEBUG)
	VulkanDebug::DestroyDebugCallback(pInstance->GetHandle());
#endif
	pInstance->Destroy();
	delete pInstance;
}

void UpdateMVP(uint32_t CurrentImage)
{
	static auto StartTime = std::chrono::high_resolution_clock::now();

	auto CurrentTime = std::chrono::high_resolution_clock::now();
	float TimePassed = std::chrono::duration<float, std::chrono::seconds::period>(CurrentTime - StartTime).count();

	glm::mat4 Model = glm::rotate(glm::mat4(1.0f), TimePassed * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 View = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), pSwapchain->GetDetails().Extent.width / static_cast<float>(pSwapchain->GetDetails().Extent.height), 0.1f, 10.0f);
	Projection[1][1] *= -1;

	MVP.MVP = Projection * View * Model;

	void* RawData;
	vkMapMemory(pLogicalDevice->GetInstanceHandle(), UniformBufferMemories[CurrentImage], 0, sizeof(MVP), 0, &RawData);
	memcpy(RawData, &MVP, sizeof(MVP));
	vkUnmapMemory(pLogicalDevice->GetInstanceHandle(), UniformBufferMemories[CurrentImage]);
}

/* Depends on:
 *  - Device
 *  - Semaphore
 *  - SwapchainKHR
 *  - CommandBuffer
 */
void DrawFrame()
{
	// Don't try to draw to a minimized window
	if (Window.IsIconified())
		return;

	// Get the next available image to work on
	{
		VkResult Result = pSwapchain->AcquireNextImage(WaitSemaphore);
		if (Result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapchain();
			return;
		}
		else
		{
			VK_ASSERT(Result);
		}
	}

	UpdateMVP(pSwapchain->GetActiveImageIndex());

	// Submit commands to the queue
	pLogicalDevice->GetGraphicsQueue().QueueSubmit({ pCommandBuffers[pSwapchain->GetActiveImageIndex()]->GetHandle() }, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, { WaitSemaphore }, { SignalSemaphore });

	{
		VkResult Result = pSwapchain->QueuePresent(pLogicalDevice->GetPresentQueue().GetHandle(), SignalSemaphore);
		if (Result == VK_ERROR_OUT_OF_DATE_KHR || Result == VK_SUBOPTIMAL_KHR || Window.IsFramebufferResized())
		{
			Window.SetFramebufferResized(false);
			RecreateSwapchain();
		}
		else
		{
			VK_ASSERT(Result);
		}
	}
}

///////////////// APP /////////////////

void MainLoop()
{
	while (!Window.ShouldClose())
	{
		Window.PollEvents();
		DrawFrame();
	}
}

int main()
{
	Window.StartGlfwWindow();
	StartVulkan();

	MainLoop();

	ShutdownVulkan();
	Window.ShutdownGlfw();

	return 0;
}
