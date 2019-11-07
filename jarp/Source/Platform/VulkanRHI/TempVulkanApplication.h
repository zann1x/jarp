#pragma once

#include "jarp/Camera.h"
#include "jarp/Renderer/CommandBuffer.h"
#include "jarp/Renderer/CommandPool.h"
#include "jarp/Renderer/Shader.h"
#include "jarp/Renderer/Swapchain.h"
#include "jarp/Renderer/VertexBuffer.h"
#include "Platform/VulkanRHI/Model.h"
#include "Platform/VulkanRHI/Texture.h"
#include "Platform/VulkanRHI/VulkanBuffer.h"
#include "Platform/VulkanRHI/VulkanCommandBuffer.h"
#include "Platform/VulkanRHI/VulkanCommandPool.h"
#include "Platform/VulkanRHI/VulkanFence.h"
#include "Platform/VulkanRHI/VulkanFramebuffer.h"
#include "Platform/VulkanRHI/VulkanGraphicsPipeline.h"
#include "Platform/VulkanRHI/VulkanImage.h"
#include "Platform/VulkanRHI/VulkanImageView.h"
#include "Platform/VulkanRHI/VulkanRenderPass.h"
#include "Platform/VulkanRHI/VulkanSemaphore.h"
#include "Platform/VulkanRHI/VulkanShader.h"
#include "Platform/VulkanRHI/VulkanUtils.hpp"

namespace jarp {

	class TempVulkanApplication
	{
	public:
		void StartVulkan();
		void Render(uint32_t deltaTime);
		void ShutdownVulkan();
	private:
		void RecordCommandBuffer();
		void RecreateSwapchain();
		void CleanupSwapchain();
		void UpdateMVP(uint32_t durrentImage);

	private:
		SUniformBufferObject m_UBO;

		std::shared_ptr<CommandPool> m_CommandPool;
		std::shared_ptr<VulkanCommandPool> m_TransientCommandPool;
		std::shared_ptr<VulkanCommandBuffer> m_TransientCommandBuffer;
		VulkanImage* m_DepthImage;
		VulkanImageView* m_DepthImageView;
		Model* m_Model;
		Texture* m_Texture;
		VulkanRenderPass* m_RenderPass;
		std::shared_ptr<Shader> m_Shader;
		std::shared_ptr<VulkanGraphicsPipeline> m_GraphicsPipeline;
		std::vector<VulkanFramebuffer*> m_Framebuffers;
		std::vector<std::shared_ptr<CommandBuffer>> m_DrawCommandBuffers;
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		std::vector<VulkanBuffer*> m_UniformBuffers;
		std::vector<VulkanSemaphore*> m_RenderingFinishedSemaphores;
		std::vector<VulkanSemaphore*> m_ImageAvailableSemaphores;
		std::vector<VulkanFence*> m_FencesInFlight;

		uint32_t m_MaxFramesInFlight;
		Camera m_Camera;
	};

}
