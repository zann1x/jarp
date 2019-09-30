#pragma once

#include <vulkan/vulkan.h>

namespace jarp {

	class VulkanImageView
	{
	public:
		VulkanImageView();
		~VulkanImageView();

		void CreateImageView(VkImage Image, VkFormat Format, VkImageAspectFlags ImageAspectFlags);
		void Destroy();

		inline const VkImageView GetHandle() const { return ImageView; }

	private:
		VkImageView ImageView;
	};

}
