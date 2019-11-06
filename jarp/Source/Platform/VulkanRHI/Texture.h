#pragma once

#include <volk.h>

namespace jarp {

	class VulkanCommandBuffer;
	class VulkanImage;
	class VulkanImageView;

	class Texture
	{
	public:
		Texture();
		~Texture();

		void Load(VulkanCommandBuffer& commandBuffer, const std::string& fileName);
		void Destroy();

		inline const VkSampler GetSampler() const { return m_Sampler; }
		inline const VulkanImageView& GetImageView() const { return *m_TextureImageView; }

	private:
		VulkanImage* m_TextureImage;
		VulkanImageView* m_TextureImageView;

		VkSampler m_Sampler;
	};

}
