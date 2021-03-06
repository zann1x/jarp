#include "jarppch.h"
#include "TempVulkanApplication.h"

#include <volk.h>

#include "jarp/Application.h"
#include "jarp/Time.h"
#include "Platform/Windows/WindowsWindow.h"
#include "Platform/VulkanRHI/VulkanRendererAPI.h"

namespace jarp {

	void TempVulkanApplication::StartVulkan()
	{
		VulkanRendererAPI::pInstance = std::make_unique<VulkanInstance>();
		VulkanRendererAPI::pInstance->CreateInstance();

		VulkanRendererAPI::pDevice = std::make_unique<VulkanDevice>();
		VulkanRendererAPI::pDevice->CreateLogicalDevice();

		pSwapchain = new VulkanSwapchain();
		pSwapchain->CreateSwapchain(Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight(), Settings.VSync);
		MyCamera.SetAspectRatio(pSwapchain->GetDetails().Extent.width / static_cast<float>(pSwapchain->GetDetails().Extent.height));
		MaxFramesInFlight = static_cast<uint32_t>(pSwapchain->GetImageViews().size());

		// TODO: replace all occurrences of plain device ptr with Renderer API object
		pCommandPool = new VulkanCommandPool();
		pCommandPool->CreateCommandPool();
		pTransientCommandPool = new VulkanCommandPool();
		pTransientCommandPool->CreateCommandPool(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);
		pTransientCommandBuffer = new VulkanCommandBuffer(*pTransientCommandPool);

		pDepthImage = new VulkanImage();
		pDepthImageView = new VulkanImageView();
		VkFormat DepthFormat = VulkanRendererAPI::pDevice->FindDepthFormat();
		pDepthImage->CreateImage(pSwapchain->GetDetails().Extent.width, pSwapchain->GetDetails().Extent.height, DepthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		pDepthImageView->CreateImageView(pDepthImage->GetHandle(), DepthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
		pDepthImage->TransitionImageLayout(*pTransientCommandBuffer, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

		pDescriptorSetLayout = new VulkanDescriptorSetLayout();
		pDescriptorSetLayout->AddLayout(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
		pDescriptorSetLayout->AddLayout(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		pDescriptorSetLayout->CreateDescriptorSetLayout();

		pModel = new Model();
		pTexture = new Texture();

		pRenderPass = new VulkanRenderPass(*pSwapchain);
		pRenderPass->CreateRenderPass();
		pShader = new VulkanShader();
		pShader->AddDescriptorSetLayout(*pDescriptorSetLayout);
		pShader->CreateShaderModule(VK_SHADER_STAGE_VERTEX_BIT, "E:/VisualStudioProjects/jarp-master/jarp/Shaders/Phong.vert.spv");
		pShader->CreateShaderModule(VK_SHADER_STAGE_FRAGMENT_BIT, "E:/VisualStudioProjects/jarp-master/jarp/Shaders/Phong.frag.spv");
		pGraphicsPipeline = new VulkanGraphicsPipeline(*pRenderPass, *pShader);
		pGraphicsPipeline->CreateGraphicsPipeline(pModel->GetPipelineVertexInputStateCreateInfo(), pSwapchain->GetDetails().Extent);

		pFramebuffers.resize(pSwapchain->GetImages().size());
		for (size_t i = 0; i < pSwapchain->GetImages().size(); ++i)
		{
			pFramebuffers[i] = new VulkanFramebuffer(*pRenderPass);
			pFramebuffers[i]->CreateFramebuffer({ pSwapchain->GetImageViews()[i].GetHandle(), pDepthImageView->GetHandle() }, pSwapchain->GetDetails().Extent);
		}

		pDescriptorPool = new VulkanDescriptorPool(*pSwapchain);
		pDescriptorPool->CreateDescriptorPool();

		pDrawCommandBuffers.resize(pFramebuffers.size());
		for (size_t i = 0; i < pFramebuffers.size(); ++i)
		{
			pDrawCommandBuffers[i] = new VulkanCommandBuffer(*pCommandPool);
			pDrawCommandBuffers[i]->CreateCommandBuffer();
		}

		pModel->Load("E:/VisualStudioProjects/jarp-master/jarp/Content/kitten.obj");
		pTexture->Load(*pTransientCommandBuffer, "E:/VisualStudioProjects/jarp-master/jarp/Content/texture.jpg");
		pVertexBuffer = new VulkanBuffer(pModel->GetVerticesDeviceSize(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
		pVertexBuffer->CreateBuffer(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		pVertexBuffer->UploadBuffer(*pTransientCommandBuffer, pModel->GetVertices());
		pIndexBuffer = new VulkanBuffer(pModel->GetIndicesDeviceSize(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
		pIndexBuffer->CreateBuffer(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		pIndexBuffer->UploadBuffer(*pTransientCommandBuffer, pModel->GetIndices());
		delete pTransientCommandBuffer;

		UniformBuffers.resize(pSwapchain->GetImages().size());
		for (size_t i = 0; i < pSwapchain->GetImages().size(); ++i)
		{
			UniformBuffers[i] = new VulkanBuffer(sizeof(UBO), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
			UniformBuffers[i]->CreateBuffer(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		}
		std::vector<VkBuffer> UniBuffers;
		for (auto& UniformBuffer : UniformBuffers)
			UniBuffers.push_back(UniformBuffer->GetHandle());
		pDescriptorSet = new VulkanDescriptorSet();
		pDescriptorSet->CreateDescriptorSets(*pDescriptorSetLayout, *pDescriptorPool, pSwapchain->GetImages().size(), sizeof(UBO), UniBuffers, pTexture->GetSampler(), pTexture->GetImageView().GetHandle());

		pRenderingFinishedSemaphores.resize(MaxFramesInFlight);
		pImageAvailableSemaphores.resize(MaxFramesInFlight);
		pFencesInFlight.resize(MaxFramesInFlight);
		for (uint32_t i = 0; i < MaxFramesInFlight; ++i)
		{
			pRenderingFinishedSemaphores[i] = new VulkanSemaphore();
			pRenderingFinishedSemaphores[i]->CreateSemaphore();

			pImageAvailableSemaphores[i] = new VulkanSemaphore();
			pImageAvailableSemaphores[i]->CreateSemaphore();

			pFencesInFlight[i] = new VulkanFence();
			pFencesInFlight[i]->CreateFence();
		}

		RecordCommandBuffer();
	}
	
	void TempVulkanApplication::Render(uint32_t DeltaTime)
	{
		static size_t CurrentFrame = 0;

		// Don't try to draw to a minimized window
		if (Application::Get().GetWindow().IsMinimized())
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
		VulkanRendererAPI::pDevice->GetGraphicsQueue().QueueSubmitAndWait(
			{ pDrawCommandBuffers[pSwapchain->GetActiveImageIndex()]->GetHandle() },
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			{ pImageAvailableSemaphores[CurrentFrame]->GetHandle() },
			{ pRenderingFinishedSemaphores[CurrentFrame]->GetHandle() },
			pFencesInFlight[CurrentFrame]->GetHandle(),
			{ pFencesInFlight[CurrentFrame]->GetHandle() }
		);

		{
			VkResult Result = VulkanRendererAPI::pDevice->GetPresentQueue().QueuePresent(
				pSwapchain->GetHandle(),
				{ pSwapchain->GetActiveImageIndex() },
				{ pRenderingFinishedSemaphores[CurrentFrame]->GetHandle() }
			);
			// TODO: register Renderer for WindowResizedEvent to omit explicitly checking for resize here
			WindowsWindow& Win = static_cast<WindowsWindow&>(Application::Get().GetWindow());
			if (Result == VK_ERROR_OUT_OF_DATE_KHR || Result == VK_SUBOPTIMAL_KHR || Win.IsFramebufferResized())
			{
				Win.SetFramebufferResized(false);
				RecreateSwapchain();
			}
			else
			{
				VK_ASSERT(Result);
			}
		}

		CurrentFrame = (CurrentFrame + 1) % MaxFramesInFlight;
	}
	
	void TempVulkanApplication::ShutdownVulkan()
	{
		// Free all resources
		VulkanRendererAPI::pDevice->WaitUntilIdle();

		CleanupSwapchain();
		delete pRenderPass;
		delete pSwapchain;
		for (auto& CommandBuffer : pDrawCommandBuffers)
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

		for (uint32_t i = 0; i < MaxFramesInFlight; ++i)
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

		VulkanRendererAPI::pDevice->Destroy();
		VulkanRendererAPI::pInstance->Destroy();
	}

	void TempVulkanApplication::RecordCommandBuffer()
	{
		for (size_t i = 0; i < pDrawCommandBuffers.size(); ++i)
		{
			const VkCommandBuffer& CommandBuffer = pDrawCommandBuffers[i]->GetHandle();

			VkCommandBufferBeginInfo CommandBufferBeginInfo = {};
			CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			CommandBufferBeginInfo.pNext = nullptr;
			CommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
			CommandBufferBeginInfo.pInheritanceInfo = nullptr; // This is a primary command buffer, so the value can be ignored

			VK_ASSERT(vkBeginCommandBuffer(CommandBuffer, &CommandBufferBeginInfo));

			std::array<VkClearValue, 2> ClearValues = {};
			ClearValues[0] = { 48.0f / 255.0f, 10.0f / 255.0f, 36.0f / 255.0f, 1.0f };
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

	void TempVulkanApplication::RecreateSwapchain()
	{
		// If the window is minimized, the framebuffer size will be 0
		auto [FramebufferWidth, FramebufferHeight] = Application::Get().GetWindow().GetFramebufferSize();
		while (FramebufferWidth == 0 || FramebufferHeight == 0)
		{
			auto [FramebufferWidth, FramebufferHeight] = Application::Get().GetWindow().GetFramebufferSize();
			SDL_WaitEvent(nullptr);
		}

		VulkanRendererAPI::pDevice->WaitUntilIdle();
		CleanupSwapchain();

		pSwapchain->CreateSwapchain(FramebufferWidth, FramebufferHeight, Settings.VSync);
		MyCamera.SetAspectRatio(pSwapchain->GetDetails().Extent.width / static_cast<float>(pSwapchain->GetDetails().Extent.height));
		MaxFramesInFlight = static_cast<uint32_t>(pSwapchain->GetImageViews().size());

		VkFormat DepthFormat = VulkanRendererAPI::pDevice->FindDepthFormat();
		pDepthImage->CreateImage(pSwapchain->GetDetails().Extent.width, pSwapchain->GetDetails().Extent.height, DepthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		pDepthImageView->CreateImageView(pDepthImage->GetHandle(), DepthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
		pTransientCommandBuffer = new VulkanCommandBuffer(*pTransientCommandPool);
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
			pDrawCommandBuffers[i]->CreateCommandBuffer();
		}

		RecordCommandBuffer();
	}

	void TempVulkanApplication::CleanupSwapchain()
	{
		for (auto & pDrawCommandBuffer : pDrawCommandBuffers)
			pDrawCommandBuffer->Destroy();
		for (auto& Framebuffer : pFramebuffers)
			Framebuffer->Destroy();
		pDepthImageView->Destroy();
		pDepthImage->Destroy();
		pGraphicsPipeline->Destroy();
		pRenderPass->Destroy();
		pSwapchain->Destroy();
	}

	void TempVulkanApplication::UpdateMVP(uint32_t CurrentImage)
	{
		static auto StartTime = std::chrono::high_resolution_clock::now();

		auto CurrentTime = std::chrono::high_resolution_clock::now();
		float TimePassed = std::chrono::duration<float, std::chrono::seconds::period>(CurrentTime - StartTime).count();

		UBO.Model = glm::mat4();
		UBO.Model = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		UBO.Model = glm::translate(UBO.Model, glm::vec3(0.0f, 0.0f, 0.0f));
		UBO.Model = glm::rotate(UBO.Model, TimePassed * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		UBO.View = MyCamera.GetViewMatrix();
		UBO.Projection = MyCamera.GetProjectionMatrix();
		UBO.LightPosition = glm::vec3(10.0f, 100.0f, -100.0f);

		// Map uniform buffer data persistently
		static void* RawData[3];
		if (!RawData[CurrentImage])
			vkMapMemory(VulkanRendererAPI::pDevice->GetInstanceHandle(), UniformBuffers[CurrentImage]->GetMemoryHandle(), 0, sizeof(UBO), 0, &RawData[CurrentImage]);
		memcpy(RawData[CurrentImage], &UBO, sizeof(UBO));
	}

}
