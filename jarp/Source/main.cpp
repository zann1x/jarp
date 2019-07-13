#include <vulkan/vulkan.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <chrono>
#include <limits>

#include <set>
#include <vector>

#include <string>
#include <fstream>

#include "Camera.h"
#include "CrossPlatformWindow.h"
#include "Utils.hpp"

#include "VulkanRHI/Model.h"
#include "VulkanRHI/Texture.h"
#include "VulkanRHI/VulkanBuffer.h"
#include "VulkanRHI/VulkanCommandBuffer.h"
#include "VulkanRHI/VulkanCommandPool.h"
#include "VulkanRHI/VulkanDescriptorPool.h"
#include "VulkanRHI/VulkanDescriptorSet.h"
#include "VulkanRHI/VulkanDescriptorSetLayout.h"
#include "VulkanRHI/VulkanDevice.h"
#include "VulkanRHI/VulkanFence.h"
#include "VulkanRHI/VulkanFramebuffer.h"
#include "VulkanRHI/VulkanGraphicsPipeline.h"
#include "VulkanRHI/VulkanInstance.h"
#include "VulkanRHI/VulkanImage.h"
#include "VulkanRHI/VulkanImageView.h"
#include "VulkanRHI/VulkanQueue.h"
#include "VulkanRHI/VulkanRenderPass.h"
#include "VulkanRHI/VulkanSemaphore.h"
#include "VulkanRHI/VulkanShader.h"
#include "VulkanRHI/VulkanSwapchain.h"
#include "VulkanRHI/VulkanUtils.hpp"

///////////////// VULKAN APPLICATION /////////////////

Model* pModel;
Texture* pTexture;

VulkanBuffer* pVertexBuffer;
VulkanBuffer* pIndexBuffer;

VulkanDescriptorPool* pDescriptorPool;
VulkanDescriptorSetLayout* pDescriptorSetLayout;
VulkanDescriptorSet* pDescriptorSet;

struct SUniformBufferObject
{
	alignas(16) glm::mat4 Model;
	alignas(16) glm::mat4 View;
	alignas(16) glm::mat4 Projection;
	alignas(16) glm::vec3 LightPosition;
};

SUniformBufferObject UBO;

std::vector<VulkanBuffer*> UniformBuffers;

///////////////// VULKAN /////////////////

struct
{
	bool VSync = false;
} Settings;

VulkanInstance* pInstance;
CrossPlatformWindow Window;
Camera MyCamera;

VulkanDevice* pLogicalDevice;
VulkanSwapchain* pSwapchain;

VulkanShader* pShader;
VulkanRenderPass* pRenderPass;
VulkanGraphicsPipeline* pGraphicsPipeline;
std::vector<VulkanFramebuffer*> pFramebuffers;

VulkanImage* pDepthImage;
VulkanImageView* pDepthImageView;

VulkanCommandPool* pCommandPool;
std::vector<VulkanCommandBuffer*> pCommandBuffers;
VulkanCommandPool* pTransientCommandPool;
VulkanCommandBuffer* pTransientCommandBuffer;

int MaxFramesInFlight;
std::vector<VulkanSemaphore*> pRenderingFinishedSemaphores;
std::vector<VulkanSemaphore*> pImageAvailableSemaphores;
std::vector<VulkanFence*> pFencesInFlight;

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

		VK_ASSERT(vkBeginCommandBuffer(CommandBuffer, &CommandBufferBeginInfo));

		std::array<VkClearValue, 2> ClearValues = {};
		ClearValues[0] = { 0.2f, 0.3f, 0.8f, 1.0f };
		ClearValues[1] = { 1.0f, 0.0f }; // Initial value should be the furthest possible depth (= 1.0)

		VkRenderPassBeginInfo RenderPassBeginInfo = {};
		RenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		RenderPassBeginInfo.pNext = nullptr;
		RenderPassBeginInfo.renderPass = pRenderPass->GetHandle();
		RenderPassBeginInfo.framebuffer = pFramebuffers[i]->GetHandle();
		RenderPassBeginInfo.renderArea.extent = pSwapchain->GetDetails().Extent;
		RenderPassBeginInfo.renderArea.offset = { 0, 0 };
		RenderPassBeginInfo.clearValueCount = static_cast<uint32_t>(ClearValues.size());
		RenderPassBeginInfo.pClearValues = ClearValues.data();

		vkCmdBeginRenderPass(CommandBuffer, &RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE); // We only have primary command buffers, so an inline subpass suffices

		vkCmdBindPipeline(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pGraphicsPipeline->GetHandle());

		VkBuffer VertexBuffers[] = { pVertexBuffer->GetHandle() };
		VkDeviceSize Offsets[] = { 0 };
		vkCmdBindVertexBuffers(CommandBuffer, 0, 1, VertexBuffers, Offsets);
		vkCmdBindIndexBuffer(CommandBuffer, pIndexBuffer->GetHandle(), 0, VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pGraphicsPipeline->GetLayoutHandle(), 0, 1, &pDescriptorSet->At(i), 0, nullptr);

		vkCmdDrawIndexed(CommandBuffer, static_cast<uint32_t>(pModel->GetIndices().size()), 1, 0, 0, 0);
		
		vkCmdEndRenderPass(CommandBuffer);

		VK_ASSERT(vkEndCommandBuffer(CommandBuffer));
	}
}

