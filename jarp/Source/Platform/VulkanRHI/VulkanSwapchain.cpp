#include "jarppch.h"
#include "VulkanSwapchain.h"

#include "VulkanDevice.h"
#include "VulkanImageView.h"
#include "VulkanUtils.hpp"

#include "jarp/Window.h"
#include "Platform/Windows/WindowsWindow.h"

namespace jarp {

	VulkanSwapchain::VulkanSwapchain(Window& Window, VkInstance Instance, VulkanDevice& Device)
		: Instance(Instance), Device(Device)
	{
		// Create surface
#if defined(VK_USE_PLATFORM_WIN32_KHR)
		WindowsWindow& Win32 = dynamic_cast<WindowsWindow&>(Window);

		VkWin32SurfaceCreateInfoKHR Win32SurfaceCreateInfoKHR = {};
		Win32SurfaceCreateInfoKHR.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		Win32SurfaceCreateInfoKHR.pNext = nullptr;
		Win32SurfaceCreateInfoKHR.flags = 0;
		Win32SurfaceCreateInfoKHR.hinstance = Win32.GetNativeInstanceHandle();
		Win32SurfaceCreateInfoKHR.hwnd = Win32.GetNativeWindowHandle();

		VK_ASSERT(vkCreateWin32SurfaceKHR(Instance, &Win32SurfaceCreateInfoKHR, nullptr, &SurfaceKHR));
#else
#error UNSUPPORTED PLATFORM
#endif

	// Create present queue from surface
		Device.SetupPresentQueue(SurfaceKHR);
	}

	VulkanSwapchain::~VulkanSwapchain()
	{
		Destroy();

		if (SurfaceKHR != VK_NULL_HANDLE)
		{
			vkDestroySurfaceKHR(Instance, SurfaceKHR, nullptr);
			SurfaceKHR = VK_NULL_HANDLE;
		}
	}

