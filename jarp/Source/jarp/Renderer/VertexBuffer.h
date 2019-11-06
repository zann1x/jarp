#pragma once

#include "jarp/Renderer/VertexComponent.h"
#include "jarp/Renderer/CommandBuffer.h"

namespace jarp {

	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() = default;

		virtual void Bind(const std::shared_ptr<CommandBuffer> commandBuffer) = 0;

		static VertexBuffer* Create(const std::vector<SVertex>& vertices, uint32_t size);
	};

}
