#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class CrossPlatformWindow;
class VulkanDevice;

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
	VulkanSwapchain(CrossPlatformWindow& OutWindow, VkInstance Instance, VulkanDevice& OutDevice);
	~VulkanSwapchain();

	void CreateSwapchain(uint32_t Width, uint32_t Height, bool bUseVSync = true);
	void Destroy();

	inline const VkSwapchainKHR GetHandle() const { return Swapchain; }
	inline const VkSurfaceKHR GetSurfaceHandle() const { return SurfaceKHR; }
	inline const SSwapchainDetails GetDetails() const { return SwapchainDetails; }
	inline const std::vector<VkImage>& GetImages() const { return SwapchainImages; }
	inline const std::vector<VkImageView>& GetImageViews() const { return SwapchainImageViews; }
	inline const uint32_t GetActiveImageIndex() const { return ActiveImageIndex; }

	static SSwapchainSupportDetails QuerySwapchainSupport(VkPhysicalDevice Device, VkSurfaceKHR SurfaceKHR);

	VkResult AcquireNextImage(const VkSemaphore WaitSemaphore);
	VkResult QueuePresent(const VkQueue PresentQueue, const VkSemaphore WaitSemaphore = VK_NULL_HANDLE);

private:
	VkInstance Instance;
	VulkanDevice& Device;
	VkSurfaceKHR SurfaceKHR;
	VkSwapchainKHR Swapchain;

	SSwapchainSupportDetails SwapchainSupportDetails;
	SSwapchainDetails SwapchainDetails;
	std::vector<VkImage> SwapchainImages;
	std::vector<VkImageView> SwapchainImageViews;

	uint32_t ActiveImageIndex;
};
