#include "jarppch.h"
#include "TempVulkanApplication.h"

#include <volk.h>

#include "jarp/Application.h"
#include "jarp/Time.h"
#include "Platform/Windows/WindowsWindow.h"
#include "Platform/VulkanRHI/CommandBufferPool.h"
#include "Platform/VulkanRHI/VulkanRendererAPI.h"

namespace jarp {

	void TempVulkanApplication::StartVulkan()
	{
		VulkanRendererAPI::s_Instance = std::make_unique<VulkanInstance>();
		VulkanRendererAPI::s_Instance->CreateInstance();

		VulkanRendererAPI::s_Device = std::make_unique<VulkanDevice>();
		VulkanRendererAPI::s_Device->CreateLogicalDevice();

		m_Swapchain = new VulkanSwapchain();
		m_Swapchain->CreateSwapchain(Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight(), m_settings.VSync);
		m_Camera.SetAspectRatio(m_Swapchain->GetDetails().Extent.width / static_cast<float>(m_Swapchain->GetDetails().Extent.height));
		m_MaxFramesInFlight = static_cast<uint32_t>(m_Swapchain->GetImageViews().size());

		m_CommandPool.reset(CommandPool::Create());
		m_TransientCommandPool = std::make_shared<VulkanCommandPool>(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);
		m_TransientCommandBuffer = std::make_shared<VulkanCommandBuffer>(m_TransientCommandPool);

		m_DepthImage = new VulkanImage();
		m_DepthImageView = new VulkanImageView();
		VkFormat DepthFormat = VulkanRendererAPI::s_Device->FindDepthFormat();
		m_DepthImage->CreateImage(m_Swapchain->GetDetails().Extent.width, m_Swapchain->GetDetails().Extent.height, DepthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		m_DepthImageView->CreateImageView(m_DepthImage->GetHandle(), DepthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
		m_DepthImage->TransitionImageLayout(*m_TransientCommandBuffer, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

		m_DescriptorSetLayout = new VulkanDescriptorSetLayout();
		m_DescriptorSetLayout->AddLayout(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
		m_DescriptorSetLayout->AddLayout(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		m_DescriptorSetLayout->CreateDescriptorSetLayout();

		m_Model = new Model();
		m_Texture = new Texture();

		m_RenderPass = new VulkanRenderPass(*m_Swapchain);
		m_RenderPass->CreateRenderPass();
		m_Shader = new VulkanShader();
		m_Shader->AddDescriptorSetLayout(*m_DescriptorSetLayout);
		m_Shader->CreateShaderModule(VK_SHADER_STAGE_VERTEX_BIT, "E:/VisualStudioProjects/jarp/jarp/Shaders/Phong.vert.spv");
		m_Shader->CreateShaderModule(VK_SHADER_STAGE_FRAGMENT_BIT, "E:/VisualStudioProjects/jarp/jarp/Shaders/Phong.frag.spv");
		m_GraphicsPipeline = new VulkanGraphicsPipeline(*m_RenderPass, *m_Shader);
		m_GraphicsPipeline->CreateGraphicsPipeline(m_Model->GetPipelineVertexInputStateCreateInfo(), m_Swapchain->GetDetails().Extent);

		m_Framebuffers.resize(m_Swapchain->GetImages().size());
		for (size_t i = 0; i < m_Swapchain->GetImages().size(); ++i)
		{
			m_Framebuffers[i] = new VulkanFramebuffer(*m_RenderPass);
			m_Framebuffers[i]->CreateFramebuffer({ m_Swapchain->GetImageViews()[i].GetHandle(), m_DepthImageView->GetHandle() }, m_Swapchain->GetDetails().Extent);
		}

		m_DescriptorPool = new VulkanDescriptorPool(*m_Swapchain);
		m_DescriptorPool->CreateDescriptorPool();

		m_DrawCommandBuffers.resize(m_Framebuffers.size());
		for (size_t i = 0; i < m_Framebuffers.size(); ++i)
		{
			m_DrawCommandBuffers[i].reset(CommandBuffer::Create(m_CommandPool));
		}

		m_Model->Load("E:/VisualStudioProjects/jarp/jarp/Content/kitten.obj");
		m_Texture->Load(*m_TransientCommandBuffer, "E:/VisualStudioProjects/jarp/jarp/Content/texture.jpg");
		m_VertexBuffer.reset(VertexBuffer::Create(m_Model->GetVertices(), static_cast<uint32_t>(m_Model->GetVertices().size())));
		m_IndexBuffer.reset(IndexBuffer::Create(m_Model->GetIndices(), static_cast<uint32_t>(m_Model->GetIndices().size())));

		m_UniformBuffers.resize(m_Swapchain->GetImages().size());
		for (size_t i = 0; i < m_Swapchain->GetImages().size(); ++i)
		{
			m_UniformBuffers[i] = new VulkanBuffer(sizeof(m_UBO), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
			m_UniformBuffers[i]->CreateBuffer(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		}
		std::vector<VkBuffer> uniBuffers;
		for (auto& uniformBuffer : m_UniformBuffers)
			uniBuffers.push_back(uniformBuffer->GetHandle());
		m_DescriptorSet = new VulkanDescriptorSet();
		m_DescriptorSet->CreateDescriptorSets(*m_DescriptorSetLayout, *m_DescriptorPool, m_Swapchain->GetImages().size(), sizeof(m_UBO), uniBuffers, m_Texture->GetSampler(), m_Texture->GetImageView().GetHandle());

		m_RenderingFinishedSemaphores.resize(m_MaxFramesInFlight);
		m_ImageAvailableSemaphores.resize(m_MaxFramesInFlight);
		m_FencesInFlight.resize(m_MaxFramesInFlight);
		for (uint32_t i = 0; i < m_MaxFramesInFlight; ++i)
		{
			m_RenderingFinishedSemaphores[i] = new VulkanSemaphore();
			m_RenderingFinishedSemaphores[i]->CreateSemaphore();

			m_ImageAvailableSemaphores[i] = new VulkanSemaphore();
			m_ImageAvailableSemaphores[i]->CreateSemaphore();

			m_FencesInFlight[i] = new VulkanFence();
			m_FencesInFlight[i]->CreateFence();
		}

		RecordCommandBuffer();
	}
	
	void TempVulkanApplication::Render(uint32_t deltaTime)
	{
		static size_t currentFrame = 0;

		// Get the next available image to work on
		{
			VkResult result = m_Swapchain->AcquireNextImage(m_ImageAvailableSemaphores[currentFrame]->GetHandle());
			if (result == VK_ERROR_OUT_OF_DATE_KHR)
			{
				RecreateSwapchain();
				return;
			}
			else
			{
				VK_ASSERT(result);
			}
		}

		m_Camera.Move(deltaTime);
		UpdateMVP(m_Swapchain->GetActiveImageIndex());

		// Submit commands to the queue
		VulkanRendererAPI::s_Device->GetGraphicsQueue().QueueSubmitAndWait(
			{ std::dynamic_pointer_cast<VulkanCommandBuffer>(m_DrawCommandBuffers[m_Swapchain->GetActiveImageIndex()])->GetHandle() },
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			{ m_ImageAvailableSemaphores[currentFrame]->GetHandle() },
			{ m_RenderingFinishedSemaphores[currentFrame]->GetHandle() },
			m_FencesInFlight[currentFrame]->GetHandle(),
			{ m_FencesInFlight[currentFrame]->GetHandle() }
		);

		{
			VkResult result = VulkanRendererAPI::s_Device->GetPresentQueue().QueuePresent(
				m_Swapchain->GetHandle(),
				{ m_Swapchain->GetActiveImageIndex() },
				{ m_RenderingFinishedSemaphores[currentFrame]->GetHandle() }
			);
			// TODO: register Renderer for WindowResizedEvent to omit explicitly checking for resize here
			WindowsWindow& windowsWindow = static_cast<WindowsWindow&>(Application::Get().GetWindow());
			if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || windowsWindow.IsFramebufferResized())
			{
				windowsWindow.SetFramebufferResized(false);
				RecreateSwapchain();
			}
			else
			{
				VK_ASSERT(result);
			}
		}

		currentFrame = (currentFrame + 1) % m_MaxFramesInFlight;
	}
	
	void TempVulkanApplication::ShutdownVulkan()
	{
		// Free all resources
		VulkanRendererAPI::s_Device->WaitUntilIdle();

		CleanupSwapchain();
		delete m_RenderPass;
		delete m_Swapchain;

		delete m_DepthImageView;
		delete m_DepthImage;

		m_Shader->Destroy();
		delete m_Shader;

		m_Texture->Destroy();
		delete m_Texture;
		delete m_Model;

		for (uint32_t i = 0; i < m_MaxFramesInFlight; ++i)
		{
			m_FencesInFlight[i]->Destroy();
			delete m_FencesInFlight[i];
			m_RenderingFinishedSemaphores[i]->Destroy();
			delete m_RenderingFinishedSemaphores[i];
			m_ImageAvailableSemaphores[i]->Destroy();
			delete m_ImageAvailableSemaphores[i];
		}

		// Even though the uniform buffer depends on the number of swapchain images, it seems that it doesn't need to be recreated with the swapchain
		for (auto& uniformBuffer : m_UniformBuffers)
		{
			uniformBuffer->Destroy();
			delete uniformBuffer;
		}
		std::dynamic_pointer_cast<VulkanIndexBuffer>(m_IndexBuffer)->Destroy();
		std::dynamic_pointer_cast<VulkanVertexBuffer>(m_VertexBuffer)->Destroy();

		m_TransientCommandPool->Destroy();
		std::dynamic_pointer_cast<VulkanCommandPool>(m_CommandPool)->Destroy();
		CommandBufferPool::Get()->Destroy();

		m_DescriptorPool->Destroy();
		delete m_DescriptorPool;
		delete m_DescriptorSet; // Vulkan objects are implicitly destroyed by the owning pool
		m_DescriptorSetLayout->Destroy();
		delete m_DescriptorSetLayout;

		VulkanRendererAPI::s_Device->Destroy();
		VulkanRendererAPI::s_Instance->Destroy();
	}

	void TempVulkanApplication::RecordCommandBuffer()
	{
		for (size_t i = 0; i < m_DrawCommandBuffers.size(); ++i)
		{
			std::shared_ptr<VulkanCommandBuffer> vulkanDrawCommandBuffer = std::dynamic_pointer_cast<VulkanCommandBuffer>(m_DrawCommandBuffers[i]);
			const VkCommandBuffer& commandBuffer = vulkanDrawCommandBuffer->GetHandle();

			VkCommandBufferBeginInfo commandBufferBeginInfo = {};
			commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			commandBufferBeginInfo.pNext = nullptr;
			commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
			commandBufferBeginInfo.pInheritanceInfo = nullptr; // This is a primary command buffer, so the value can be ignored

			VK_ASSERT(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo));

			std::array<VkClearValue, 2> clearValues = {};
			clearValues[0] = { 48.0f / 255.0f, 10.0f / 255.0f, 36.0f / 255.0f, 1.0f };
			clearValues[1] = { 1.0f, 0.0f }; // Initial value should be the furthest possible depth (= 1.0)

			VkRenderPassBeginInfo renderPassBeginInfo = {};
			renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassBeginInfo.pNext = nullptr;
			renderPassBeginInfo.renderPass = m_RenderPass->GetHandle();
			renderPassBeginInfo.framebuffer = m_Framebuffers[i]->GetHandle();
			renderPassBeginInfo.renderArea.extent = m_Swapchain->GetDetails().Extent;
			renderPassBeginInfo.renderArea.offset = { 0, 0 };
			renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassBeginInfo.pClearValues = clearValues.data();

			vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE); // We only have primary command buffers, so an inline subpass suffices

			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline->GetHandle());

			m_VertexBuffer->Bind(vulkanDrawCommandBuffer);
			m_IndexBuffer->Bind(vulkanDrawCommandBuffer);
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline->GetLayoutHandle(), 0, 1, &m_DescriptorSet->At(i), 0, nullptr);

			vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_Model->GetIndices().size()), 1, 0, 0, 0);

			vkCmdEndRenderPass(commandBuffer);

			VK_ASSERT(vkEndCommandBuffer(commandBuffer));
		}
	}

