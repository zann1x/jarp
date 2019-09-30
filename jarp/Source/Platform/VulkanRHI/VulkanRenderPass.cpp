#include "jarppch.h"
#include "VulkanRenderPass.h"

#include "VulkanRendererAPI.h"
#include "VulkanSwapchain.h"
#include "VulkanUtils.hpp"

namespace jarp {

	VulkanRenderPass::VulkanRenderPass(VulkanSwapchain& OutSwapchain)
		: Swapchain(OutSwapchain)
	{
	}

	VulkanRenderPass::~VulkanRenderPass()
	{
	}

	void VulkanRenderPass::CreateRenderPass()
	{
		std::array<VkAttachmentDescription, 2> AttachmentDescriptions;
		// Color attachment
		AttachmentDescriptions[0] = {};
		AttachmentDescriptions[0].flags = 0;
		AttachmentDescriptions[0].format = Swapchain.GetDetails().SurfaceFormat.format;
		AttachmentDescriptions[0].samples = VK_SAMPLE_COUNT_1_BIT;
		AttachmentDescriptions[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		AttachmentDescriptions[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		AttachmentDescriptions[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		AttachmentDescriptions[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		AttachmentDescriptions[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // The image will automatically be transitioned from UNDEFINED to COLOR_ATTACHMENT_OPTIMAL for rendering, then out to PRESENT_SRC_KHR at the end.
		AttachmentDescriptions[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		// Depth attachment
		AttachmentDescriptions[1] = {};
		AttachmentDescriptions[1].flags = 0;
		AttachmentDescriptions[1].format = VulkanRendererAPI::pDevice->FindDepthFormat();
		AttachmentDescriptions[1].samples = VK_SAMPLE_COUNT_1_BIT;
		AttachmentDescriptions[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		AttachmentDescriptions[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		AttachmentDescriptions[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		AttachmentDescriptions[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		AttachmentDescriptions[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		AttachmentDescriptions[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference ColorAttachmentReference = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
		VkAttachmentReference DepthAttachmentReference = { 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };

		VkSubpassDescription SubpassDescription = {};
		SubpassDescription.flags = 0;
		SubpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		SubpassDescription.inputAttachmentCount = 0;
		SubpassDescription.pInputAttachments = nullptr;
		SubpassDescription.colorAttachmentCount = 1;
		SubpassDescription.pColorAttachments = &ColorAttachmentReference;
		SubpassDescription.pResolveAttachments = nullptr;
		SubpassDescription.pDepthStencilAttachment = &DepthAttachmentReference;
		SubpassDescription.preserveAttachmentCount = 0;
		SubpassDescription.pPreserveAttachments = nullptr;

		VkSubpassDependency SubpassDependency = {};
		SubpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		SubpassDependency.dstSubpass = 0; // Refers to our one and only subpass
		SubpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // Needs to be pWaitDstStageMask in the WSI semaphore
		SubpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		SubpassDependency.srcAccessMask = 0;
		SubpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		SubpassDependency.dependencyFlags = 0;

		/*
		Normally, we would need an external dependency at the end as well since we are changing layout in finalLayout,
		but since we are signaling a semaphore, we can rely on Vulkan's default behavior,
		which injects an external dependency here with
			dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			dstAccessMask = 0
		*/

		VkRenderPassCreateInfo RenderPassCreateInfo = {};
		RenderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		RenderPassCreateInfo.pNext = nullptr;
		RenderPassCreateInfo.flags = 0;
		RenderPassCreateInfo.attachmentCount = static_cast<uint32_t>(AttachmentDescriptions.size());
		RenderPassCreateInfo.pAttachments = AttachmentDescriptions.data();
		RenderPassCreateInfo.subpassCount = 1;
		RenderPassCreateInfo.pSubpasses = &SubpassDescription;
		RenderPassCreateInfo.dependencyCount = 1;
		RenderPassCreateInfo.pDependencies = &SubpassDependency;

		VK_ASSERT(vkCreateRenderPass(VulkanRendererAPI::pDevice->GetInstanceHandle(), &RenderPassCreateInfo, nullptr, &RenderPass));
	}

	void VulkanRenderPass::Destroy()
	{
		vkDestroyRenderPass(VulkanRendererAPI::pDevice->GetInstanceHandle(), RenderPass, nullptr);
	}

}
