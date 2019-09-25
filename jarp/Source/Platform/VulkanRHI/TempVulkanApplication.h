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

namespace jarp {

	class TempVulkanApplication
	{
	public:
		void StartVulkan();
		void Render(uint32_t DeltaTime);
		void ShutdownVulkan();
	private:
		void RecordCommandBuffer();
		void RecreateSwapchain();
		void CleanupSwapchain();
		void UpdateMVP(uint32_t CurrentImage);

	private:
		struct SSettings
		{
			bool VSync = false;
		};
		SSettings Settings;

		struct SUniformBufferObject
		{
			alignas(16) glm::mat4 Model;
			alignas(16) glm::mat4 View;
			alignas(16) glm::mat4 Projection;
			alignas(16) glm::vec3 LightPosition;
		};
		SUniformBufferObject UBO;

		VulkanSwapchain* pSwapchain;
		VulkanCommandPool* pCommandPool;
		VulkanCommandPool* pTransientCommandPool;
		VulkanCommandBuffer* pTransientCommandBuffer;
		VulkanImage* pDepthImage;
		VulkanImageView* pDepthImageView;
		VulkanDescriptorSetLayout* pDescriptorSetLayout;
		Model* pModel;
		Texture* pTexture;
		VulkanRenderPass* pRenderPass;
		VulkanShader* pShader;
		VulkanGraphicsPipeline* pGraphicsPipeline;
		std::vector<VulkanFramebuffer*> pFramebuffers;
		VulkanDescriptorPool* pDescriptorPool;
		std::vector<VulkanCommandBuffer*> pDrawCommandBuffers;
		VulkanBuffer* pVertexBuffer;
		VulkanBuffer* pIndexBuffer;
		std::vector<VulkanBuffer*> UniformBuffers;
		VulkanDescriptorSet* pDescriptorSet;
		std::vector<VulkanSemaphore*> pRenderingFinishedSemaphores;
		std::vector<VulkanSemaphore*> pImageAvailableSemaphores;
		std::vector<VulkanFence*> pFencesInFlight;

		uint32_t MaxFramesInFlight;
		Camera MyCamera;
	};

}
