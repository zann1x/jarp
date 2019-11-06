#pragma once

#include <volk.h>

namespace jarp {

	class VulkanImageView
	{
	public:
		VulkanImageView();
		~VulkanImageView();

		void CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags imageAspectFlags);
		void Destroy();

		inline const VkImageView GetHandle() const { return m_ImageView; }

	private:
		VkImageView m_ImageView;
	};

}
