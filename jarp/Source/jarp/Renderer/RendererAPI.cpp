#include "jarppch.h"
#include "RendererAPI.h"

#include "Platform/VulkanRHI/VulkanRendererAPI.h"

namespace jarp {

	RendererAPI::API RendererAPI::s_API = RendererAPI::API::Vulkan;

}
