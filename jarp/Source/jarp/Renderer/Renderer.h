#pragma once

namespace jarp {

	class Renderer
	{
	public:
		enum class API
		{
			None,
			DirectX,
			OpenGL,
			Vulkan
		};
	public:
		inline static API GetAPI() { return API::Vulkan; }
	};

}