	void VulkanSwapchain::CreateSwapchain(uint32_t Width, uint32_t Height, bool bUseVSync)
	{
		SwapchainSupportDetails = QuerySwapchainSupport(Device.GetPhysicalHandle(), SurfaceKHR);

		// Check image count of swapchain
		// If max image count is 0 then there are no limits besides memory requirements
		uint32_t SwapchainMinImageCount = SwapchainSupportDetails.SurfaceCapabilities.minImageCount + 1;
		if (SwapchainSupportDetails.SurfaceCapabilities.maxImageCount > 0 && SwapchainMinImageCount > SwapchainSupportDetails.SurfaceCapabilities.maxImageCount)
			SwapchainMinImageCount = SwapchainSupportDetails.SurfaceCapabilities.maxImageCount;

		// Choose surface format
		SwapchainDetails.SurfaceFormat = SwapchainSupportDetails.SurfaceFormats[0];
		if (SwapchainSupportDetails.SurfaceFormats.size() == 1 && SwapchainSupportDetails.SurfaceFormats[0].format == VK_FORMAT_UNDEFINED)
		{
			SwapchainDetails.SurfaceFormat = { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
		}
		else
		{
			for (const auto& AvailableImageFormat : SwapchainSupportDetails.SurfaceFormats)
			{
				if (AvailableImageFormat.format == VK_FORMAT_B8G8R8A8_UNORM && AvailableImageFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				{
					SwapchainDetails.SurfaceFormat = AvailableImageFormat;
					break;
				}
			}
		}

		// Choose extent
		if (SwapchainSupportDetails.SurfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			SwapchainDetails.Extent = SwapchainSupportDetails.SurfaceCapabilities.currentExtent;
		}
		else
		{
			VkExtent2D ActualExtent = { static_cast<uint32_t>(Width), static_cast<uint32_t>(Height) };
			ActualExtent.width = std::max(SwapchainSupportDetails.SurfaceCapabilities.minImageExtent.width, std::min(SwapchainSupportDetails.SurfaceCapabilities.maxImageExtent.width, ActualExtent.width));
			ActualExtent.height = std::max(SwapchainSupportDetails.SurfaceCapabilities.minImageExtent.height, std::min(SwapchainSupportDetails.SurfaceCapabilities.maxImageExtent.height, ActualExtent.height));

			SwapchainDetails.Extent = ActualExtent;
		}

		// Choose present mode
		VkPresentModeKHR PresentMode = VK_PRESENT_MODE_FIFO_KHR;
		if (!bUseVSync)
		{
			for (const auto& AvailablePresentMode : SwapchainSupportDetails.PresentModes)
			{
				if (AvailablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
				{
					PresentMode = AvailablePresentMode;
					break;
				}
				else if (AvailablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
				{
					PresentMode = AvailablePresentMode;
				}
			}
		}

		// Create swapchain
		VkSwapchainCreateInfoKHR SwapchainCreateInfoKHR = {};
		SwapchainCreateInfoKHR.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		SwapchainCreateInfoKHR.pNext = nullptr;
		SwapchainCreateInfoKHR.flags = 0;
		SwapchainCreateInfoKHR.surface = SurfaceKHR;
		SwapchainCreateInfoKHR.minImageCount = SwapchainMinImageCount;
		SwapchainCreateInfoKHR.imageFormat = SwapchainDetails.SurfaceFormat.format;
		SwapchainCreateInfoKHR.imageColorSpace = SwapchainDetails.SurfaceFormat.colorSpace;
		SwapchainCreateInfoKHR.imageExtent = SwapchainDetails.Extent;
		SwapchainCreateInfoKHR.imageArrayLayers = SwapchainSupportDetails.SurfaceCapabilities.maxImageArrayLayers;
		SwapchainCreateInfoKHR.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		SwapchainCreateInfoKHR.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		SwapchainCreateInfoKHR.queueFamilyIndexCount = 0;
		SwapchainCreateInfoKHR.pQueueFamilyIndices = nullptr;

		// Prefer non rotated transforms
		if (SwapchainSupportDetails.SurfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
			SwapchainCreateInfoKHR.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		else
			SwapchainCreateInfoKHR.preTransform = SwapchainSupportDetails.SurfaceCapabilities.currentTransform;

		// Find a composite alpha to use as not all devices support alpha opaque
		SwapchainCreateInfoKHR.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		std::vector<VkCompositeAlphaFlagBitsKHR> CompositeAlphaFlags = {
			VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR
		};
		for (auto& CompositeAlpha : CompositeAlphaFlags)
		{
			// Select the first one that is supoorted
			if (SwapchainSupportDetails.SurfaceCapabilities.supportedCompositeAlpha & CompositeAlpha)
			{
				SwapchainCreateInfoKHR.compositeAlpha = CompositeAlpha;
				break;
			}
		}

		SwapchainCreateInfoKHR.presentMode = PresentMode;
		SwapchainCreateInfoKHR.clipped = VK_TRUE;
		SwapchainCreateInfoKHR.oldSwapchain = VK_NULL_HANDLE;

		VK_ASSERT(vkCreateSwapchainKHR(Device.GetInstanceHandle(), &SwapchainCreateInfoKHR, nullptr, &Swapchain));

		// Get images from the created swapchain
		uint32_t SwapchainImageCount;
		vkGetSwapchainImagesKHR(Device.GetInstanceHandle(), Swapchain, &SwapchainImageCount, nullptr);
		SwapchainImages.resize(SwapchainImageCount);
		vkGetSwapchainImagesKHR(Device.GetInstanceHandle(), Swapchain, &SwapchainImageCount, SwapchainImages.data());

		// Create image views
		SwapchainImageViews.reserve(SwapchainImages.size());
		for (size_t i = 0; i < SwapchainImages.size(); ++i)
		{
			SwapchainImageViews.push_back(VulkanImageView(Device));
			SwapchainImageViews[i].CreateImageView(SwapchainImages[i], SwapchainDetails.SurfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT);
		}
	}

	void VulkanSwapchain::Destroy()
	{
		if (!SwapchainImageViews.empty())
		{
			for (auto& ImageView : SwapchainImageViews)
			{
				ImageView.Destroy();
				//vkDestroyImageView(Device.GetInstanceHandle(), ImageView, nullptr);
			}
			SwapchainImageViews.clear();
		}

		if (Swapchain != VK_NULL_HANDLE)
		{
			vkDestroySwapchainKHR(Device.GetInstanceHandle(), Swapchain, nullptr);
			Swapchain = VK_NULL_HANDLE;
		}
	}

	VulkanSwapchain::SSwapchainSupportDetails VulkanSwapchain::QuerySwapchainSupport(VkPhysicalDevice Device, VkSurfaceKHR SurfaceKHR)
	{
		SSwapchainSupportDetails SupportDetails = {};

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Device, SurfaceKHR, &SupportDetails.SurfaceCapabilities);

		uint32_t SurfaceFormatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(Device, SurfaceKHR, &SurfaceFormatCount, nullptr);
		SupportDetails.SurfaceFormats.resize(SurfaceFormatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(Device, SurfaceKHR, &SurfaceFormatCount, SupportDetails.SurfaceFormats.data());

		uint32_t PresentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(Device, SurfaceKHR, &PresentModeCount, nullptr);
		SupportDetails.PresentModes.resize(PresentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(Device, SurfaceKHR, &PresentModeCount, SupportDetails.PresentModes.data());

		return SupportDetails;
	}

	VkResult VulkanSwapchain::AcquireNextImage(const VkSemaphore WaitSemaphore)
	{
		return vkAcquireNextImageKHR(Device.GetInstanceHandle(), Swapchain, std::numeric_limits<uint64_t>::max(), WaitSemaphore, VK_NULL_HANDLE, &ActiveImageIndex);
	}

}
