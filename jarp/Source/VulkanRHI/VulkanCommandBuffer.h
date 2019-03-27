#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class VulkanDevice;
class VulkanCommandPool;

/*
Depends on:
- Device
- CommandPool
*/
class VulkanCommandBuffer
{
public:
	VulkanCommandBuffer(VulkanDevice& OutDevice, VulkanCommandPool& OutCommandPool);
	~VulkanCommandBuffer();

	void CreateCommandBuffer();
	void Destroy();

	inline const VkCommandBuffer& GetHandle() const { return CommandBuffer; }

private:
	VulkanDevice& Device;
	VulkanCommandPool& CommandPool;

	VkCommandBuffer CommandBuffer;
};
