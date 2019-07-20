#pragma once

#include <vulkan/vulkan.h>

namespace jarp {

	class VulkanCommandBuffer;
	class VulkanDevice;
	class VulkanImage;
	class VulkanImageView;

	class Texture
	{
	public:
		Texture(VulkanDevice& Device);
		~Texture();

		void Load(VulkanCommandBuffer& CommandBuffer, const std::string& FileName);
		void Destroy();

		inline const VkSampler GetSampler() const { return Sampler; }
		inline const VulkanImageView& GetImageView() const { return *pTextureImageView; }

	private:
		VulkanDevice& Device;
		VulkanImage* pTextureImage;
		VulkanImageView* pTextureImageView;

		VkSampler Sampler;
	};

}
