#pragma once

#include "jarp/Renderer/RendererAPI.h"

namespace jarp {

	class Renderer
	{
	public:
		static void Init();
		static void Render(uint32_t deltaTime);
		static void Shutdown();

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

	private:
		static std::shared_ptr<RendererAPI> s_RendererAPI;
	};

}