void StartVulkan()
{
	pInstance = new VulkanInstance();
	pInstance->CreateInstance(Window);

	pLogicalDevice = new VulkanDevice(*pInstance);
	pLogicalDevice->CreateLogicalDevice();
	pSwapchain = new VulkanSwapchain(Window, pInstance->GetHandle(), *pLogicalDevice);
	pSwapchain->CreateSwapchain(Window.GetWidth(), Window.GetHeight(), Settings.VSync);
	MyCamera.SetAspectRatio(pSwapchain->GetDetails().Extent.width / static_cast<float>(pSwapchain->GetDetails().Extent.height));
	MaxFramesInFlight = pSwapchain->GetImageViews().size();

	pDescriptorSetLayout = new VulkanDescriptorSetLayout(*pLogicalDevice);
	pDescriptorSetLayout->AddLayout(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
	pDescriptorSetLayout->AddLayout(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	pDescriptorSetLayout->CreateDescriptorSetLayout();

	pCommandPool = new VulkanCommandPool(*pLogicalDevice);
	pCommandPool->CreateCommandPool();
	pTransientCommandPool = new VulkanCommandPool(*pLogicalDevice);
	pTransientCommandPool->CreateCommandPool(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);
	pTransientCommandBuffer = new VulkanCommandBuffer(*pLogicalDevice, *pTransientCommandPool);

	pDepthImage = new VulkanImage(*pLogicalDevice);
	pDepthImageView = new VulkanImageView(*pLogicalDevice);
	VkFormat DepthFormat = pLogicalDevice->FindDepthFormat();
	pDepthImage->CreateImage(pSwapchain->GetDetails().Extent.width, pSwapchain->GetDetails().Extent.height, DepthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	pDepthImageView->CreateImageView(pDepthImage->GetHandle(), DepthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
	pDepthImage->TransitionImageLayout(*pTransientCommandBuffer, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

	pModel = new Model(*pLogicalDevice);
	pTexture = new Texture(*pLogicalDevice);

	pRenderPass = new VulkanRenderPass(*pLogicalDevice, *pSwapchain);
	pRenderPass->CreateRenderPass();
	pShader = new VulkanShader(*pLogicalDevice);
	pShader->AddDescriptorSetLayout(*pDescriptorSetLayout);
	pShader->CreateShaderModule(VK_SHADER_STAGE_VERTEX_BIT, "Shaders/Phong.vert.spv");
	pShader->CreateShaderModule(VK_SHADER_STAGE_FRAGMENT_BIT, "Shaders/Phong.frag.spv");
	pGraphicsPipeline = new VulkanGraphicsPipeline(*pLogicalDevice, *pRenderPass, *pShader);
	pGraphicsPipeline->CreateGraphicsPipeline(pModel->GetPipelineVertexInputStateCreateInfo(), pSwapchain->GetDetails().Extent);

	pFramebuffers.resize(pSwapchain->GetImages().size());
	for (size_t i = 0; i < pSwapchain->GetImages().size(); ++i)
	{
		pFramebuffers[i] = new VulkanFramebuffer(*pLogicalDevice, *pRenderPass);
		pFramebuffers[i]->CreateFramebuffer({ pSwapchain->GetImageViews()[i].GetHandle(), pDepthImageView->GetHandle() }, pSwapchain->GetDetails().Extent);
	}

	pDescriptorPool = new VulkanDescriptorPool(*pLogicalDevice, *pSwapchain);
	pDescriptorPool->CreateDescriptorPool();

	pCommandBuffers.resize(pFramebuffers.size());
	for (size_t i = 0; i < pFramebuffers.size(); ++i)
	{
		pCommandBuffers[i] = new VulkanCommandBuffer(*pLogicalDevice, *pCommandPool);
		pCommandBuffers[i]->CreateCommandBuffer();
	}

	pModel->Load(*pTransientCommandBuffer, "Content/dragon.obj");
	pTexture->Load(*pTransientCommandBuffer, "Content/texture.jpg");
	pVertexBuffer = new VulkanBuffer(*pLogicalDevice, pModel->GetVerticesDeviceSize(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
	pVertexBuffer->CreateBuffer(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	pVertexBuffer->UploadBuffer(*pTransientCommandBuffer, pModel->GetVertices());
	pIndexBuffer = new VulkanBuffer(*pLogicalDevice, pModel->GetIndicesDeviceSize(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
	pIndexBuffer->CreateBuffer(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	pIndexBuffer->UploadBuffer(*pTransientCommandBuffer, pModel->GetIndices());
	delete pTransientCommandBuffer;

	UniformBuffers.resize(pSwapchain->GetImages().size());
	for (size_t i = 0; i < pSwapchain->GetImages().size(); ++i)
	{
		UniformBuffers[i] = new VulkanBuffer(*pLogicalDevice, sizeof(UBO), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
		UniformBuffers[i]->CreateBuffer(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	}
	std::vector<VkBuffer> UniBuffers;
	for (auto& UniformBuffer : UniformBuffers)
		UniBuffers.push_back(UniformBuffer->GetHandle());
	pDescriptorSet = new VulkanDescriptorSet(*pLogicalDevice);
	pDescriptorSet->CreateDescriptorSets(*pDescriptorSetLayout, *pDescriptorPool, pSwapchain->GetImages().size(), sizeof(UBO), UniBuffers, pTexture->GetSampler(), pTexture->GetImageView().GetHandle());

	pRenderingFinishedSemaphores.resize(MaxFramesInFlight);
	pImageAvailableSemaphores.resize(MaxFramesInFlight);
	pFencesInFlight.resize(MaxFramesInFlight);
	for (int i = 0; i < MaxFramesInFlight; ++i)
	{
		pRenderingFinishedSemaphores[i] = new VulkanSemaphore(*pLogicalDevice);
		pRenderingFinishedSemaphores[i]->CreateSemaphore();

		pImageAvailableSemaphores[i] = new VulkanSemaphore(*pLogicalDevice);
		pImageAvailableSemaphores[i]->CreateSemaphore();

		pFencesInFlight[i] = new VulkanFence(*pLogicalDevice);
		pFencesInFlight[i]->CreateFence();
	}

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
	pDepthImageView->Destroy();
	pDepthImage->Destroy();
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
		SDL_WaitEvent(nullptr);
	} while (FramebufferSize.first == 0 || FramebufferSize.second == 0);

	pLogicalDevice->WaitUntilIdle();
	CleanupSwapchain();

	pSwapchain->CreateSwapchain(FramebufferSize.first, FramebufferSize.second, Settings.VSync);
	MyCamera.SetAspectRatio(pSwapchain->GetDetails().Extent.width / static_cast<float>(pSwapchain->GetDetails().Extent.height));
	MaxFramesInFlight = pSwapchain->GetImageViews().size();
	
	VkFormat DepthFormat = pLogicalDevice->FindDepthFormat();
	pDepthImage->CreateImage(pSwapchain->GetDetails().Extent.width, pSwapchain->GetDetails().Extent.height, DepthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	pDepthImageView->CreateImageView(pDepthImage->GetHandle(), DepthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
	pTransientCommandBuffer = new VulkanCommandBuffer(*pLogicalDevice, *pTransientCommandPool);
	pDepthImage->TransitionImageLayout(*pTransientCommandBuffer, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	delete pTransientCommandBuffer;

	pRenderPass->CreateRenderPass();
	pGraphicsPipeline->CreateGraphicsPipeline(pModel->GetPipelineVertexInputStateCreateInfo(), pSwapchain->GetDetails().Extent);

	for (size_t i = 0; i < pSwapchain->GetImages().size(); ++i)
	{
		pFramebuffers[i]->CreateFramebuffer({ pSwapchain->GetImageViews()[i].GetHandle(), pDepthImageView->GetHandle() }, pSwapchain->GetDetails().Extent);
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

	delete pDepthImageView;
	delete pDepthImage;

	pShader->Destroy();
	delete pShader;

	pTexture->Destroy();
	delete pTexture;
	delete pModel;

	for (int i = 0; i < MaxFramesInFlight; ++i)
	{
		pFencesInFlight[i]->Destroy();
		delete pFencesInFlight[i];
		pRenderingFinishedSemaphores[i]->Destroy();
		delete pRenderingFinishedSemaphores[i];
		pImageAvailableSemaphores[i]->Destroy();
		delete pImageAvailableSemaphores[i];
	}
	
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

	pInstance->Destroy();
	delete pInstance;
}

void UpdateMVP(uint32_t CurrentImage)
{
	static auto StartTime = std::chrono::high_resolution_clock::now();

	auto CurrentTime = std::chrono::high_resolution_clock::now();
	float TimePassed = std::chrono::duration<float, std::chrono::seconds::period>(CurrentTime - StartTime).count();

	UBO.Model = glm::mat4();
	UBO.Model = glm::scale(glm::mat4(2.0f), glm::vec3(0.1, 0.1f, 0.1f));
	UBO.Model = glm::translate(UBO.Model, glm::vec3(0.0f, -5.0f, 0.0f));
	UBO.Model = glm::rotate(UBO.Model, TimePassed * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	UBO.View = MyCamera.GetViewMatrix();
	UBO.Projection = MyCamera.GetProjectionMatrix();
	UBO.LightPosition = glm::vec3(10.0f, 10.0f, 10.0f);

	void* RawData;
	vkMapMemory(pLogicalDevice->GetInstanceHandle(), UniformBuffers[CurrentImage]->GetMemoryHandle(), 0, sizeof(UBO), 0, &RawData);
	memcpy(RawData, &UBO, sizeof(UBO));
	vkUnmapMemory(pLogicalDevice->GetInstanceHandle(), UniformBuffers[CurrentImage]->GetMemoryHandle());
}

/* Depends on:
 *  - Device
 *  - Semaphore
 *  - SwapchainKHR
 *  - CommandBuffer
 */
void DrawFrame(uint32_t DeltaTime)
{
	static size_t CurrentFrame = 0;

	// Don't try to draw to a minimized window
	if (Window.IsIconified())
		return;

	// Get the next available image to work on
	{
		VkResult Result = pSwapchain->AcquireNextImage(pImageAvailableSemaphores[CurrentFrame]->GetHandle());
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

	MyCamera.Move(DeltaTime);
	UpdateMVP(pSwapchain->GetActiveImageIndex());

	// Submit commands to the queue
	pLogicalDevice->GetGraphicsQueue().QueueSubmitAndWait({ pCommandBuffers[pSwapchain->GetActiveImageIndex()]->GetHandle() }, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, { pImageAvailableSemaphores[CurrentFrame]->GetHandle() }, { pRenderingFinishedSemaphores[CurrentFrame]->GetHandle() }, pFencesInFlight[CurrentFrame]->GetHandle());

	{
		VkResult Result = pLogicalDevice->GetPresentQueue().QueuePresent(pSwapchain->GetHandle(), { pSwapchain->GetActiveImageIndex() }, { pRenderingFinishedSemaphores[CurrentFrame]->GetHandle() });
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

	CurrentFrame = (CurrentFrame + 1) % MaxFramesInFlight;
}

///////////////// APP /////////////////

void MainLoop()
{
	uint32_t FrameCount = 0;
	Uint32 LastFrameTime = SDL_GetTicks();
	Uint32 LastFPSTime = SDL_GetTicks();

	while (!Window.ShouldClose())
	{
		Uint32 CurrentFPSTime = SDL_GetTicks();
		FrameCount++;

		if (CurrentFPSTime > LastFPSTime + 1000)
		{
			LastFPSTime = CurrentFPSTime;
			CONSOLE_LOG(FrameCount << " fps");
			FrameCount = 0;
		}

		Uint32 CurrentFrameTime = SDL_GetTicks();
		Uint32 DeltaTime = CurrentFrameTime - LastFrameTime;

		DrawFrame(DeltaTime);

		LastFrameTime = CurrentFrameTime;

		Window.Update(DeltaTime);
	}
}

int main()
{
	Window.Create();
	StartVulkan();

	MainLoop();

	ShutdownVulkan();
	Window.Shutdown();

	return 0;
}
