#include "jarppch.h"
#include "VulkanSwapchain.h"

#include "VulkanRendererAPI.h"
#include "VulkanImageView.h"
#include "VulkanUtils.hpp"

#include "jarp/Window.h"
#include "Platform/Windows/WindowsWindow.h"

namespace jarp {

	VulkanSwapchain::VulkanSwapchain()
		: m_SurfaceKHR(VK_NULL_HANDLE), m_Swapchain(VK_NULL_HANDLE)
	{
	}

	VulkanSwapchain::~VulkanSwapchain()
	{
		Destroy();

		if (m_SurfaceKHR != VK_NULL_HANDLE)
		{
			vkDestroySurfaceKHR(VulkanRendererAPI::s_Instance->GetHandle(), m_SurfaceKHR, nullptr);
			m_SurfaceKHR = VK_NULL_HANDLE;
		}
	}

	void VulkanSwapchain::CreateSwapchain(uint32_t width, uint32_t height, bool bUseVSync)
	{
		m_SwapchainSupportDetails = QuerySwapchainSupport(VulkanRendererAPI::s_Device->GetPhysicalHandle(), VulkanRendererAPI::s_Device->GetSurface().GetHandle());

		// Check image count of swapchain
		// If max image count is 0 then there are no limits besides memory requirements
		uint32_t swapchainMinImageCount = m_SwapchainSupportDetails.SurfaceCapabilities.minImageCount + 1;
		if (m_SwapchainSupportDetails.SurfaceCapabilities.maxImageCount > 0 && swapchainMinImageCount > m_SwapchainSupportDetails.SurfaceCapabilities.maxImageCount)
			swapchainMinImageCount = m_SwapchainSupportDetails.SurfaceCapabilities.maxImageCount;

		// Choose surface format
		m_SwapchainDetails.SurfaceFormat = m_SwapchainSupportDetails.SurfaceFormats[0];
		if (m_SwapchainSupportDetails.SurfaceFormats.size() == 1 && m_SwapchainSupportDetails.SurfaceFormats[0].format == VK_FORMAT_UNDEFINED)
		{
			m_SwapchainDetails.SurfaceFormat = { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
		}
		else
		{
			for (const auto& availableImageFormat : m_SwapchainSupportDetails.SurfaceFormats)
			{
				if (availableImageFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableImageFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				{
					m_SwapchainDetails.SurfaceFormat = availableImageFormat;
					break;
				}
			}
		}

		// Choose extent
		if (m_SwapchainSupportDetails.SurfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			m_SwapchainDetails.Extent = m_SwapchainSupportDetails.SurfaceCapabilities.currentExtent;
		}
		else
		{
			VkExtent2D actualExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
			actualExtent.width = std::max(m_SwapchainSupportDetails.SurfaceCapabilities.minImageExtent.width, std::min(m_SwapchainSupportDetails.SurfaceCapabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(m_SwapchainSupportDetails.SurfaceCapabilities.minImageExtent.height, std::min(m_SwapchainSupportDetails.SurfaceCapabilities.maxImageExtent.height, actualExtent.height));

			m_SwapchainDetails.Extent = actualExtent;
		}

		// Choose present mode
		VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
		if (!bUseVSync)
		{
			for (const auto& availablePresentMode : m_SwapchainSupportDetails.PresentModes)
			{
				if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
				{
					presentMode = availablePresentMode;
					break;
				}
				else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
				{
					presentMode = availablePresentMode;
				}
			}
		}

		// Create swapchain
		VkSwapchainCreateInfoKHR swapchainCreateInfoKHR = {};
		swapchainCreateInfoKHR.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainCreateInfoKHR.pNext = nullptr;
		swapchainCreateInfoKHR.flags = 0;
		swapchainCreateInfoKHR.surface = VulkanRendererAPI::s_Device->GetSurface().GetHandle();
		swapchainCreateInfoKHR.minImageCount = swapchainMinImageCount;
		swapchainCreateInfoKHR.imageFormat = m_SwapchainDetails.SurfaceFormat.format;
		swapchainCreateInfoKHR.imageColorSpace = m_SwapchainDetails.SurfaceFormat.colorSpace;
		swapchainCreateInfoKHR.imageExtent = m_SwapchainDetails.Extent;
		swapchainCreateInfoKHR.imageArrayLayers = m_SwapchainSupportDetails.SurfaceCapabilities.maxImageArrayLayers;
		swapchainCreateInfoKHR.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapchainCreateInfoKHR.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchainCreateInfoKHR.queueFamilyIndexCount = 0;
		swapchainCreateInfoKHR.pQueueFamilyIndices = nullptr;

		// Prefer non rotated transforms
		if (m_SwapchainSupportDetails.SurfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
			swapchainCreateInfoKHR.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		else
			swapchainCreateInfoKHR.preTransform = m_SwapchainSupportDetails.SurfaceCapabilities.currentTransform;

		// Find a composite alpha to use as not all devices support alpha opaque
		swapchainCreateInfoKHR.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		std::vector<VkCompositeAlphaFlagBitsKHR> compositeAlphaFlags = {
			VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR
		};
		for (auto& compositeAlpha : compositeAlphaFlags)
		{
			// Select the first one that is supported
			if (m_SwapchainSupportDetails.SurfaceCapabilities.supportedCompositeAlpha & compositeAlpha)
			{
				swapchainCreateInfoKHR.compositeAlpha = compositeAlpha;
				break;
			}
		}

		swapchainCreateInfoKHR.presentMode = presentMode;
		swapchainCreateInfoKHR.clipped = VK_TRUE;
		swapchainCreateInfoKHR.oldSwapchain = VK_NULL_HANDLE;

		VkDevice device = VulkanRendererAPI::s_Device->GetInstanceHandle();
		VK_ASSERT(vkCreateSwapchainKHR(VulkanRendererAPI::s_Device->GetInstanceHandle(), &swapchainCreateInfoKHR, nullptr, &m_Swapchain));

		// Get images from the created swapchain
		uint32_t swapchainImageCount;
		vkGetSwapchainImagesKHR(VulkanRendererAPI::s_Device->GetInstanceHandle(), m_Swapchain, &swapchainImageCount, nullptr);
		m_SwapchainImages.resize(swapchainImageCount);
		vkGetSwapchainImagesKHR(VulkanRendererAPI::s_Device->GetInstanceHandle(), m_Swapchain, &swapchainImageCount, m_SwapchainImages.data());

		// Create image views
		m_SwapchainImageViews.reserve(m_SwapchainImages.size());
		for (size_t i = 0; i < m_SwapchainImages.size(); ++i)
		{
			m_SwapchainImageViews.push_back(VulkanImageView());
			m_SwapchainImageViews[i].CreateImageView(m_SwapchainImages[i], m_SwapchainDetails.SurfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT);
		}
	}

	void VulkanSwapchain::Destroy()
	{
		if (!m_SwapchainImageViews.empty())
		{
			for (auto& imageView : m_SwapchainImageViews)
			{
				imageView.Destroy();
			}
			m_SwapchainImageViews.clear();
		}

		if (m_Swapchain != VK_NULL_HANDLE)
		{
			vkDestroySwapchainKHR(VulkanRendererAPI::s_Device->GetInstanceHandle(), m_Swapchain, nullptr);
			m_Swapchain = VK_NULL_HANDLE;
		}
	}

	VulkanSwapchain::SSwapchainSupportDetails VulkanSwapchain::QuerySwapchainSupport(VkPhysicalDevice device, VkSurfaceKHR surfaceKHR)
	{
		SSwapchainSupportDetails supportDetails = {};

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surfaceKHR, &supportDetails.SurfaceCapabilities);

		uint32_t surfaceFormatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surfaceKHR, &surfaceFormatCount, nullptr);
		supportDetails.SurfaceFormats.resize(surfaceFormatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surfaceKHR, &surfaceFormatCount, supportDetails.SurfaceFormats.data());

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surfaceKHR, &presentModeCount, nullptr);
		supportDetails.PresentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surfaceKHR, &presentModeCount, supportDetails.PresentModes.data());

		return supportDetails;
	}

	VkResult VulkanSwapchain::AcquireNextImage(const VkSemaphore waitSemaphore)
	{
		return vkAcquireNextImageKHR(VulkanRendererAPI::s_Device->GetInstanceHandle(), m_Swapchain, std::numeric_limits<uint64_t>::max(), waitSemaphore, VK_NULL_HANDLE, &m_ActiveImageIndex);
	}

}
