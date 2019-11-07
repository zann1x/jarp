#pragma once

#include <volk.h>

#include "jarp/Renderer/Swapchain.h"

namespace jarp {

	class Window;
	class VulkanImageView;

	class VulkanSwapchain : public Swapchain
	{
	public:
		struct SSwapchainSupportDetails
		{
			VkSurfaceCapabilitiesKHR SurfaceCapabilities;
			std::vector<VkSurfaceFormatKHR> SurfaceFormats;
			std::vector<VkPresentModeKHR> PresentModes;
		};

		struct SSwapchainDetails
		{
			VkSurfaceFormatKHR SurfaceFormat;
			VkExtent2D Extent;
		};

	public:
		VulkanSwapchain(uint32_t width, uint32_t height, bool bUseVSync);
		virtual ~VulkanSwapchain();
		void Destroy();

		inline const VkSwapchainKHR GetHandle() const { return m_Swapchain; }
		inline const VkSurfaceKHR GetSurfaceHandle() const { return m_SurfaceKHR; }
		inline const SSwapchainDetails GetDetails() const { return m_SwapchainDetails; }
		inline const std::vector<VkImage>& GetImages() const { return m_SwapchainImages; }
		inline const std::vector<VulkanImageView>& GetImageViews() const { return m_SwapchainImageViews; }
		inline const uint32_t GetActiveImageIndex() const { return m_ActiveImageIndex; }

		static SSwapchainSupportDetails QuerySwapchainSupport(VkPhysicalDevice device, VkSurfaceKHR surfaceKHR);

		VkResult AcquireNextImage(const VkSemaphore waitSemaphore);

	private:
		void CreateSwapchain(uint32_t width, uint32_t height, bool bUseVSync);

	private:
		VkSurfaceKHR m_SurfaceKHR;
		VkSwapchainKHR m_Swapchain;

		SSwapchainSupportDetails m_SwapchainSupportDetails;
		SSwapchainDetails m_SwapchainDetails;
		std::vector<VkImage> m_SwapchainImages;
		std::vector<VulkanImageView> m_SwapchainImageViews;

		uint32_t m_ActiveImageIndex;
	};

}
