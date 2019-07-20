#pragma once

#include <vulkan/vulkan.h>

namespace jarp {

	class WindowsWindow;
	class VulkanDevice;
	class VulkanImageView;

	/*
	Depends on:
	- PhysicalDevice
	- Device
	- SurfaceKHR
	*/
	class VulkanSwapchain
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
		VulkanSwapchain(WindowsWindow& Window, VkInstance Instance, VulkanDevice& Device);
		~VulkanSwapchain();

		void CreateSwapchain(uint32_t Width, uint32_t Height, bool bUseVSync = true);
		void Destroy();

		inline const VkSwapchainKHR GetHandle() const { return Swapchain; }
		inline const VkSurfaceKHR GetSurfaceHandle() const { return SurfaceKHR; }
		inline const SSwapchainDetails GetDetails() const { return SwapchainDetails; }
		inline const std::vector<VkImage>& GetImages() const { return SwapchainImages; }
		inline const std::vector<VulkanImageView>& GetImageViews() const { return SwapchainImageViews; }
		inline const uint32_t GetActiveImageIndex() const { return ActiveImageIndex; }

		static SSwapchainSupportDetails QuerySwapchainSupport(VkPhysicalDevice Device, VkSurfaceKHR SurfaceKHR);

		VkResult AcquireNextImage(const VkSemaphore WaitSemaphore);

	private:
		VkInstance Instance;
		VulkanDevice& Device;
		VkSurfaceKHR SurfaceKHR;
		VkSwapchainKHR Swapchain;

		SSwapchainSupportDetails SwapchainSupportDetails;
		SSwapchainDetails SwapchainDetails;
		std::vector<VkImage> SwapchainImages;
		std::vector<VulkanImageView> SwapchainImageViews;

		uint32_t ActiveImageIndex;
	};

}
