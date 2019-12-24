#pragma once

#include "jarp/Camera.h"
#include "Platform/VulkanRHI/Model.h"
#include "Platform/VulkanRHI/Texture.h"
#include "Platform/VulkanRHI/VulkanBuffer.h"
#include "Platform/VulkanRHI/VulkanCommandBuffer.h"
#include "Platform/VulkanRHI/VulkanCommandPool.h"
#include "Platform/VulkanRHI/VulkanDescriptorPool.h"
#include "Platform/VulkanRHI/VulkanDescriptorSet.h"
#include "Platform/VulkanRHI/VulkanDescriptorSetLayout.h"
#include "Platform/VulkanRHI/VulkanFence.h"
#include "Platform/VulkanRHI/VulkanFramebuffer.h"
#include "Platform/VulkanRHI/VulkanGraphicsPipeline.h"
#include "Platform/VulkanRHI/VulkanImage.h"
#include "Platform/VulkanRHI/VulkanImageView.h"
#include "Platform/VulkanRHI/VulkanRenderPass.h"
#include "Platform/VulkanRHI/VulkanSemaphore.h"
#include "Platform/VulkanRHI/VulkanShader.h"
#include "Platform/VulkanRHI/VulkanSwapchain.h"
#include "Platform/VulkanRHI/VulkanUtils.hpp"

#include <vector>

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
		struct SSettings
		{
			bool VSync = false;
		};
		SSettings m_settings;

		struct SUniformBufferObject
		{
			alignas(16) glm::mat4 Model;
			alignas(16) glm::mat4 View;
			alignas(16) glm::mat4 Projection;
			alignas(16) glm::vec3 LightPosition;
		};
		SUniformBufferObject m_UBO;

		VulkanSwapchain* m_Swapchain;
		VulkanCommandPool* m_CommandPool;
		VulkanCommandPool* m_TransientCommandPool;
		VulkanCommandBuffer* m_TransientCommandBuffer;
		VulkanImage* m_DepthImage;
		VulkanImageView* m_DepthImageView;
		VulkanDescriptorSetLayout* m_DescriptorSetLayout;
		Model* m_Model;
		Texture* m_Texture;
		VulkanRenderPass* m_RenderPass;
		VulkanShader* m_Shader;
		VulkanGraphicsPipeline* m_GraphicsPipeline;
		std::vector<VulkanFramebuffer*> m_Framebuffers;
		VulkanDescriptorPool* m_DescriptorPool;
		std::vector<VulkanCommandBuffer*> m_DrawCommandBuffers;
		VulkanBuffer* m_VertexBuffer;
		VulkanBuffer* m_IndexBuffer;
		std::vector<VulkanBuffer*> m_UniformBuffers;
		VulkanDescriptorSet* m_DescriptorSet;
		std::vector<VulkanSemaphore*> m_RenderingFinishedSemaphores;
		std::vector<VulkanSemaphore*> m_ImageAvailableSemaphores;
		std::vector<VulkanFence*> m_FencesInFlight;

		uint32_t m_MaxFramesInFlight;
		Camera m_Camera;
	};

}
