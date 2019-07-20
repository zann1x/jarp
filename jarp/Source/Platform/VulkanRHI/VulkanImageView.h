#pragma once

#include <vulkan/vulkan.h>

namespace jarp {

	class VulkanDevice;

	class VulkanImageView
	{
	public:
		VulkanImageView(VulkanDevice& Device);
		~VulkanImageView();

		void CreateImageView(VkImage Image, VkFormat Format, VkImageAspectFlags ImageAspectFlags);
		void Destroy();

		inline const VkImageView GetHandle() const { return ImageView; }

	private:
		VulkanDevice& Device;

		VkImageView ImageView;
	};

}
