#pragma once

#include <volk.h>

namespace jarp {

	class VulkanInstance
	{
	public:
		VulkanInstance();
		~VulkanInstance();

		void CreateInstance();
		void Destroy();

		inline const VkInstance& GetHandle() const { return m_Instance; }

	private:
		VkInstance m_Instance;

		std::vector<const char*> m_InstanceLayers;
		std::vector<const char*> m_InstanceExtensions;
	};

}
