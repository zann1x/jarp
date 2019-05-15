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
#include "VulkanRHI/VulkanBuffer.h"
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
#include "VulkanRHI/VulkanSemaphore.h"
#include "VulkanRHI/VulkanShader.h"
#include "VulkanRHI/VulkanSwapchain.h"
#include "VulkanRHI/VulkanUtils.hpp"

#include "Model.h"
#include "Utils.hpp"

///////////////// VULKAN APPLICATION /////////////////

Model* pModel;

VulkanBuffer* pVertexBuffer;
VulkanBuffer* pIndexBuffer;

VulkanDescriptorPool* pDescriptorPool;
VulkanDescriptorSetLayout* pDescriptorSetLayout;
VulkanDescriptorSet* pDescriptorSet;

struct SUniformBufferObject
{
	alignas(16) glm::mat4 MVP;
};

SUniformBufferObject MVP;

std::vector<VulkanBuffer*> UniformBuffers;

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
VulkanCommandPool* pTransientCommandPool;
VulkanCommandBuffer* pTransientCommandBuffer;

VulkanSemaphore* pSignalSemaphore;
VulkanSemaphore* pWaitSemaphore;

/* Depends on:
 *	- CommandBuffer
 *  - RenderPass
 *  - SwapchainKHR
 */
void RecordCommandBuffers()
{
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

		VkBuffer VertexBuffers[] = { pVertexBuffer->GetHandle() };
		VkDeviceSize Offsets[] = { 0 };
		vkCmdBindVertexBuffers(CommandBuffer, 0, 1, VertexBuffers, Offsets);
		vkCmdBindIndexBuffer(CommandBuffer, pIndexBuffer->GetHandle(), 0, VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pGraphicsPipeline->GetLayoutHandle(), 0, 1, &pDescriptorSet->At(i), 0, nullptr);

		vkCmdDrawIndexed(CommandBuffer, static_cast<uint32_t>(pModel->GetIndices().size()), 1, 0, 0, 0);
		
		vkCmdEndRenderPass(CommandBuffer);

		VK_ASSERT(vkEndCommandBuffer(CommandBuffer));
		// END OF RECORD //
	}
}

void StartVulkan()
{
	pInstance = new VulkanInstance();
	pInstance->CreateInstance();
#if defined(_DEBUG)
	VulkanDebug::SetupDebugCallback(pInstance->GetHandle());
#endif

	pLogicalDevice = new VulkanDevice(*pInstance);
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
	pModel->LoadModel("Content/monkey.obj");

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
	pTransientCommandPool = new VulkanCommandPool(*pLogicalDevice);
	pTransientCommandPool->CreateCommandPool(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);

	pCommandBuffers.resize(pFramebuffers.size());
	for (size_t i = 0; i < pFramebuffers.size(); ++i)
	{
		pCommandBuffers[i] = new VulkanCommandBuffer(*pLogicalDevice, *pCommandPool);
		pCommandBuffers[i]->CreateCommandBuffer();
	}

	pVertexBuffer = new VulkanBuffer(*pLogicalDevice);
	pVertexBuffer->CreateBuffer(pModel->GetVerticesDeviceSize(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	pIndexBuffer = new VulkanBuffer(*pLogicalDevice);
	pIndexBuffer->CreateBuffer(pModel->GetIndicesDeviceSize(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	
	pTransientCommandBuffer = new VulkanCommandBuffer(*pLogicalDevice, *pTransientCommandPool);
	pVertexBuffer->UploadBuffer(*pTransientCommandBuffer, pModel->GetVertices());
	pIndexBuffer->UploadBuffer(*pTransientCommandBuffer, pModel->GetIndices());
	delete pTransientCommandBuffer;

	UniformBuffers.resize(pSwapchain->GetImages().size());
	for (size_t i = 0; i < pSwapchain->GetImages().size(); ++i)
	{
		UniformBuffers[i] = new VulkanBuffer(*pLogicalDevice);
		UniformBuffers[i]->CreateBuffer(sizeof(MVP), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		UniformBuffers[i]->Bind();
	}
	std::vector<VkBuffer> UniBuffers;
	for (auto& UniformBuffer : UniformBuffers)
		UniBuffers.push_back(UniformBuffer->GetHandle());
	pDescriptorSet = new VulkanDescriptorSet(*pLogicalDevice);
	pDescriptorSet->CreateDescriptorSets(*pDescriptorSetLayout, *pDescriptorPool, pSwapchain->GetImages().size(), sizeof(MVP), UniBuffers);

	pSignalSemaphore = new VulkanSemaphore(*pLogicalDevice);
	pSignalSemaphore->CreateSemaphore();
	pWaitSemaphore = new VulkanSemaphore(*pLogicalDevice);
	pWaitSemaphore->CreateSemaphore();

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

	pSignalSemaphore->Destroy();
	delete pSignalSemaphore;
	pWaitSemaphore->Destroy();
	delete pWaitSemaphore;
	
	// Even though the uniform buffer depends on the number of swapchain images, it seems that it doesn't need to be recreated with the swapchain
	for (auto& UniformBuffer : UniformBuffers)
	{
		UniformBuffer->Destroy();
		delete UniformBuffer;
	}
	pIndexBuffer->Destroy();
	delete pIndexBuffer;
	pVertexBuffer->Destroy();
	delete pVertexBuffer;

	pTransientCommandPool->Destroy();
	delete pTransientCommandPool;
	pCommandPool->Destroy();
	delete pCommandPool;

	pDescriptorPool->Destroy();
	delete pDescriptorPool;
	delete pDescriptorSet; // Vulkan objects are implicitly destroyed by the owning pool
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
	vkMapMemory(pLogicalDevice->GetInstanceHandle(), UniformBuffers[CurrentImage]->GetMemoryHandle(), 0, sizeof(MVP), 0, &RawData);
	memcpy(RawData, &MVP, sizeof(MVP));
	vkUnmapMemory(pLogicalDevice->GetInstanceHandle(), UniformBuffers[CurrentImage]->GetMemoryHandle());
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
		VkResult Result = pSwapchain->AcquireNextImage(pWaitSemaphore->GetHandle());
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
	pLogicalDevice->GetGraphicsQueue().QueueSubmit({ pCommandBuffers[pSwapchain->GetActiveImageIndex()]->GetHandle() }, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, { pWaitSemaphore->GetHandle() }, { pSignalSemaphore->GetHandle() });

	{
		VkResult Result = pSwapchain->QueuePresent(pLogicalDevice->GetPresentQueue().GetHandle(), pSignalSemaphore->GetHandle());
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