	void TempVulkanApplication::RecreateSwapchain()
	{
		// If the window is minimized, the framebuffer size should theoretically be 0
		auto [framebufferWidth, framebufferHeight] = Application::Get().GetWindow().GetFramebufferSize();
		if (framebufferWidth == 0 || framebufferHeight == 0 || Application::Get().GetWindow().IsMinimized())
		{
			return;
		}

		VulkanRendererAPI::s_Device->WaitUntilIdle();
		CleanupSwapchain();

		m_Swapchain->CreateSwapchain(framebufferWidth, framebufferHeight, m_settings.VSync);
		m_Camera.SetAspectRatio(m_Swapchain->GetDetails().Extent.width / static_cast<float>(m_Swapchain->GetDetails().Extent.height));
		m_MaxFramesInFlight = static_cast<uint32_t>(m_Swapchain->GetImageViews().size());

		VkFormat depthFormat = VulkanRendererAPI::s_Device->FindDepthFormat();
		m_DepthImage->CreateImage(m_Swapchain->GetDetails().Extent.width, m_Swapchain->GetDetails().Extent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		m_DepthImageView->CreateImageView(m_DepthImage->GetHandle(), depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
		m_TransientCommandBuffer = std::make_shared<VulkanCommandBuffer>(m_TransientCommandPool);
		m_DepthImage->TransitionImageLayout(*m_TransientCommandBuffer, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

		m_RenderPass->CreateRenderPass();
		m_GraphicsPipeline->CreateGraphicsPipeline(m_Model->GetPipelineVertexInputStateCreateInfo(), m_Swapchain->GetDetails().Extent);

		for (size_t i = 0; i < m_Swapchain->GetImages().size(); ++i)
		{
			m_Framebuffers[i]->CreateFramebuffer({ m_Swapchain->GetImageViews()[i].GetHandle(), m_DepthImageView->GetHandle() }, m_Swapchain->GetDetails().Extent);
		}

		for (size_t i = 0; i < m_Framebuffers.size(); ++i)
		{
			m_DrawCommandBuffers[i].reset(CommandBuffer::Create(m_CommandPool));
		}

		RecordCommandBuffer();
	}

	void TempVulkanApplication::CleanupSwapchain()
	{
		for (auto& framebuffer : m_Framebuffers)
			framebuffer->Destroy();
		m_DepthImageView->Destroy();
		m_DepthImage->Destroy();
		m_GraphicsPipeline->Destroy();
		m_RenderPass->Destroy();
		m_Swapchain->Destroy();
	}

	void TempVulkanApplication::UpdateMVP(uint32_t CurrentImage)
	{
		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float timePassed = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		m_UBO.Model = glm::mat4();
		m_UBO.Model = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		m_UBO.Model = glm::translate(m_UBO.Model, glm::vec3(0.0f, 0.0f, 0.0f));
		m_UBO.Model = glm::rotate(m_UBO.Model, timePassed * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		m_UBO.View = m_Camera.GetViewMatrix();
		m_UBO.Projection = m_Camera.GetProjectionMatrix();
		m_UBO.LightPosition = glm::vec3(10.0f, 100.0f, -100.0f);

		// Map uniform buffer data persistently
		static void* rawData[3];
		if (!rawData[CurrentImage])
			vkMapMemory(VulkanRendererAPI::s_Device->GetInstanceHandle(), m_UniformBuffers[CurrentImage]->GetMemoryHandle(), 0, sizeof(m_UBO), 0, &rawData[CurrentImage]);
		memcpy(rawData[CurrentImage], &m_UBO, sizeof(m_UBO));
	}

}
