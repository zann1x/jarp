#pragma once

namespace jarp {

	class RendererAPI
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
		virtual void Init() = 0;
		virtual void Render(uint32_t deltaTime) = 0;
		virtual void Shutdown() = 0;

		inline static API GetAPI() { return s_API; }

	private:
		static API s_API;
	};

}
